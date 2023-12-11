#include<sys/time.h>
#include<sstream>
#include<stdio.h>
#include "log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/common/util.h"
#include "/home/xxxj/rpc/rocket/rocket/common/config.h"

// #define 

namespace rocket{ 

    static Logger* g_logger = NULL; 
    Logger* Logger::GetGlobalLogger(){
        // if(g_logger){
        //     return g_logger;
        // }
        // g_logger = new Logger();
        return g_logger;
    }
 
    void Logger::InitGlobalLogger(){
        LogLevel glob_log_level =StringToLoglevel(Config::GetGlobalConfiger()->m_log_level); //GetGlobalConfiger()返回的是Config* 对象，然后调用m_log_level赋值给level
        printf("Init log level [%s]\n",LogLevelToString(glob_log_level).c_str());
        g_logger = new Logger(glob_log_level); 
        // return g_logger;
    }
    


    std::string LogLevelToString(LogLevel level){
        switch(level){
            case Debug:
                return "DEBUG";
            case Info:
                return "Info";
            case Error:
                return "Error";
            default:
                return "UNKNOW";
                
        }
    }

    LogLevel StringToLoglevel(const std::string &log_level){
        if(log_level == "DEBUG"){
            return Debug;
        }else if(log_level == "Info"){
            return Info;
        }else if(log_level == "Error"){
            return Error;
        }else{
            return Unknown;
        }

    }


    std::string LogEvent::toString(){
        struct timeval now_time; 
        gettimeofday(&now_time,nullptr);
        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec),&now_time_t);

        char buf[128];
        strftime(&buf[0],128,"%y-%m-%d %H:%M:%S",&now_time_t);
        std::string time_str(buf);

        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms); 

        m_pid = getPid();
        m_thread_id = getThreadId();
        
        std::stringstream ss;
        ss<<"["<<LogLevelToString(m_level)<<"]\t"
            <<"["<<time_str<<"]\t"
            <<"["<<m_pid<<":"<<m_thread_id<<"]\t";
            
        return ss.str();
    }
    
    void Logger::pushLog(const std::string &msg){  //    //event时间的level必须大于设置的m_set_leve才能打印，否则是不能打印的
        ScopeMutex<Mutex> lock(m_mutex); //创建锁对象
        m_buffer.push(msg);
        lock.unlock(); //析构自己调
    }
    void Logger::log(){
        ScopeMutex<Mutex>lock(m_mutex);
        std::queue<std::string> tmp;
        m_buffer.swap(tmp); //为什么这里把buffer的内容赋值给了tmp，还要用swap函数？   
        lock.unlock(); //提前把锁释放了

        while(!tmp.empty()){
            std::string msg = tmp.front();
            tmp.pop();
            printf(msg.c_str());
        }
        lock.unlock();
    }
}