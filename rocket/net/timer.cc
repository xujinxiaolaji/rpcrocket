#include<sys/timerfd.h>
#include<string.h>
#include<vector>
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"
#include "/home/xxxj/rpc/rocket/rocket/net/timer.h"
#include "/home/xxxj/rpc/rocket/rocket/net/timer_event.h"

namespace rocket{
Timer::Timer() : FdEvent(){  //再构造函数之前，需要先构造父类
    m_fd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    DEBUGLOG("timer fd = %d",m_fd);
    //把fd的可读事件放到了eventloop上监听
    listen(FdEvent::IN_EVENT,std::bind(&Timer::onTimer,this)); //把onTimer绑定到fd的回调函数
}

Timer::~Timer(){
}

void Timer::onTimer() {
    //处理缓冲区数据，防止下一次触发可读事件
    char buf[8];
    while(1){
        if((read(m_fd,buf,8) == -1) && (errno == EAGAIN)) {
            break;
        }
    }

    //执行定时任务,其实没有执行，只是存起了
    int64_t now = getNowMs();
    std::vector<TimerEvent::s_ptr>tmps;
    std::vector<std::pair<int64_t,std::function<void()>>> task;

    ScopeMutex<Mutex>lock(m_mutex);
    auto it = m_pending_events.begin();
    
    for(it;it!=m_pending_events.end();it++){
        if((*it).first <= now ) {
            if(!(*it).second->isCanceled()){
                tmps.push_back((*it).second);
                task.push_back(std::make_pair((*it).second->getArriveTime(),(*it).second->getCallBack()));
            }
        }else{
           break; 
        }
    }
    m_pending_events.erase(m_pending_events.begin(),it);
    lock.unlock(); //而且解锁在这里解锁吗？

    //需要把重复(repeat)的Event 再添加进去,需要重新调整一下时间
    for(auto it = tmps.begin();it!=tmps.end();++it){
        if((*it)->isRepeated()){
            //调整arriveTime
            (*it)->resetArriveTime();
            addTimerEvent(*it);
        }
    }
    resetArriveTime();
//执行定时任务,为什么直接执行，是因为这些任务都是到时间的定时任务
    for(auto i : task) {
        if(i.second){
            i.second();
        }
    }
}

void Timer::resetArriveTime(){
    ScopeMutex<Mutex>lock(m_mutex);
    auto tmp = m_pending_events;
    lock.unlock();
    if(tmp.size() == 0) {
        return;
    }

    int64_t now = getNowMs();
    auto it = tmp.begin();
    int64_t interval = 0;
    if(it->second->getArriveTime() > now){
        interval = it->second->getArriveTime() - now;
    }else{
        interval = 100; //拯救过期任务
    }

    timespec ts;
    memset(&ts,0,sizeof(ts));
    ts.tv_sec = interval / 1000;
    ts.tv_nsec = (interval % 1000) * 1000000;

    itimerspec value;
    memset(&value,0,sizeof(value));
    value.it_value = ts;

    int rt = timerfd_settime(m_fd,0,&value,NULL);
    if(rt!=0){
        ERRORLOG("timerfd_settime error, errno = %d.error=%s",errno,strerror(errno));
    }
    DEBUGLOG("timer reset to %lld",now+interval);
}

void Timer::addTimerEvent(TimerEvent::s_ptr event) {
    bool is_reset_timerfd = false;
    ScopeMutex<Mutex> lock(m_mutex);//构造加锁，析构释放锁
    // lock.lock();
    //如果这个map是空的话
    if(m_pending_events.empty()){
        is_reset_timerfd = true;
    }else{
        auto it = m_pending_events.begin();
        if((*it).second->getArriveTime() > event->getArriveTime()){
            is_reset_timerfd = true;
        }
    }
    m_pending_events.emplace(event->getArriveTime(),event); //
    lock.unlock();

    if(is_reset_timerfd){
        resetArriveTime();
    }
}

void Timer::deleteTimerEvent(TimerEvent::s_ptr event) {
    event->setCancel(true);
    ScopeMutex<Mutex>lock(m_mutex);
    
    auto begin = m_pending_events.lower_bound(event->getArriveTime()); //用key找到一个集合列表
    auto end = m_pending_events.upper_bound(event->getArriveTime());
    auto it = begin;
    for(auto it = begin;it!=end;it++){
        if(it->second == event){
            break;
        }
    }
    if(it!=end){
        m_pending_events.erase(it);
    }
    lock.unlock();
    DEBUGLOG("success delete TimerEvent at arrive time %lld",event->getArriveTime());
}

}