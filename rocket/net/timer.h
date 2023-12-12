#ifndef ROCKET_NET_TIMER_H
#define ROCKET_NET_TIMER_H

#include<map>
#include "/home/xxxj/rpc/rocket/rocket/net/timer_event.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/net/fd_event.h"
#include "/home/xxxj/rpc/rocket/rocket/common/mutex.h"

namespace rocket{
class Timer : public FdEvent{
public:
    Timer(); //需要和一个套接字绑定
    ~Timer();

    void addTimerEvent(TimerEvent::s_ptr event);
    void deleteTimerEvent(TimerEvent::s_ptr event);
    void onTimer(); //当发生io事件后，eventloop会执行这个函数

private:
    void resetArriveTime();

private:
    std::multimap<int64_t,TimerEvent::s_ptr> m_pending_events;
    Mutex m_mutex;
};



}


#endif