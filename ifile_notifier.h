#ifndef _IFILE_NOTIFIER_H_
#define _IFILE_NOTIFIER_H_

#include <functional>
#include "isubject.h"

namespace FileUtils
{
    template<typename T>
    class IFileNotifier
    {
    public:
        virtual ~IFileNotifier() {}

        virtual void add_subject(ISubject<T>* target, FileOperations watchFor) = 0;

        //  This is a blocking call. The class spawns a separate thread
        //  and watches for changes to any of the registered subjects
        //  on that thread. When a subject becomes ready for IO, the
        //  callback is executed from that thread.
        //  The return value of the callback indicates whether or not to
        //  continue watching.
        //  If the callback returns false, the watcher thread stops
        //  watching 
        virtual void watch(std::function<bool(ISubject<T> *)> callback);
    };
}

#endif
