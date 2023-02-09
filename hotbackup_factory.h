
#include <memory>
#include "ifile_observer.h"
#include "ifile_observer_subject.h"
#include "ifile_backup_queue.h"
#include "ibackup_manager.h"
#include "ifile_backup_plugin.h"
#include "ilogger.h"

using FileUtils::IFileNotifier;
using FileUtils::IFileObserverSubject;
using FileUtils::IFileObserver;
using BackupManagement::IFileBackupQueue;
using BackupManagement::IBackupManager;
using BackupManagement::IFileBackupPlugin;
using HotBackup::ILogger;

namespace HotBackup
{
    class HotBackupFactory
    {
    public:
        static std::shared_ptr<IFileObserver<int>> create_file_observer(int pollTimeoutInMs = 5000);
        static std::shared_ptr<IFileObserverSubject<int>> create_file_observer_subject(std::string name);

        static std::shared_ptr<IFileBackupQueue> create_file_backup_queue();
        static std::shared_ptr<IBackupManager> create_backup_manager();
        static std::shared_ptr<IFileBackupPlugin> create_deleter_plugin();

        static std::shared_ptr<ILogger> create_logger();
    };
}