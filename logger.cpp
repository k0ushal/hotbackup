#include <sstream>
#include <regex>
#include <vector>
#include <iostream>
#include <iomanip>
#include "logger.h"

using HotBackup::Logger;

void Logger::init(const std::filesystem::path& logfilePath)
{
    m_logfilePath = logfilePath;

    if (std::filesystem::is_directory(m_logfilePath))
    {
        std::ostringstream msg;
        msg << "Invalid log file path (" << logfilePath << ")";
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

    auto fStream { std::ifstream(m_logfilePath) };
    if (not fStream.good())
        return;

    while (std::getline(fStream, line))
        m_logs.push_back(line);
}

void Logger::flush_logs_to_file()
{
    auto fStream { std::ofstream(m_logfilePath) };

    for (auto& line : m_logs)
        fStream << line << std::endl;

    fStream.flush();
    fStream.close();
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
