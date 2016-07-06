#include "HistoryVideoManager.h"
#include "DateTime.h"
#include "Debugger.h" 

HistoryVideoManager::HistoryVideoManager()
{
}

HistoryVideoManager::~HistoryVideoManager()
{
}

int HistoryVideoManager::PlayHistoryVideo(DateTime startTime, DateTime endTime)     // ����HistoryVideoPlayer����ָ��ʱ��ε���Ƶ��������ÿ������һ�ν�����һ���߳�
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
    return 0;
}

void HistoryVideoManager::SetFinishPlayVideo(byte videoID)                          // ������Ƶ�Ѿ�������ϱ�־
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
}

bool HistoryVideoManager::IsFinishPlayVideo()                                       // ����VideoManager��ѯ��Ƶ�Ƿ��Ѿ��������
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
    return true;
}

void HistoryVideoManager::SetPlaySpeed(byte playSpeed)                              // ������Ƶ�����ٶ�
{
    PRINT(ALWAYS_PRINT, "VideoNode", __FUNCTION__, __LINE__);
}

