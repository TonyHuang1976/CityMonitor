#pragma once
#include "Thread.h"

#define TMP_BUFFER_LEN  1024

class Buffer;
class VideoRecorder;

class Recorder : public Thread
{
public:
    Recorder(VideoRecorder* videoRecorder, Buffer* buffer);
    virtual ~Recorder();

private:
    VideoRecorder*  videoRecorder;
    Buffer*         buffer;
    byte*           tmpBuffer;
    uint            videoLength;

public:
    virtual void Start();   // ִ�������߳��мƻ�������
    bool SaveLiveVideo(byte* data, uint dataLen);
};

