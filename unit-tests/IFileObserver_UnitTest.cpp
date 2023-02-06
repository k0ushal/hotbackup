#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
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
        IFileObserver_UnitTest() :
            m_observer(HotBackupFactory::create_file_observer(-1)),
            m_subject(HotBackupFactory::create_file_observer_subject())
        {
            std::filesystem::create_directory(m_testingDirectory);
            m_subject->add_file(m_testingDirectory);

            m_observer->add_subject(m_subject.get());
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
    };

    TEST_F(IFileObserver_UnitTest, SmokeTest)
    {
        std::ostringstream result;

        std::function<bool(std::filesystem::path fPath, FileEvents event)> getChangedFilesList = 
            [&](std::filesystem::path fPath, FileEvents event) -> bool {
                result << "(" << fPath.filename() << ")";
            };

        auto observerCallback = [&](ISubject<int>* subject) -> bool {
            if (subject != m_subject.get())
            {
                result << "Invalid subject pointer received";
                return true;
            }

            m_subject->get_changed_files_list(getChangedFilesList);
        };

        m_observer->start_observer(observerCallback);

        std::ofstream{m_testingDirectory / "file1"}.put('z');
        std::ofstream{m_testingDirectory / "file2"}.put('B');
    }
}
