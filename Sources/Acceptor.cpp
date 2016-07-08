#include "Acceptor.h"
#include "BasicTCP.h" 
#include "Debugger.h" 

#ifdef __WIN32__
#include "ws2tcpip.h"
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <string.h>

Acceptor::Acceptor(BasicTCP* basicTCP) : Thread(basicTCP)
{
    PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__);

    sockID = basicTCP->GetSockID();
}

Acceptor::~Acceptor(void)
{
}

void Acceptor::Start()
{
    PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__);

    while(1)
    {
        SOCKET acceptSock = Accept();
        if (acceptSock > 0)
        { 
            PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__, "�ͻ������ӳɹ�");
            basicTCP->Receive(acceptSock);
            basicTCP->Send(acceptSock); 
        }
        else
        {      
            PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__, "�ͻ�������ʧ��");
        }
    }
}

// �����ڷ�����ģʽʱ����Զ�̿ͻ��˵�����
SOCKET Acceptor::Accept()
{
    PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__);
    struct sockaddr_in client_address;
    int client_len = sizeof(client_address);

    SOCKET acceptSock = accept(sockID, (struct sockaddr *)&client_address, (socklen_t*)&client_len);

    if(acceptSock>0)
    { //�����ӵ���
        PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__, "�������µ����ӣ�ip=%s, port=%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        return  acceptSock;
    }
    else
    {
        int err = errno;
        PRINT(ALWAYS_PRINT, "Acceptor", __FUNCTION__, __LINE__, "������accept error: %s \n",   strerror(errno));
    }
    return -1;
}           

