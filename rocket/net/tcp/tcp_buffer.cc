#include<memory>
#include<string.h>
#include "/home/xxxj/rpc/rocket/rocket/net/tcp/tcp_buffer.h"
#include "/home/xxxj/rpc/rocket/rocket/common/log.h"

namespace rocket{
TcpBuffer::TcpBuffer(int size):m_size(size) {
    m_buffer.resize(size);
}

TcpBuffer::~TcpBuffer() {

}

//返回可读的字节数
int TcpBuffer::readAble() {
    return m_write_index-m_read_index;
}

    //返回可写的字节数
int TcpBuffer::writeAble() {
    return m_buffer.size() - m_write_index;
}

int TcpBuffer::readIndex() {
    return m_read_index;
}

int TcpBuffer::writeIndex() {
    return m_write_index;
}

//这个函数的目的是改变原来buffer的大小
void TcpBuffer::resizeBUffer(int new_size) {
    std::vector<char>temp(new_size);
    int count = std::min(new_size,readAble());
    //把从 m_buffer[m_read_index] 这个位置开始的到m_buffer[m_read_index+count]的内容复制到以&temp[0]开头的地址
    memcpy(&temp[0],&m_buffer[m_read_index],count);
    m_buffer.swap(temp); //不仅交换内容 还交换大小
    
    m_size = m_buffer.size();
    m_read_index = 0;
    m_write_index = m_read_index + count;
}

void TcpBuffer::writeToBuffer(const char* buf, int size) {
    if(size > writeAble()) {
        //无法写入 需要调整buffer大小
        int new_size = (int)(1.5 * (m_write_index + size));
        resizeBUffer(new_size);
    }
    memcpy(&m_buffer[m_write_index],buf,size);
}

void TcpBuffer::readFromBuffer(std::vector<char>& re , int size) {
    if(readAble() == 0) {
        return;
    }
    int read_size = readAble() > size ? size : readAble();
    std::vector<char> temp(read_size);
    memcpy(&temp[0],&m_buffer[m_read_index],read_size);
    re.swap(temp);
    
    m_read_index += read_size;
    adjustBuffer();  //读了之后判断一下 是否需要调整
}

void TcpBuffer::adjustBuffer () {
    if(m_read_index < int(m_buffer.size() / 3)) {
        return;
    }
    std::vector<char> buffer(m_buffer.size());
    int count = readAble();
    
    memcpy(&buffer[0],&m_buffer[m_read_index],count);
    m_buffer.swap(buffer);

    m_read_index = 0;
    m_write_index = m_read_index + count;
    buffer.clear();
}

void TcpBuffer::moveReadIndex(int size) {
    size_t j = m_read_index + size;
    if(j >= m_buffer.size()) {
        ERRORLOG("moveReadIndex Error, invalid size %d,old_read_index %d,buffer size %d",size,m_read_index,m_buffer.size());
        return ;
    }
    m_read_index = j;
    adjustBuffer();
}

void TcpBuffer::moveWriteIndex(int size) {
    size_t j = m_write_index + size;
    if(j >= m_buffer.size()) {
        ERRORLOG("moveWriteIndex Error, invalid size %d,old_write_index %d,buffer size %d",size,m_write_index,m_buffer.size());
        return ;
    }
    m_write_index = j;
    adjustBuffer();
}

}