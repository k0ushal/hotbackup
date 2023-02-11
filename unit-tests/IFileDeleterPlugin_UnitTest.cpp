
#include <gtest/gtest.h>
#include <chrono>
#include "IFile_Deleter_Plugin_Mock.h"
#include "ILogger_Mock.h"

using BackupManagement::IFileBackupPlugin;
using HotBackup_UnitTests::IFileDeleterPlugin_Mock;
using HotBackup::ILogger;
using HotBackup_Mocks::ILogger_Mock;

namespace HotBackup_UnitTests
{
    class IFileDeleterPlugin_UnitTest : public testing::Test
    {
    protected:
        IFileDeleterPlugin_UnitTest() :
            m_fileDeleterPlugin { std::make_shared<IFileDeleterPlugin_Mock>() },
            m_logger { std::make_shared<ILogger_Mock>() }
        {
            std::filesystem::create_directory(m_backupDirectory);
            m_fileDeleterPlugin->init(m_logger, m_backupDirectory);
        }

        ~IFileDeleterPlugin_UnitTest()
        {
            std::filesystem::remove_all(m_backupDirectory);
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
            m_logger->clear_logs();
        }

    public:
        std::shared_ptr<IFileDeleterPlugin_Mock> m_fileDeleterPlugin;
        std::shared_ptr<ILogger_Mock> m_logger;
        std::filesystem::path m_testingDirectory { "/tmp/testing" };
        std::filesystem::path m_backupDirectory { "/tmp/backup" };
    };

    TEST_F(IFileDeleterPlugin_UnitTest, SmokeTest)
    {
        auto [filename, deletionTime] = m_fileDeleterPlugin->get_filename_and_deletion_time("/tmp/test/delete_abc.txt");
        ASSERT_EQ(filename, "abc.txt");
        ASSERT_EQ(deletionTime, "");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, Multiple_Extension_Filename)
    {
        auto [filename, deletionTime] = m_fileDeleterPlugin->get_filename_and_deletion_time("/tmp/test/delete_abc.txt.doc");
        ASSERT_EQ(filename, "abc.txt.doc");
        ASSERT_EQ(deletionTime, "");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, File_With_ISO_timestamp)
    {
        std::filesystem::path filepath { "/tmp/test/delete_2022-01-21T09:21:00Z_abc.doc" };
        auto [filename, deletionTime] = m_fileDeleterPlugin->get_filename_and_deletion_time(filepath);
        ASSERT_EQ(filename, "abc.doc");
        ASSERT_EQ(deletionTime, "2022-01-21T09:21:00Z");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, Convert_ISO_timestamp_to_waiting_duration)
    {
        auto fiveMins17secFromNow = std::chrono::system_clock::now() + std::chrono::minutes(5) + std::chrono::seconds(17);
        auto tt { std::chrono::system_clock::to_time_t(fiveMins17secFromNow) };
        auto lt { std::localtime(&tt) };

        std::ostringstream timestamp;
        timestamp << std::put_time(lt, "%Y-%m-%dT%H:%M:%SZ");

        auto waitingTimeInSeconds { m_fileDeleterPlugin->get_waiting_time_before_deletion(timestamp.str()) };

        //  Account for 1 sec tolerance.
        ASSERT_TRUE(waitingTimeInSeconds.count() >= 316000 && waitingTimeInSeconds.count() <= 317000);
    }

    TEST_F(IFileDeleterPlugin_UnitTest, Execute_test)
    {
        std::filesystem::path file1 { "/tmp/test/delete_pqr.doc" };
        std::filesystem::path file2 { "/tmp/test/delete_2026-01-21T09:21:00Z_abc.doc" };

        bool continueExecution;
        m_fileDeleterPlugin->execute(file1, continueExecution);
        m_fileDeleterPlugin->execute(file2, continueExecution);

        sleep(1);

        ASSERT_NO_THROW(m_fileDeleterPlugin->shutdown());

        auto logs { m_logger->get_logged_messages() };
        ASSERT_EQ(logs.size(), 2);
        ASSERT_EQ(logs[0], "deleted (pqr.doc)");
        ASSERT_EQ(logs[1], "scheduled delete (abc.doc) on 2026-01-21T09:21:00Z");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, Scheduled_deletion)
    {
        auto fourSeconds = std::chrono::system_clock::now() + std::chrono::seconds(4);
        auto tt { std::chrono::system_clock::to_time_t(fourSeconds) };
        auto lt { std::localtime(&tt) };

        std::ostringstream timestamp;
        timestamp << std::put_time(lt, "%Y-%m-%dT%H:%M:%SZ");

        auto fpath { m_testingDirectory / (std::string("delete_") + timestamp.str() + "_pqr.txt") };

        bool bContinue;
        m_fileDeleterPlugin->execute(fpath, bContinue);

        sleep(5);

        auto logs { m_logger->get_logged_messages() };

        ASSERT_EQ(logs.size(), 2);
        ASSERT_EQ(logs[0], std::string("scheduled delete (pqr.txt) on ") + timestamp.str());
        ASSERT_EQ(logs[1], "deleted (pqr.txt)");
    }
}
