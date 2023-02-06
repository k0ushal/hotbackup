#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "../ibackup_manager.h"
#include "../hotbackup_factory.h"

using HotBackup::HotBackupFactory;
using BackupManagement::IBackupManager;

namespace HotBackup_UnitTests
{
    class IBackupManager_UnitTest : public testing::Test
    {
    protected:
        IBackupManager_UnitTest()
        {
        }

        ~IBackupManager_UnitTest()
        {
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

    public:
        std::shared_ptr<IBackupManager> m_backupQueue;
    };

    TEST_F(IBackupManager_UnitTest, SmokeTest)
    {
        ASSERT_TRUE(5 == 5);
    }
}
