
#include "hotbackup_factory.h"
#include "linux/file_observer.h"
#include "linux/file_observer_subject.h"
#include "file_backup_queue.h"
#include "backup_manager.h"
#include "logger.h"

using HotBackup::HotBackupFactory;
using HotBackup::FileBackupQueue;
using HotBackup::Logger;
using FileUtils::FileObserver;
using FileUtils::FileObserverSubject;
using BackupManagement::BackupManager;

std::shared_ptr<IFileObserver<int>> HotBackupFactory::create_file_observer(
    int pollTimeoutInMs
)
{
    return std::make_shared<FileObserver>(pollTimeoutInMs);
}

std::shared_ptr<IFileObserverSubject<int>> HotBackupFactory::create_file_observer_subject(std::string name)
{
    return std::make_shared<FileObserverSubject>(name);
}

std::shared_ptr<IFileBackupQueue> HotBackupFactory::create_file_backup_queue()
{
    return std::make_shared<FileBackupQueue>();
}

std::shared_ptr<ILogger> HotBackupFactory::create_logger()
{
    return std::make_shared<Logger>();
}

std::shared_ptr<IBackupManager> HotBackupFactory::create_backup_manager()
{
    return std::make_shared<BackupManager>();
}
