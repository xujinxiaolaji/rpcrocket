#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include<string.h>
#include "/home/xxxj/rpc/rocket/rocket/net/eventloop.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"

//封装成一个宏
#define ADD_TO_EPOLL()\
    auto it = m_listen_fds.find(event->getFd());  \ 
    int op = EPOLL_CTL_ADD; \
    if(it != m_listen_fds.end()){ \
        op = EPOLL_CTL_MOD; \
    }                        \
    epoll_event temp = event->getEpollEvent();  \
    int rt = epoll_ctl(m_epoll_fds,op,event->getFd(),&temp); \
    if(rt == -1){                                                      \
        ERRORLOG("failed epoll_ctl when add fd %d,errno = %d,error = %s",event->getFd(),errno,strerror(errno)); \
    }\
    DEBUGLOG("add event success,fd[%d]",event->getFd())\

#define DELETE_TO_EPOLL()\
    auto it =  m_listen_fds.find(event->getFd()); \
    if(it == m_listen_fds.end()){    \
        return;              \
    }                       \
    int op = EPOLL_CTL_DEL;   \
    epoll_event temp = event->getEpollEvent();   \
    int rt = epoll_ctl(m_epoll_fds,op,event->getFd(),&temp);  \
    if(rt == -1){                \
        ERRORLOG("failed epoll_ctl when add fd %d,errno = %d,error = %s",event->getFd(),errno,strerror(errno)) \
    }   \
    DEBUGLOG("delete event success,fd[%d]",event->getFd());   \


namespace rocket{
    static thread_local EventLoop* t_current_eventloop = NULL;
    static int g_epoll_max_timeout = 10000;
    static int g_epoll_max_events = 10;

    EventLoop::EventLoop(){
        //创建eventloop之前需要先判断一下当前线 程是否已经创建了eventloop，如果创建了-->报错
        if(t_current_eventloop!=NULL){
            ERRORLOG("fail to create eventloop, this thread has created even loop");  //up这里写的是Error
            exit(0); //退出程序，exit(0)定义干净出口，没有任何错误； exit(1)定义程序中存在错误，并且程序终止。
        }
        m_thread_id = getThreadId(); //获取线程id
    
        m_epoll_fds = epoll_create(10);
        if(m_epoll_fds == -1){
            ERRORLOG("failed to create eventloop, epoll_create error,error info [%d]",errno);
            exit(0);
        }
        
        m_wakeup_fd = eventfd(0,EFD_NONBLOCK); //非阻塞的
        
        if(m_wakeup_fd < 0){
            ERRORLOG("failed to create eventloop,eventfd create error,error info [%d]",errno);
            exit(0);
        }

        initWakeUpFdEvent();
        initTimer();
 
        INFOLOG("successfully create eventloop in thread %d",m_thread_id);

        t_current_eventloop = this; //传this指针给这个静态变量,这不就赋值了吗
    }

    EventLoop::~EventLoop(){
        close(m_epoll_fds);
        if(m_wake_fd_event) {
            delete m_wake_fd_event;
            m_wake_fd_event = NULL;
        }
        if(m_timer){
            delete m_timer;
            m_timer = NULL;
        }
    }

    void EventLoop::initTimer() {
        m_timer = new Timer();
        addEpollEvent(m_timer);
    }

    void EventLoop::addTimerEvent(TimerEvent::s_ptr event) {
        m_timer->addTimerEvent(event);
    }

    void EventLoop::initWakeUpFdEvent(){
        m_wake_fd_event = new WakeUpFdEvent(m_wakeup_fd);

        m_wake_fd_event->listen(FdEvent::IN_EVENT,[this](){  //2
            char buf[8];
            while(int rt = read(m_wakeup_fd,buf,8) != -1 && errno != EAGAIN){ //只要没有读失败就一直读
            //只要有数据可读就一直读， read ！=-1表示只要没发生错误就一直读，而 errno ！=Eagain限制了read == 0的情况   
            }
            DEBUGLOG("read full bytes from wake fd [%d]",m_wakeup_fd);
        });

        addEpollEvent(m_wake_fd_event);
    }
        //下面代码已经在上面函数里面实现了
        // epoll_event event;
        // event.events = EPOLLIN;
        // int rt = epoll_ctl(m_epoll_fds,EPOLL_CTL_ADD,m_wakeup_fd,&event);

        // if(rt == -1){
        //     ERRORLOG("failed to create eventloop,epoll_ctl eventfd create error,error info [%d]",errno);
        //     exit(0);
        // }
        //2
        // auto  read_callback = [this](){
        //     char buf[8];
        //     while(int rt = read(m_wakeup_fd,buf,8) != -1 && errno != EAGAIN){ //只要没有读失败就一直读
        //     //只要有数据可读就一直读， read ！=-1表示只要没发生错误就一直读，而 errno ！=Eagain限制了read == 0的情况   
        //     }
        //     DEBUGLOG("read full bytes from wake fd [%d]",m_wakeup_fd);
        // };
    

