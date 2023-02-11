
#ifndef _BACKUP_MANAGER_H_
#define _BACKUP_MANAGER_H_

#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "interfaces/ibackup_manager.h"
#include "interfaces/ifile_backup_queue_consumer.h"

namespace BackupManagement
{
    class BackupManager : public IBackupManager
    {
    public:
        BackupManager(const BackupManager&) = delete;
        BackupManager& operator =(const BackupManager&) = delete;
        BackupManager(BackupManager&&) = delete;
        BackupManager& operator =(BackupManager&&) = delete;

        BackupManager() = default;
        virtual ~BackupManager() = default;

    public:
        virtual void init(
            const std::filesystem::path& backupDirectory,
            std::shared_ptr<IFileBackupQueueConsumer> backupQueue,
            std::shared_ptr<ILogger> logger) override;

        virtual void add_plugin(std::shared_ptr<IFileBackupPlugin> plugin) override;

        virtual void start_workers(unsigned workerCount) override;

        virtual void stop_workers() override;

    protected:
        virtual void backup_file(const std::filesystem::path& sourceFile);

    private:
        std::vector<std::thread> m_workers;
        std::vector<std::shared_ptr<IFileBackupPlugin>> m_plugins;
        std::shared_ptr<IFileBackupQueueConsumer> m_queue;
        std::shared_ptr<ILogger> m_logger;
        std::filesystem::path m_backupDirectory;
        std::atomic_bool m_shutdown { false };
    };
}

#endif  //  _BACKUP_MANAGER_H_
