
#ifndef _FILE_OBSERVER_H_
#define _FILE_OBSERVER_H_

#include <iostream>
#include <map>
#include <exception>
#include <vector>
#include <thread>
#include <poll.h>
#include <atomic>
#include "../interfaces/isubject.h"
#include "../interfaces/ifile_observer.h"
#include "controller.h"

namespace FileUtils
{
    class FileObserver : public IFileObserver<int>
    {
    public:
        //  pollDurationInMs < 0 means the poll timeout
        //  is set to infinite.
        FileObserver(int pollTimeoutInMs = 5000);

        virtual ~FileObserver();

        virtual void add_subject(std::shared_ptr<ISubject<int>> target) override;

        //  This is a non-blocking call. The class spawns a separate thread
        //  and watches for changes to any of the registered subjects
        //  on that thread. When a subject becomes ready for IO, the
        //  callback is executed from that thread.
        //  The return value of the callback indicates whether or not to
        //  continue watching.
        //  If the callback returns false, the watcher thread stops
        //  watching
        virtual void start_observer(std::function<bool(std::shared_ptr<ISubject<int>>)> callback) override;
        virtual void stop_observer() override;

    private:
        void watcherThreadFunc(std::function<bool(std::shared_ptr<ISubject<int>>)> callback);

    private:
        std::map<int, std::shared_ptr<ISubject<int>>> m_subjectsList;
        std::thread m_watcherThread;
        int m_pollTimeoutInMs;
        std::shared_ptr<Controller> m_controller;
    };
}

#endif  //  _FILE_OBSERVER_H_
