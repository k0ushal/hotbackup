#ifndef _FILE_DELETER_PLUGIN_H_
#define _FILE_DELETER_PLUGIN_H_

#include <memory>
#include <filesystem>
#include "ilogger.h"
#include "ifile_backup_plugin.h"

using HotBackup::ILogger;

namespace BackupManagement
{
    class FileDeleterPlugin : public IFileBackupPlugin
    {
    public:
        virtual void init(
            std::shared_ptr<ILogger> logger,
            const std::filesystem::path& backupDirectory) override;

        virtual void execute(
            const std::filesystem::path& sourceFile,
            bool& continueExecutingOtherPlugins) override;

    protected:
        //  Assumes that sourceFile name has delete_ prefix.
        virtual std::filesystem::path get_backup_file_path(
            const std::filesystem::path& sourceFilePath);

        virtual std::tuple<std::string, std::string> get_filename_and_deletion_time(
            const std::filesystem::path& sourceFilePath);

    private:
        std::shared_ptr<ILogger> m_logger;
        const std::string m_deleteFilePrefix { "delete_" };
        std::filesystem::path m_backupDirectory;
    };
}

#endif  //  _FILE_DELETER_PLUGIN_H_
