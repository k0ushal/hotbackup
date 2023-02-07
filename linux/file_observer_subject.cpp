#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <exception>
#include <unistd.h>
#include "file_observer_subject.h"

using FileUtils::FileObserverSubject;
using FileUtils::FileEvents;

FileObserverSubject::FileObserverSubject(std::string name)
    : m_name(name)
{
    m_notifyFd = inotify_init1(IN_NONBLOCK);
    if (-1 == m_notifyFd)
    {
        std::ostringstream msg;
        msg << "inotify_init(errcode: " << errno << ") FAILED";
        throw std::runtime_error(msg.str().c_str());
    }
}

FileObserverSubject::~FileObserverSubject()
{
    close(m_notifyFd);
    m_notifyFd = -99;
}

void FileObserverSubject::add_file(const std::filesystem::path& targetFile)
{
    auto filePath {std::filesystem::canonical(targetFile)};

    if (m_filepathToWatchIndex.count(filePath.u8string()) > 0)
        return;

    int wd = inotify_add_watch(m_notifyFd, filePath.c_str(), IN_MODIFY | IN_CREATE);
    if (-1 == wd)
    {
        std::ostringstream msg;
        msg << "inotify_add_watch (" << filePath << ")(errcode: " << errno << ") FAILED.";
        throw std::runtime_error(msg.str().c_str());
    }

    m_filepathToWatchIndex[filePath] = wd;
    m_watchToFilepathIndex[wd] = filePath;
}

void FileObserverSubject::remove_file(const std::filesystem::path& targetFile)
{
    auto filePath {std::filesystem::canonical(targetFile)};

    if (0 == m_filepathToWatchIndex.count(filePath))
        return;

    auto wd = m_filepathToWatchIndex[filePath.u8string()];

    auto ret = inotify_rm_watch(m_notifyFd, wd);
    if (-1 == ret)
    {
        std::ostringstream msg;
        msg << "inotify_rm_watch (" << filePath << ")(errcode: " << errno << ") FAILED.";
        throw std::runtime_error(msg.str().c_str());
    }

    m_filepathToWatchIndex.erase(filePath);
    m_watchToFilepathIndex.erase(wd);
}

//  This is a non-blocking call.
void FileObserverSubject::get_changed_files_list(
    std::function<bool(std::filesystem::path, FileEvents)>& callback
)
{
    const int RECORD_SIZE { sizeof(inotify_event) + 16 };
    const int BUFF_SIZE { RECORD_SIZE * 50 };

    //  Leveraging the memory allocation and cleanup
    //  features of the vector class to create a buffer.
    std::vector<char> buff(BUFF_SIZE);

    while (true)
    {
        auto readLen = read(m_notifyFd, buff.data(), buff.size());
        if (-1 == readLen)
        {
            if (EAGAIN == errno)
                break;

            std::ostringstream msg;
            msg << "read (notifyFd: " << m_notifyFd << ")(errcode: " << errno << ") FAILED.";
            throw std::runtime_error(msg.str().c_str());
        }

        inotify_event* pEvt;
        bool continueEnumeration {true};
        ssize_t bytesProcessed {0};

        do
        {
            pEvt = reinterpret_cast<inotify_event *>(buff.data() + bytesProcessed);

            #if IGNORE_DIRECTORIES
            //  Ignore directories
            if (IN_ISDIR & pEvt->mask)
            {
                bytesProcessed += (sizeof(inotify_event) + pEvt->len);
                continue;
            }
            #endif

            FileEvents event;
            std::filesystem::path filePath {m_watchToFilepathIndex[pEvt->wd]};

            if (pEvt->len)
                filePath /= pEvt->name;

            if (IN_MODIFY & pEvt->mask)
                event = FileEvents::FE_MODIFIED;
            else if (IN_CREATE & pEvt->mask)
                event = FileEvents::FE_CREATED;

            continueEnumeration = callback(filePath, event);
            bytesProcessed += (sizeof(inotify_event) + pEvt->len);

        } while (continueEnumeration && bytesProcessed < readLen);
    }
}
