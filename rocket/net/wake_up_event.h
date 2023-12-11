#ifndef ROCKET_NET_WAKEUP_EVENT_H
#define ROCKET_NET_WAKEUP_EVENT_H

#include "/home/xxxj/rpc/rocket/rocket/net/fd_event.h"


//针对wakefd，所以只关心这个文件描描述符上面的可读事件
namespace rocket{

class WakeUpFdEvent : public FdEvent{
public:
    WakeUpFdEvent(int fd);  
    ~WakeUpFdEvent();

    // void init();

    void wakeup();
private:
    



};

};


#endif