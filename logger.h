
#include <filesystem>
#include <fstream>
#include <string>
#include <mutex>
#include "interfaces/ilogger.h"
#include "interfaces/ilog_view.h"

namespace HotBackup
{
    class Logger : public ILogger, public ILogView
    {
    public:
        Logger(const Logger&) = delete;
        Logger& operator =(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator =(Logger&&) = delete;

        Logger() = default;
        virtual ~Logger() = default;

    public:
        virtual void init(const std::filesystem::path& logfilePath) override;
        virtual void log(const std::string logMessage) override;
        virtual void shutdown() override;
        virtual std::vector<std::string> search(const std::string regex) override;

    protected:
        virtual void load_previous_logs();
        virtual void flush_logs_to_file();
        virtual std::string create_logmessage_prefix();
        virtual std::string get_current_timestamp();

    private:
        std::vector<std::string> m_logs;
        std::filesystem::path m_logfilePath;
        std::mutex m_mutex;
    };
}
