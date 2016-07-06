#include "VideoRecorder.h"
#include "Debugger.h" 
#include <cstring>
#include <stdio.h>
#ifdef __WIN32__
	#include <direct.h>
#else
	#include <sys/stat.h>
#endif
/*
mode�����м�����̬�ַ���:
��r�� ��ֻ����ʽ���ļ������ļ�������ڡ�
��r+�� �Կɶ�д��ʽ���ļ������ļ�������ڡ�
��rb+�� ��д��һ���������ļ��������д���ݣ��ļ�������ڡ�
��w�� ��ֻд�ļ������ļ��������ļ�������Ϊ0�������ļ����ݻ���ʧ�����ļ��������������ļ���
��w+�� �򿪿ɶ�д�ļ������ļ��������ļ�������Ϊ�㣬�����ļ����ݻ���ʧ�����ļ��������������ļ���
��a�� �Ը��ӵķ�ʽ��ֻд�ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ�д������ݻᱻ�ӵ��ļ�β�����ļ�ԭ�ȵ����ݻᱻ��������EOF��������
��a+�� �Ը��ӷ�ʽ�򿪿ɶ�д���ļ������ļ������ڣ���Ὠ�����ļ�������ļ����ڣ�д������ݻᱻ�ӵ��ļ�β�󣬼��ļ�ԭ�ȵ����ݻᱻ������ ��ԭ����EOF����������
��wb�� ֻд�򿪻��½�һ���������ļ���ֻ����д���ݡ�
��wb+�� ��д�򿪻���һ���������ļ����������д
��wx�� �����ı��ļ�,ֻ����д������.[C11]
��wbx�� ����һ���������ļ�,ֻ����д������.[C11]
��w+x�� ����һ���ı��ļ�,�����д.[C11]
��wb+x�� ����һ���������ļ�,�����д.[C11]
��w+bx�� ��"wb+x"��ͬ[C11]
��rt�� ֻ����һ���ı��ļ���ֻ���������
������wt�� ֻд�򿪻���һ���ı��ļ���ֻ����д����
������at�� ׷�Ӵ�һ���ı��ļ��������ļ�ĩβд����
������rb�� ֻ����һ���������ļ���ֻ���������
������wb�� ֻд�򿪻���һ���������ļ���ֻ����д����
������ab�� ׷�Ӵ�һ���������ļ��������ļ�ĩβд����
������rt+�� ��д��һ���ı��ļ����������д
������wt+�� ��д�򿪻���һ���ı��ļ��������д
������at+�� ��д��һ���ı��ļ���������������ļ�ĩ׷������
������rb+�� ��д��һ���������ļ����������д
������ab+�� ��д��һ���������ļ���������������ļ�ĩ׷������
  */
VideoRecorder::VideoRecorder()
{
	strcpy(fileindexpath, RECORD_DATA_PATH);

	bgtime = NULL;
	edtime = NULL;
	filelen = 0;
	pathlen = 0;
	path;
	state = 0;
	fpsave = NULL;
	videoLength = 0;
	recordedVideoList = new ObjBuffer(MAX_FILE_NUM);
	  
	ReadVideoRecordsFromFile();  
}

VideoRecorder::~VideoRecorder()
{
}

// �洢�����������Ƶ����
bool VideoRecorder::SaveLiveVideo(byte* data, uint dataLen)				
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__, " dataLen = %d", dataLen);

	if (NULL == fpsave)
	{
		if (!CreateHistoryVideo())
		{
			return false;
		}
	}

	fwrite(data, sizeof(byte), dataLen, fpsave);

	videoLength += dataLen;

	if (videoLength > MAX_FILE_LEN)
	{
		uint write_index = GetHistoryVideos()->GetWriteIndex();
		HistoryVideo* hv = (HistoryVideo*)(GetHistoryVideos()->GetObjectAt(write_index));
		if (hv != NULL)
		{
			std::string ppath = hv->GetPath();
			remove(ppath.c_str());
		}

		fclose(fpsave);
		fpsave = NULL;
		RefreshCurHistoryVideo();		//b���浱ǰHistoryVideo��Ϣ

//		delete hv;
		PrintVideoRecords();            //����¼�����ļ�

		videoLength = 0;
		return true;
	}
	return true;
}

