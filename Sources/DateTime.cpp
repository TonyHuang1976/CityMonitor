#include "DateTime.h"
#include "Debugger.h" 
#include <time.h>

DateTime::DateTime()
{
    time_t timep;
    tm *p;
    time(&timep);
    p=localtime(&timep);

    year    = p->tm_year - 100; //c++���Ǵ�1900�꿪ʼ���
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
// int tm_sec; /* �� �C ȡֵ����Ϊ[0,59] */
// int tm_min; /* �� - ȡֵ����Ϊ[0,59] */
// int tm_hour; /* ʱ - ȡֵ����Ϊ[0,23] */
// int tm_mday; /* һ�����е����� - ȡֵ����Ϊ[1,31] */
// int tm_mon; /* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
// int tm_year; /* ��ݣ���ֵ����ʵ����ݼ�ȥ1900 */
// int tm_wday; /* ���� �C ȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
// int tm_yday; /* ��ÿ���1��1�տ�ʼ������ �C ȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
// int tm_isdst; /* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/
// }
int DateTime::TimeDiffer(DateTime time1, DateTime time2) 
{
    PRINT(ALWAYS_PRINT, "DateTime", __FUNCTION__, __LINE__);

    tm tmObj1 , tmObj2; 
    tmObj1.tm_year = time1.GetYear() + 100;  //c++���Ǵ�1900�꿪ʼ���
    tmObj1.tm_mon  = time1.GetMouth();  
    tmObj1.tm_mday = time1.GetDay();  
    tmObj1.tm_hour = time1.GetHour();  
    tmObj1.tm_min  = time1.GetMin();  
    tmObj1.tm_sec  = time1.GetSec();  
    tmObj1.tm_isdst= -1;  

    tmObj2.tm_year = time2.GetYear() + 100;  //c++���Ǵ�1900�꿪ʼ���
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