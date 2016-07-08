/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : BasicTCP.h
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
#ifndef __BASICTCP_H__

#define __BASICTCP_H__

#include "Global.h" 
#include "BasicObj.h" 
#include <errno.h>

#ifdef __WIN32__
#include <WinSock2.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#define SOCKET int
#endif

#define     MODE_CLIENT     0
#define     MODE_SERVER     1
#define     MAX_RECV_BUF    1024

#define     COMMAND_CHAN_CONNECTION             0
#define     LOCAL_CAMERA_CONNECTION             1
#define     LEFT_NODE_CONNECTION                2
#define     RIGHT_NODE_CONNECTION               3

class Acceptor;
class Buffer;

class BasicTCP : public BasicObj
{
public:
    BasicTCP();
    BasicTCP(byte connectType);
    virtual ~BasicTCP();
    virtual int Receive(SOCKET socket);         // 从网络连接的另一端接收数据
    virtual int Send(SOCKET socket);            // 向网络连接的另一端发送数据
private:
    Acceptor*   acceptor;           // 服务器创建时需要通过Accept来接受一个客户端连接，这个过程在一个独立的线程中运行

protected:
    char        connTypeStr[4][10]; // 连接类型描述字符串
    char        modeStr[2][10];     // 工作模式描述字符串
    Buffer*     readBuffer;         // 一个TCP/IP连接发送数据所需的数据读出缓冲器
    Buffer*     writeBuffer;        // 一个TCP/IP连接接收数据所需的数据存储缓冲器
    byte        status;             // 本TCP/IP连接的工作状态
    byte        mode;               // 工作模式：本连接是服务器或是客户端
    byte        connectType;        // 连接类型：命令通道、本地摄像机、左邻视频单元、右邻视频单元
    uint        localPort;          // 本TCP/IP通信连接的本地端口
    char*       localIpAddr;        // 本TCP/IP通信连接的本地IP地址
    uint        remotePort;         // 本TCP/IP通信连接的远程端口
    char*       remoteIpAddr;       // 本TCP/IP通信连接的远程IP地址
    SOCKET      sockID;

private:
    void Init();                                // 建立一个TCP/IP连接所需的初始化
    void Bind();                                // 做网络连接的端口及IP地址的绑定
    void Bind2();
    bool Connect();                             // 工作在客户端模式时连接远程服务器
    void Listen();                              // 工作在服务器模式时开始等待远程客户端的连接
    //virtual int Receive(SOCKET socket);           // 从网络连接的另一端接收数据
    //virtual int Send(SOCKET socket);          // 向网络连接的另一端发送数据
    void Close();                               // 关闭服务器或客户端连接
    void Elapse(int timeout);                   // 定时函数
    
public:
    void CreateServer4Cam();                    // 建立连接本地摄像机的服务器
    void CreateServer();                        // 建立服务器连接
    void CreateServerOld();                     // 建立服务器连接
    void CreateClient();                        // 建立客户端连接
    SOCKET Accept();                            // 工作在服务器模式时接受远程客户端的连接
    SOCKET GetSockID();                         // 获取本机套接字
    void SetReadBuffer(Buffer* readBuffer);     // 设置一个TCP/IP连接发送数据所需的数据读出缓冲器
    void SetWriteBuffer(Buffer* writeBuffer);   // 设置一个TCP/IP连接接收数据所需的数据存储缓冲器
};

#endif  // __BASICTCP_H__
