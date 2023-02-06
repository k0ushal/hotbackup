#include <sstream>
#include "file_backup_queue.h"

using HotBackup::FileBackupQueue;

void FileBackupQueue::push_item(const std::filesystem::path& filePath)
{
    if (std::filesystem::is_directory(filePath))
    {
        std::ostringstream msg;
        msg << "Input arg (" << filePath << ") is a directory which is not supported";
        throw std::runtime_error(msg.str().c_str());
    }

    auto fPath { std::filesystem::canonical(filePath) };

    {
        std::unique_lock lock(m_queueMutex);

        if (m_uniqueFilePaths.count(fPath) > 0)
            return;

        m_uniqueFilePaths.emplace(fPath);
        m_queue.push(fPath);
    }
}

std::filesystem::path FileBackupQueue::get_next_item(int timeoutInMs)
{
    {
        std::unique_lock lock(m_queueMutex);

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