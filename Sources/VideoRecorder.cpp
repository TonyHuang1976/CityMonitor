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
mode有下列几种形态字符串:
“r” 以只读方式打开文件，该文件必须存在。
“r+” 以可读写方式打开文件，该文件必须存在。
”rb+“ 读写打开一个二进制文件，允许读写数据，文件必须存在。
“w” 打开只写文件，若文件存在则文件长度清为0，即该文件内容会消失。若文件不存在则建立该文件。
“w+” 打开可读写文件，若文件存在则文件长度清为零，即该文件内容会消失。若文件不存在则建立该文件。
“a” 以附加的方式打开只写文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾，即文件原先的内容会被保留。（EOF符保留）
”a+“ 以附加方式打开可读写的文件。若文件不存在，则会建立该文件，如果文件存在，写入的数据会被加到文件尾后，即文件原先的内容会被保留。 （原来的EOF符不保留）
“wb” 只写打开或新建一个二进制文件；只允许写数据。
“wb+” 读写打开或建立一个二进制文件，允许读和写
“wx” 创建文本文件,只允许写入数据.[C11]
“wbx” 创建一个二进制文件,只允许写入数据.[C11]
”w+x“ 创建一个文本文件,允许读写.[C11]
“wb+x” 创建一个二进制文件,允许读写.[C11]
“w+bx” 和"wb+x"相同[C11]
“rt” 只读打开一个文本文件，只允许读数据
　　“wt” 只写打开或建立一个文本文件，只允许写数据
　　“at” 追加打开一个文本文件，并在文件末尾写数据
　　“rb” 只读打开一个二进制文件，只允许读数据
　　“wb” 只写打开或建立一个二进制文件，只允许写数据
　　“ab” 追加打开一个二进制文件，并在文件末尾写数据
　　“rt+” 读写打开一个文本文件，允许读和写
　　“wt+” 读写打开或建立一个文本文件，允许读写
　　“at+” 读写打开一个文本文件，允许读，或在文件末追加数据
　　“rb+” 读写打开一个二进制文件，允许读和写
　　“ab+” 读写打开一个二进制文件，允许读，或在文件末追加数据
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

// 存储本地摄像机视频数据
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
		RefreshCurHistoryVideo();		//b保存当前HistoryVideo信息

//		delete hv;
		PrintVideoRecords();            //将记录存入文件

		videoLength = 0;
		return true;
	}
	return true;
}

// 获取历史视频文件清单
ObjBuffer* VideoRecorder::GetHistoryVideos()					
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__);
	return recordedVideoList;
}
// 按指定的速度读取视频文件
int	VideoRecorder::ReadVideo(byte* buf, byte speed)						
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__, " speed = %d", speed);
	return 0;
}
// 添加欲播放的历史视频信息进播放清单
void VideoRecorder::AddRecodedVideo(HistoryVideo* historyVideo)		
{
	PRINT(ALWAYS_PRINT, "VideoRecorder", __FUNCTION__, __LINE__);
	recordedVideoList->AddObject(historyVideo);
}
// 寻找包含指定时间的视频
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
void VideoRecorder::ReadVideoRecordsFromFile()                 // 从索引读出日志文件 
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
	fseek(fpindex, 0L, SEEK_END); /* 定位到文件末尾 */
	flen = ftell(fpindex); /* 得到文件大小 */

	fseek(fpindex, 0L, SEEK_SET); /* 定位到文件头 */

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
		case 0:	//开始年
			bgtime = new DateTime;
			bgtime->SetYear(b);
			curindex++; state++;
			break;
		case 1: //开始月
			bgtime->SetMouth(b);
			curindex++; state++;
			break;
		case 2: //开始日
			bgtime->SetDay(b);
			curindex++; state++;
			break;
		case 3: //开始时
			bgtime->SetHour(b);
			curindex++; state++;
			break;
		case 4: //开始分
			bgtime->SetMin(b);
			curindex++; state++;
			break;
		case 5: //开始秒
			bgtime->SetSec(b);
			curindex++; state++;
			break;
		case 6: //结束年
			edtime = new DateTime;
			edtime->SetYear(b);
			curindex++; state++;
			break;
		case 7: //结束月
			edtime->SetMouth(b);
			curindex++; state++;
			break;
		case 8: //结束日
			edtime->SetDay(b);
			curindex++; state++;
			break;
		case 9: //结束时
			edtime->SetHour(b);
			curindex++; state++;
			break;
		case 10: //结束分
			edtime->SetMin(b);
			curindex++; state++;
			break;
		case 11: //结束秒
			edtime->SetSec(b);
			curindex++; state++;
			break;
		case 12: //文件长度1
			filelen += b;
			curindex++; state++;
			break;
		case 13: //文件长度2
			filelen += b<<8;
			curindex++; state++;
			break;
		case 14: //文件长度3
			filelen += b<<16;
			curindex++; state++;
			break;
		case 15: //文件长度4
			filelen += b<<24;
			curindex++; state++;
			break;
		case 16: //路径长度
			pathlen = b;
			curindex++; state++;
			break;
		case 17: //路径
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

bool VideoRecorder::CreateHistoryVideo()                       // 创建新的索引文件并添加到历史视频列表中   
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
            return 1;//文件夹创建成功  
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
