#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <string>
#include "../interfaces/isubject.h"

using FileUtils::ISubject;

namespace FileUtils
{
    //  Use this class to interrupt the File observer
    //  and pass control commands to the manager.
    //  Right now, it doesn't serve any control commands
    //  other than a terminate event. If the observer
    //  tells that this file is ready for reading,
    //  it indicates that we're terminating the hotbackup
    //  process.
    class Controller : public ISubject<int>
    {
    public:
        Controller(const Controller&) = delete;
        Controller& operator =(const Controller&) = delete;
        Controller(Controller&&) = delete;
        Controller& operator =(Controller&&) = delete;

    public:
        Controller();
        virtual ~Controller();

        void signal_event(const std::string& eventMsg);
        std::string read_event();

        //  ISubject methods
        virtual int get_handle() override;

        virtual std::string name() override;

    private:
        int m_fdReadController;
        int m_fdWriteController;
    };
}

#endif  //  _CONTROLLER_H_
