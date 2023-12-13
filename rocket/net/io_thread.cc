#include<pthread.h>
#include<assert.h>
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"

namespace rocket{

IOThread::IOThread() {
    int rt = sem_init(&m_init_semaphore,0,0);
    assert(rt==0);
    rt = sem_init(&m_start_semaphore,0,0);
    assert(rt == 0);

    pthread_create(&m_thread,NULL,&IOThread::Main,this); //传this对象的话，可以在Main中把这个对象取出来,因为最后一个参数是Main函数的输入参数

    //wait,直到新的线程执行完Main的函数前置（就是开启loop()循环之前）,怎么实现呢，需要用信号量
    sem_wait(&m_init_semaphore);
    DEBUGLOG("IOThread %d create success",m_thread_id);
}

IOThread::~IOThread() {
    m_eventloop->stop();
    sem_destroy(&m_init_semaphore);
    sem_destroy(&m_start_semaphore);
    pthread_join(m_thread,NULL);

    if (m_eventloop) {
        delete m_eventloop;
        m_eventloop = NULL;
    }    
}

void* IOThread::Main(void* arg){ //进入main函数就相当于进入的新的线程
    IOThread* thread = static_cast<IOThread*> (arg);
    thread->m_eventloop = new EventLoop();//新线程创建新的EventLoop
    thread->m_thread_id = getThreadId();
    
    //唤醒等待的线程
    DEBUGLOG("IOThread %d created, wait start semaphore", thread->m_thread_id);
    sem_post(&thread->m_init_semaphore);
    //阻塞在这里
    DEBUGLOG("IOThread %d create,wait start semaphore",thread->m_thread_id);
    sem_wait(&thread->m_start_semaphore);
    DEBUGLOG("IOThread %d start loop ",thread->m_thread_id);
    thread->m_eventloop->loop();

    DEBUGLOG("IOThread %d end loop!",thread->m_thread_id);

    return NULL;
}

EventLoop* IOThread::getEventLoop() {
    return m_eventloop;
}

void IOThread::start() {
    DEBUGLOG("Now invoke IOThread %d",m_thread_id);
    sem_post(&m_start_semaphore);
}

void IOThread::join() {
    pthread_join(m_thread,NULL);
}

}