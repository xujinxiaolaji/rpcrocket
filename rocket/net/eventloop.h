#ifndef ROCKTE_NET_EVENTLOOP_H
#define ROCKTE_NET_EVENTLOOP_H

#include<pthread.h>
#include<set>
#include<functional>
#include<queue>
#include<sys/eventfd.h>
#include "/home/xxxj/rpc/rocket/rocket/common/mutex.h"
#include "/home/xxxj/rpc/rocket/rocket/net/fd_event.h"
#include "/home/xxxj/rpc/rocket/rocket/net/wake_up_event.h"
#include "/home/xxxj/rpc/rocket/rocket/net/timer.h"

//每个线程只能有一个EventLoop，需要需要把线程id记录下来
namespace rocket{

    class EventLoop{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        void wakeup();
        void stop();

        void addEpollEvent(FdEvent* event);
        void deleteEpollEvent(FdEvent* event); //传一个FdEvent类型,但是如何和epollfd结合起来？
        
        bool isInLoopThread(); //是否是当前线程添加，如果不是的话 存在条件竞争

        void addTask(std::function<void()> cb,bool is_wake_up = false);
        
        void addTimerEvent(TimerEvent::s_ptr event);

    public:
        static EventLoop* GetCurrentEventLoop();
    private:
        //处理wakeup的函数
        void dealWakeup();

        void initWakeUpFdEvent();

        void initTimer();
    private:
        pid_t m_thread_id {0};
        int m_epoll_fds{0};
        int m_wakeup_fd {0}; //用于唤醒epoll_wait

        WakeUpFdEvent* m_wake_fd_event{NULL};
 
        bool m_stop_flag{false};
        std::set<int>m_listen_fds;
        std::queue<std::function<void()>> m_pending_tasks; //所有待执行的队列!!!vocal,新世界
        Mutex m_mutex;

        Timer* m_timer{NULL};
    };


};

#endif