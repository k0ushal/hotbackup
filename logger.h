
#include <filesystem>
#include <fstream>
#include <string>
#include <mutex>
#include "interfaces/ilogger.h"

namespace HotBackup
{
    class Logger : public ILogger
    {
    public:
        Logger();
        virtual ~Logger();
        virtual void init(const std::filesystem::path& logfilePath) override;
        virtual void log(const std::string logMessage) override;

    // protected:
        virtual std::string create_logmessage_prefix();
        virtual std::string get_current_timestamp();

    private:
        std::filesystem::path m_logfilePath;
        std::mutex m_mutex;
        std::ofstream m_logfileStream;
    };
}
