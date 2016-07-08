/**
 * �ļ�����ģ�顣�������£�
 * 1. ��������Ƶ�ļ����ļ�������ֽ�����������100MB���ļ���ʽΪH264������100MB�������������ļ���
 * 2. ���ȴ���ɾ�ķ�ʽɾ��һ���ļ�
 * 3. ��ȡ��ʷ��Ƶ�ļ��嵥
 * 4. ��ָ�����ٶȶ�ȡ��Ƶ��
 * �����ˣ�����
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 
#include <list>
#include "ObjBuffer.h"
#include <stdio.h>
#include <string>

#include "HistoryVideo.h"
#include "Recorder.h"
#include "DateTime.h"
class HistoryVideo; 
class Recorder;
class DateTime;
class Buffer;

/** 
 * ��Ƶ¼�������ļ����ݴ洢��ʽ��

���ݽṹ ��ʼʱ�� + ����ʱ�� + ���ݳ��� + �ļ������� + �ļ���
�ֽ���       6         6         4           4        ���ļ�������ָ��

���У�
1����ʼʱ�� �C �������ָ������ʷ��Ƶ���ſ�ʼʱ�䣬6�ֽڣ��ֱ�Ϊ�ꡢ�¡��ա�ʱ���֡��룩
2������ʱ�� �C �������ָ������ʷ��Ƶ���Ž���ʱ�䣬6�ֽڣ��ֱ�Ϊ�ꡢ�¡��ա�ʱ���֡��룩

 */


#define MAX_FILE_PATH               256
#define MAX_FILE_NUM            6
#define RECORD_DATA_PATH        "g:\\data\\"
#define MAX_FILE_LEN            (10240/5)

class VideoRecorder : BasicObj
{
public:
    VideoRecorder(Buffer* videoDataBuffer);
    virtual ~VideoRecorder();

private:
    char fileindexpath[MAX_FILE_PATH];                                      // �ļ�����·��
    ObjBuffer       *recordedVideoList;                         // ������Ƶ�ڵ����ʷ��Ƶ�嵥�б�  
    Buffer*         videoDataBuffer;
    Recorder*       recorder;

public:
    bool            SaveLiveVideoOld(byte* data, uint dataLen);     // �洢�����������Ƶ����
    int             ReadVideo(byte* buf, byte speed);               // ��ָ�����ٶȶ�ȡ��Ƶ�ļ�
    void            AddRecodedVideo(HistoryVideo* historyVideo);    // ��������ŵ���ʷ��Ƶ��Ϣ�������嵥
    void            AddRecodedVideo(byte* historyVideo, int len);   // ��������ŵ���ʷ��Ƶ��Ϣ�������嵥,��������Ϊ����־�ļ��ж���
    HistoryVideo*   FindHistoryVideo(DateTime startTime);           // Ѱ�Ұ���ָ��ʱ�����Ƶ
    ObjBuffer*      GetHistoryVideos();                             // ��ȡ��ʷ��Ƶ�ļ��嵥
    void            SaveVideoRecords();                             // ����¼�����ļ���ɾ���ļ��еĵ�һ����¼���������½��ļ�¼�����ļ����ͨ����ɾ�����м�¼���ؽ����м�¼��ʵ�֣�
    void            ReadVideoRecordsFromFile();                     // ������������־�ļ� 
    
    int             CheckFileDir(char* dir);                        // ����ļ����Ƿ���ڣ������ھ��½�   
    bool            CreateFile(char* path);                         // ������·���µ��ļ�

    bool            CreateHistoryVideo();                           // �����µ������ļ�����ӵ���ʷ��Ƶ�б���   
    bool            RefreshCurHistoryVideo();                       // ��Ƶ���ļ���ȡ������ϣ����¸���Ƶ����¼��Ϣ  
    FILE*           GetRecordingFile();     
    void            CloseRecordingFile();
protected:
    //��ȡ�����ļ�����Ҫ�Ļ���
    DateTime* bgtime;               // ��������ʼʱ�仺��
    DateTime* edtime;               // ����������ʱ�仺��
    uint filelen;                   // �ļ����Ȼ���
    uint pathlen;                   // ·�����Ȼ���
    std::string path;               // ·������
    int  state;                     // ��ȡ������ǰ��ȡ�ļ������ڵ�ǰ�����ļ�λ��  
    FILE *historyVideoIndexFile;    // �����ʷ��Ƶ��Ϣ��HistoryVideo���������ļ�ָ��

    //д����Ƶ���ݵĻ���
    DateTime*   startTime;          // ������Ƶ����ʼʱ��
    DateTime*   endTime;            // ������Ƶ�Ľ���ʱ��
    std::string pathfile;           // ������Ƶ�Ĵ��λ�ü��ļ���
    uint        videoLength;        // ������Ƶ���ֽڳ���  
    FILE*       recordingFile;      // ��ǰ���ڴ洢��Ƶ���ݵ��ļ�ָ��
    HistoryVideo*  hVideo;          // 
};
