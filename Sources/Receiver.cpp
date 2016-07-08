/**
 * ���߳����ݽ���ģ��
 * �����ˣ�����
 */
#include "Global.h"
#ifdef __WIN32__
#include <WinSock2.h>
#include "windows.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>
#endif

#include<errno.h>
#include "Receiver.h" 
#include "Buffer.h" 
#include "Debugger.h" 

Receiver::Receiver()
{
    this->socket = 0;
    this->buffer = 0;
}
Receiver::Receiver(SOCKET socket, Buffer* buffer)
{
    this->socket = socket;
    this->buffer = buffer;

}
Receiver::~Receiver()
{
}
void Receiver::Start()
{
    while (0 != isTerminted)
    {
        /*
        If no error occurs, this function returns the number of bytes received. If the connection has been gracefully closed,
        the return value is zero. If an error occurs, a value of SOCKET_ERROR is returned, and a specific error code can be
        retrieved by calling WSAGetLastError
        */
        //int len = recv( socket, (char*)recvBuffer, NUM_BYTES_TO_RECEIVE_EACH_TIME, MSG_OOB ); // ����ÿ��Ӧ�ý��ն����ֽڣ�MSG_OOB or MSG_PEEK ?
        int len = recv( socket, (char*)recvBuffer, NUM_BYTES_TO_RECEIVE_EACH_TIME, 0 ); // 0Ϊû������Ч��
        if (len > 0)
        {
            uint i = 0;
            int result=0;

        #if 0
            printf("receive data=%d\n",len);
            for ( int j=0 ; j<len; j++ )
            {
                printf("%x  ",*(recvBuffer + j));
                if(((j+1)%16) == 0)
                    printf("\n");
            }
            printf("\n");
            printf("receive data end\n");
            result=codec->ParseDataPackage(recvBuffer,len);
            printf("receive data send=%d,%d\n",len,result);

            // �����ν��յ������ݴ���ѭ�������� buffer ��
            while (( len > 0 ) && (result > 0))
        #else
            PRINT(ALWAYS_PRINT, "Receiver", __FUNCTION__, __LINE__, "Received Data=%s,%d\n",recvBuffer,len);
            while ( len > 0 )
        #endif
            {
                // ��ѯѭ��������ʣ��ռ�
                uint writelen = 0;
                uint availLen = buffer->GetUnusedSpace();
                if (availLen < len)
                {
                    writelen = availLen;
                }
                else
                {
                    writelen = len;
                }

                if (writelen > 0)
                {
                    // ѭ�����������ο�д��availLen���ֽ�
                    result=buffer->Write(recvBuffer + i, writelen); // д������
                    //PRINT(ALWAYS_PRINT, "Receiver", __FUNCTION__, __LINE__, "write len=%d,%d\n",writelen,result);
                    len -= writelen;                            // �����´ο�д����ֽ���
                    i  += writelen;                         // ��������Դ��ַָ��
                }
                else
                {
#ifdef __WIN32__
                    Sleep(100);
#else
                    usleep(100000);
#endif
                }
            }
        }
        else
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

            if (0 != lastError)
            {
                PRINT(ALWAYS_PRINT, "Receiver", __FUNCTION__, __LINE__, "receiveerror: %d \n",   lastError);
            }
#else
            lastError=errno;
            if (0 != lastError)
            {
                PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "receiveerror: %s \n",   strerror(errno));
            }
#endif
        }
    }
/*
#else
    uint len = read(socket, (char*)recvBuffer, NUM_BYTES_TO_RECEIVE_EACH_TIME); // 
        if (len > 0)
        {
            uint i = 0;
            // �����ν��յ������ݴ���ѭ�������� buffer ��
            while ( len > 0 )
            {
                // ��ѯѭ��������ʣ��ռ�
                uint availLen = buffer->GetUnusedSpace();
                if (availLen > 0)
                {
                    // ѭ�����������ο�д��availLen���ֽ�
                    buffer->Write(recvBuffer + i, availLen);    // д������
                    len -= availLen;                            // �����´ο�д����ֽ���
                    i  += availLen;                         // ��������Դ��ַָ��
                }
            }
        }
        else if(len==0)
 {  
              break;//���ӶϿ�
        } else { 
            break; //����errno
        }
    }

#endif
*/
}
