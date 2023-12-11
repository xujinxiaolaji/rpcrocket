#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include<pthread.h>
#include "/home/xxxj/rpc/rocket/rocket/common/config.h"


void* func(void *){
    DEBUGLOG("dubug this is thread in %s","func");
    INFOLOG("info this is thread in %s","func");
    return NULL;
}

int main(){
    
    rocket::Config::SetGlobalConfiger("../conf/rocket.xml");
    rocket::Logger::InitGlobalLogger();

    // pthread_t thread;
    // pthread_create(&thread,NULL,&func,NULL);
    // DEBUGLOG("test log %s","11");
    INFOLOG("test info log %s","11");
    // pthread_join(thread,NULL);
    return 0;
}