#ifndef _IFILE_OBSERVER_H_
#define _IFILE_OBSERVER_H_

#include <functional>
#include <memory>
#include "isubject.h"

namespace FileUtils
{
    template<typename T>
    class IFileObserver
    {
    public:
        virtual ~IFileObserver() = default;

    public:
        virtual void add_subject(std::shared_ptr<ISubject<T>> target) = 0;

        //  This is a blocking call. The class spawns a separate thread
        //  and watches for changes to any of the registered subjects
        //  on that thread. When a subject becomes ready for IO, the
        //  callback is executed from that thread.
        //  The return value of the callback indicates whether or not to
        //  continue watching.
        //  If the callback returns false, the watcher thread stops
        //  watching 
        virtual void start_observer(std::function<bool(std::shared_ptr<ISubject<T>>)> callback) = 0;
        virtual void stop_observer() = 0;
    };
}

#endif
