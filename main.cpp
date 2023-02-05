#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <thread>
#include <unistd.h>
#include <string.h>
#include <sys/inotify.h>
#include "isubject.h"
#include "linux/file_notifier.h"
#include "linux/file_notifier_target.h"

using FileUtils::ISubject;
using FileUtils::FileNotifier;
using FileUtils::FileObserverSubject;

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

    std::cout << "Hot dir: " << hotDirectory.c_str() << ": " <<
        (std::filesystem::exists(hotDirectory) ? "Valid" : "Invalid") << std::endl;

    std::cout << "Backup dir: " << backupDirectory << std::endl;

    FileObserverSubject target;

    target.add_file(argv[1]);

    FileNotifier fwatch { std::vector<ISubject<int> *>{ target.get_handle() } };

    auto func = [&](ISubject<int>* subject) {
        
    };
    // fwatch.watch
#if 0
    //  POC
    int fdNotify = inotify_init();
    if (-1 == fdNotify)
    {
        std::cerr << "inotify_init(errcode: " << errno << ")...FAILED" << std::endl;
        return 0;
    }

    int watchFlags = IN_MODIFY | IN_CREATE;
    int wd = inotify_add_watch(fdNotify, hotDirectory.c_str(), watchFlags);
    if (-1 == wd)
    {
        std::cerr << "inotify_add_watch (errcode: " << errno << ")...FAILED." << std::endl;
        close(fdNotify);
        return 0;
    }

    struct inotify_event* pEvt;
    char eventList[150];

    int count = 0;
    while (true)
    {
        int len = 0;
        std::cout << "Calling read" << std::endl;
        len = read(fdNotify, eventList, 63);
        if (len < 0)
        {
            std::cerr << "read event (errcode: " << errno << "...FAILED." << std::endl;
            break;
        }
        std::cout << "Read " << len << " bytes" << std::endl;

        int i = 0;
        pEvt = reinterpret_cast<inotify_event *>(eventList);

        while (i < len)
        {
            pEvt = (inotify_event *)(reinterpret_cast<char *>(pEvt) + i);
            size_t recSize = sizeof(inotify_event) + pEvt->len;

            std::cout << "File: (" << pEvt->name << ")" << std::endl;
            std::cout << "File name len: " << pEvt->len << std::endl;
            if (pEvt->mask & IN_CREATE)
                std::cout << "  CREATED" << std::endl;
            if (pEvt->mask & IN_MODIFY)
                std::cout << "  MODIFIED" << std::endl;

            std::cout << std::endl;
            i += recSize;
            count++;
        }

        if (count > 1)
            break;
    }

    inotify_rm_watch(fdNotify, wd);
    close(fdNotify);

#endif

    return 0;
}
