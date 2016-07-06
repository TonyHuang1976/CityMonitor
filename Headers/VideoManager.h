/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : VideoManager.h
 * Author      : 董超
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *				 不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : 视频单元管理模块。
 *=======================================================================================================================
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-11  董超    新建
 * 2016-06-xx  董超    完成Windows版本V1.0.0
 *=======================================================================================================================

 一、系统工作原理

1. 视频数据本地存储
2. 视频数据按监控中心电视墙显示需要按需传输
3. 视频数据站内WIFI接力传输
4. 视频数据站外光纤传输
5. 所有节点视频通过WIFI接力方式在站点汇聚后由站点发送至监控中心
6. 当某一站点的特定节点摄像机被选中在电视墙上显示后，监控中心向该站点发送请求命令，该站点将此命令通过WIFI接力的方式发往指定视频单元
7. 该节点在接到命令后，通过WIFI将本节点的视频流传送给左邻视频处理单元或左邻节点
8. 目标节点的左邻节点继续讲此视频流传送给它的左邻节点，如此直到视频流到达站点为止
9. 站点则将此视频通过光纤传送到监控中心

二、每个视频节点的任务

1. 接收本地摄像机视频
2. 存储本地摄像机视频
3. 读取本地摄像机历史视频
4. 接收来自右邻节点的视频
5. 转发来自右邻节点的视频至左邻节点
6. 发送本地摄像机视频至左邻节点
7. 发送本地摄像机历史视频至左邻节点
8. 接收来自左邻节点的命令
9. 执行来自左邻节点的命令
10.转发来自左邻节点的命令至右邻节点


三、视频前端上行消息帧数据格式：

数据结构   消息标识符 + 消息类型 + 站点ID + 摄像机ID + 消息字长度 + 消息字 + 消息结束符 + 校验码
字节数         2            1         2        2          1                     2          1
数值	    0x5AC3                                                            0x3CA5

注：8位校验码值为除校验码自己外本数据包所有数据之和。

消息类型：
1．	心跳：0x1。消息字长度为0，无消息字。无视频数据传输时，每5秒钟传一次心跳包。无视频数据期间超过30秒没有收到心跳包，监控中心可视为该节点离线或失去联系。
2．	视频节点报名：0x2。消息字长度为单字节，其值为8，消息字前4个字节为放大10000000倍的经度，后4个字节为放大10000000倍的纬度，高字节在前。视频节点向监控中心报告自己的存在及其物理位置，用于监控中心软件做节点列表。摄像机ID>=1。
3．	视频站点报名：0x3。消息字长度为0，无消息字。视频站点向监控中心报告自己的存在，用于监控中心软件做站点及所属节点的列表。摄像机ID=0。
4．	工作异常上报：0x4。消息字长度为1字节，其值为1。异常消息按位定义。

消息字值     异常内容
Bit0=1		无法读取指定节点视频数据
Bit1=1		本节点的下一个节点通信异常。如果摄像机ID=0，则表示本站点与所有视频节点失去联系。
Bit2=1		无法读取指定的历史视频段
Bit3=1		不存在指定的历史视频段
Bit4=1		无法定位本节点位置 
Bit5=1		暂无定义 
Bit6=1		暂无定义 
Bit7=1		暂无定义 

5．	历史视频列表：0x5。消息字长度为2字节，消息字为如下结构：
本节点历史视频段个数（2字节） + { 6字节起始时间 + 6字节结束时间 } … … +  { 6字节起始时间 + 6字节结束时间 }
其中起始时间和结束时间为该历史视频列表中的某一段视频的录制起始时间和结束时间。时间分别为年、月、日、时、分、秒。

6．	系统监测：0x6。消息字长度为2字节，消息字为ASCII字符串。监控中心在控制台打印消息字并按站点名称分文件存储。同时将消息字发往124.162.183.185:6666。

四、视频前端上行视频数据格式：

数据结构 消息标识符 + 站点ID + 摄像机ID + 开始时间 + 结束时间 + 数据长度 + 视频数据 + 结束符 + 校验码
字节数        2			2         2          6         6           4                    2        2
数值	   0x55AA                                                                     0x33CC

其中：
1．开始时间 – 监控中心指定的历史视频播放开始时间，6字节（分别为年、月、日、时、分、秒）
2．结束时间 – 监控中心指定的历史视频播放结束时间，6字节（分别为年、月、日、时、分、秒）
3．如果开始时间和结束时间均为0，则表示这是实时视频
4．2字节校验码值为除校验码自己外本数据包所有数据之和

*/
#pragma once

#include "Global.h" 
#include "BasicObj.h" 

#define		CMD_DATA_BUF_LEN			256
#define		VIDEO_DATA_BUF_LEN			4096

#define		CMD_RELAY_CONN_PORT			6001
#define		LOCAL_CAMERA_CONN_PORT		6002
#define		VIDEO_RELAY_CONN_PORT		6003

class VideoChannel;
class HistoryVideoPlayer;
class TcpConn;
class RemoteCmd;

class VideoManager : BasicObj
{
public:
	VideoManager();
	virtual ~VideoManager();

private:
	TcpConn*	videoServer4LocalCam;									// 本地视频节点的实时视频
	TcpConn*	videoServer4RightNode;									// 右邻视频节点传送来的视频和命令响应信息
	TcpConn*	cmdServer4LeftNode;										// 接收下行命令的服务器
	TcpConn*	cmdClient4RightNode;									// 向下一个远程视频节点转发下行命令的客户端
	TcpConn*	leftNodeVideoConn;										// 传往下一个远程视频节点的视频
	HistoryVideoPlayer*	historyVideoPlayers;							// 本地文件视频流，最大5个
	byte		numHistoryVideoPlayers;									// 同时播放的本地文件视频流数量，最大5个
	byte		playSpeed;												// 播放速度: 1 - 30，分别对应每秒 1 - 30 帧
	RemoteCmd*	remoteCmd;												// 远程下行命令

public:	
	void	Run();														// 管理系统运行：解析和执行下行命令、获取和存储本地摄像机视频数据、转发及播放视频
	void	CreateVideoClient(char* serverAddr, byte serverPort);		// 创建视频数据传输客户端
	void	SetPlaySpeed(byte playSpeed);								// 设置视频播放速度
	void	SaveLocalVideo(byte* data, uint dataLen);					// 存储视频数据至文件，文件大小限制在100MB，如果本次存储数据长度导致超过了这一限制，则应创建新的HistoryVideo
	void	AddHistoryVideoPlayer(byte videoID);						// 添加同时播放的文件视频流
	void	SetLocalLiveVideo(byte* data, uint dataLen);				// 设置本地视频节点的实时视频数据
	void	SetRemoteLiveVideo(byte* data, uint dataLen);				// 设置上一个远程视频节点传送来的实时视频数据
	void	SetHistoryVideo(byte videoID, byte* data, uint dataLen);	// 设置本地文件视频数据
	void	SetRemoteCommand(byte* command, uint dataLen);				// 设置远程命令
	void	DispatchVideo();											// 传输本地视频、远程视频、及文件视频给下一个视频节点
	char*	GetLocalIpAddr();											// 获取本机的IP地址
	char*	GetLeftNodeIpAddr(char* localIPAddress);					// 获取左邻节点的IP地址
	char*	GetRightNodeIpAddr(char* localIPAddress);					// 获取左邻节点的IP地址
};