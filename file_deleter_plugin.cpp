
#include <filesystem>
#include <regex>
#include <sstream>
#include "file_deleter_plugin.h"

using BackupManagement::FileDeleterPlugin;

void FileDeleterPlugin::init(
    std::shared_ptr<ILogger> logger,
    const std::filesystem::path& backupDirectory)
{
    m_logger = logger;
    m_backupDirectory = backupDirectory;
}

std::filesystem::path FileDeleterPlugin::get_backup_file_path(
    const std::filesystem::path& sourceFilePath)
{
    return {};
}

std::tuple<std::string, std::string> FileDeleterPlugin::get_filename_and_deletion_time(
            const std::filesystem::path& sourceFilePath)
{
    auto srcFilename { sourceFilePath.stem().u8string() };
    auto deletePrefixRegex { std::regex(R"(delete_(.*))") };
    auto isoDatePrefix { std::regex(R"(([0-9]{4}\-[0-9]{2}\-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}Z)_(.*))") };

    //  Discard delete_ prefix
    std::smatch match;
    auto ret { std::regex_match(srcFilename, match, deletePrefixRegex) };
    if (!ret)
        throw std::runtime_error("delete prefix not found in filename");

    auto filename { match[1].str() };

    std::string timestamp;
    std::smatch matchIsoDate;
    ret = std::regex_match(filename, matchIsoDate, isoDatePrefix);
    if (ret)
    {
        timestamp = matchIsoDate[1];
        filename = matchIsoDate[2];
    }

    return { filename, timestamp };
}

void FileDeleterPlugin::execute(
    const std::filesystem::path& path,
    bool& continueExecutingOtherPlugins)
{
    continueExecutingOtherPlugins = true;

    //  Check for delete_ prefix in file name
    if (0 != path.filename().u8string().find(m_deleteFilePrefix))
        return;

    std::ostringstream msg;
    auto backedUpFilepath { get_backup_file_path(path) };
    auto delSourceFile { std::filesystem::remove(path) };
    // auto delBackupFile { std::filesystem::remove() };
    if (!delSourceFile)
    {
        msg << "FAILED";
    }
    else
    {
        msg << "SUCCEEDED";
    }

    msg << " :: delete (" << path.filename() << ")";
    m_logger->log(msg.str());

    continueExecutingOtherPlugins = false;
}
