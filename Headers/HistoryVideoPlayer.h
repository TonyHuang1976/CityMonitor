
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
    VideoManager*   manager;                // ����Ƶ�ڵ����Ƶ����ģ��
    DateTime        startTime;              // ������Ƶ���ŵĽ���ʱ��
    DateTime        endTime;                // ������Ƶ���ŵĽ���ʱ��
    byte            videoID;                // �����ŵ���ʷ��ƵID
    HistoryVideo*   historyVideo;           // ָ�����ڲ��ŵ���ʷ��Ƶ��ָ�룬����HistoryVideoManager::FindHistoryVideo(DateTime startTime)���
    byte            playSpeed;              // �����ٶ�: 1 - 30���ֱ��Ӧÿ�� 1 - 30 ֡

public:
    void    run();                          // �ڴ��߳��ڵ���VideoManager::SetHistoryVideo()������Ƶ��
    void    SetPlaySpeed(byte playSpeed);   // ������Ƶ�����ٶ�
};