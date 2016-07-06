
#pragma once
#include "BasicObj.h"
#include "Debugger.h" 
#include "DateTime.h" 

class VideoManager;

#define DOWN_COMMEND 0xcc33
#define UP_COMMEND 0x5ac3
#define UP_VIDEO 0x55aa

#define END_MESSAGE_FLAG 0xaa55
#define END_UPVIDEO_FLAG 0x33cc
#define END_UPMESSAGE_FLAG 0x3ca5

#define DOWN_COMMEND_RESTART 1
#define DOWN_COMMEND_REALVIDEO 2
#define DOWN_COMMEND_HISTORYVIDEO 3
#define DOWN_COMMEND_HISTORYLIST 4

#define UP_COMMEND_HEARTBEAT 1
#define UP_COMMEND_VIDEOCAMEAR 2
#define UP_COMMEND_VIDEOSITE 3
#define UP_COMMEND_ERRORMESSAGE 4
#define UP_COMMEND_HISTORYVIDEO 5
#define UP_COMMEND_SYSTEMCONTROL 6

class Codec :public BasicObj
{
public:
    Codec(void);
    ~Codec(void);

    void ParseDataPackage(VideoManager *manger,byte * buffer,int buflen);
    void SendUpPackage(VideoManager *manger,byte message);
    void SendUpVideoPackage(VideoManager *manger,byte *buffer,int len);
private:
    int siteId;
    int cameraId;
    DateTime historyVideoStTime;
    DateTime historyVideoEndTime;

private:
    void ParseDownPackage(VideoManager *manger,byte * buffer,int buflen);
    void SendHeartbeatPacket(VideoManager *manger);
    void SendCameraPos(VideoManager *manger);
    void SendSitePos(VideoManager *manger);
    void SendErrorMessage(VideoManager *manger);
    void SendHistoryVideoList(VideoManager *manger);
    void SendSystemControl(VideoManager *manger);
    void SendHistoryVideoList(void);
};

