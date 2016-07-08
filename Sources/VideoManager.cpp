#include "VideoManager.h"
#include "VideoChannel.h" 
#include "HistoryVideoPlayer.h" 
#include "VideoRecorder.h" 
#include "TcpConn.h" 
#include "RemoteCmd.h" 
#include "Buffer.h" 
#include "Codec.h" 
#include "DateTime.h" 
#include "Debugger.h" 
#include <cstring>

#ifndef __WIN32__
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>    // for socket
#include <netinet/in.h>
#include <signal.h>
#include <assert.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#endif


VideoManager::VideoManager()
{
    Init();
}
VideoManager::~VideoManager()
{
    ReleaseMemory();
}
/**
 * 完成视频管理器的初始化。初始化包括：
 * 1. 下行命令通道服务器的建立
 * 2. 传送下行命令响应的上行信息通道的建立
 * 3. 本地摄像机数据获取服务器的建立
 * 4. 本地摄像机控制通道的建立
 * 5. 视频流接力传输服务器的建立
 * 6. 本地视频录像机功能的创建
 * 7. 创建命令译码器
 * 8. 创建历史视频播放器
 * 完成视频转发功能的客户端在视频流接力传输服务器内部动态建立：服务器每连接一个右邻客户端就动态地创建一个本地客户端与左邻服务器的连接
 * 完成本地视频播放功能的客户端在Codec译码下行命令时创建。
 */
void VideoManager::Init()
{
    debugger = new Debugger();
    GetIpAddr();
    char* localAddr = GetLocalIpAddr();
    char* leftNodeAddr  = GetLeftNodeIpAddr(localAddr);
    char* rightNodeAddr = GetRightNodeIpAddr(localAddr);
    Buffer* cmdBuffer  = new Buffer(CMD_BUFFER_LEN, CMD_BUFFER_ID);
    Buffer* cmdAckBuffer = new Buffer(CMD_BUFFER_LEN, CMD_ACK_BUFFER_ID);
    Buffer* cameraCtrlBuffer  = new Buffer(CAM_CTRL_BUFFER_LEN, CAM_CTRL_BUFFER_ID);
    Buffer* videoDataBuffer = new Buffer(VIDEO_BUFFER_LEN, VIDEO_BUFFER_ID);
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__,"local ip=%s",localAddr);

    // 创建将来自左邻节点的下行命令转发至右邻节点的客户端
    cmdClient4RightNode = new TcpConn(MODE_CLIENT, COMMAND_CHAN_CONNECTION, this, cmdBuffer, cmdAckBuffer);
    cmdClient4RightNode->SetLocalInfo(localAddr);
    cmdClient4RightNode->SetRemoteInfo(rightNodeAddr, CMD_RELAY_CONN_PORT);
    // 建立TCP/IP客户端
#ifndef NO_RIGHT_SERVER
    cmdClient4RightNode->CreateClient();
#endif

    // 创建接收来自左邻节点的下行命令的服务器
    #ifdef NO_RIGHT_SERVER
    cmdServer4LeftNode = new TcpConn(MODE_SERVER, COMMAND_CHAN_CONNECTION, this, cmdBuffer, cmdBuffer);
    #else
    cmdServer4LeftNode = new TcpConn(MODE_SERVER, COMMAND_CHAN_CONNECTION, this, cmdAckBuffer, cmdBuffer);
    #endif
    cmdServer4LeftNode->SetLocalInfo(localAddr, CMD_RELAY_CONN_PORT);
    // 建立TCP/IP服务器
    cmdServer4LeftNode->CreateServer();

    // 创建本地摄像机视频数据接收服务器（由于我们不知道所连接的摄像机IP地址，所以只能通过服务器的方式获取视频数据，让摄像机通过push的方式传输数据）
    videoServer4LocalCam = new TcpConn(MODE_SERVER, LOCAL_CAMERA_CONNECTION, this, cameraCtrlBuffer, videoDataBuffer);
    videoServer4LocalCam->SetLocalInfo(localAddr, LOCAL_CAMERA_CONN_PORT);
    // 建立TCP/IP服务器
    videoServer4LocalCam->CreateServer4Cam();

    // 创建本地摄像机视频录像机
    videoRecorder = new VideoRecorder(videoDataBuffer);

    // 创建接收来自右邻节点视频流的服务器
    videoServer4RightNode = new TcpConn(MODE_SERVER, RIGHT_NODE_CONNECTION, this);
    videoServer4RightNode->SetLocalInfo(localAddr, VIDEO_RELAY_CONN_PORT);
    // 建立TCP/IP服务器
    videoServer4RightNode->CreateServer();

    codec = new Codec(this, cmdBuffer);
    codec->Init();
}
/**
 * 监控中心系统重启命令执行入口
 * 完成系统重启，包括所有模块的重启。先释放内存，然后重建系统。
 */
