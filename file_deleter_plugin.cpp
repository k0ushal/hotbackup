
#include <iostream>
#include <filesystem>
#include <future>
#include <regex>
#include <sstream>
#include "file_deleter_plugin.h"

using BackupManagement::FileDeleterPlugin;

FileDeleterPlugin::FileDeleterPlugin()
{
}

FileDeleterPlugin::~FileDeleterPlugin()
{
    shutdown();
}

void FileDeleterPlugin::init(
    std::shared_ptr<ILogger> logger,
    const std::filesystem::path& backupDirectory)
{
    m_logger = logger;
    m_backupDirectory = backupDirectory;
}

std::tuple<std::string, std::string> FileDeleterPlugin::get_filename_and_deletion_time(
            const std::filesystem::path& sourceFilePath)
{
    auto srcFilename { sourceFilePath.filename().u8string() };
    auto deletePrefixRegex { std::regex(R"(delete_(.*))") };

    //  Discard delete_ prefix
    std::smatch match;
    auto ret { std::regex_match(srcFilename, match, deletePrefixRegex) };
    if (!ret)
        throw std::runtime_error("delete prefix not found in filename");

    auto isoTimestampAndfilename { match[1].str() };

    //  Check for and extract ISO timestamp
    std::string timestamp;
    std::smatch matchIsoDate;
    auto isoDatePrefixRegex { std::regex(R"(([0-9]{4}\-[0-9]{2}\-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}Z)_(.*))") };
    ret = std::regex_match(isoTimestampAndfilename, matchIsoDate, isoDatePrefixRegex);
    if (ret)
    {
        timestamp = matchIsoDate[1];
        isoTimestampAndfilename = matchIsoDate[2];
    }

    return { isoTimestampAndfilename, timestamp };
}

void FileDeleterPlugin::execute(
    const std::filesystem::path& path,
    bool& continueExecutingOtherPlugins)
{
    std::ostringstream msg;
    continueExecutingOtherPlugins = true;

    //  Check for delete_ prefix in file name
    if (0 != path.filename().u8string().find(m_deleteFilePrefix))
        return;

    //  Cease further execution of backup plugins.
    continueExecutingOtherPlugins = false;

    //  Parse file name and extract iso timestamp and actual file name.
    auto [filename, deletionTime] = get_filename_and_deletion_time(path);

    auto backupFilePath { m_backupDirectory / (filename + ".bak") };

    //  No ISO timestamp: Delete immediately.
    if (deletionTime.empty())
    {
        auto delSourceFile { delete_file(path) && delete_file(backupFilePath) };
        if (!delSourceFile)
            msg << "FAILED delete (" << filename << ")";
        else
            msg << "deleted (" << filename << ")";

        m_logger->log(msg.str());
        return;
    }

    //  Schedule deletion on ISO timestamp
    std::vector<std::filesystem::path> filesToDelete;
    filesToDelete.push_back(path);
    filesToDelete.push_back(m_backupDirectory / (filename + ".bak"));

    schedule_files_for_deletion(filesToDelete, deletionTime);
}

void FileDeleterPlugin::schedule_files_for_deletion(
    std::vector<std::filesystem::path> files,
    std::string isoTimestamp)
{
    auto threadFunc { [this](std::vector<std::filesystem::path> filepaths, std::string isoTimestamp) {

        //  Log entry
        std::ostringstream msg;
        std::string tBuff { filepaths[0].filename().u8string() };
        std::string sourceFileName (
            tBuff.begin() + (m_deleteFilePrefix.length() + isoTimestamp.length() + 1),
            tBuff.end());

        msg << "scheduled delete (" << sourceFileName << ") on " << isoTimestamp;
        m_logger->log(msg.str());

        //  Put the thread on wait until timestamp. The wait will timeout
        //  after the set duration and then we execute the delete operation.
        //  The wait is interruptible by a shutdown request.
        auto waitingTimeInMilliseconds { get_waiting_time_before_deletion(isoTimestamp) };

        std::unique_lock lock(m_scheduledDeletionThreadsMutex);
        m_scheduledDeletionThreadsCondVar.wait_for(lock, waitingTimeInMilliseconds, [&] {
            return (true == m_shutdown);
        });

        if (m_shutdown)
            return;

        //  Delete files.
        delete_file(filepaths[0]);
        delete_file(filepaths[1]);

        msg.str("");
        msg.clear();
        msg << "deleted (" << sourceFileName << ")";

        m_logger->log(msg.str());
    }};

    std::future<void> future { std::async(threadFunc, files, isoTimestamp) };
    m_scheduledDeletionFutures.push_back(std::move(future));
}

std::chrono::milliseconds FileDeleterPlugin::get_waiting_time_before_deletion(
    std::string isoTimestamp)
{
    struct tm tmTimestamp {};

    std::memset(&tmTimestamp, 0, sizeof(tmTimestamp));
    sscanf(
        isoTimestamp.c_str(),
        "%4u-%2u-%2uT%2u:%2u:%2uZ",
        &tmTimestamp.tm_year,
        &tmTimestamp.tm_mon,
        &tmTimestamp.tm_mday,
        &tmTimestamp.tm_hour,
        &tmTimestamp.tm_min,
        &tmTimestamp.tm_sec
        );

    tmTimestamp.tm_year -= 1900;    //  Year since 1900
    tmTimestamp.tm_mon -= 1;        //  Month 0 - 11
    tmTimestamp.tm_isdst = 0;

    auto timeT { std::mktime(&tmTimestamp) };
    auto tPoint { std::chrono::system_clock::from_time_t(timeT) };
    auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(tPoint - std::chrono::system_clock::now());

    return diffTime;
}

void FileDeleterPlugin::shutdown()
{
    m_shutdown = true;
    m_scheduledDeletionThreadsCondVar.notify_all();

    for (auto& future : m_scheduledDeletionFutures)
        future.wait();
}

bool FileDeleterPlugin::delete_file(
    const std::filesystem::path& filepath)
{
    return std::filesystem::remove(filepath);
}
