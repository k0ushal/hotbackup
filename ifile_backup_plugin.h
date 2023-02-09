#ifndef _IFILE_BACKUP_PLUGIN_H_
#define _IFILE_BACKUP_PLUGIN_H_

#include <memory>
#include <filesystem>
#include "ilogger.h"

using HotBackup::ILogger;

namespace BackupManagement
{
    class IFileBackupPlugin
    {
    public:
        virtual void init(std::shared_ptr<ILogger> logger) = 0;

        virtual void execute(const std::filesystem::path& path, bool& continueExecutingOtherPlugins) = 0;
    };
}

#endif  //  _IFILE_BACKUP_PLUGIN_H_
