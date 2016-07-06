/**
 * ʱ�����ģ�顣�������£�
 * 1. ��������ʱ����
 * 2. ʵʱʱ��
 * 3. ʱ������
 * 4. ʱ��Ƚϣ������ڡ�С�ڡ����ڣ�
 * 5. ʱ��ͬ��
 * �����ˣ�����
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
    byte Compare(byte b1, byte b2);  // ��С�Ƚ�
    byte Compare(DateTime time1, DateTime time2);       // ���� 0 - ��ȣ�1 - ���ڣ�2 - С��
    int TimeDiffer(DateTime time1, DateTime time2);     // ��������
    bool SetTime(byte year_, byte mouth_, byte day_, byte hour_, byte minute_, byte second_); //����ʱ��
    
    void SetYear(byte b);               //������
    void SetMouth(byte b);              //������
    void SetDay(byte b);                //������
    void SetHour(byte b);               //����ʱ
    void SetMin(byte b);                //���÷�
    void SetSec(byte b);                //������

    byte GetYear();             //��ȡ��
    byte GetMouth();            //��ȡ��
    byte GetDay();              //��ȡ��
    byte GetHour();             //��ȡʱ
    byte GetMin();              //��ȡ��
    byte GetSec();              //��ȡ��
};
