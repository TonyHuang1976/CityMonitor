/**
 *=======================================================================================================================
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : BasicTCP.cpp
 * Author      : ����
 * Version     : V1.0.0
 * Copyright   : �����������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ��ɵ�����£��κ�������������
 *               ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : TCP/IP ͨ��ģ��
 *=======================================================================================================================
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-11  ����    �½�
 * 2016-06-xx  ����    ��ɰ汾V1.0.0
 *=======================================================================================================================
 */
#include "BasicTCP.h" 
#include "Sender.h" 
#include "Receiver.h" 
#include "Debugger.h" 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "ThreadManager.h"

#ifdef __WIN32__

#include <winsock2.h>
#include "ws2tcpip.h"
#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
#define bzero(a, b)     memset(a, 0, b)
#else

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>    // for socket
#include <netinet/in.h>
#include <signal.h>
#include <assert.h>
#endif

BasicTCP::BasicTCP()
{
    sockID = 0;
#ifdef __WIN32__
    WSADATA Data;
    int status = WSAStartup(MAKEWORD(1, 1), &Data);
#endif
}

BasicTCP::~BasicTCP()
{
}
// ��������������
void BasicTCP::CreateServer()
{
        PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);


         sockID = socket(PF_INET,SOCK_STREAM,0);
        if( sockID < 0)
        {
            PRINT(ALWAYS_PRINT, "Create Socket Failed", __FUNCTION__, __LINE__);
        }
        {
            struct linger l;
            l.l_onoff = 1; //�ڵ���close(socket)ʱ��������δ�����꣬����ȴ�
            l.l_linger = 0;
            setsockopt(sockID, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(l));

            int opt =1;
            setsockopt(sockID, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
        }

     Bind1();
     Listen();
//  while(1)
    {
         SOCKET acceptSock=Accept();
         if(acceptSock>0)
         { 
                Receive(acceptSock);
                Send(acceptSock);
             
           }else{      
           PRINT(ALWAYS_PRINT, "accept the client connect error", __FUNCTION__, __LINE__);
           }
     }
}       

// �����ͻ�������
void BasicTCP::CreateClient()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
//  #ifdef LINUX
    int maxConnectRetryTime=0 ;
    bool isConnected= false;
    do
 {
     sockID=socket(AF_INET, SOCK_STREAM, 0); //����socket
//   signal(SIGPIPE,SIG_IGN);
     bool isConnected=Connect() ;
     if(isConnected)
    { 
        Receive(sockID);
            Send(sockID);
            break;
        }else{
            Close();
            Elapse(5);

            PRINT(ALWAYS_PRINT, "connect the server error and retry", __FUNCTION__, __LINE__);
        }
    }while((maxConnectRetryTime++)<MAX_NUM_CONN_TIME);
    
    if(!isConnected){
        PRINT(ALWAYS_PRINT, "connect the server error", __FUNCTION__, __LINE__);
    }
    
//#endif     
    
}                                   

// �����ڷ�����ģʽʱ����Զ�̿ͻ��˵�����
SOCKET BasicTCP::Accept()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    struct sockaddr_in client_address;
    int client_len = sizeof(client_address);
    SOCKET acceptSock = accept(sockID, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
    if(acceptSock>0)
    { //�����ӵ���
        PRINT(ALWAYS_PRINT, "BasicTCP ", __FUNCTION__, __LINE__, "accept�µ�����ip=%s, port=%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
        return  acceptSock;
    }
    else
    {
        int err = errno;
//      printf("binderror: %s \n",   strerror(errno));
        PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "acceptterror: %s \n",   strerror(errno));
    }
    return -1;
}           

void BasicTCP::Bind2()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr)); //��һ���ڴ���������ȫ������Ϊ0
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    //server_addr.sin_addr.s_addr = inet_addr(localIpAddr);
    server_addr.sin_port = htons(localPort);

    //��������internet����Э��(TCP)socket,��server_socket���������socket

    //��socket��socket��ַ�ṹ��ϵ����
    if( 0 != bind(sockID,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
       // printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
        int err = errno;
  //    printf("binderror: %s \n",   strerror(errno));
        PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "binderror: %s \n",   strerror(errno));
    }
}

