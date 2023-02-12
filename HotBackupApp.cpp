
#include <iostream>
#include <exception>
#include <filesystem>
#include "HotBackupApp.h"
#include "interfaces/ifile_backup_queue.h"
#include "interfaces/ifile_notifier.h"
#include "hotbackup_factory.h"

using HotBackup::HotBackupApp;
using HotBackup::HotBackupFactory;
using FileUtils::ISubject;
using FileUtils::FileEvents;

HotBackupApp::HotBackupApp(
    const std::filesystem::path& hotDirectory,
    std::filesystem::path backupDirectory
    ) :
    m_hotDirectory(hotDirectory),
    m_backupDirectory(backupDirectory)
{
    if (not std::filesystem::exists(m_hotDirectory) || not std::filesystem::is_directory(m_hotDirectory))
        throw std::runtime_error("Hot directory path doesn't exist or is not a directory");

    if (not std::filesystem::exists(m_backupDirectory) && false == std::filesystem::create_directories(m_backupDirectory))
        throw std::runtime_error("Error creating backup directory");
}

void HotBackupApp::init()
{
    m_logger = HotBackupFactory::create_logger();
    m_logger->init("./audit.log");

    m_queue = HotBackupFactory::create_file_backup_queue();

    m_backupManager = HotBackupFactory::create_backup_manager();
    m_fileDeleterBackupPlugin = HotBackupFactory::create_deleter_plugin();

    m_fileObserverSubject = HotBackupFactory::create_file_observer_subject("notifier");
    m_fileObserverSubject->add_file(m_hotDirectory);
    m_fileObserver = HotBackupFactory::create_file_observer(-1);
    m_fileObserver->add_subject(m_fileObserverSubject);
}

void HotBackupApp::start_directory_listeners()
{
    //  Changed files handler.
    static std::function<bool(std::filesystem::path, FileEvents)> changedFilesHandler =
    [&](std::filesystem::path filePath, FileEvents events) -> bool {

        std::ostringstream msg;
        std::vector<std::string> eventNames { "created", "modified" };

        msg << eventNames[(int)events] << " :: " << filePath.filename();
        m_logger->log(msg.str());

        m_queue->push_item(filePath);
        return true;
    };

    //  Observer function. Called when any of the subjects
    //  are signaled i.e. ready to read from.
    static std::function<bool(std::shared_ptr<ISubject<int>>)> observerFunc =
    [&](std::shared_ptr<ISubject<int>> subject) -> bool {

        if (m_fileObserverSubject == subject)
        {
            m_fileObserverSubject->get_changed_files_list(changedFilesHandler);
        }

        return true;
    };

    m_fileObserver->start_observer(observerFunc);
}

void HotBackupApp::start_backup_workers()
{
    m_backupManager->init(
        m_backupDirectory,
        std::dynamic_pointer_cast<IFileBackupQueueConsumer>(m_queue),
        m_logger);

    m_backupManager->add_plugin(m_fileDeleterBackupPlugin);
    m_backupManager->start_workers(m_backupWorkersCount);
}

void HotBackupApp::shutdown()
{
    m_shutdown = true;
    m_fileObserver->stop_observer();
    m_backupManager->stop_workers();
}
