#include<unistd.h>
#include "/home/xxxj/rpc/rocket/rocket/net/wake_up_event.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"

namespace rocket{
  
WakeUpFdEvent::WakeUpFdEvent(int fd) : FdEvent(fd) {
    // init();
}

WakeUpFdEvent::~WakeUpFdEvent(){
    
}

// void WakeUpFdEvent::init(){
//     m_read_callback = [this](){
//         char buf[8];
//         while(int rt = read(m_fd,buf,8) != -1 && errno != EAGAIN){ //只要没有读失败就一直读
//             //只要有数据可读就一直读， read ！=-1表示只要没发生错误就一直读，而 errno ！=Eagain限制了read == 0的情况   
//         }
//         DEBUGLOG("read full bytes from wake fd [%d]",m_fd);
//     };
// }

void WakeUpFdEvent::wakeup(){
    char buf[8] = {'a'};
    int rt = write(m_fd,buf,8); //向wakeup的这个文件描述符中写
    if(rt != 8){
        ERRORLOG("Write to wakeup fd less than 8 bytes, fd [%d]",m_fd);        
    }
    DEBUGLOG("success read 8 bytes");
}

};