// ���������ӵĶ˿ڼ�IP��ַ�İ�
void BasicTCP::Bind1()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    //#ifdef LINUX
    int server_len ;
    struct sockaddr_in address;
    bzero(&address,sizeof(address)); //��һ���ڴ���������ȫ������Ϊ0
    address.sin_family = AF_INET;

    address.sin_addr.s_addr = htons(INADDR_ANY);

    address.sin_port = htons(localPort);
    server_len =sizeof(address);
    int  result=bind(sockID, (struct sockaddr *)&address, server_len);
}                       
// �����ڿͻ���ģʽʱ����Զ�̷�����
bool BasicTCP::Connect()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(remoteIpAddr);
    address.sin_port = htons(remotePort);
    /*  Now connect our socket to the server's socket.  */
    int  result = connect(sockID, (struct sockaddr *)&address, sizeof(address));
    //  assert(result == 0) ;
    if(result != 0)
    {
        int err = errno;
//      printf("binderror: %s \n",   strerror(errno));
        PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "connecterror: %s \n",   strerror(errno));
        return false;
    }

    return true;
}                       
// �����ڷ�����ģʽʱ��ʼ�ȴ�Զ�̿ͻ��˵�����
void BasicTCP::Listen()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
//#ifdef LINUX
  /*  Now connect our socket to the server's socket.  */
  int rest = listen(sockID, 5);
  
  if (0 != rest)
  {
         int err = errno;
          //    printf("binderror: %s \n",   strerror(errno));
         PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "connecterror: %s \n",   strerror(errno));
  }
//#endif
}                       
// ���������ӵ���һ�˽������ݡ��������һ�����е����Ҳ������һ�����е���Ƶ���ݡ��ú�����Ҫ��һ���߳������С�
// 1. �뱾����������ӣ��������Ա������������Ƶ����
// 2. ��������Ƶ��Ԫ���ӣ���������������Ƶ��Ԫ����Ƶ���������������������Ƶ��Ԫ�Ķ������
// 3. ��������Ƶ��Ԫ���ӣ�����������Ƶ��Ԫת��������Զ�̼�����ĵ���������
int BasicTCP::Receive(SOCKET socket)
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    Receiver* receiver = new Receiver(socket, writeBuffer); //���߳�
    receiver->Init();

    ThreadManager::GetInstance()->AddSocketThread(socket, receiver);
    return 0;
}                       
// ���������ӵ���һ�˷������ݡ��������ת��һ�����е����Ҳ������һ�����е���Ƶ���ݡ�
// 1. �뱾����������ӣ�����������Ŀ�����Ϣ��������Ϣ
// 2. ��������Ƶ��Ԫ���ӣ�ת������Զ�̼�����ĵ���������
// 3. ��������Ƶ��Ԫ���ӣ����ͻ�ת����Ƶ��
int BasicTCP::Send(SOCKET socket)
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    Sender* sender = new Sender(socket, readBuffer); //д�߳�
    sender->Init();

    ThreadManager::GetInstance()->AddSocketThread(socket, sender);
    return 1;
}                       
// �ر�����
void BasicTCP::Close()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
#ifdef __WIN32__
    shutdown(sockID, SD_BOTH);
    closesocket(sockID);

#else
//  close();
    shutdown(sockID, SHUT_RDWR);
    close(sockID);
#endif
}                       

void BasicTCP::Elapse(int  timeout) //��ʱ����
{
#ifdef LINUX    
    struct timeval tval; 
    tval.tv_sec = timeout; 
    tval.tv_usec = 0; 
    select(0,NULL,NULL,NULL,&tval);
#endif
}                       

