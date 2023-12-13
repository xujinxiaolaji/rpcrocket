#include<assert.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<string.h>
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/tcp_acceptor.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/net_addr.h"

namespace rocket {
TcpAcceptot::TcpAcceptot(NetAddr::s_ptr local_addr): m_local_addr(local_addr){
    if(!!local_addr->checkValid()) {
        ERRORLOG("invalid local addr %s",local_addr->toString().c_str());
        exit(0);
    }
    m_family = m_local_addr->getFamily();
    m_listenfd =  socket(m_family,SOCK_STREAM,0);
    if(m_listenfd < 0) {
        ERRORLOG("invalid listenfd %d ",m_listenfd);
        exit(0);
    }
    int value = 1;
    if (setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&value,sizeof(value)) != 0) {
        ERRORLOG("setsockopt REUSEADDR error, errno = %d,error = %s",errno,strerror(errno));
    }

    socklen_t len = m_local_addr->getSockLen();
    if (bind(m_listenfd,m_local_addr->getSockAddr(),len) !=0 ) {
        ERRORLOG("bind error,errno=%d,error=%s",errno,strerror(errno));
        exit(0);
    }

    if (listen(m_listenfd,1000) !=0 ) {
        ERRORLOG("listen error,errno=%d,error=%s",errno,strerror(errno));
        exit(0);
    }
}

TcpAcceptot::~TcpAcceptot() {
    
}

}