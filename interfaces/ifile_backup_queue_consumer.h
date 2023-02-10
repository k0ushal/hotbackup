
#ifndef _IFILE_BACKUP_QUEUE_CONSUMER_H_
#define _IFILE_BACKUP_QUEUE_CONSUMER_H_

#include <filesystem>

namespace BackupManagement
{
    class IFileBackupQueueConsumer
    {
    public:
        virtual std::filesystem::path get_next_item(int timeoutInMs) = 0;
    };
}

#endif  //  _IFILE_BACKUP_QUEUE_CONSUMER_H_
