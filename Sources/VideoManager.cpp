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
 * �����Ƶ�������ĳ�ʼ������ʼ��������
 * 1. ��������ͨ���������Ľ���
 * 2. ��������������Ӧ��������Ϣͨ���Ľ���
 * 3. ������������ݻ�ȡ�������Ľ���
 * 4. �������������ͨ���Ľ���
 * 5. ��Ƶ����������������Ľ���
 * 6. ������Ƶ¼������ܵĴ���
 * 7. ��������������
 * 8. ������ʷ��Ƶ������
 * �����Ƶת�����ܵĿͻ�������Ƶ����������������ڲ���̬������������ÿ����һ�����ڿͻ��˾Ͷ�̬�ش���һ�����ؿͻ��������ڷ�����������
 * ��ɱ�����Ƶ���Ź��ܵĿͻ�����Codec������������ʱ������
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

    // �������������ڽڵ����������ת�������ڽڵ�Ŀͻ���
    cmdClient4RightNode = new TcpConn(MODE_CLIENT, COMMAND_CHAN_CONNECTION, this, cmdBuffer, cmdAckBuffer);
    cmdClient4RightNode->SetLocalInfo(localAddr);
    cmdClient4RightNode->SetRemoteInfo(rightNodeAddr, CMD_RELAY_CONN_PORT);
    // ����TCP/IP�ͻ���
#ifndef NO_RIGHT_SERVER
    cmdClient4RightNode->CreateClient();
#endif

    // ���������������ڽڵ����������ķ�����
    #ifdef NO_RIGHT_SERVER
    cmdServer4LeftNode = new TcpConn(MODE_SERVER, COMMAND_CHAN_CONNECTION, this, cmdBuffer, cmdBuffer);
    #else
    cmdServer4LeftNode = new TcpConn(MODE_SERVER, COMMAND_CHAN_CONNECTION, this, cmdAckBuffer, cmdBuffer);
    #endif
    cmdServer4LeftNode->SetLocalInfo(localAddr, CMD_RELAY_CONN_PORT);
    // ����TCP/IP������
    cmdServer4LeftNode->CreateServer();

    // ���������������Ƶ���ݽ��շ��������������ǲ�֪�������ӵ������IP��ַ������ֻ��ͨ���������ķ�ʽ��ȡ��Ƶ���ݣ��������ͨ��push�ķ�ʽ�������ݣ�
    videoServer4LocalCam = new TcpConn(MODE_SERVER, LOCAL_CAMERA_CONNECTION, this, cameraCtrlBuffer, videoDataBuffer);
    videoServer4LocalCam->SetLocalInfo(localAddr, LOCAL_CAMERA_CONN_PORT);
    // ����TCP/IP������
    videoServer4LocalCam->CreateServer4Cam();

    // ���������������Ƶ¼���
    videoRecorder = new VideoRecorder(videoDataBuffer);

    // ���������������ڽڵ���Ƶ���ķ�����
    videoServer4RightNode = new TcpConn(MODE_SERVER, RIGHT_NODE_CONNECTION, this);
    videoServer4RightNode->SetLocalInfo(localAddr, VIDEO_RELAY_CONN_PORT);
    // ����TCP/IP������
    videoServer4RightNode->CreateServer();

    codec = new Codec(this, cmdBuffer);
    codec->Init();
}
/**
 * �������ϵͳ��������ִ�����
 * ���ϵͳ��������������ģ������������ͷ��ڴ棬Ȼ���ؽ�ϵͳ��
 */
void VideoManager::SystemRestart()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    Reset();            // �ȸ�λVideoManager

    // TODO
    // ����ģ����ȸ�λ�ڴ����
}
/**
 * �������ϵͳʵʱ��Ƶֱ������ִ�����
 */
void VideoManager::PlayRealTimeVideo()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // ����һ�������ڽڵ�����������Ŀͻ������ӷ��ͱ���ʵʱ��Ƶ
}
/**
 * ������ĵ㲥��ʷ��Ƶ����ִ�����
 */
void VideoManager::PlayHistoryVideo(DateTime* startTime, DateTime* endTime)
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // ��HistoryVideoPlayer�д���һ��VideoFileReader��һ�������ڽڵ�����������Ŀͻ�������
}
/**
 * ������Ļ�ȡ¼���б�����ִ�����
 */
void VideoManager::SendHistoyVideoList()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    // TODO
    // ��HistoryVideoManager��ȡ¼���б�������¼���б�д��cmdAckBuffer
}
/**
 * �ͷ��ڴ档
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
 * ���VideoManager��ʼ�������ͷ��ڴ棬Ȼ���ؽ���
 */
void VideoManager::Reset()
{
    PRINT(ALWAYS_PRINT, "VideoManager",  __FUNCTION__, __LINE__);
    //ReleaseMemory();
    //Init();
}
// ��ȡ���ڽڵ��IP��ַ
char* VideoManager::GetLeftNodeIpAddr(char* localIPAddress)
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LeftNodeIpAddr = %s", LEFTNODE_IP_ADDR);
    return LEFTNODE_IP_ADDR;
}
// ��ȡ������IP��ַ��ע�⣺���� "127.0.0.1"��
char* VideoManager::GetLocalIpAddr()
{
    PRINT(ALWAYS_PRINT, "VideoManager", __FUNCTION__, __LINE__, " LocalIpAddr = %s", LOCAL_IP_ADDR);
#ifdef __WIN32__
    return LOCAL_IP_ADDR;
#else
    return localIpAddr;
#endif
}
// ��ȡ���ڽڵ��IP��ַ
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


