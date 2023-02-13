#ifndef _MENU_COMMANDS_H_
#define _MENU_COMMANDS_H_

#include <string>

namespace HotBackup
{
    class MenuCommands
    {
    public:
        MenuCommands() = default;
        virtual ~MenuCommands() = default;

    public:
        void show_command_options();
        std::string get_user_command();
        void init_commands_handler();
    };
}

#endif  //  _MENU_COMMANDS_H_
