
#include "hotbackup_factory.h"
#include "linux/file_observer.h"
#include "linux/file_observer_subject.h"

using HotBackup::HotBackupFactory;
using FileUtils::FileObserver;
using FileUtils::FileObserverSubject;

std::shared_ptr<IFileObserver<int>> HotBackupFactory::create_file_observer(
    int pollTimeoutInMs
)
{
    return std::make_shared<FileObserver>(pollTimeoutInMs);
}
std::shared_ptr<IFileObserverSubject<int>> HotBackupFactory::create_file_observer_subject()
{
    return std::make_shared<FileObserverSubject>();
}