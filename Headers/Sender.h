/**
 * 多线程数据发送模块
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

#define     MAX_NUM_BYTES_TO_SEND_EACH_TIME     1024

class Buffer;

class Sender : public Thread
{
public:
    Sender();
    Sender(SOCKET socket, Buffer* buffer);
    virtual ~Sender();

private:
    SOCKET      socket;
    Buffer*     buffer;                                         // 数据接收循环缓冲器
    byte        sendBuffer[MAX_NUM_BYTES_TO_SEND_EACH_TIME];    // 临时接收缓冲器
public:
    virtual void        Start();                                        // 执行拟在线程中计划的任务
    int     SendAll(int sock, char* data, int len);
};