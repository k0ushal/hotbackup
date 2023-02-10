
#ifndef _HOTBACKUP_FACTORY_H_
#define _HOTBACKUP_FACTORY_H_

#include <memory>
#include "interfaces/ifile_observer.h"
#include "interfaces/ifile_observer_subject.h"
#include "interfaces/ifile_backup_queue.h"
#include "interfaces/ibackup_manager.h"
#include "interfaces/ifile_backup_plugin.h"
#include "interfaces/ilogger.h"

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

#endif  //  _HOTBACKUP_FACTORY_H_
