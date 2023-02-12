#include <sstream>
#include <regex>
#include <vector>
#include <iostream>
#include <iomanip>
#include "logger.h"

using HotBackup::Logger;

void Logger::init(const std::filesystem::path& logfilePath)
{
    if (std::filesystem::is_directory(logfilePath))
    {
        std::ostringstream msg;
        msg << "Invalid log file path (" << logfilePath << ")";
        throw std::runtime_error(msg.str());
    }

    m_logfileStream = std::fstream(logfilePath, std::ios::in | std::ios::out);
    if (not m_logfileStream.good())
    {
        std::ostringstream msg;
        msg << "Unable to open log file (" << logfilePath << ")";
        throw std::runtime_error(msg.str());
    }

    load_previous_logs();
}

void Logger::shutdown()
{
    flush_logs_to_file();
}

void Logger::load_previous_logs()
{
    std::string line;
    while (std::getline(m_logfileStream, line))
        m_logs.push_back(line);
}

void Logger::flush_logs_to_file()
{
    if (not m_logfileStream.good())
        return;

    for (auto& line : m_logs)
        m_logfileStream << line << std::endl;

    m_logfileStream.flush();
}

void Logger::log(const std::string logMessage)
{
    auto prefix { create_logmessage_prefix() };
    auto fullLogMsg { prefix + " " + logMessage };

    {
        std::unique_lock lock(m_mutex);
        m_logs.push_back(fullLogMsg);
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

std::vector<std::string> Logger::search(const std::string regex)
{
    std::vector<std::string> result;
    std::regex pattern{ regex };

    {
        std::unique_lock lock(m_mutex);
        for (auto& line : m_logs)
        {
            if (std::regex_search(line, pattern))
                result.push_back(line);
        }
    }

    return result;
}
