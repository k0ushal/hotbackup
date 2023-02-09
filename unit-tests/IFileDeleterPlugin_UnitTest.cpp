
#include <gtest/gtest.h>
#include <chrono>
#include "IFile_Deleter_Plugin_Mock.h"

using BackupManagement::IFileBackupPlugin;
using HotBackup_UnitTests::IFileDeleterPlugin_Mock;

using namespace std::chrono_literals;

namespace HotBackup_UnitTests
{
    class IFileDeleterPlugin_UnitTest : public testing::Test
    {
    protected:
        IFileDeleterPlugin_UnitTest() :
            m_fileDeleterPlugin { std::make_shared<IFileDeleterPlugin_Mock>() }
        {
        }

        ~IFileDeleterPlugin_UnitTest()
        {
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

    public:
        std::shared_ptr<IFileDeleterPlugin_Mock> m_fileDeleterPlugin;
    };

    TEST_F(IFileDeleterPlugin_UnitTest, SmokeTest)
    {
        auto [filename, deletionTime] = m_fileDeleterPlugin->process_file_name("/tmp/test/delete_abc.txt");
        ASSERT_EQ(filename, "abc");
        ASSERT_EQ(deletionTime, "");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, Multiple_Extension_Filename)
    {
        auto [filename, deletionTime] = m_fileDeleterPlugin->process_file_name("/tmp/test/delete_abc.txt.doc");
        ASSERT_EQ(filename, "abc.txt");
        ASSERT_EQ(deletionTime, "");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, File_With_ISO_timestamp)
    {
        std::filesystem::path filepath { "/tmp/test/delete_2022-01-21T09:21:00Z_abc.doc" };
        auto [filename, deletionTime] = m_fileDeleterPlugin->process_file_name(filepath);
        ASSERT_EQ(filename, "abc");
        ASSERT_EQ(deletionTime, "2022-01-21T09:21:00Z");
    }

    TEST_F(IFileDeleterPlugin_UnitTest, ISO_timestamp_to_waiting_duration)
    {
        auto fiveMins17secFromNow = std::chrono::system_clock::now() + std::chrono::minutes(5) + std::chrono::seconds(17);
        auto tt { std::chrono::system_clock::to_time_t(fiveMins17secFromNow) };
        auto lt { std::localtime(&tt) };

        std::ostringstream timestamp;
        timestamp << std::put_time(lt, "%Y-%m-%dT%H:%M:%SZ");

        auto waitingTime { m_fileDeleterPlugin->get_waiting_time_before_deletion(timestamp.str()) };

        //  Account for 1 sec tolerance.
        ASSERT_TRUE(waitingTime.count() >= 316 && waitingTime.count() <= 317);
    }
}
