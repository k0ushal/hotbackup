#ifndef _IFILE_DELETER_PLUGIN_MOCK_H_
#define _IFILE_DELETER_PLUGIN_MOCK_H_

#include "../file_deleter_plugin.h"

using BackupManagement::FileDeleterPlugin;

namespace HotBackup_UnitTests
{
    class IFileDeleterPlugin_Mock : public FileDeleterPlugin
    {
    public:
        virtual std::tuple<std::string, std::string> get_filename_and_deletion_time(
            const std::filesystem::path& filePath)
        {
            return FileDeleterPlugin::get_filename_and_deletion_time(filePath);
        }

        virtual std::chrono::milliseconds get_waiting_time_before_deletion(std::string timestamp)
        {
            return FileDeleterPlugin::get_waiting_time_before_deletion(timestamp);
        }

        virtual bool delete_file(const std::filesystem::path& path)
        {
            return true;
        }
    };
}

#endif  //  _IFILE_DELETER_PLUGIN_MOCK_H_
