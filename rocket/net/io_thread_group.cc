#include<pthread.h>
#include "/home/xxxj/rpc/rocket/rocket/net/io_thread_group.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"
namespace rocket{

IOThreadGroup::IOThreadGroup(int size):m_size(size) {
    m_io_thread_groups.resize(m_size);
    for(size_t i = 0 ;i<size;i++) {
        m_io_thread_groups[i] = new IOThread(); //创建一个IOThread 里面就有一个线程
    }
}

IOThreadGroup::~IOThreadGroup() {
    
}

void IOThreadGroup::start() {
    for(size_t i = 0;i<m_io_thread_groups.size();i++) {
        m_io_thread_groups[i]->start(); //开启每个IO线程的循环
    }
}

void IOThreadGroup::join() {
    for(size_t i = 0;i<m_io_thread_groups.size();i++) {
        m_io_thread_groups[i]->join(); //
    }
}

IOThread* IOThreadGroup::getIOThread() { //从线程组中获取一个线程
    if(m_index == m_io_thread_groups.size() || m_index == -1) {
        m_index = 0;
    }
    return m_io_thread_groups[m_index++];
}
}