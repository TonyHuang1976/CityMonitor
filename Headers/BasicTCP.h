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

class Buffer;

class BasicTCP : public BasicObj
{
public:
    BasicTCP();
    virtual ~BasicTCP();

protected:
    Buffer*         readBuffer;
    Buffer*         writeBuffer;
    byte            status;             // 本TCP/IP连接的工作状态
    byte            mode;               // 工作模式：本连接是服务器或是客户端
    uint            localPort;          // 本TCP/IP通信连接的本地端口
    char*           localIpAddr;        // 本TCP/IP通信连接的本地IP地址
    uint            remotePort;         // 本TCP/IP通信连接的远程端口
    char*           remoteIpAddr;       // 本TCP/IP通信连接的远程IP地址
    SOCKET          sockID;

protected:
    
public:
    void CreateServer();                        // 建立服务器连接
    void CreateClient();                        // 建立客户端连接
    SOCKET Accept();                            // 工作在服务器模式时接受远程客户端的连接
    void Bind1();                               // 做网络连接的端口及IP地址的绑定
    void Bind2();
    bool Connect();                             // 工作在客户端模式时连接远程服务器
    void Listen();                              // 工作在服务器模式时开始等待远程客户端的连接
    virtual int Receive(SOCKET socket);         // 从网络连接的另一端接收数据
    virtual int Send(SOCKET socket);            // 向网络连接的另一端发送数据
    void Close();                               // 关闭服务器或客户端连接
    void Elapse(int  timeout); //定时函数
};

#endif  // __BASICTCP_H__
