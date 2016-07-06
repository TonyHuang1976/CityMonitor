#include "DateTime.h"
#include "Debugger.h" 
#include <time.h>

DateTime::DateTime()
{
    time_t timep;
    tm *p;
    time(&timep);
    p=localtime(&timep);

    year    = p->tm_year - 100; //c++年是从1900年开始算的
    month   = p->tm_mon;
    day     = p->tm_mday;
    hour    = p->tm_hour;
    minute  = p->tm_min;
    second  = p->tm_sec;
}

DateTime::~DateTime()
{
}

void DateTime::TimeSync()
{
    PRINT(ALWAYS_PRINT, "DateTime", __FUNCTION__, __LINE__);
}

byte DateTime::Compare(byte b1, byte b2)
{
    if (b1 == b2)
    {
        return 0;
    }else if (b1 > b2)
    {
        return 1;
    }else if (b1 < b2)
    {
        return 2;
    }
    return 0;
}

byte DateTime::Compare(DateTime time1, DateTime time2)
{
    PRINT(ALWAYS_PRINT, "DateTime", __FUNCTION__, __LINE__);

    int stat = 0;
    int rest;
    byte b1 = 0;
    byte b2 = 0;
    while(stat < 6)
    {
        b1 = 0;
        b2 = 0;
        switch(stat)
        {
        case 0:
            b1 = time1.GetYear();
            b2 = time2.GetYear();
            break;
        case 1:
            b1 = time1.GetMouth();
            b2 = time2.GetMouth();
            break;
        case 2:
            b1 = time1.GetDay();
            b2 = time2.GetDay();
            break;
        case 3:
            b1 = time1.GetHour();
            b2 = time2.GetHour();
            break;
        case 4:
            b1 = time1.GetMin();
            b2 = time2.GetMin();
            break;
        case 5:
            b1 = time1.GetSec();
            b2 = time2.GetSec();
            break;
        }
        stat++;

        rest = Compare(b1, b2);
        if (rest > 0)
        {
            return rest;
        }
    }
    return 0;
}

// struct tm {
// int tm_sec; /* 秒 – 取值区间为[0,59] */
// int tm_min; /* 分 - 取值区间为[0,59] */
// int tm_hour; /* 时 - 取值区间为[0,23] */
// int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
// int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
// int tm_year; /* 年份，其值等于实际年份减去1900 */
// int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
// int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
// int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/
// }
int DateTime::TimeDiffer(DateTime time1, DateTime time2) 
{
    PRINT(ALWAYS_PRINT, "DateTime", __FUNCTION__, __LINE__);

    tm tmObj1 , tmObj2; 
    tmObj1.tm_year = time1.GetYear() + 100;  //c++年是从1900年开始算的
    tmObj1.tm_mon  = time1.GetMouth();  
    tmObj1.tm_mday = time1.GetDay();  
    tmObj1.tm_hour = time1.GetHour();  
    tmObj1.tm_min  = time1.GetMin();  
    tmObj1.tm_sec  = time1.GetSec();  
    tmObj1.tm_isdst= -1;  

    tmObj2.tm_year = time2.GetYear() + 100;  //c++年是从1900年开始算的
    tmObj2.tm_mon  = time2.GetMouth();  
    tmObj2.tm_mday = time2.GetDay();  
    tmObj2.tm_hour = time2.GetHour();  
    tmObj2.tm_min  = time2.GetMin();  
    tmObj2.tm_sec  = time2.GetSec();  
    tmObj2.tm_isdst= -1;  
    return mktime(&tmObj1) - mktime(&tmObj2);
}


byte DateTime::GetYear()
{
    return year;
}

byte DateTime::GetMouth()
{
    return month;
}

byte DateTime::GetDay()
{
    return day;
}

byte DateTime::GetHour()
{
    return hour;
}

byte DateTime::GetMin()
{
    return minute;
}

byte DateTime::GetSec()
{
    return second;
}

void DateTime::SetYear(byte b)
{
    year = b;
}
void DateTime::SetMouth(byte b)
{
    month = b;
}
void DateTime::SetDay(byte b)
{
    day = b;
}
void DateTime::SetHour(byte b)
{
    hour = b;
}
void DateTime::SetMin(byte b)
{
    minute = b;
}
void DateTime::SetSec(byte b)
{
    second = b;
}