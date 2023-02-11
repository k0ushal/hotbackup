
#ifndef _ISUBJECT_H_
#define _ISUBJECT_H_

namespace FileUtils
{
    //  [TODO] This interface enables an object to
    //  return a unique identifier which can be
    //  used to track runtime changes occurring
    //  on that object.
    template<typename T>
    class ISubject
    {
    public:
        virtual ~ISubject() = default;

    public:
        virtual T get_handle() = 0;

        virtual std::string name() = 0;
    };
}

#endif  //  _ISUBJECT_H_
