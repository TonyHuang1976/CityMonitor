#include "VideoChannel.h" 
#include "Debugger.h" 
#include "VideoManager.h" 
#include "Buffer.h" 
#include "TcpConn.h" 

VideoChannel::VideoChannel()
{
}

VideoChannel::VideoChannel(VideoManager* videoManager, char* localIpAddr, byte localPort, char* remoteIpAddr, byte remotePort)
{
}

VideoChannel::~VideoChannel()
{
}

void VideoChannel::CreateBuffer()
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__);
}

bool VideoChannel::IsAlive()
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__);
    return true;
}

Buffer* VideoChannel::PlayVideo()
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__);
    return NULL;
}

uint VideoChannel::GetVideoDataLength()
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__);
    return 0;
}

void VideoChannel::SetLocalIpAddr(char* ipAddr, byte localPort) 
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__, "ipAddr = %s, localPort = %d", ipAddr, localPort);
}

void VideoChannel::SetRemoteIpAddr(char* ipAddr, byte remotePort)
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__, "ipAddr = %s, remotePort = %d", ipAddr, remotePort);
}

void VideoChannel::ConnectVideoSource()
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__);
}
void VideoChannel::SetPlaySpeed(byte playSpeed)
{
    PRINT(ALWAYS_PRINT, "VideoChannel", __FUNCTION__, __LINE__, "playSpeed = %d", playSpeed);
}

