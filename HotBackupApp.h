#ifndef _HOTBACKUP_APP_H_
#define _HOTBACKUP_APP_H_


#include <iostream>
#include <filesystem>
#include <memory>
#include <atomic>
#include "hotbackup_factory.h"
#include "ilogger.h"
#include "ifile_backup_plugin.h"

using HotBackup::ILogger;
using BackupManagement::IFileBackupPlugin;

namespace HotBackup
{
    class HotBackupApp
    {
    public:
        HotBackupApp(const std::filesystem::path& hotDirectory, std::filesystem::path backupDirectory);
        ~HotBackupApp();

        void init();
        void start_directory_listeners();
        void start_backup_workers();

        void shutdown();

    private:
        std::atomic_bool m_shutdown { false };

        std::filesystem::path m_hotDirectory;
        std::filesystem::path m_backupDirectory;
        std::shared_ptr<ILogger> m_logger;
        std::shared_ptr<IFileBackupQueue> m_queue;

        std::shared_ptr<IFileObserverSubject<int>> m_fileObserverSubject;
        std::shared_ptr<IFileObserver<int>> m_fileObserver;

        const uint32_t m_backupWorkersCount { 3 };
        std::shared_ptr<IBackupManager> m_backupManager;
        std::shared_ptr<IFileBackupPlugin> m_fileDeleterBackupPlugin;
    };
}

#endif  //  _HOTBACKUP_APP_H_
