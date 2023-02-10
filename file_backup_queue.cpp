#include <sstream>
#include "file_backup_queue.h"

using HotBackup::FileBackupQueue;

FileBackupQueue::FileBackupQueue()
{
}

FileBackupQueue::~FileBackupQueue()
{
    shutdown();
}

void FileBackupQueue::push_item(const std::filesystem::path& filePath)
{
    if (m_shutdown)
        return;

    if (std::filesystem::is_directory(filePath))
        return;

    // auto fPath { std::filesystem::canonical(filePath) };
    auto fPath { filePath };

    {
        std::unique_lock lock(m_mutex);

        if (m_uniqueFilePaths.count(fPath) > 0)
            return;

        m_uniqueFilePaths.emplace(fPath);
        m_queue.push(fPath);
    }

    m_condVar.notify_one();
}

std::filesystem::path FileBackupQueue::get_next_item(int timeoutInMs)
{
    if (m_shutdown)
        return {};

    {
        std::unique_lock lock(m_mutex);

        m_condVar.wait_for(lock, std::chrono::milliseconds(timeoutInMs), [&] {
            return (!m_queue.empty() || m_shutdown);
        });

        if (m_shutdown || m_queue.empty())
            return {};

        auto fPath { m_queue.front() };
        m_queue.pop();
        m_uniqueFilePaths.erase(fPath);

        return fPath;
    }
}

void FileBackupQueue::shutdown()
{
    m_shutdown = true;
    m_condVar.notify_all();
}
