#ifndef _FILE_DELETER_PLUGIN_H_
#define _FILE_DELETER_PLUGIN_H_

#include <memory>
#include <mutex>
#include <future>
#include <atomic>
#include <filesystem>
#include "ilogger.h"
#include "ifile_backup_plugin.h"

using HotBackup::ILogger;

namespace BackupManagement
{
    class FileDeleterPlugin : public IFileBackupPlugin
    {
    public:
        FileDeleterPlugin();

        virtual ~FileDeleterPlugin();

        virtual void init(
            std::shared_ptr<ILogger> logger,
            const std::filesystem::path& backupDirectory) override;

        virtual void execute(
            const std::filesystem::path& sourceFile,
            bool& continueExecutingOtherPlugins) override;

        virtual void shutdown() override;

    protected:
        //  Assumes that sourceFile name has delete_ prefix.
        virtual std::tuple<std::string, std::string> get_filename_and_deletion_time(
            const std::filesystem::path& sourceFilePath);

        //  Assumes that all args are valid.
        virtual void schedule_files_for_deletion(
            std::vector<std::filesystem::path> filepaths,
            std::string isoTimestamp);

        virtual std::chrono::seconds get_waiting_time_before_deletion(
            std::string isoTimestamp);

        virtual bool delete_file(const std::filesystem::path& filepath);

    private:
        std::shared_ptr<ILogger> m_logger;
        const std::string m_deleteFilePrefix { "delete_" };
        std::filesystem::path m_backupDirectory;
        std::atomic_bool m_shutdown { false };

        std::mutex m_scheduledDeletionThreadsMutex;
        std::condition_variable m_scheduledDeletionThreadsCondVar;
        std::vector<std::future<void>> m_scheduledDeletionFutures;
        // std::vector<std::thread
    };
}

#endif  //  _FILE_DELETER_PLUGIN_H_
