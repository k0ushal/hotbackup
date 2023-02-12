#include <gtest/gtest.h>
#include <regex>
#include "../logger.h"

using HotBackup::Logger;

namespace HotBackup_UnitTests
{
    class Logger_UnitTest : public testing::Test
    {
    protected:
        Logger_UnitTest() :
            m_logger{ std::make_shared<Logger>() }
        {
        }

        ~Logger_UnitTest()
        {
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

    public:
        std::shared_ptr<Logger> m_logger;
        std::filesystem::path m_testLogFile { "/tmp/testlogs" };
    };

    TEST_F(Logger_UnitTest, SmokeTest)
    {
    }

    TEST_F(Logger_UnitTest, Search_Test)
    {
        std::vector<std::string> logs {
            "delete (/tmp/testing/abc.txt)",
            "modified (/tmp/testing/a1bc.txt)",
            "created (/tmp/testing/settings.json)",
            "delete (/tmp/testing/abdcae.txt)"
        };

        for (auto& log : logs)
            m_logger->log(log);

        //  find all operations performed on file named abc.
        {
            auto result { m_logger->search("abc") };
            ASSERT_EQ(result.size(), 1);
            ASSERT_TRUE(result[0].find(logs[0]) != std::string::npos);
        }

        //  find info about files with names starting with a,
        //  ending with c and only alphabets in the middle.
        {
            auto result { m_logger->search("a[a-zA-Z]+c") };
            ASSERT_EQ(result.size(), 2);
            ASSERT_TRUE(result[0].find(logs[0]) != std::string::npos);
            ASSERT_TRUE(result[1].find(logs[3]) != std::string::npos);
        }

        //  find logs from all json files.
        {
            auto result { m_logger->search(".*\\.json") };
            ASSERT_EQ(result.size(), 1);
            ASSERT_TRUE(result[0].find(logs[2]) != std::string::npos);
        }

        //  find deleted files.
        {
            auto result { m_logger->search("delete") };
            ASSERT_EQ(result.size(), 2);
            ASSERT_TRUE(result[0].find(logs[0]) != std::string::npos);
            ASSERT_TRUE(result[1].find(logs[3]) != std::string::npos);
        }
    }

    TEST_F(Logger_UnitTest, Invalid_Regex_format_throws_exception)
    {
        ASSERT_THROW(m_logger->search("[a-b[filename"), std::regex_error);
    }

    TEST_F(Logger_UnitTest, Regex_mismatch_returns_empty)
    {
        std::vector<std::string> logs {
            "delete (/tmp/testing/abc.txt)",
            "modified (/tmp/testing/a1bc.txt)",
            "created (/tmp/testing/settings.json)",
            "delete (/tmp/testing/abdcae.txt)"
        };

        for (auto& log : logs)
            m_logger->log(log);

        {
            auto result { m_logger->search("created.*\\.txt") };
            ASSERT_TRUE(result.empty());
        }

        {
            auto result { m_logger->search("created.*\\.json") };
            ASSERT_EQ(result.size(), 1);
            ASSERT_TRUE(result[0].find(logs[2]));
        }
    }
}

