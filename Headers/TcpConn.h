/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : TcpConn.h
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
#pragma once
#include "Global.h" 
#include "BasicTCP.h" 

#define     LOCAL_CAMERA_CONNECTION             1
#define     RIGHT_NODE_CONNECTION           2
#define     REMOTE_VIDEO_UNIT_CONNECTION        3


class VideoManager;
class Buffer;

class TcpConn : public BasicTCP
{
public:
    TcpConn(byte mode, VideoManager* manager, Buffer* readBuffer, Buffer* writeBuffer);
    TcpConn(byte mode);
    virtual ~TcpConn();

private:
    byte            config;             // 定义本连接的用途：连接本地摄像机、上一级摄像机、还是下一级视频处理单元
    VideoManager*   manager;            // 本视频节点的视频管理模块

public:
    void SetLocalInfo(char* ipAddr, uint port);     // 设置网络连接的本地地址和端口
    void SetLocalInfo(char* ipAddr);                // 设置网络连接的本地地址和端口
    void SetRemoteInfo(char* ipAddr, uint port);    // 设置网络连接的远程地址和端口
    void Rebind();                                  // 智能组网中删除已有连接重新做网络连接的端口及IP地址的绑定
};
