### 2.3日志模块开发

日志模块
···
1.日志级别
2.打印到文件，支持日期命名，以及日志的滚动
3.c 格式化风控
4.线程安全
···

LogLevel:
```
Debug
Info
Error
```

LogEvent
```
文件名、行号
MsgNo
进程号
Thread ID
日期、时间，精确到ms
自定义消息
```

日志格式
```
[Level][%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][%msg]
```

Logger 日志器
1.提供打印日志的方法
2.设置日志输出的路径
#rpcrocket


### 2.4.1 TimerEvent 定时任务
```
1.指定时间点 arrive_time
2.interval（间隔）,ms
3.is_repeated（是否为重复任务,周期性的定时任务）
4.is_canceld（是否取消该定时任务）
5.task
cancle()
cancleRepeated()
```

### 2.4.2 timer
定时器，要加入到eventloop中，他是一个TimerEvent的集合
Timer继承FdEvent（像wakeupfd一样）
```
addTimerEvent();

deleteTimerEvent();

onTimer();//当发生io事件之后，需要执行的方法

reserArriveTime()

multimap 存储 TimerEvent<key(arrivetime),TimerEvent>
```

### 2.5 IO线程
创建一个IO线程 他会帮我们执行：
1. 创建一个新的线程（pthread_create）
2. 在新线程里面创建一个EventLoop，完成初始化
3. 开启loop循环
```
class {

    pthread_t m_thread;
    pid_t m_thread_id;
    EventLoop event_loop;
}
```