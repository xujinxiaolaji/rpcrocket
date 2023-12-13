#ifndef ROCK_NET_IO_THREAD_GROUP_H
#define ROCK_NET_IO_THREAD_GROUP_H

#include<vector>
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread.h"
namespace rocket{

class IOThreadGroup{
public:
    IOThreadGroup(int size);
    ~IOThreadGroup();
    void start();

    void join();

    IOThread* getIOThread(); //从线程组中获取一个线程

private:
    int m_size{0};
    std::vector<IOThread*> m_io_thread_groups;

    int m_index{0};
};

}


#endif