/*
http://c.biancheng.net/cpp/html/3031.html

Linux ����������

01.#include <stdio.h>
02.#include <string.h>
03.#include <stdlib.h>
04.#include <unistd.h>
05.#include <arpa/inet.h>
06.#include <sys/socket.h>
07.#include <netinet/in.h>
08.
09.int main(){
10.    //�����׽���
11.    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
12.
13.    //���׽��ֺ�IP���˿ڰ�
14.    struct sockaddr_in serv_addr;
15.    memset(&serv_addr, 0, sizeof(serv_addr));  //ÿ���ֽڶ���0���
16.    serv_addr.sin_family = AF_INET;  //ʹ��IPv4��ַ
17.    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
18.    serv_addr.sin_port = htons(1234);  //�˿�
19.    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
20.
21.    //�������״̬���ȴ��û���������
22.    listen(serv_sock, 20);
23.
24.    //���տͻ�������
25.    struct sockaddr_in clnt_addr;
26.    socklen_t clnt_addr_size = sizeof(clnt_addr);
27.    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
28.
29.    //��ͻ��˷�������
30.    char str[] = "Hello World!";
31.    write(clnt_sock, str, sizeof(str));
32.   
33.    //�ر��׽���
34.    close(clnt_sock);
35.    close(serv_sock);
36.
37.    return 0;
38.}


Linux �ͻ��˴���

01.#include <stdio.h>
02.#include <string.h>
03.#include <stdlib.h>
04.#include <unistd.h>
05.#include <arpa/inet.h>
06.#include <sys/socket.h>
07.
08.int main(){
09.    //�����׽���
10.    int sock = socket(AF_INET, SOCK_STREAM, 0);
11.
12.    //����������ض���IP�Ͷ˿ڣ���������
13.    struct sockaddr_in serv_addr;
14.    memset(&serv_addr, 0, sizeof(serv_addr));  //ÿ���ֽڶ���0���
15.    serv_addr.sin_family = AF_INET;  //ʹ��IPv4��ַ
16.    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
17.    serv_addr.sin_port = htons(1234);  //�˿�
18.    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
19.   
20.    //��ȡ���������ص�����
21.    char buffer[40];
22.    read(sock, buffer, sizeof(buffer)-1);
23.   
24.    printf("Message form server: %s\n", buffer);
25.   
26.    //�ر��׽���
27.    close(sock);
28.
29.    return 0;
30.}


Windows ����������

01.#include <stdio.h>
02.#include <winsock2.h>
03.#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
04.
05.int main(){
06.    //��ʼ�� DLL
07.    WSADATA wsaData;
08.    WSAStartup( MAKEWORD(2, 2), &wsaData);
09.
10.    //�����׽���
11.    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
12.
13.    //���׽���
14.    sockaddr_in sockAddr;
15.    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
16.    sockAddr.sin_family = PF_INET;  //ʹ��IPv4��ַ
17.    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //�����IP��ַ
18.    sockAddr.sin_port = htons(1234);  //�˿�
19.    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
20.
21.    //�������״̬
22.    listen(servSock, 20);
23.
24.    //���տͻ�������
25.    SOCKADDR clntAddr;
26.    int nSize = sizeof(SOCKADDR);
27.    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
28.
29.    //��ͻ��˷�������
30.    char *str = "Hello World!";
31.    send(clntSock, str, strlen(str)+sizeof(char), NULL);
32.
33.    //�ر��׽���
34.    closesocket(clntSock);
35.    closesocket(servSock);
36.
37.    //��ֹ DLL ��ʹ��
38.    WSACleanup();
39.
40.    return 0;
41.}


Windows �ͻ��˴���

01.#include <stdio.h>
02.#include <stdlib.h>
03.#include <WinSock2.h>
04.#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll
05.
06.int main(){
07.    //��ʼ��DLL
08.    WSADATA wsaData;
09.    WSAStartup(MAKEWORD(2, 2), &wsaData);
10.
11.    //�����׽���
12.    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
13.
14.    //���������������
15.    sockaddr_in sockAddr;
16.    memset(&sockAddr, 0, sizeof(sockAddr));  //ÿ���ֽڶ���0���
17.    sockAddr.sin_family = PF_INET;
18.    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
19.    sockAddr.sin_port = htons(1234);
20.    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
21.
22.    //���շ��������ص�����
23.    char szBuffer[MAXBYTE] = {0};
24.    recv(sock, szBuffer, MAXBYTE, NULL);
25.
26.    //������յ�������
27.    printf("Message form server: %s\n", szBuffer);
28.
29.    //�ر��׽���
30.    closesocket(sock);
31.
32.    //��ֹʹ�� DLL
33.    WSACleanup();
34.
35.    system("pause");
36.    return 0;
37.}


Windows �µ� socket ����� Linux ˼·��ͬ����ϸ���������
1) Windows �µ� socket �������� Winsock.dll �� ws2_32.dll��������ǰ���ء�DLL �����ּ��ط�ʽ����鿴����̬���ӿ�DLL�ļ���

2) Linux ʹ�á��ļ����������ĸ���� Windows ʹ�á��ļ�������ĸ��Linux ������ socket �ļ�����ͨ�ļ����� Windows ���֣�Linux �� socket() �����ķ���ֵΪ int ���ͣ��� Windows ��Ϊ SOCKET ���ͣ�Ҳ���Ǿ����

3) Linux ��ʹ�� read() / write() ������д���� Windows ��ʹ�� recv() / send() �������ͺͽ��ա�

4) �ر� socket ʱ��Linux ʹ�� close() �������� Windows ʹ�� closesocket() ������
*/