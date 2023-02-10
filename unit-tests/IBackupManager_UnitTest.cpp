#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "../ibackup_manager.h"
#include "../hotbackup_factory.h"
#include "../ilogger.h"
#include "ILogger_Mock.h"

using HotBackup::HotBackupFactory;
using HotBackup::ILogger;
using BackupManagement::IBackupManager;
using HotBackup_Mocks::ILogger_Mock;

namespace HotBackup_UnitTests
{
    class IBackupManager_UnitTest : public testing::Test
    {
    protected:
        IBackupManager_UnitTest() :
            m_queue { HotBackupFactory::create_file_backup_queue() },
            m_backupManager { HotBackupFactory::create_backup_manager() },
            m_logger { std::make_shared<ILogger_Mock>() }
        {
            m_backupManager->init(m_backupDirectory, std::dynamic_pointer_cast<IFileBackupQueueConsumer>(m_queue), m_logger);
        }

        ~IBackupManager_UnitTest()
        {
        }

        void SetUp() override
        {
            std::filesystem::create_directory(m_backupDirectory);
            std::filesystem::create_directory(m_testingDirectory);
        }

        void TearDown() override
        {
            std::filesystem::remove_all(m_testingDirectory);
            std::filesystem::remove_all(m_backupDirectory);
        }

        std::filesystem::path create_file(std::string fname, const std::string& data)
        {
            auto fpath { m_testingDirectory / fname };
            std::ofstream f1 { fpath };

            if (not data.empty())
            {
                f1 << data;
                f1.flush();
            }

            return fpath;
        }

    public:
        std::shared_ptr<IFileBackupQueue> m_queue;
        std::shared_ptr<IBackupManager> m_backupManager;
        std::shared_ptr<ILogger_Mock> m_logger;
        std::filesystem::path m_backupDirectory { "/tmp/backup" };
        std::filesystem::path m_testingDirectory { "/tmp/testing" };
    };

    TEST_F(IBackupManager_UnitTest, SmokeTest)
    {
        m_backupManager->start_workers(1);

        auto f1 { create_file("abc", "some text") };
        m_queue->push_item(f1);

        sleep(1);

        std::string expectedSuccessMsg { std::string("SUCCESS:: backed up file (") + f1.c_str() + ")" };
        ASSERT_TRUE(m_logger->get_logged_messages()[0].find(expectedSuccessMsg));

        m_backupManager->stop_workers();
        m_queue->shutdown();
    }

    TEST_F(IBackupManager_UnitTest, Multiple_Workers_Verify_FileData)
    {
        m_backupManager->start_workers(3);

        std::vector<std::string> filenames { "abc", "abd", "abe", "abf", "abg", "abh" };
        std::vector<std::filesystem::path> filepaths;

        //  create files
        for (auto& file : filenames)
        {
            auto f { create_file(file, file) };
            filepaths.push_back(f);
        }

        //  push them all on queue
        for (auto& filepath : filepaths)
            m_queue->push_item(filepath);

        //  wait for threads to finish processing.
        sleep(3);

        //  Extract filenames from log messages.
        std::regex pattern { "backedup \\(\"/tmp/testing/([a-z]+)\"\\)" };
        std::smatch match;
        std::set<std::string> backedUpFiles;

        auto logs { m_logger->get_logged_messages() };
        for (auto& log : logs)
        {
            auto ret { std::regex_match(log, match, pattern) };
            ASSERT_TRUE(ret);

            backedUpFiles.emplace(match[1]);
        }

        //  Check that all files have been backed up
        for (auto& f : filenames)
            ASSERT_TRUE(backedUpFiles.count(f) > 0);

        //  Verify file data
        for (auto& filename : filenames)
        {
            auto fpath { m_backupDirectory / filename };
            fpath += ".bak";

            std::ifstream fStream { fpath };
            std::string fileData;

            fStream >> fileData;
            ASSERT_EQ(filename, fileData);
        }

        m_backupManager->stop_workers();
        m_queue->shutdown();
    }

    TEST_F(IBackupManager_UnitTest, Deleter_Plugin_SmokeTest)
    {
        // auto plugin { HotBackupFactory::create_deleter_plugin() };
        // m_backupManager->add_plugin(plugin);
    }
}
