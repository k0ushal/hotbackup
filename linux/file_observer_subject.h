#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <sys/inotify.h>
#include "../ifile_notifier_target.h"
#include "../ifile_observer_subject.h"

namespace FileUtils
{
    class FileObserverSubject : public IFileObserverSubject<int>
    {
    public:
        FileObserverSubject();
        virtual ~FileObserverSubject();

        //  IFileNotifierTarget methods
        virtual void add_file(const std::filesystem::path& targetFile) override;

        virtual void remove_file(const std::filesystem::path& targetFile) override;

        //  This is a blocking call.
        virtual void get_changed_files_list(
            std::function<bool(std::filesystem::path, FileEvents)>& callback
            ) override;

        //  ISubject methods
        virtual int get_handle() override;

    private:
        int m_notifyFd;
        std::unordered_map<std::string, int> m_filepathToWatchIndex;
        std::unordered_map<int, std::string> m_watchToFilepathIndex;
    };
}
