
#include <filesystem>

namespace BackupManagement
{
    class IFileBackupQueueConsumer
    {
    public:
        virtual std::filesystem::path get_next_item(int timeoutInMs) = 0;
    };
}
