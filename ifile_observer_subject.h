
#include "../ifile_notifier_target.h"
#include "../isubject.h"

namespace FileUtils
{
    template<typename T>
    class IFileObserverSubject : public IFileNotifierTarget, public ISubject<T>
    {
    public:
        //  IFileNotifierTarget methods
        virtual void add_file(const std::filesystem::path& targetFile) = 0;
        virtual void remove_file(const std::filesystem::path& targetFile) = 0;
        virtual void get_changed_files_list(
            std::function<bool(std::filesystem::path, FileEvents)>& callback) = 0;

        //  ISubject methods
        virtual T get_handle();
    };
}
