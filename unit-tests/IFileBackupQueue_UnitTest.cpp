#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "../interfaces/ifile_backup_queue.h"
#include "../hotbackup_factory.h"

using HotBackup::HotBackupFactory;
using BackupManagement::IFileBackupQueue;

namespace HotBackup_UnitTests
{
    class IFileBackupQueue_UnitTest : public testing::Test
    {
    protected:
        IFileBackupQueue_UnitTest() :
            m_backupQueue(HotBackupFactory::create_file_backup_queue())
        {
            std::filesystem::create_directory(m_testingDirectory);
        }

        ~IFileBackupQueue_UnitTest()
        {
            std::filesystem::remove_all(m_testingDirectory);
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

        std::filesystem::path create_file(std::string fname)
        {
            auto fpath { m_testingDirectory / fname };
            std::ofstream { fpath };

            return fpath;
        }

    public:
        std::shared_ptr<IFileBackupQueue> m_backupQueue;
        const std::filesystem::path m_testingDirectory { "/tmp/testing" };
    };

    TEST_F(IFileBackupQueue_UnitTest, SmokeTest)
    {
        std::ostringstream result;

        m_backupQueue->push_item(create_file("abc"));
        auto nextItem { m_backupQueue->get_next_item(1000) };
        ASSERT_TRUE(nextItem.u8string() == (m_testingDirectory / "abc").u8string());
    }

    TEST_F(IFileBackupQueue_UnitTest, DedupeTest)
    {
        std::ostringstream result;

        m_backupQueue->push_item(create_file("abc"));
        m_backupQueue->push_item(create_file("pqr"));
        m_backupQueue->push_item(create_file(m_testingDirectory / "abc"));

        auto nextItem { m_backupQueue->get_next_item(200) };
        auto nextItem1 { m_backupQueue->get_next_item(200) };
        auto nextItem2 { m_backupQueue->get_next_item(200) };

        ASSERT_TRUE(nextItem.u8string() == (m_testingDirectory / "abc").u8string());
        ASSERT_TRUE(nextItem1.u8string() == (m_testingDirectory / "pqr").u8string());
        ASSERT_TRUE(nextItem2.u8string().empty());
    }

    TEST_F(IFileBackupQueue_UnitTest, DedupeTest_2)
    {
        std::ostringstream result;

        m_backupQueue->push_item(create_file("abc"));
        m_backupQueue->push_item(create_file("pqr"));
        m_backupQueue->push_item(create_file(m_testingDirectory / "abc"));
        
        auto nextItem { m_backupQueue->get_next_item(200) };
        auto nextItem1 { m_backupQueue->get_next_item(200) };
        auto nextItem2 { m_backupQueue->get_next_item(200) };

        ASSERT_TRUE(nextItem.u8string() == (m_testingDirectory / "abc").u8string());
        ASSERT_TRUE(nextItem1.u8string() == (m_testingDirectory / "pqr").u8string());
        ASSERT_TRUE(nextItem2.u8string().empty());
    }

    TEST_F(IFileBackupQueue_UnitTest, Multiple_Threads_Waiting_When_Shutdown_Is_Signalled)
    {
    }
}
