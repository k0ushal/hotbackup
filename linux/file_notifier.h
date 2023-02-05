
#ifndef _FILE_WATCHER_H_
#define _FILE_WATCHER_H_

#include <iostream>
#include <unordered_map>
#include <exception>
#include <vector>
#include <thread>
#include <poll.h>
#include "../isubject.h"
#include "../ifile_notifier.h"

namespace FileUtils
{
    class FileNotifier
    {
    public:
        virtual ~FileNotifier() {}

        FileNotifier(std::vector<ISubject<int> *> subjectsList)
        {
            for (auto subject : subjectsList)
            {
                if (!subject)
                    throw std::invalid_argument("Invalid argument");

                //  Ignore duplicate entries (if any)
                if (m_subjectsList.count(subject->get_handle()) > 0)
                    continue;

                m_subjectsList[subject->get_handle()] = subject;
            }
        }

        //  This is a non-blocking call. The class spawns a separate thread
        //  and watches for changes to any of the registered subjects
        //  on that thread. When a subject becomes ready for IO, the
        //  callback is executed from that thread.
        //  The return value of the callback indicates whether or not to
        //  continue watching.
        //  If the callback returns false, the watcher thread stops
        //  watching
        virtual void watch(std::function<bool(ISubject<int> *)> callback)
        {
            if (m_watcherThread.joinable())
                return;

            m_watcherThread = std::thread(FileNotifier::watcherThreadFunc, this, callback);
        }

    private:
        void watcherThreadFunc(std::function<bool(ISubject<int> *)> callback)
        {
            std::vector<pollfd> fds(m_subjectsList.size());

            auto subjIter { m_subjectsList.begin() };
            for (size_t i = 0; i < fds.size() && subjIter != m_subjectsList.end(); i++, subjIter++)
            {
                fds[i].fd = subjIter->first;
                fds[i].events = POLLIN;
            }

            bool continuePolling { true };
            do
            {
                auto signalledFds { poll(fds.data(), fds.size(), -1) };
                if (signalledFds <= 0)
                {
                    std::cout << "poll(errcode: " << errno << ") FAILED." << std::endl;
                }

                while (signalledFds > 0)
                {
                    for (size_t i = 0; i < fds.size(); i++)
                    {
                        if (POLLIN & fds[i].revents)
                        {
                            fds[i].revents = 0;

                            //  notify
                            auto signalledSubject { m_subjectsList[fds[i].fd] };
                            continuePolling = callback(signalledSubject);
                            if (!continuePolling)
                                break;
                        }
                    }
                    signalledFds--;
                }

            } while (continuePolling);
        }

    private:
        std::unordered_map<int, ISubject<int>*> m_subjectsList;
        std::thread m_watcherThread;
    };
}

#endif  //  _FILE_WATCHER_H_
