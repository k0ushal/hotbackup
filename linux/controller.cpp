#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "controller.h"

using FileUtils::Controller;

Controller::Controller()
{
    int pipeFds[2];
    auto ret { pipe2(pipeFds, O_DIRECT) };
    if (0 != ret)
    {
        std::ostringstream msg;
        msg << "signal_event:: write(errcode: " << errno << ") FAILED.";
        throw std::runtime_error(msg.str().c_str());
    }

    m_fdReadController = pipeFds[0];
    m_fdWriteController = pipeFds[1];
}

Controller::~Controller()
{
    close(m_fdWriteController);
    close(m_fdReadController);
}

void Controller::signal_event(const std::string& eventMsg)
{
    auto len { write(m_fdWriteController, eventMsg.c_str(), eventMsg.length()) };
    if (static_cast<ssize_t>(eventMsg.length()) != len)
    {
        std::ostringstream msg;
        msg << "signal_event:: write(errcode: " << errno << ") FAILED.";
        throw std::runtime_error(msg.str().c_str());
    }
}

std::string Controller::read_event()
{
    std::string buff(20, '\0');

    auto len { read(m_fdReadController, buff.data(), buff.size() - 1) };
    if (len < 0)
    {
        std::cout << "Controller::read_event(errcode: " << errno << ") FAILED." << std::endl;
        return "";
    }

    return buff;
}

//  ISubject methods
int Controller::get_handle()
{
    return m_fdReadController;
}
