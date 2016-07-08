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
            // ��ʵʱ��Ƶ�������������ȡ���ݣ�ÿ������TMP_BUFFER_LEN�ֽ�����
            int len = buffer->Read(tmpBuffer, TMP_BUFFER_LEN); 
            // ����len���ֽڵ�ʵʱ��Ƶ����
            SaveLiveVideo(tmpBuffer, len);
        }
    }
}

// �洢�����������Ƶ����
bool Recorder::SaveLiveVideo(byte* data, uint dataLen)              
{
    PRINT(DEBUG_LEVEL_8, "Recorder", __FUNCTION__, __LINE__, " ���� %d �ֽ���Ƶ����", dataLen);

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
        // ���浱ǰHistoryVideo��Ϣ�������ǰ��¼λ���Ѿ�����ʷ��¼���ڣ���ô�����¼�¼���Ǿɼ�¼
        // �����ǰ��¼λ��û����ʷ��¼���ڣ��ʹ���һ���¼�¼��
        videoRecorder->RefreshCurHistoryVideo();    
        // ����¼�����ļ���ɾ���ļ��еĵ�һ����¼���������½��ļ�¼�����ļ����ͨ����ɾ�����м�¼���ؽ����м�¼��ʵ�֣�
        videoRecorder->SaveVideoRecords();         

        videoLength = 0;
        return true;
    }
    return true;
}

