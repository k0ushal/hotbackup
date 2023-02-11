
#ifndef _IBACKUP_MANAGER_H_
#define _IBACKUP_MANAGER_H_

#include <filesystem>
#include "ifile_backup_queue_consumer.h"
#include "ifile_backup_plugin.h"
#include "ilogger.h"

using BackupManagement::IFileBackupQueueConsumer;
using BackupManagement::IFileBackupPlugin;
using HotBackup::ILogger;

namespace BackupManagement
{
    class IBackupManager
    {
    public:
        IBackupManager(const IBackupManager&) = delete;
        IBackupManager& operator =(const IBackupManager&) = delete;
        IBackupManager(IBackupManager&&) = delete;
        IBackupManager& operator =(IBackupManager&&) = delete;

        IBackupManager() {}
        virtual ~IBackupManager() {}

        virtual void init(
            const std::filesystem::path& backupDirectory,
            std::shared_ptr<IFileBackupQueueConsumer> backupQueue,
            std::shared_ptr<ILogger> logger) = 0;

        virtual void add_plugin(std::shared_ptr<IFileBackupPlugin> plugin) = 0;

        virtual void start_workers(unsigned workerCount) = 0;
        virtual void stop_workers() = 0;
    };
}

#endif  //  _IBACKUP_MANAGER_H_
