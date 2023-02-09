#ifndef _IFILE_DELETER_PLUGIN_MOCK_H_
#define _IFILE_DELETER_PLUGIN_MOCK_H_

#include "../file_deleter_plugin.h"

using BackupManagement::FileDeleterPlugin;

namespace HotBackup_UnitTests
{
    class IFileDeleterPlugin_Mock : public FileDeleterPlugin
    {
    public:
        std::tuple<std::string, std::string> process_file_name(
            const std::filesystem::path& filePath)
        {
            return get_filename_and_deletion_time(filePath);
        }

        std::chrono::seconds get_waiting_time_before_deletion(std::string timestamp)
        {
            return FileDeleterPlugin::get_waiting_time_before_deletion(timestamp);
        }
    };
}

#endif  //  _IFILE_DELETER_PLUGIN_MOCK_H_
