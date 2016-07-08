
#pragma once
#include "BasicObj.h"
#include "Debugger.h" 
#include "Thread.h"
#include "DateTime.h" 

class VideoManager;
class Buffer;

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

#define DECODE_STATE_CMD_START      1
#define DECODE_STATE_CMD_TYPE       2
#define DECODE_STATE_SITE_ID        3
#define DECODE_STATE_CAM_ID         4
#define DECODE_STATE_CMD_LEN        5
#define DECODE_STATE_CMD_INFO       6
#define DECODE_STATE_CMD_END        7
#define DECODE_STATE_CHECKSUM       8
#define DECODE_STATE_EXECUTE        9

#define CMD_LEN_RESTART             14
#define CMD_LEN_PLAY_REALTIME_VIDEO 0
#define CMD_LEN_PLAY_HISTORY_VIDEO  12
#define CMD_LEN_GET_VIDEO_RECORDS   0

#define STATUS_OK                   1
#define STATUS_ERR                  0

class Codec :public Thread
{
public:
    Codec(VideoManager* videoManager, Buffer* buffer);
    virtual ~Codec(void);

private:
    VideoManager*   videoManager;
    Buffer*         cmdBuffer;
    byte            state;
    int             siteId;
    int             cameraId;
    DateTime        historyVideoStTime;
    DateTime        historyVideoEndTime;

public:
    void Start();

private:
    void ParseDataPackage(VideoManager *manger,byte * buffer,int buflen);
    void SendUpPackage(VideoManager *manger,byte message);
    void SendUpVideoPackage(VideoManager *manger,byte *buffer,int len);
    void ParseDownPackage(VideoManager *manger,byte * buffer,int buflen);
    void SendHeartbeatPacket(VideoManager *manger);
    void SendCameraPos(VideoManager *manger);
    void SendSitePos(VideoManager *manger);
    void SendErrorMessage(VideoManager *manger);
    void SendHistoryVideoList(VideoManager *manger);
    void SendSystemControl(VideoManager *manger);
    void SendHistoryVideoList(void);
};

