/**
 * 视频节点管理模块
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

class VideoManager;
class Buffer;
class TcpConn;

class VideoChannel : public BasicObj
{
public:
	VideoChannel();
	VideoChannel(VideoManager* videoManager, char* localIpAddr, byte localPort, char* remoteIpAddr, byte remotePort);
	virtual ~VideoChannel();

private:
	bool			alive;						// 视频流传输标志: TRUE - 正在传输; FALSE - 没有传输
	VideoManager*	manager;					// 本视频节点的视频管理模块
	TcpConn*		TcpConnect;					// 本视频节点的TCP/IP通信模块
	byte			localPort;					// 本视频节点的本地TCP/IP通信端口
	char*			localIpAddr;				// 本视频节点的本地IP地址
	byte			remotePort;					// 本视频节点的远程连接TCP/IP通信端口
	char*			remoteIpAddr;				// 本视频节点的远程连接IP地址
	Buffer*			videoData;					// 视频缓冲区：能容纳一帧视频数据
	uint			validDataLength;			// 缓冲区中的有效数据长度
	byte			playSpeed;					// 播放速度: 1 - 30，分别对应每秒 1 - 30 帧

public:	
	void	CreateBuffer();											// 创建视频缓冲区
	bool	IsAlive();												// 返回视频流传输标志: TRUE - 正在传输; FALSE - 没有传输
	Buffer*	PlayVideo();											// 播放远程摄像机当前视频片段。在这里调用VideoManager::SetLocalLiveVideo()、VideoManager::SetRemoteLiveVideo()
	uint	GetVideoDataLength();									// 获取远程摄像机当前视频片段的数据长度
	void	SetLocalIpAddr(char* ipAddr, byte localPort);		// 设置本地视频处理卡的IP地址和端口
	void	SetRemoteIpAddr(char* ipAddr, byte remotePort);	// 设置欲连接的摄像机的IP地址和端口
	void	ConnectVideoSource();									// 连接指定IP地址和端口的摄像机，如果已经连接了一个，则应该先断开后连接
	void	SetPlaySpeed(byte playSpeed);							// 设置视频播放速度
};
