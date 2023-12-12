#include "/home/xxxj/rpc/rocket/rocket/net/timer_event.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"

namespace rocket{
TimerEvent::TimerEvent(int interval,bool is_repeated,std::function<void()>cb)
:m_interval(interval),m_is_repeated(is_repeated),m_task(cb) {
    // m_arrive_time = getNowMs() + m_interval; //获取当前的ms数+间隔时间
    resetArriveTime();
    // DEBUGLOG("success create timer event,will excute at [%lld]",m_arrive_time);
}

void TimerEvent::resetArriveTime(){
    m_arrive_time = getNowMs() + m_interval;
    DEBUGLOG("success create timer event,will excute at [%lld]",m_arrive_time); //我觉得这里不应该加呢！
}


}