#ifndef _ILOGGER_MOCK_H_
#define _ILOGGER_MOCK_H_

#include <vector>
#include <string>
#include <mutex>
#include "../ilogger.h"

using HotBackup::ILogger;

namespace HotBackup_Mocks
{
    class ILogger_Mock : public ILogger
    {
    public:
        virtual ~ILogger_Mock()
        {
        }

        void init(const std::filesystem::path& logfilePath) override
        {
        }

        void log(const std::string logMessage) override
        {
            {
                std::unique_lock lock(m_mutex);
                m_loggedMsgs.push_back(logMessage);
            }
        }

        std::vector<std::string> get_logged_messages()
        {
            return m_loggedMsgs;
        }

        void clear_logs()
        {
            m_loggedMsgs.clear();
        }

    private:
        std::vector<std::string> m_loggedMsgs;
        std::mutex m_mutex;
    };
}

#endif  //  _ILOGGER_MOCK_H_
