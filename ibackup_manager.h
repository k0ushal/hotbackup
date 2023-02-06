
#ifndef _IBACKUP_MANAGER_H_
#define _IBACKUP_MANAGER_H_

#include <filesystem>

namespace BackupManagement
{
    class IBackupManager
    {
    public:
        virtual void init(const std::filesystem::path& dirPath, unsigned workerCount) = 0;
    };
}

#endif  //  _IBACKUP_MANAGER_H_
