#ifndef ROCK_NET_IO_THREAD_H
#define ROCK_NET_IO_THREAD_H

#include<pthread.h>
#include<semaphore.h>
#include "/home/xxxj/rpc/rocket/rocket/net/eventloop.h"
// #include "/home/xxxj/rpc/rocket/rocket/net/eventloop.h"


namespace rocket{

class IOThread{
public:
    IOThread();
    ~IOThread();

    EventLoop* getEventLoop();

    void start();
    
    void join();

public:
    static void* Main(void* arg);

private:
    pid_t m_thread_id{-1};//线程号
    pthread_t m_thread{0};//线程句柄(线程文件描述符)
    EventLoop* m_eventloop{NULL}; //当前io 线程的loop对象

    sem_t m_init_semaphore;
    sem_t m_start_semaphore;
};


}


#endif