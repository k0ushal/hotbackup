#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <sys/inotify.h>
#include "../interfaces/isubject.h"
#include "../interfaces/ifile_notifier.h"
#include "../interfaces/ifile_observer_subject.h"

namespace FileUtils
{
    class FileObserverSubject : public IFileObserverSubject<int>
    {
    public:
        FileObserverSubject(std::string name);
        virtual ~FileObserverSubject();

        //  IFileNotifier methods
        virtual void add_file(const std::filesystem::path& targetFile) override;

        virtual void remove_file(const std::filesystem::path& targetFile) override;

        //  This is a blocking call.
        virtual void get_changed_files_list(
            std::function<bool(std::filesystem::path, FileEvents)>& callback
            ) override;

        //  ISubject methods (inherited)
        virtual int get_handle() override
        {
            return m_notifyFd;
        }

        virtual std::string name() override
        {
            return m_name;
        }

    private:
        int m_notifyFd;
        std::string m_name;
        std::unordered_map<std::string, int> m_filepathToWatchIndex;
        std::unordered_map<int, std::string> m_watchToFilepathIndex;
    };
}
