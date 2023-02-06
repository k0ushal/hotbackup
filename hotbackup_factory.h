
#include <memory>
#include "ifile_observer.h"
#include "ifile_observer_subject.h"
#include "ifile_backup_queue.h"

using FileUtils::IFileNotifier;
using FileUtils::IFileObserverSubject;
using FileUtils::IFileObserver;
using BackupManagement::IFileBackupQueue;

namespace HotBackup
{
    class HotBackupFactory
    {
    public:
        static std::shared_ptr<IFileObserver<int>> create_file_observer(int pollTimeoutInMs = 5000);
        static std::shared_ptr<IFileObserverSubject<int>> create_file_observer_subject();

        static std::shared_ptr<IFileBackupQueue> create_file_backup_queue();
    };
}