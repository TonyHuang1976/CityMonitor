#include "HistoryVideoManager.h"
#include "DateTime.h"
#include "Debugger.h" 

HistoryVideoManager::HistoryVideoManager()
{
}

HistoryVideoManager::~HistoryVideoManager()
{
}

int HistoryVideoManager::PlayHistoryVideo(DateTime startTime, DateTime endTime)     // 运用HistoryVideoPlayer播放指定时间段的视频，本函数每被调用一次将创建一个线程
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
    return 0;
}

void HistoryVideoManager::SetFinishPlayVideo(byte videoID)                          // 设置视频已经播放完毕标志
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
}

bool HistoryVideoManager::IsFinishPlayVideo()                                       // 用于VideoManager查询视频是否已经播放完毕
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
    return true;
}

void HistoryVideoManager::SetPlaySpeed(byte playSpeed)                              // 设置视频播放速度
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
}

