#include "VideoManager.h"
#include "VideoChannel.h" 
#include "HistoryVideoPlayer.h" 
#include "TcpConn.h" 
#include "RemoteCmd.h" 
#include "Buffer.h" 
#include "Debugger.h" 

#define     CMD_BUFFER_LEN          256
#define     VIDEO_BUFFER_LEN        4096

VideoManager::VideoManager()
{
    debugger = new Debugger();
    char* localAddr = GetLocalIpAddr();
    char* leftNodeAddr  = GetLeftNodeIpAddr(localAddr);
    char* rightNodeAddr = GetRightNodeIpAddr(localAddr);
    Buffer* cmdBuffer  = new Buffer(CMD_BUFFER_LEN);
    Buffer* cmdAckBuffer = new Buffer(CMD_BUFFER_LEN);
    Buffer* localVideoReadBuffer  = new Buffer(VIDEO_BUFFER_LEN);
    Buffer* localVideoWriteBuffer = new Buffer(VIDEO_BUFFER_LEN);

    // 创建将来自左邻节点的下行命令转发至右邻节点的客户端
    cmdClient4RightNode = new TcpConn(MODE_CLIENT, this, cmdBuffer, cmdAckBuffer);
    cmdClient4RightNode->SetLocalInfo(localAddr);
    cmdClient4RightNode->SetRemoteInfo(rightNodeAddr, CMD_RELAY_CONN_PORT);

    // 建立TCP/IP客户端
    cmdClient4RightNode->CreateClient();

    // 创建接收来自左邻节点的下行命令的服务器
    cmdServer4LeftNode = new TcpConn(MODE_SERVER, this, cmdAckBuffer, cmdBuffer);
    cmdServer4LeftNode->SetLocalInfo(localAddr, CMD_RELAY_CONN_PORT);

    // 建立TCP/IP服务器
    cmdServer4LeftNode->CreateServer();

/*
    // 创建本地摄像机视频数据接收服务器
    videoServer4LocalCam = new TcpConn(MODE_SERVER, this, localVideoReadBuffer, localVideoWriteBuffer);
    videoServer4LocalCam->SetLocalInfo(localAddr, LOCAL_CAMERA_CONN_PORT);
    // 建立TCP/IP服务器
    videoServer4LocalCam->CreateServer();
*/
}

VideoManager::~VideoManager()
{
    delete debugger;
    delete leftNodeVideoConn;
    delete cmdServer4LeftNode;
    delete videoServer4LocalCam;
    delete videoServer4RightNode;
}

void VideoManager::Run()
{
//  PRINT(ALWAYS_PRINT, "VideoManager::Run()\n");
    SetPlaySpeed(12);
    SaveLocalVideo(0, 122);
    AddHistoryVideoPlayer(9);
}
void VideoManager::CreateVideoClient(char* serverAddr, byte serverPort)
{
    PRINT(ALWAYS_PRINT, "VideoManager::CreateClient()\n");
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
    return LOCAL_IP_ADDR;
}
// 获取右邻节点的IP地址
char* VideoManager::GetRightNodeIpAddr(char* localIPAddress)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LeftNodeIpAddr = %s", RIGHTNODE_IP_ADDR);
    return RIGHTNODE_IP_ADDR;
}
void VideoManager::SetPlaySpeed(byte playSpeed)
{
//  PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__, " playSpeed = %d", playSpeed);
}
void VideoManager::SaveLocalVideo(byte* data, uint dataLen) 
{
//  PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, "VideoManager::SaveLocalVideo - dataLen = %d", dataLen);
}
void VideoManager::AddHistoryVideoPlayer(byte videoID)
{
//  PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " videoID = %d", videoID);
}
void VideoManager::SetLocalLiveVideo(byte* data, uint dataLen)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " dataLen = %d", dataLen);
}
void VideoManager::SetRemoteLiveVideo(byte* data, uint dataLen)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " dataLen = %d", dataLen);
}
void VideoManager::SetHistoryVideo(byte videoID, byte* data, uint dataLen)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " dataLen = %d", dataLen);
}
void VideoManager::SetRemoteCommand(byte* command, uint dataLen)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " dataLen = %d", dataLen);
}
void VideoManager::DispatchVideo()
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__);
}
