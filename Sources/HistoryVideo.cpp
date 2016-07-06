#include "HistoryVideo.h" 
#include "Debugger.h"
#ifdef __WIN32__
#include <direct.h>
#endif
#include <cstring>

HistoryVideo::HistoryVideo()
{
     startTime = NULL;
     endTime   = NULL;
     videoLength = 0;
}

HistoryVideo::~HistoryVideo()
{
}
/**
 * 文件格式：起始时间
 */
void HistoryVideo::SaveRecord(FILE *findex)
{
    int datalen = 0;
    byte *p = new byte[20 + path.length()];
    p[0] = startTime->GetYear();
    p[1] = startTime->GetMouth();
    p[2] = startTime->GetDay();
    p[3] = startTime->GetHour();
    p[4] = startTime->GetMin();
    p[5] = startTime->GetSec();

    p[6] = endTime->GetYear();
    p[7] = endTime->GetMouth();
    p[8] = endTime->GetDay();
    p[9] = endTime->GetHour();
    p[10] = endTime->GetMin();
    p[11] = endTime->GetSec();

    p[12] = videoLength & 0xFF;
    p[13] = (videoLength >> 8) & 0xFF;
    p[14] = (videoLength >> 16) & 0xFF;
    p[15] = (videoLength >> 24) & 0xFF;

    p[16] = path.length();
    strcpy((char*)p + 17, path.c_str());
    fwrite(p, 1, 17 + p[16], findex);

    delete [] p;
}

bool HistoryVideo::IsInTimeRange(DateTime time)
{
    DateTime t;
    if ((1 == t.Compare(time, *startTime)) &&  (2 == t.Compare(time, *endTime)))
        return true;

    return false;
}

void HistoryVideo::SetStartTime(DateTime* pDateTime)
{
    if (startTime != NULL)
    {
        delete startTime;
    }
    startTime = pDateTime;
}

void HistoryVideo::SetEndTime(DateTime* pDateTime)
{
    if (endTime != NULL)
    {
        delete endTime;
    }
    endTime = pDateTime;
}

void HistoryVideo::SetPath(std::string path_)
{
    path = path_;
}

void HistoryVideo::SetVideoLen(uint videoLength_)
{
    videoLength = videoLength_;
}

std::string HistoryVideo::GetPath()
{
    return path;
}