/**
 * ѭ��������ģ��
 * �����ˣ�����
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

#define     BUFFER_OK           0
#define     BUFFER_EMPTY        1
#define     BUFFER_OVERFLOW     2

class Debugger;

class Buffer : BasicObj
{
public:
    Buffer(uint bufLen);
    virtual ~Buffer();

private:
    uint    bufLen;                         // ���������ݵ�Ԫ����   
    uint    rdIndex;                        // ѭ����������ǰ����ַ
    uint    wrIndex;                        // ѭ����������ǰд��ַ
    byte*   buffer;                         // ѭ�����ݻ�����
    byte    status;                         // ѭ��������״̬
    bool    lock;                           // ���̷߳���ʱ�Ļ���������֤���κ�ʱ��ֻ��һ���̷߳��ʸ�Buffer

public:
    uint    Read(byte* rdBuf, uint len);
    uint    Write(byte* wrBuf, uint len);
    uint    GetUnusedSpace();               // 
    uint    GetValidDataLen();
    bool    IsLocked();
    bool    IsEmpty();
    bool    IsFull();
    void    TakeLock();
    void    ReleaseLock();
};