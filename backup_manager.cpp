
#include <iostream>
#include <sstream>
#include <algorithm>
#include "backup_manager.h"
#include "interfaces/ifile_backup_queue.h"
#include "interfaces/ilogger.h"

using BackupManagement::BackupManager;
using BackupManagement::IFileBackupQueueConsumer;
using HotBackup::ILogger;

void BackupManager::init(
    const std::filesystem::path& backupDirectory,
    std::shared_ptr<IFileBackupQueueConsumer> backupQueue,
    std::shared_ptr<ILogger> logger)
{
    if (std::filesystem::exists(backupDirectory) && not std::filesystem::is_directory(backupDirectory))
    {
        std::ostringstream msg;
        msg << "Backup directory (" << backupDirectory << ") is not a directory";
        throw std::runtime_error(msg.str().c_str());
    }
    else
    {
        std::filesystem::create_directory(backupDirectory);
    }

    m_queue = backupQueue;
    m_backupDirectory = backupDirectory;
    m_logger = logger;
}

void BackupManager::add_plugin(std::shared_ptr<IFileBackupPlugin> plugin)
{
    if (m_shutdown || m_workers.size())
        throw std::runtime_error("Cannot add plugins while worker threads are running.");

    plugin->init(m_logger, m_backupDirectory);
    m_plugins.push_back(plugin);
}

void BackupManager::start_workers(unsigned workerCount)
{
    if (!workerCount)
        return;

    auto workerFunc { [&]() {

        while (!m_shutdown)
        {
            try
            {
                std::filesystem::path queueItem;

                //  Queue itself is re-entrant and shutdown aware.
                //  No need for synchronization here.
                queueItem = m_queue->get_next_item(-1);
                if (queueItem.empty())
                    continue;

                bool continueExecutingOtherPlugins { true };

                //  process the item through plugins
                for (auto& plugin : m_plugins)
                {
                    plugin->execute(queueItem, continueExecutingOtherPlugins);
                    if (not continueExecutingOtherPlugins)
                        break;
                }

                if (not continueExecutingOtherPlugins)
                    continue;

                //  backup the file
                backup_file(queueItem);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }};

    for (unsigned i = 0; i < workerCount; i++)
    {
        m_workers.push_back(std::thread(workerFunc));
    }
}

void BackupManager::stop_workers()
{
    m_shutdown = true;
    // m_condVar.notify_all();

    for (auto& worker : m_workers)
    {
        if (worker.joinable())
            worker.join();
    }

    m_workers.resize(0);
    m_shutdown = false;
}

void BackupManager::backup_file(const std::filesystem::path& sourceFile)
{
    std::ostringstream msg;
    if (not std::filesystem::exists(sourceFile) || not std::filesystem::is_regular_file(sourceFile))
    {
        std::ostringstream msg;
        msg << "Invalid file type (" << sourceFile << "), cannot back up";
        throw std::runtime_error(msg.str().c_str());
    }

    auto backupFilePath { m_backupDirectory / sourceFile.filename() };
    backupFilePath += std::string(".bak");

    auto ret { std::filesystem::copy_file(sourceFile, backupFilePath, std::filesystem::copy_options::overwrite_existing) };
    if (not ret)
    {
        //  write log
        msg << "Failed to backup file (" << sourceFile << ")";
        m_logger->log(msg.str());

        msg.str("");
        msg.clear();
        msg << "copy_file (" << sourceFile << ", " << backupFilePath << ") FAILED";
        throw std::runtime_error(msg.str().c_str());
    }

    msg << "backedup (" << sourceFile.filename().u8string() << ")";
    m_logger->log(msg.str());
}
