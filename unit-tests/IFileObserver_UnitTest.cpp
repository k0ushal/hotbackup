#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <thread>
#include "../hotbackup_factory.h"

using HotBackup::HotBackupFactory;
using FileUtils::IFileObserver;
using FileUtils::ISubject;
using FileUtils::FileEvents;

namespace HotBackup_UnitTests
{
    class IFileObserver_UnitTest : public testing::Test
    {
    protected:
        IFileObserver_UnitTest()
        {
            std::filesystem::create_directory(m_testingDirectory);

            m_subject = HotBackupFactory::create_file_observer_subject("notifier");
            m_subject->add_file(m_testingDirectory);
            m_observer = HotBackupFactory::create_file_observer(-1);

            m_eventStrings.push_back("created");
            m_eventStrings.push_back("modified");

            m_observer->add_subject(m_subject);
        }

        ~IFileObserver_UnitTest()
        {
            std::filesystem::remove_all(m_testingDirectory);
        }

        void SetUp() override
        {
        }

        void TearDown() override
        {
        }

    public:
        const std::filesystem::path m_testingDirectory { "/tmp/testing" };
        std::shared_ptr<IFileObserver<int>> m_observer;
        std::shared_ptr<IFileObserverSubject<int>> m_subject;
        std::vector<std::string> m_eventStrings;
    };

    TEST_F(IFileObserver_UnitTest, SmokeTest)
    {
        std::vector<std::string> result;

        std::function<bool(std::filesystem::path, FileEvents)> getChangedFilesList = 
            [&](std::filesystem::path fPath, FileEvents event) -> bool {

                result.push_back(fPath.filename().u8string() + "-" + m_eventStrings[(int)event]);
                return true;
            };

        std::function<bool(std::shared_ptr<ISubject<int>>)> observerCallback = 
        [&](std::shared_ptr<ISubject<int>> subject) -> bool {

            auto fileSubject { std::dynamic_pointer_cast<IFileObserverSubject<int>>(subject) };
            fileSubject->get_changed_files_list(getChangedFilesList);
            return true;
        };

        m_observer->start_observer(observerCallback);

        //  Create 2 files
        std::ofstream f1 {m_testingDirectory / "file1"};
        std::ofstream f2 {m_testingDirectory / "file2"};

        sleep(1);
        ASSERT_EQ(result[0], "file1-created");
        ASSERT_EQ(result[1], "file2-created");

        //  Create a 3rd file and modify an old file
        std::ofstream f3{m_testingDirectory / "file3"};

        f2 << "lkjhl";
        f2.flush();

        sleep(1);
        ASSERT_EQ(result[2], "file3-created");
        ASSERT_EQ(result[3], "file2-modified");

        m_observer->stop_observer();
    }
}
