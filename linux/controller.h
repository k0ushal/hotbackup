#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <string>
#include "../isubject.h"

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
        Controller();
        ~Controller();

        void signal_event(const std::string& eventMsg);
        std::string read_event();

        //  ISubject methods
        virtual int get_handle() override;

    private:
        int m_fdReadController;
        int m_fdWriteController;
    };
}

#endif  //  _CONTROLLER_H_
