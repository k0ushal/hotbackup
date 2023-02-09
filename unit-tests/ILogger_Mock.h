#ifndef _ILOGGER_MOCK_H_
#define _ILOGGER_MOCK_H_

#include <vector>
#include <string>
#include "../logger.h"

using HotBackup::Logger;

namespace HotBackup_Mocks
{
    class ILogger_Mock : public Logger
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
            auto logMsg { this->create_logmessage_prefix() + " " + logMessage };
            m_loggedMsgs.push_back(logMsg);
        }

        std::vector<std::string> get_logged_messages()
        {
            return m_loggedMsgs;
        }

    private:
        std::vector<std::string> m_loggedMsgs;
    };
}

#endif  //  _ILOGGER_MOCK_H_
