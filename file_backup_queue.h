
#include <set>
#include <filesystem>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "ifile_backup_queue.h"

using BackupManagement::IFileBackupQueue;

namespace HotBackup
{
    //
    //  File backup class with dedupe provision.
    //  Imagine that we have only 1 worker thread which is busy
    //  backing up a large sized file. In the meantime,
    //  another file was modified twice. This file will now
    //  appear 2 times in the queue but it doesn't have to be
    //  backed up 2 separate times as it's the same file content.
    //  This class takes the above scenario into account and
    //  dedupes file entries in the queue.
    //
    class FileBackupQueue : public IFileBackupQueue
    {
    public:
        FileBackupQueue();

        virtual ~FileBackupQueue();

        //  IFileBackupQueueProducer methods (inherited)
        virtual void push_item(const std::filesystem::path& filePath) override;

        //  IFileBackupQueueConsumer methods (inherited)
        virtual std::filesystem::path get_next_item(int timeoutInMs) override;

        virtual void shutdown() override;

    private:
        //  Provides dedupe functionality
        std::set<std::filesystem::path> m_uniqueFilePaths;

        //  Maintains the order of incoming items.
        std::queue<std::filesystem::path> m_queue;
        std::mutex m_mutex;

        std::atomic_bool m_shutdown { false };
        std::condition_variable m_condVar;
    };
}
