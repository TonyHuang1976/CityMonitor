/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : BasicTCP.cpp
 * Author      : 周林
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *               不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : TCP/IP 通信模块
 *=======================================================================================================================
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-11  董超    新建
 * 2016-06-xx  周林    完成版本V1.0.0
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
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
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
// 建立服务器连接
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
            l.l_onoff = 1; //在调用close(socket)时还有数据未发送完，允许等待
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

// 建立客户端连接
void BasicTCP::CreateClient()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
//  #ifdef LINUX
    int maxConnectRetryTime=0 ;
    bool isConnected= false;
    do
 {
     sockID=socket(AF_INET, SOCK_STREAM, 0); //建立socket
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

// 工作在服务器模式时接受远程客户端的连接
SOCKET BasicTCP::Accept()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    struct sockaddr_in client_address;
    int client_len = sizeof(client_address);
    SOCKET acceptSock = accept(sockID, (struct sockaddr *)&client_address, (socklen_t*)&client_len);
    if(acceptSock>0)
    { //有链接到来
        PRINT(ALWAYS_PRINT, "BasicTCP ", __FUNCTION__, __LINE__, "accept新到连接ip=%s, port=%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
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
    bzero(&server_addr,sizeof(server_addr)); //把一段内存区的内容全部设置为0
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    //server_addr.sin_addr.s_addr = inet_addr(localIpAddr);
    server_addr.sin_port = htons(localPort);

    //创建用于internet的流协议(TCP)socket,用server_socket代表服务器socket

    //把socket和socket地址结构联系起来
    if( 0 != bind(sockID,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
       // printf("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT);
        int err = errno;
  //    printf("binderror: %s \n",   strerror(errno));
        PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__, "binderror: %s \n",   strerror(errno));
    }
}

// 做网络连接的端口及IP地址的绑定
void BasicTCP::Bind1()
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    //#ifdef LINUX
    int server_len ;
    struct sockaddr_in address;
    bzero(&address,sizeof(address)); //把一段内存区的内容全部设置为0
    address.sin_family = AF_INET;

    address.sin_addr.s_addr = htons(INADDR_ANY);

    address.sin_port = htons(localPort);
    server_len =sizeof(address);
    int  result=bind(sockID, (struct sockaddr *)&address, server_len);
}                       
// 工作在客户端模式时连接远程服务器
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
// 工作在服务器模式时开始等待远程客户端的连接
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
// 从网络连接的另一端接收数据。这可能是一个下行的命令，也可能是一段上行的视频数据。该函数需要在一个线程中运行。
// 1. 与本地摄像机连接：接收来自本地摄像机的视频数据
// 2. 与右邻视频单元连接：接收来自右邻视频单元的视频流，这可能是来自右邻视频单元的多个连接
// 3. 与左邻视频单元连接：接收左邻视频单元转发的来自远程监控中心的下行命令
int BasicTCP::Receive(SOCKET socket)
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    Receiver* receiver = new Receiver(socket, writeBuffer); //读线程
    receiver->Init();

    ThreadManager::GetInstance()->AddSocketThread(socket, receiver);
    return 0;
}                       
// 向网络连接的另一端发送数据。这可能是转发一个下行的命令，也可能是一段上行的视频数据。
// 1. 与本地摄像机连接：发送摄像机的控制信息和配置信息
// 2. 与右邻视频单元连接：转发来自远程监控中心的下行命令
// 3. 与左邻视频单元连接：发送或转发视频流
int BasicTCP::Send(SOCKET socket)
{
    PRINT(ALWAYS_PRINT, "BasicTCP", __FUNCTION__, __LINE__);
    Sender* sender = new Sender(socket, readBuffer); //写线程
    sender->Init();

    ThreadManager::GetInstance()->AddSocketThread(socket, sender);
    return 1;
}                       
// 关闭连接
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

void BasicTCP::Elapse(int  timeout) //定时函数
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

Linux 服务器代码

01.#include <stdio.h>
02.#include <string.h>
03.#include <stdlib.h>
04.#include <unistd.h>
05.#include <arpa/inet.h>
06.#include <sys/socket.h>
07.#include <netinet/in.h>
08.
09.int main(){
10.    //创建套接字
11.    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
12.
13.    //将套接字和IP、端口绑定
14.    struct sockaddr_in serv_addr;
15.    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
16.    serv_addr.sin_family = AF_INET;  //使用IPv4地址
17.    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
18.    serv_addr.sin_port = htons(1234);  //端口
19.    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
20.
21.    //进入监听状态，等待用户发起请求
22.    listen(serv_sock, 20);
23.
24.    //接收客户端请求
25.    struct sockaddr_in clnt_addr;
26.    socklen_t clnt_addr_size = sizeof(clnt_addr);
27.    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
28.
29.    //向客户端发送数据
30.    char str[] = "Hello World!";
31.    write(clnt_sock, str, sizeof(str));
32.   
33.    //关闭套接字
34.    close(clnt_sock);
35.    close(serv_sock);
36.
37.    return 0;
38.}


