#include "Recorder.h"
#include "VideoRecorder.h"
#include "HistoryVideo.h"
#include "Buffer.h" 
#include "Debugger.h" 


Recorder::Recorder(VideoRecorder* videoRecorder, Buffer* buffer)
{
    PRINT(ALWAYS_PRINT, "Recorder", __FUNCTION__, __LINE__);
    this->videoRecorder = videoRecorder;
    this->buffer = buffer;
    tmpBuffer = new byte[TMP_BUFFER_LEN];
    videoLength = 0;
}

Recorder::~Recorder(void)
{
    delete [] tmpBuffer;
}

void Recorder::Start()
{
    PRINT(ALWAYS_PRINT, "Recorder", __FUNCTION__, __LINE__);
    while (true)
    {
        if (!buffer->IsEmpty())
        {
            // 从实时视频流缓冲器里面获取数据，每次最多读TMP_BUFFER_LEN字节数据
            int len = buffer->Read(tmpBuffer, TMP_BUFFER_LEN); 
            // 保存len个字节的实时视频数据
            SaveLiveVideo(tmpBuffer, len);
        }
    }
}

// 存储本地摄像机视频数据
bool Recorder::SaveLiveVideo(byte* data, uint dataLen)              
{
    PRINT(DEBUG_LEVEL_8, "Recorder", __FUNCTION__, __LINE__, " 保存 %d 字节视频数据", dataLen);

    FILE* recordingFile ;//= videoRecorder->GetRecordingFile();

    if (NULL == recordingFile)
    {
        if (!videoRecorder->CreateHistoryVideo())
        {
            return false;
        }
    }

    fwrite(data, sizeof(byte), dataLen, recordingFile);

    videoLength += dataLen;

    if (videoLength > MAX_FILE_LEN)
    {
        uint write_index = videoRecorder->GetHistoryVideos()->GetWriteIndex();
        HistoryVideo* hv = (HistoryVideo*)(videoRecorder->GetHistoryVideos()->GetObjectAt(write_index));
        if (hv != NULL)
        {
            std::string ppath = hv->GetPath();
            remove(ppath.c_str());
        }
        videoRecorder->CloseRecordingFile();
        // 保存当前HistoryVideo信息。如果当前记录位置已经有历史记录存在，那么就用新记录覆盖旧记录
        // 如果当前记录位置没有历史记录存在，就创建一个新记录。
        videoRecorder->RefreshCurHistoryVideo();    
        // 将记录存入文件。删除文件中的第一个记录，将这里新建的记录放在文件最后（通过先删除所有记录再重建所有记录来实现）
        videoRecorder->SaveVideoRecords();         

        videoLength = 0;
        return true;
    }
    return true;
}

