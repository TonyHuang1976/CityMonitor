/**
 * 多线程数据发送模块
 * 负责人：董超
 */
#include "Global.h"

#ifdef __WIN32__
#include <WinSock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#endif

#include <errno.h>
#include "Sender.h" 
#include "Buffer.h" 
#include "Debugger.h" 

Sender::Sender()
{
    this->socket = 0;
    this->buffer = 0;
}
Sender::Sender(SOCKET socket, Buffer* buffer)
{
    this->socket = socket;
    this->buffer = buffer;
}
Sender::~Sender()
{
}

int Sender::SendAll(int sock, char* data, int len)
{
    int n = len, k, nWrite = 0;
    while(n>0)
    {
#ifdef __WIN32__
        k = send(sock, data, n, 0);
#else
        k = write(sock, data, n);
#endif
        if(k < 0) { return k; }
        if(k == 0) { return nWrite; }
        nWrite += k;
        data += k;
        n -= k;
    }
    return nWrite;
}
void Sender::Start()
{
    while (0 != isTerminted)
    {
        /*
        If no error occurs, this function returns the number of bytes received. If the connection has been gracefully closed,
        the return value is zero. If an error occurs, a value of SOCKET_ERROR is returned, and a specific error code can be
        retrieved by calling WSAGetLastError
        */
        // 检查循环缓冲器中是否有有效数据在等待发送
        uint availLen = buffer->GetValidDataLen();
        if (availLen == 0)
        {
            #ifdef __WIN32__
            Sleep(100);
            #else
            usleep(100000);
            #endif
            continue;
        }
        if (MAX_NUM_BYTES_TO_SEND_EACH_TIME < availLen)
        {
            availLen = MAX_NUM_BYTES_TO_SEND_EACH_TIME;
        }

        // 在循环缓冲器中读取 availLen 个字节数据到 sendBuffer 中
        buffer->Read(sendBuffer, availLen);
        // 从 sendBuffer 中发送 availLen 个字节数据到左邻节点 
        int len = SendAll( socket, (char*)sendBuffer, availLen);
        if (len <= 0)
        {
#ifdef __WIN32__
            lastError = WSAGetLastError();

            switch (lastError)
            {
            case 0:                 // The connection has been gracefully closed
                break;
            case WSAENETDOWN:       // The network subsystem has failed
                break;
            case WSAEFAULT:         // The buffer parameter is not completely contained in a valid part of the user address space
                break;
            case WSAENOTCONN:       // The socket is not connected
                break;
            case WSAEINTR:          // The socket was closed
                break;
            case WSAENETRESET:      // The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress
                break;
            case WSAESHUTDOWN:      // The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH
                break;
            case WSAEWOULDBLOCK:    // The socket is marked as nonblocking and the receive operation would block
                break;
            case WSAEMSGSIZE:       // The message was too large to fit into the specified buffer and was truncated
                break;
            case WSAECONNABORTED:   // The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable
                break;
            case WSAETIMEDOUT:      // The connection has been dropped because of a network failure or because the peer system failed to respond
                break;
            case WSAECONNRESET:     // The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket because it is no longer usable
                break;
            }
            lastError = errno;
            PRINT(ALWAYS_PRINT, "Sender", __FUNCTION__, __LINE__, "senderror: %s \n",   lastError);
#else
            lastError = errno;
            PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "senderror: %s \n",   strerror(errno));
#endif
        }
    }
}