Linux 客户端代码

01.#include <stdio.h>
02.#include <string.h>
03.#include <stdlib.h>
04.#include <unistd.h>
05.#include <arpa/inet.h>
06.#include <sys/socket.h>
07.
08.int main(){
09.    //创建套接字
10.    int sock = socket(AF_INET, SOCK_STREAM, 0);
11.
12.    //向服务器（特定的IP和端口）发起请求
13.    struct sockaddr_in serv_addr;
14.    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
15.    serv_addr.sin_family = AF_INET;  //使用IPv4地址
16.    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
17.    serv_addr.sin_port = htons(1234);  //端口
18.    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
19.   
20.    //读取服务器传回的数据
21.    char buffer[40];
22.    read(sock, buffer, sizeof(buffer)-1);
23.   
24.    printf("Message form server: %s\n", buffer);
25.   
26.    //关闭套接字
27.    close(sock);
28.
29.    return 0;
30.}


Windows 服务器代码

01.#include <stdio.h>
02.#include <winsock2.h>
03.#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
04.
05.int main(){
06.    //初始化 DLL
07.    WSADATA wsaData;
08.    WSAStartup( MAKEWORD(2, 2), &wsaData);
09.
10.    //创建套接字
11.    SOCKET servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
12.
13.    //绑定套接字
14.    sockaddr_in sockAddr;
15.    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
16.    sockAddr.sin_family = PF_INET;  //使用IPv4地址
17.    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
18.    sockAddr.sin_port = htons(1234);  //端口
19.    bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
20.
21.    //进入监听状态
22.    listen(servSock, 20);
23.
24.    //接收客户端请求
25.    SOCKADDR clntAddr;
26.    int nSize = sizeof(SOCKADDR);
27.    SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);
28.
29.    //向客户端发送数据
30.    char *str = "Hello World!";
31.    send(clntSock, str, strlen(str)+sizeof(char), NULL);
32.
33.    //关闭套接字
34.    closesocket(clntSock);
35.    closesocket(servSock);
36.
37.    //终止 DLL 的使用
38.    WSACleanup();
39.
40.    return 0;
41.}


Windows 客户端代码

01.#include <stdio.h>
02.#include <stdlib.h>
03.#include <WinSock2.h>
04.#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
05.
06.int main(){
07.    //初始化DLL
08.    WSADATA wsaData;
09.    WSAStartup(MAKEWORD(2, 2), &wsaData);
10.
11.    //创建套接字
12.    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
13.
14.    //向服务器发起请求
15.    sockaddr_in sockAddr;
16.    memset(&sockAddr, 0, sizeof(sockAddr));  //每个字节都用0填充
17.    sockAddr.sin_family = PF_INET;
18.    sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
19.    sockAddr.sin_port = htons(1234);
20.    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
21.
22.    //接收服务器传回的数据
23.    char szBuffer[MAXBYTE] = {0};
24.    recv(sock, szBuffer, MAXBYTE, NULL);
25.
26.    //输出接收到的数据
27.    printf("Message form server: %s\n", szBuffer);
28.
29.    //关闭套接字
30.    closesocket(sock);
31.
32.    //终止使用 DLL
33.    WSACleanup();
34.
35.    system("pause");
36.    return 0;
37.}


Windows 下的 socket 程序和 Linux 思路相同，但细节有所差别：
1) Windows 下的 socket 程序依赖 Winsock.dll 或 ws2_32.dll，必须提前加载。DLL 有两种加载方式，请查看：动态链接库DLL的加载

2) Linux 使用“文件描述符”的概念，而 Windows 使用“文件句柄”的概念；Linux 不区分 socket 文件和普通文件，而 Windows 区分；Linux 下 socket() 函数的返回值为 int 类型，而 Windows 下为 SOCKET 类型，也就是句柄。

3) Linux 下使用 read() / write() 函数读写，而 Windows 下使用 recv() / send() 函数发送和接收。

4) 关闭 socket 时，Linux 使用 close() 函数，而 Windows 使用 closesocket() 函数。
*/