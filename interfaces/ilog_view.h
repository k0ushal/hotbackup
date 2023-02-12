
#ifndef _ILOG_VIEW_H_
#define _ILOG_VIEW_H_

#include <filesystem>
#include <string>

namespace HotBackup
{
    class ILogView
    {
    public:
        virtual ~ILogView() = default;

    public:
        virtual std::vector<std::string> search(const std::string regex) = 0;
    };
}

#endif  //  _ILOG_VIEW_H_
