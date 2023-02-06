
#include <filesystem>

namespace BackupManagement
{
    class IFileBackupQueueProducer
    {
    public:
        virtual void push_item(const std::filesystem::path& filePath) = 0;
    };
}
