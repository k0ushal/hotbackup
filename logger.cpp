#include <sstream>
#include <iostream>
#include <iomanip>
#include "logger.h"

using HotBackup::Logger;

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::init(const std::filesystem::path& logfilePath)
{
    if (std::filesystem::is_directory(logfilePath))
    {
        std::ostringstream msg;
        msg << "Invalid log file path (" << logfilePath << ")";
        throw std::runtime_error(msg.str());
    }

    m_logfileStream = std::ofstream(logfilePath, std::ios_base::app);
    if (not m_logfileStream.good())
    {
        std::ostringstream msg;
        msg << "Unable to open log file (" << logfilePath << ")";
        throw std::runtime_error(msg.str());
    }
}

void Logger::log(const std::string logMessage)
{
    auto prefix { create_logmessage_prefix() };
    auto fullLogMsg { prefix + " " + logMessage };

    {
        std::unique_lock lock(m_mutex);
        m_logfileStream << fullLogMsg << std::endl;
        m_logfileStream.flush();
    }
}

std::string Logger::get_current_timestamp()
{
    //  timestamp
    auto currTime { std::chrono::system_clock::now() };
    auto tt { std::chrono::system_clock::to_time_t(currTime) };

    auto lt { std::localtime(&tt) };

    std::ostringstream timestamp;
    timestamp << std::put_time(lt, "%Y-%m-%dT%H:%M:%SZ");

    return timestamp.str();
}

std::string Logger::create_logmessage_prefix()
{
    return (get_current_timestamp() + " ::");
}