void VideoManager::SystemRestart()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    Reset();            // 热复位VideoManager

    // TODO
    // 其它模块的热复位在此完成
}
/**
 * 监控中心系统实时视频直播命令执行入口
 */
void VideoManager::PlayRealTimeVideo()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // 创建一个与左邻节点服务器相连的客户端连接发送本地实时视频
}
/**
 * 监控中心点播历史视频命令执行入口
 */
void VideoManager::PlayHistoryVideo(DateTime* startTime, DateTime* endTime)
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // 在HistoryVideoPlayer中创建一个VideoFileReader和一个与左邻节点服务器相连的客户端连接
}
/**
 * 监控中心获取录像列表命令执行入口
 */
void VideoManager::SendHistoyVideoList()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // 从HistoryVideoManager获取录像列表，并将此录像列表写入cmdAckBuffer
}
/**
 * 释放内存。
 */
void VideoManager::ReleaseMemory()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    delete debugger;
    delete videoRecorder;
    delete cmdClient4RightNode;
    delete cmdServer4LeftNode;
    delete videoServer4LocalCam;
    delete videoServer4RightNode;
    delete codec;
}
/**
 * 完成VideoManager初始化。先释放内存，然后重建。
 */
void VideoManager::Reset()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    //ReleaseMemory();
    //Init();
}
// 获取左邻节点的IP地址
char* VideoManager::GetLeftNodeIpAddr(char* localIPAddress)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LeftNodeIpAddr = %s", LEFTNODE_IP_ADDR);
    return LEFTNODE_IP_ADDR;
}
// 获取本机的IP地址（注意：不是 "127.0.0.1"）
char* VideoManager::GetLocalIpAddr()
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LocalIpAddr = %s", LOCAL_IP_ADDR);
#ifdef __WIN32__
    return LOCAL_IP_ADDR;
#else
    return localIpAddr;
#endif
}
// 获取右邻节点的IP地址
char* VideoManager::GetRightNodeIpAddr(char* localIPAddress)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LeftNodeIpAddr = %s", RIGHTNODE_IP_ADDR);
    return RIGHTNODE_IP_ADDR;
}

char* VideoManager::GetIpAddr()
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__);
#ifdef __WIN32__
    return LOCAL_IP_ADDR;
#else
    int sock_get_ip;
    //char ipaddr[50];
    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, "open socket error");
        return "";
    }
    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, (const char *)"eth0", sizeof(ifr_ip.ifr_name) - 1);
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )
    {
        PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, "ioctrl  error");
        return "";
    }
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    //strcpy(ipaddr,inet_ntoa(sin->sin_addr));
    strcpy(localIpAddr,inet_ntoa(sin->sin_addr));
    close( sock_get_ip );
    //PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, "localipaddr=%s",localipaddr);
    return localIpAddr;

#endif
}
void VideoManager::SetPlaySpeed(byte playSpeed)
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__, " playSpeed = %d", playSpeed);
}


