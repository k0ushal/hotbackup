
#include <memory>
#include "ifile_observer.h"
#include "ifile_observer_subject.h"

using FileUtils::IFileNotifier;
using FileUtils::IFileObserverSubject;
using FileUtils::IFileObserver;

namespace HotBackup
{
    class HotBackupFactory
    {
    public:
        static std::shared_ptr<IFileObserver<int>> create_file_observer(int pollTimeoutInMs);
        static std::shared_ptr<IFileObserverSubject<int>> create_file_observer_subject();
    };
}