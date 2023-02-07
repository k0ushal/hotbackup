
#ifndef _IFILE_OBJSERVER_SUBJECT_H_
#define _IFILE_OBJSERVER_SUBJECT_H_


#include "ifile_notifier.h"
#include "isubject.h"

namespace FileUtils
{
    template<typename T>
    class IFileObserverSubject : public IFileNotifier, public ISubject<T>
    {
    public:
        //  IFileNotifier methods (inherited)
        virtual void add_file(const std::filesystem::path& targetFile) = 0;
        virtual void remove_file(const std::filesystem::path& targetFile) = 0;
        virtual void get_changed_files_list(
            std::function<bool(std::filesystem::path, FileEvents)>& callback) = 0;

        //  ISubject methods (inherited)
        virtual T get_handle() = 0;
        virtual std::string name() = 0;
    };
}

#endif  //  _IFILE_OBJSERVER_SUBJECT_H_
