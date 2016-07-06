/**
 * ���߳����ݷ���ģ��
 * �����ˣ�����
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
    Buffer*     buffer;                                         // ���ݽ���ѭ��������
    byte        sendBuffer[MAX_NUM_BYTES_TO_SEND_EACH_TIME];    // ��ʱ���ջ�����
public:
    virtual void        Start();                                        // ִ�������߳��мƻ�������
    int     SendAll(int sock, char* data, int len);
};