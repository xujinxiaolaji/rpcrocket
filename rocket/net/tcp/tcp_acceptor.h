#ifndef ROCK_NET_TCP_TCP_ACCEPTOR_H
#define ROCK_NET_TCP_TCP_ACCEPTOR_H

#include "/home/xxxj/rpc/rocket/rocket/net/tcp/net_addr.h"

namespace rocket{

class TcpAcceptot{
public:
    TcpAcceptot(NetAddr::s_ptr local_addr) ;
    ~TcpAcceptot();

    
private:
    NetAddr::s_ptr m_local_addr;  //服务端监听的地址：addr->ip:port
    
    int m_family{-1};
    
    int m_listenfd {-1};//listenfd,监听套接字
};

}


#endif