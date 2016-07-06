
#pragma once
#include "Global.h" 
#include <list>
#include "BasicObj.h" 

class VideoManager;
class HistoryVideo;
class DateTime;

class HistoryVideoManager : BasicObj
{
public:
    HistoryVideoManager();
    virtual ~HistoryVideoManager();

private:
    VideoManager*   manager;                    // 本视频节点的视频管理模块
    list<HistoryVideo>*  recordedVideos;    // 本地视频节点的历史视频清单列表
    bool            finishPlayVideo;            // 完成本次视频播放的标志
    byte            numVideoPlaying;            // 正在播放的历史视频的数量
    HistoryVideo*   historyVideo;               // 指向正在播放的历史视频的指针，可能不需要
    byte            playSpeed;                  // 播放速度: 1 - 30，分别对应每秒 1 - 30 帧

public: 
    int             PlayHistoryVideo(DateTime startTime, DateTime endTime);     // 运用HistoryVideoPlayer播放指定时间段的视频，本函数每被调用一次将创建一个线程
    void            SetFinishPlayVideo(byte videoID);                           // 设置视频已经播放完毕标志
    bool            IsFinishPlayVideo();                                        // 用于VideoManager查询视频是否已经播放完毕
    void            SetPlaySpeed(byte playSpeed);                               // 设置视频播放速度
};