// ��ȡ��ʷ��Ƶ�ļ��嵥
ObjBuffer* VideoRecorder::GetHistoryVideos()					
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__);
	return recordedVideoList;
}
// ��ָ�����ٶȶ�ȡ��Ƶ�ļ�
int	VideoRecorder::ReadVideo(byte* buf, byte speed)						
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__, " speed = %d", speed);
	return 0;
}
// ��������ŵ���ʷ��Ƶ��Ϣ�������嵥
void VideoRecorder::AddRecodedVideo(HistoryVideo* historyVideo)		
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__);
	recordedVideoList->AddObject(historyVideo);
}
// Ѱ�Ұ���ָ��ʱ�����Ƶ
HistoryVideo* VideoRecorder::FindHistoryVideo(DateTime startTime)			
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__);

	ObjBuffer* obj = GetHistoryVideos();

	uint curIndex = obj->GetWriteIndex();
	uint index;	
	uint circleLen = obj->GetCircleLen();
	for (int i = 0; i < circleLen; i++)
	{
		index = (curIndex + i) % circleLen;
		HistoryVideo* p = ((HistoryVideo*)obj->GetObjectAt(index));
			
		if (p->IsInTimeRange(startTime))
		{
			return p;
		}
	}
	return NULL;
}
void VideoRecorder::PrintVideoRecords()
{
	char chpath[MAX_FILE_PATH];
#ifdef __WIN32__
	getcwd(chpath, MAX_FILE_PATH); 
#endif
	strcpy(chpath, fileindexpath);
	strcat(chpath, "index.index");

	FILE *fpindex = fopen(chpath, "wb+");

	ObjBuffer* obj = GetHistoryVideos();

	uint curIndex = obj->GetWriteIndex();
	uint index;	
	uint circleLen = obj->GetCircleLen();
	for (int i = 0; i < circleLen; i++)
	{
		index = (curIndex + i) % circleLen;
		HistoryVideo* p = ((HistoryVideo*)obj->GetObjectAt(index));
			
		if (p != NULL)
		{
			p->SaveRecord(fpindex);		
		}
	}

	fclose(fpindex);
}
void VideoRecorder::ReadVideoRecordsFromFile()                 // ������������־�ļ� 
{
	char chpath[MAX_FILE_PATH];  
#ifdef __WIN32__
	getcwd(chpath, MAX_FILE_PATH); 
#endif
	strcpy(chpath, fileindexpath);
	strcat(chpath, "index.index");

	FILE *fpindex = fopen(chpath, "rb");
	if (NULL == fpindex)
	{
		return;
	}
	uint flen = 0;
	uint flenindex = 0;
	uint freadstep = 10240;
	fseek(fpindex, 0L, SEEK_END); /* ��λ���ļ�ĩβ */
	flen = ftell(fpindex); /* �õ��ļ���С */

	fseek(fpindex, 0L, SEEK_SET); /* ��λ���ļ�ͷ */

	byte buf[1024];
	memset(buf, 0, sizeof(buf));
	if (fpindex != NULL)
	{
		fseek(fpindex, 0L, SEEK_SET);
		uint freadlen = 0;
		while (flenindex < flen)
		{
			freadlen = 0;
			if (flenindex + freadstep < flen) {freadlen = flenindex;} else {freadlen = flen - flenindex;}

			int numread = fread(buf, sizeof(byte), freadlen, fpindex);
			AddRecodedVideo((byte*)buf, numread);
			flenindex += numread;
		}
	}
}

