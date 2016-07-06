
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
    VideoManager*   manager;                    // ����Ƶ�ڵ����Ƶ����ģ��
    list<HistoryVideo>*  recordedVideos;    // ������Ƶ�ڵ����ʷ��Ƶ�嵥�б�
    bool            finishPlayVideo;            // ��ɱ�����Ƶ���ŵı�־
    byte            numVideoPlaying;            // ���ڲ��ŵ���ʷ��Ƶ������
    HistoryVideo*   historyVideo;               // ָ�����ڲ��ŵ���ʷ��Ƶ��ָ�룬���ܲ���Ҫ
    byte            playSpeed;                  // �����ٶ�: 1 - 30���ֱ��Ӧÿ�� 1 - 30 ֡

public: 
    int             PlayHistoryVideo(DateTime startTime, DateTime endTime);     // ����HistoryVideoPlayer����ָ��ʱ��ε���Ƶ��������ÿ������һ�ν�����һ���߳�
    void            SetFinishPlayVideo(byte videoID);                           // ������Ƶ�Ѿ�������ϱ�־
    bool            IsFinishPlayVideo();                                        // ����VideoManager��ѯ��Ƶ�Ƿ��Ѿ��������
    void            SetPlaySpeed(byte playSpeed);                               // ������Ƶ�����ٶ�
};