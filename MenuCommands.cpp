
#include <iostream>
#include "MenuCommands.h"

using HotBackup::MenuCommands;

void MenuCommands::show_command_options()
{
    std::cout << "M E N U" << std::endl;
    std::cout << "ls: Prints all logs" << std::endl;
    std::cout << "lf <start-date-ISO-format> <end-date-ISO-format>: Print logs filtered by date" << std::endl;
    std::cout << "lf <filename-regex>: Print logs filtered by filename regex" << std::endl;
    std::cout << "q: Terminates the app" << std::endl;
    std::cout << "Enter your choice: ";
}

std::string MenuCommands::get_user_command()
{
    std::string cmd;
    std::cin >> cmd;
    return cmd;
}

void MenuCommands::init_commands_handler()
{
    
}
