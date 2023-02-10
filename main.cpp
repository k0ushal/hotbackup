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
#include "HotBackupApp.h"

using FileUtils::ISubject;
using FileUtils::IFileObserver;
using FileUtils::IFileObserverSubject;
using FileUtils::FileEvents;
using HotBackup::HotBackupFactory;
using HotBackup::HotBackupApp;
using HotBackup::ILogger;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "hotbackup <hot-folder-path> <backup-folder-path>" << std::endl;
        return 0;
    }

    std::filesystem::path hotDirectory(argv[1]);
    std::filesystem::path backupDirectory(argv[2]);

    HotBackupApp app(hotDirectory, backupDirectory);

    try
    {
        app.init();
        app.start_backup_workers();
        app.start_directory_listeners();

        while (true)
        {
            std::cout << "Waiting for input" << std::endl;
            std::string input;
            std::cin >> input;

            if (0 == input.find("exit"))
            {
                std::cout << "Terminating..." << std::endl;
                break;
            }
        }

        app.shutdown();
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