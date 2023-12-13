#ifndef ROCK_NET_TCP_TCP_SERVER_H
#define ROCK_NET_TCP_TCP_SERVER_H

#include<memory>
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/tcp_acceptor.h"
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/net_addr.h"
#include "/home/xxxj/rpc/rocket/rocket/net/eventloop.h"
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread_group.h"

namespace rocket {
class TcpServer {
public:
    TcpServer(NetAddr::s_ptr local_addr);
    ~TcpServer();

    void start();

private:
    void init();
//当有新的客户端连接之后，执行
    void onAccept();
private:
    TcpAcceptot::s_ptr m_acceptor;
    NetAddr::s_ptr m_local_addr; //本地监听地址
    
    EventLoop* m_main_event_loop {NULL};  //main Reactor
    IOThreadGroup* m_io_thread_group {NULL}; //sub Reactor组
    
    FdEvent* m_listen_fd_event;

    int m_client_count{0};
};
}

#endif