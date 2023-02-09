#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include "isubject.h"
#include "ifile_observer.h"
#include "ifile_observer_subject.h"
#include "hotbackup_factory.h"
#include "ilogger.h"

using FileUtils::ISubject;
using FileUtils::IFileObserver;
using FileUtils::IFileObserverSubject;
using FileUtils::FileEvents;
using HotBackup::HotBackupFactory;
using HotBackup::ILogger;

int main(int argc, char* argv[])
{
    #if 0
    auto logger { HotBackupFactory::create_logger() };
    logger->log("hello");
    return 0;
    #endif

    if (argc < 3)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "hotbackup <hot-folder-path> <backup-folder-path>" << std::endl;
        return 0;
    }

    const int infinitePolling { -1 };
    std::filesystem::path hotDirectory(argv[1]);
    std::filesystem::path backupDirectory(argv[2]);

    std::cout << "Hot dir: " << hotDirectory.c_str() << ": " <<
        (std::filesystem::exists(hotDirectory) ? "Valid" : "Invalid") << std::endl;

    std::cout << "Backup dir: " << backupDirectory << std::endl;

    try
    {
        auto fileObserverSubject { HotBackupFactory::create_file_observer_subject("file-notifier") };
        auto fileObserver { HotBackupFactory::create_file_observer(infinitePolling) };

        fileObserverSubject->add_file(hotDirectory);

        fileObserver->add_subject(fileObserverSubject);

        std::function<bool(std::filesystem::path, FileEvents)> printChangedFiles = 
        [&](std::filesystem::path filePath, FileEvents events) -> bool {

            std::vector<std::string> eventNames { "Created", "Modified" };

            std::cout << eventNames[(int)events] << ": " << filePath << std::endl;

            return true;
        };

        auto watcherFunc = [&](std::shared_ptr<ISubject<int>> subject) -> bool {

            if (fileObserverSubject == subject)
            {
                fileObserverSubject->get_changed_files_list(printChangedFiles);
            }

            return true;
        };

        fileObserver->start_observer(watcherFunc);

        while (true)
        {
            std::cout << "Waiting for input" << std::endl;
            std::string input;
            std::cin >> input;

            if (0 == input.find("exit"))
            {
                std::cout << "Terminating..." << std::endl;
                fileObserver->stop_observer();
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

//  [TODO]
//  Add virtual destructors to all interfaces
//  Replace const & with movable objects.
//  Remove unnecessary copy constructors and move operators
//  Change cout to cerr wherever applicable
//  Assumption: class private/protected functions will trust all input args and not validate them.