void VideoRecorder::AddRecodedVideo(byte* historyVideo, int len)
{
	int curindex = 0;
	
	while(curindex < len)
	{
		byte b = historyVideo[curindex];
		switch(state)
		{
		case 0:	//��ʼ��
			bgtime = new DateTime;
			bgtime->SetYear(b);
			curindex++; state++;
			break;
		case 1: //��ʼ��
			bgtime->SetMouth(b);
			curindex++; state++;
			break;
		case 2: //��ʼ��
			bgtime->SetDay(b);
			curindex++; state++;
			break;
		case 3: //��ʼʱ
			bgtime->SetHour(b);
			curindex++; state++;
			break;
		case 4: //��ʼ��
			bgtime->SetMin(b);
			curindex++; state++;
			break;
		case 5: //��ʼ��
			bgtime->SetSec(b);
			curindex++; state++;
			break;
		case 6: //������
			edtime = new DateTime;
			edtime->SetYear(b);
			curindex++; state++;
			break;
		case 7: //������
			edtime->SetMouth(b);
			curindex++; state++;
			break;
		case 8: //������
			edtime->SetDay(b);
			curindex++; state++;
			break;
		case 9: //����ʱ
			edtime->SetHour(b);
			curindex++; state++;
			break;
		case 10: //������
			edtime->SetMin(b);
			curindex++; state++;
			break;
		case 11: //������
			edtime->SetSec(b);
			curindex++; state++;
			break;
		case 12: //�ļ�����1
			filelen += b;
			curindex++; state++;
			break;
		case 13: //�ļ�����2
			filelen += b<<8;
			curindex++; state++;
			break;
		case 14: //�ļ�����3
			filelen += b<<16;
			curindex++; state++;
			break;
		case 15: //�ļ�����4
			filelen += b<<24;
			curindex++; state++;
			break;
		case 16: //·������
			pathlen = b;
			curindex++; state++;
			break;
		case 17: //·��
			if (path.length() < pathlen)
			{
				path += (char)b;
			}
			if(path.length() == pathlen)
			{
				state = 0;
				HistoryVideo* h = new HistoryVideo;
				h->SetStartTime(bgtime);
				h->SetEndTime(edtime);
				h->SetPath(path);
				h->SetVideoLen(filelen);
				AddRecodedVideo(h);

				bgtime = NULL;
				edtime = NULL;
				path   = "";
				filelen = 0;
			}
			curindex++;
			break;
		}
	}
}

bool VideoRecorder::CreateHistoryVideo()                       // �����µ������ļ�����ӵ���ʷ��Ƶ�б���   
{
	startTime = new DateTime;
	char path[MAX_FILE_PATH];
	char dir[MAX_FILE_PATH];
	memset(path, '\0', sizeof(path));
	memset(dir, '\0', sizeof(path));
	sprintf(path, "%s%02d%02d%02d\\%02d%02d%02d.h264", fileindexpath, startTime->GetYear(), startTime->GetMouth(), startTime->GetDay(), startTime->GetHour(), startTime->GetMin(), startTime->GetSec());
	sprintf(dir, "%s%02d%02d%02d\\", fileindexpath, startTime->GetYear(), startTime->GetMouth(), startTime->GetDay());

	if (CheckFileDir(fileindexpath) < 0)
	{
		return false;
	}

	if (CheckFileDir(dir) < 0)
	{
		return false;
	}

	if (!CreateFile(path))
	{
		return false;
	}

	pathfile = path;
//	hVideo = new HistoryVideo;
//	hVideo->SetPath(path);

	return true;
}

bool VideoRecorder::RefreshCurHistoryVideo()
{
	endTime = new DateTime;

	ObjBuffer* obj = GetHistoryVideos();
	uint curIndex = obj->GetWriteIndex();

	hVideo = (HistoryVideo*)(obj->GetObjectAt(curIndex));

	if (NULL == hVideo)
	{
		hVideo = new HistoryVideo;
	}

	hVideo->SetStartTime(startTime);
	hVideo->SetEndTime(endTime);
	hVideo->SetVideoLen(videoLength);
	hVideo->SetPath(pathfile);
	GetHistoryVideos()->AddObject(hVideo);

	startTime    = NULL;	 
	endTime      = NULL;	 
	pathfile     = "";	 
	videoLength  = 0;	 
	fpsave       = NULL;  
	hVideo       = NULL;
	
	return true;
}


int VideoRecorder::CheckFileDir(char* dir)
{
	FILE *fp = NULL;  
	char TempFilePath[256];  
 
	strcpy(TempFilePath, dir);  
    strcat(TempFilePath, ".temp.fortest");  
    fp = fopen(TempFilePath,"w+");  

	if (fp == 0)  
    {  
#ifdef __WIN32__
        if(mkdir(dir)==0)  
#else
        if(mkdir(dir,S_IRUSR | S_IWUSR | S_IXUSR)==0)
#endif
        {  
            return 1;//�ļ��д����ɹ�  
        }  
        else  
        {  
            return -1;//can not make a dir;  
        }  
    }  
    else  
    {  
        fclose(fp);  
    }  
    return 0;  
}

bool VideoRecorder::CreateFile(char* path)
{
	fpsave = fopen(path, "wb+");

	if (fp == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}

	return true;
}