    void EventLoop:: loop(){    
        //可以先执行任务再epoll_wait;目的：还没启动epoll，但是往任务队列里面添加了任务，这样会导致这些任务不会被执行，而是等到有io任务的时候才会执行
        
        while(!m_stop_flag){
            ScopeMutex<Mutex> lock(m_mutex);
            std::queue<std::function<void()>> temp_tasks; //取出来之前需要先加锁,因为任务队列其他线程可能也会操作
            m_pending_tasks.swap(temp_tasks);
            lock.unlock();
            
            //执行完所有的任务
            while(!temp_tasks.empty()){
                // temp_tasks.front()();
                // temp_tasks.pop();
                std::function<void()> cb = temp_tasks.front();
                temp_tasks.pop();
                if(cb){
                    cb();
                }
            }

            //如果有定时任务需要执行，那么在这里执行就好了
            // 1.怎么判断一个定时任务需要执行? (now() > TimerEvent.arrive_time)
            // 2. arrive_time如何让eventloop监听

            //接着就是epoll
            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            // DEBUGLOG("now begin to epoll_wait()");
            int rt = epoll_wait(m_epoll_fds,result_events,g_epoll_max_events,timeout);

            DEBUGLOG("now end epoll_wait,rt = %d",rt);
            
            if(rt<0){
                ERRORLOG("epoll_wait error, error = [%d]",errno);
            }else{
                for(int i = 0;i<rt;++i){
                    epoll_event trigger_event = result_events[i]; //取出epoll_event
                    FdEvent* fd_event = static_cast<FdEvent*> (trigger_event.data.ptr); //强制类型转化
                    if(fd_event == NULL){
                        continue;
                    }
                    if(trigger_event.events & EPOLLIN) {
                        DEBUGLOG("fd %d trigger EPOLLIN event",fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::IN_EVENT));
                    }
                    if(trigger_event.events & EPOLLOUT) {
                        DEBUGLOG("fd %d trigger EPOLLOUT event",fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::OUT_EVENT));
                    }
                }
            }
        }
    }

    void EventLoop::wakeup(){
        INFOLOG("wake up");
        m_wake_fd_event->wakeup();
    }

    void EventLoop:: stop(){
        m_stop_flag = true;
    }
    
    void EventLoop::dealWakeup(){
        
    }

    void EventLoop::addEpollEvent(FdEvent* event) {
        if(isInLoopThread()){ //先判断是否是当前线程进程添加
            // auto it = m_listen_fds.find(event->getFd()); //是的话 先进程判断之前是否添加了这个fd，如果添加了就MOD，（默认是添加）
            // int op = EPOLL_CTL_ADD;
            // if(it != m_listen_fds.end()){
            //     op = EPOLL_CTL_MOD;
            // }
            // epoll_event temp = event->getEpollEvent();
            // int rt = epoll_ctl(m_epoll_fds,op,event->getFd(),&temp);
            // if(rt == -1){
            //     ERRORLOG("failed epoll_ctl when add fd %d,errno = %d,error = %s",event->getFd(),errno,strerror(errno));
            // }
            ADD_TO_EPOLL(); //用宏代替上面的代码
        }else{ //如果不是当前线程在添加
            auto cb = [this,event](){
                 ADD_TO_EPOLL();
            };
            addTask(cb,true);
        }
    }
    void EventLoop::deleteEpollEvent(FdEvent* event) {
        //逻辑和add是一样的
        if(isInLoopThread()){
            // auto it =  m_listen_fds.find(event->getFd());
            // if(it == m_listen_fds.end()){
            //     return; //直接退出就行了
            // }
            // //如果找到了当前event
            // int op = EPOLL_CTL_DEL;
            // epoll_event temp = event->getEpollEvent();
            // int rt = epoll_ctl(m_epoll_fds,op,event->getFd(),&temp);
            // if(rt == -1){
            //     ERRORLOG("failed epoll_ctl when add fd %d,errno = %d,error = %s",event->getFd(),errno,strerror(errno))
            // }
            // DEBUGLOG("delete event success,fd[%d]",event->getFd());
            DELETE_TO_EPOLL();
        }
        else{   
            auto cb = [this,event](){
                DELETE_TO_EPOLL();
            };
            addTask(cb,true);
        }
    }

    bool EventLoop::isInLoopThread(){ //用于判断执行isInLoopThread这个函数的线程是不是EventLoop这个线程
        return getThreadId() == m_thread_id;
    }
    void EventLoop::addTask(std::function<void()> cb,bool is_wake_up /*false*/){
        ScopeMutex<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();
        if(is_wake_up){
            wakeup();
        }
    }

};