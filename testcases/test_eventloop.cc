#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<memory>
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/config.h"
#include "/home/xxxj/rpc/rocket/rocket/net/fd_event.h"
#include "/home/xxxj/rpc/rocket/rocket/net/eventloop.h"
#include "/home/xxxj/rpc/rocket/rocket/net/timer.h"
#include "/home/xxxj/rpc/rocket/rocket/net/timer_event.h"
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread.h"
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread_group.h"

void test_io_thread(){
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(listenfd == -1){
        ERRORLOG("listenfd = -1");
        exit(0);
    }

    sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    
    addr.sin_port = htons(12311);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1",&addr.sin_addr);

    int rt = bind(listenfd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
    if(rt != 0){
        ERRORLOG("bind error!");
        exit(1);
    }

    rt = listen(listenfd,100);
    if(rt!=0){
        ERRORLOG("listen error!");
        exit(1);
    }

    rocket::FdEvent event(listenfd);
    event.listen(rocket::FdEvent::IN_EVENT,[listenfd](){
        sockaddr_in peer_addr;
        socklen_t addr_len = sizeof(peer_addr);
        memset(&peer_addr,0,sizeof(peer_addr));
        int clientfd = accept(listenfd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);
        DEBUGLOG("success get client fd[%d],peer addr [%s:%d]",clientfd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
    });

    int i = 0;
    rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
        1000,true,[&i](){
            INFOLOG("trigger timer event, count=%d",i++);
        }
    );

    // rocket::IOThread io_thread;
    
    rocket::IOThreadGroup io_thread_group(2);
    rocket::IOThread* io_thread = io_thread_group.getIOThread();
    io_thread->getEventLoop()->addEpollEvent(&event);
    io_thread->getEventLoop()->addTimerEvent(timer_event);
    // io_thread->start();

    rocket::IOThread* io_thread2 = io_thread_group.getIOThread();
    // io_thread2->getEventLoop()->addEpollEvent(&event);
    io_thread2->getEventLoop()->addTimerEvent(timer_event);

    io_thread_group.start();
    // io_thread->join();
    io_thread_group.join();
}

int main(){
    rocket::Config::SetGlobalConfiger("../conf/rocket.xml");
    
    rocket::Logger::InitGlobalLogger();

    test_io_thread();

    // rocket::EventLoop* eventloop = new rocket::EventLoop(); //创建一个新的eventloop循环
    
    // int listenfd = socket(AF_INET,SOCK_STREAM,0);
    // if(listenfd == -1){
    //     ERRORLOG("listenfd = -1");
    //     exit(0);
    // }
    // sockaddr_in addr;
    // memset(&addr,0,sizeof(addr));
    // addr.sin_port = htons(12311);
    // addr.sin_family = AF_INET;
    // inet_aton("127.0.0.1",&addr.sin_addr);

    // int rt = bind(listenfd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
    // // bind(listenfd,(struct sockaddr*)&addr,sizeof(addr));
    // if(rt != 0){
    //     ERRORLOG("bind Error");
    //     exit(1);
    // }
    // rt = listen(listenfd,100);
    // if(rt != 0){
    //     ERRORLOG("Listen Error!");
    //     exit(1);
    // }
    
    // rocket::FdEvent event(listenfd);
    // event.listen(rocket::FdEvent::IN_EVENT,[listenfd](){
    //     sockaddr_in peer_addr;
    //     socklen_t addr_len = sizeof(peer_addr);
    //     memset(&peer_addr,0,sizeof(peer_addr));
    //     int clientfd = accept(listenfd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);

    //     // inet_ntoa(peer_addr.sin_addr);
    //     // DEBUGLOG("success get client [%s:%d]",inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
    //     DEBUGLOG("success get client fd[%d],peer addr:[%s:%d]",clientfd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
    // });
    
    // eventloop->addEpollEvent(&event);
    
    // int i = 0;
    // rocket::TimerEvent::s_ptr timer_event = std::make_shared<rocket::TimerEvent>(
    //     1000,true,[&i](){
    //         INFOLOG("trigger timer event,count = %d",i++);
    //     }
    // );
    // eventloop->addTimerEvent(timer_event);

    // eventloop->loop();

    return 0;
}
