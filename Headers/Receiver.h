/**
 * ���߳����ݽ���ģ��
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
    Buffer*     buffer;         // ���ݽ���ѭ��������
    byte        recvBuffer[NUM_BYTES_TO_RECEIVE_EACH_TIME]; // ��ʱ���ջ�����
public:
    virtual void            Start();        // ִ�������߳��мƻ�������
};
