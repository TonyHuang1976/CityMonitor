/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : TcpConn.cpp
 * Author      : 董超 周林 张建贞
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *               不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : TCP/IP 通信模块
 *=======================================================================================================================
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-11  董超    新建
 * 2016-06-xx  董超    完成架构
 * 2016-06-xx  周林    完成Windows版本V1.0.0
 * 2016-06-xx  张建贞  完成Linux版本V1.0.0
 *=======================================================================================================================
 */
#include "TcpConn.h" 
#include "VideoManager.h" 
#include "Buffer.h" 
#include "Debugger.h" 

TcpConn::TcpConn(byte mode, VideoManager* manager, Buffer* readBuffer, Buffer* writeBuffer)
{
    this->mode = mode;
    this->manager = manager;
    this->readBuffer = readBuffer;
    this->writeBuffer = writeBuffer;
}

TcpConn::TcpConn(byte mode)
{
    this->mode = mode;
}

TcpConn::~TcpConn()
{
}
// 智能组网中删除已有连接重新做网络连接的端口及IP地址的绑定
void TcpConn::Rebind()
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__);
}                       
// 设置网络连接的本地地址和端口
void TcpConn::SetLocalInfo(char* ipAddr)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s", ipAddr);
    localIpAddr = ipAddr;
}                           
// 设置网络连接的本地地址和端口
void TcpConn::SetLocalInfo(char* ipAddr, uint port)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s, port = %d", ipAddr, port);
    localIpAddr = ipAddr;
    localPort = port;
}                           
// 设置网络连接的远程地址和端口
void TcpConn::SetRemoteInfo(char* ipAddr, uint port)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s, port = %d", ipAddr, port);
    remoteIpAddr = ipAddr;
    remotePort = port;
}                       
