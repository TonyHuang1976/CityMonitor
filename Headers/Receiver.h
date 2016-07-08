/**
 * 多线程数据接收模块
 * 负责人：董超
 */
#pragma once
#include "Global.h" 
#include "Thread.h" 
#ifdef  __WIN32__
    #include <windows.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define SOCKET int
#endif

#define     NUM_BYTES_TO_RECEIVE_EACH_TIME      1024

class Buffer;

class Receiver : public Thread
{
public:
    Receiver();
    Receiver(SOCKET socket, Buffer* buffer);
    virtual ~Receiver();

private:
    SOCKET      socket;
    Buffer*     buffer;         // 数据接收循环缓冲器
    byte        recvBuffer[NUM_BYTES_TO_RECEIVE_EACH_TIME]; // 临时接收缓冲器
public:
    virtual void            Start();        // 执行拟在线程中计划的任务
};
