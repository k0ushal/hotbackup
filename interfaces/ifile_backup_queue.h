
#ifndef _IFILE_BACKUP_QUEUE_
#define _IFILE_BACKUP_QUEUE_

#include <filesystem>
#include "ifile_backup_queue_producer.h"
#include "ifile_backup_queue_consumer.h"

namespace BackupManagement
{
    class IFileBackupQueue : public IFileBackupQueueProducer, public IFileBackupQueueConsumer
    {
    public:
        //  IFileBackupQueueProducer methods (inherited)
        virtual void push_item(const std::filesystem::path& filePath) = 0;

        //  IFileBackupQueueConsumer methods (inherited)
        virtual std::filesystem::path get_next_item(int timeoutInMs) = 0;

        virtual void shutdown() = 0;
    };
}

#endif  //  _IFILE_BACKUP_QUEUE_
