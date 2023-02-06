
#include <vector>
#include <exception>
#include <functional>
#include "../isubject.h"
#include "file_observer.h"
#include "controller.h"

using FileUtils::FileObserver;
using FileUtils::Controller;

FileObserver::FileObserver(int pollTimeoutInMs) :
    m_pollTimeoutInMs(pollTimeoutInMs),
    m_controller(std::make_shared<Controller>())
{
    add_subject(m_controller.get());
}

FileObserver::~FileObserver()
{
}

void FileObserver::add_subject(ISubject<int>* subject)
{
    if (!subject)
        throw std::invalid_argument("Invalid argument");

    //  Ignore duplicate entries (if any)
    if (m_subjectsList.count(subject->get_handle()) > 0)
        return;

    m_subjectsList[subject->get_handle()] = subject;
}

void FileObserver::start_observer(std::function<bool(ISubject<int> *)> callback)
{
    if (m_watcherThread.joinable())
        return;

    m_watcherThread = std::thread(&FileObserver::watcherThreadFunc, this, callback);
}

void FileObserver::stop_observer()
{
    m_controller->signal_event("exit");

    if (m_watcherThread.joinable())
        m_watcherThread.join();
}

void FileObserver::watcherThreadFunc(std::function<bool(ISubject<int> *)> callback)
{
    std::vector<pollfd> fds(m_subjectsList.size());

    auto subjIter { m_subjectsList.begin() };
    for (size_t i = 0; i < fds.size() && subjIter != m_subjectsList.end(); i++, subjIter++)
    {
        fds[i].fd = subjIter->first;
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }

    bool continuePolling { true };
    do
    {
        auto signalledFds { poll(fds.data(), fds.size(), m_pollTimeoutInMs) };
        if (signalledFds < 0)
        {
            std::cout << "poll(errcode: " << errno << ") FAILED." << std::endl;
        }
        else if (0 == signalledFds)
        {
            continue;
        }

        while (signalledFds > 0)
        {
            for (size_t i = 0; i < fds.size(); i++)
            {
                if (POLLIN & fds[i].revents)
                {
                    fds[i].revents = 0;

                    auto signalledSubject { m_subjectsList[fds[i].fd] };

                    //  check if stop_observing is called.
                    if (signalledSubject == m_controller.get())
                    {
                        continuePolling = false;
                        signalledFds = 0;
                        break;
                    }

                    //  notify registered subjects.
                    continuePolling = callback(signalledSubject);
                    if (!continuePolling)
                        break;
                }
            }
            signalledFds--;
        }

    } while (continuePolling);
}
