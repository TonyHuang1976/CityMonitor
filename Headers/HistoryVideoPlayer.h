
#pragma once
#include "Global.h" 
#include "Thread.h"
#include "DateTime.h"

class VideoManager;
class HistoryVideo;

class HistoryVideoPlayer : public Thread
{
public:
    HistoryVideoPlayer();
    HistoryVideoPlayer(VideoManager* manager, byte videoID, DateTime startTime, DateTime endTime);
    virtual ~HistoryVideoPlayer();

private:
    VideoManager*   manager;                // 本视频节点的视频管理模块
    DateTime        startTime;              // 本次视频播放的结束时间
    DateTime        endTime;                // 本次视频播放的结束时间
    byte            videoID;                // 欲播放的历史视频ID
    HistoryVideo*   historyVideo;           // 指向正在播放的历史视频的指针，调用HistoryVideoManager::FindHistoryVideo(DateTime startTime)获得
    byte            playSpeed;              // 播放速度: 1 - 30，分别对应每秒 1 - 30 帧

public:
    void    run();                          // 在此线程内调用VideoManager::SetHistoryVideo()播放视频。
    void    SetPlaySpeed(byte playSpeed);   // 设置视频播放速度
};