
#ifndef _ILOGGER_H_
#define _ILOGGER_H_

#include <filesystem>
#include <string>

namespace HotBackup
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

    public:
        virtual void init(const std::filesystem::path& logfilePath) = 0;
        virtual void log(const std::string logMessage) = 0;
    };
}

#endif  //  _ILOGGER_H_
