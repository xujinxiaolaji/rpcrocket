#include<memory>
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/net_addr.h"
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/tcp_server.h"

void test_tcp_server() {
    rocket::IPNetAddr::s_ptr addr= std::make_shared<rocket::IPNetAddr> ("127.0.0.1",12345);
    DEBUGLOG("create addr %s",addr->toString().c_str());
    rocket::TcpServer tcp_server(addr);
    
    tcp_server.start();
}

int main() {
    rocket::Config::SetGlobalConfiger("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    test_tcp_server();
    // rocket::IPNetAddr addr("127.0.0.1",12345);
    // DEBUGLOG("create addr %s",addr.toString().c_str());


    return 0;
}