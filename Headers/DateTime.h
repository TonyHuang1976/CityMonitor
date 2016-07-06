/**
 * 时间管理模块。功能如下：
 * 1. 有年月日时分秒
 * 2. 实时时钟
 * 3. 时间差计算
 * 4. 时间比较（及大于、小于、等于）
 * 5. 时间同步
 * 负责人：董超
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

class DateTime : BasicObj
{
public:
    DateTime();
    virtual ~DateTime();

private:
    byte year;
    byte month;
    byte day;
    byte hour;
    byte minute;
    byte second;

public:
    void TimeSync();
    byte Compare(byte b1, byte b2);  // 大小比较
    byte Compare(DateTime time1, DateTime time2);       // 返回 0 - 相等，1 - 大于，2 - 小于
    int TimeDiffer(DateTime time1, DateTime time2);     // 返回秒数
    bool SetTime(byte year_, byte mouth_, byte day_, byte hour_, byte minute_, byte second_); //设置时间
    
    void SetYear(byte b);               //设置年
    void SetMouth(byte b);              //设置月
    void SetDay(byte b);                //设置日
    void SetHour(byte b);               //设置时
    void SetMin(byte b);                //设置分
    void SetSec(byte b);                //设置秒

    byte GetYear();             //获取年
    byte GetMouth();            //获取月
    byte GetDay();              //获取日
    byte GetHour();             //获取时
    byte GetMin();              //获取分
    byte GetSec();              //获取秒
};
