#ifndef _IFILE_NOTIFIER_H_
#define _IFILE_NOTIFIER_H_

#include <filesystem>
#include <functional>
#include "isubject.h"

namespace FileUtils
{
    enum class FileEvents : int
    {
        FE_MODIFIED,
        FE_CREATED
    };

    //  Collection of files logically grouped to become a
    //  single watcher target.
    //  The template arg determines the type of the watchable
    //  subject for the current platform which usually will
    //  be the type used to identify file descriptors.
    // 
    //  For Windows, it'll be HANDLE.
    //  For linux, it is int.
    //
    class IFileNotifier
    {
    public:
        virtual ~IFileNotifier() {}

        //  IFileNotifier methods
        virtual void add_file(const std::filesystem::path& targetFile) = 0;
        virtual void remove_file(const std::filesystem::path& targetFile) = 0;

        //  This is a blocking call.
        //  The callback is called once per altered file found.
        //  The return value from callback() indicates whether or
        //  not to continue fetching changed files list.
        //  If callback returns true, get_changed_files_list() will
        //  continue calling callback() for subsequently found files.
        //  If callback returns false, it will cease its operation
        //  and return.
        //
        //  An alternative way to return information of altered
        //  files to the caller would be to collect it all together
        //  and return it once in a vector through the function's
        //  return value. But there are few issues with that design.
        //  In a highly actively system with hundreds or even thousands
        //  of files being constantly created/modified, the thread calling
        //  get_changed_files_list() will remain stuck until the function
        //  returns. It will get a chance to backup the modified files
        //  only after a long wait and the whole operation will be serialized
        //  reducing performance. This way, the caller gets a chance to start
        //  working immediately.
        virtual void get_changed_files_list(
            std::function<bool(std::filesystem::path, FileEvents)>& callback) = 0;
    };
}

#endif  //  _IFILE_NOTIFIER_H_
