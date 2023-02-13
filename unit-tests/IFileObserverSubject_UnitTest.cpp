#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "../hotbackup_factory.h"

using HotBackup::HotBackupFactory;
using FileUtils::ISubject;
using FileUtils::FileEvents;

namespace HotBackup_UnitTests
{
    class IFileObserverSubject_UnitTest : public testing::Test
    {
    protected:
        IFileObserverSubject_UnitTest() :
            m_subject(HotBackupFactory::create_file_observer_subject("test-notifier"))
        {
            m_fileEventStrings = {"created", "modified"};

            std::filesystem::remove_all(m_testingDirectory);
            std::filesystem::create_directory(m_testingDirectory);

            m_subject->add_file(m_testingDirectory);
        }

        ~IFileObserverSubject_UnitTest()
        {
        }

        void SetUp() override
        {
            std::filesystem::create_directory(m_testingDirectory);
        }

        void TearDown() override
        {
            std::filesystem::remove_all(m_testingDirectory);
        }

    public:
        std::vector<std::string> m_fileEventStrings;
        const std::filesystem::path m_testingDirectory { "/tmp/testing" };
        std::shared_ptr<IFileObserverSubject<int>> m_subject;
    };

    TEST_F(IFileObserverSubject_UnitTest, SmokeTest)
    {
        std::ostringstream result;

        std::function<bool(std::filesystem::path fPath, FileEvents event)> getChangedFilesList = 
            [&](std::filesystem::path fPath, FileEvents event) -> bool {
                result << "(" << fPath.filename().c_str() << "-" << m_fileEventStrings[(int)event] << ")";

                return true;
            };

        //  Create files
        std::ofstream f1{m_testingDirectory / "f1"};
        std::ofstream f2{m_testingDirectory / "f2"};

        m_subject->get_changed_files_list(getChangedFilesList);

        ASSERT_TRUE(result.str() == "(f1-created)(f2-created)");

        result.str("");

        //  Modify a file
        f1 << "Hello";
        f1.flush();
        
        m_subject->get_changed_files_list(getChangedFilesList);
        ASSERT_TRUE(result.str() == "(f1-modified)");
    }
}
