
#include "hotbackup_factory.h"
#include "linux/file_observer.h"
#include "linux/file_observer_subject.h"
#include "file_backup_queue.h"

using HotBackup::HotBackupFactory;
using HotBackup::FileBackupQueue;
using FileUtils::FileObserver;
using FileUtils::FileObserverSubject;

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
