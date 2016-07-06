/**
 * 循环缓冲器模块
 * 负责人：董超
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
    uint    bufLen;                         // 缓冲器数据单元个数   
    uint    rdIndex;                        // 循环缓冲器当前读地址
    uint    wrIndex;                        // 循环缓冲器当前写地址
    byte*   buffer;                         // 循环数据缓冲器
    byte    status;                         // 循环缓冲器状态
    bool    lock;                           // 多线程访问时的互斥锁，保证在任何时刻只有一个线程访问该Buffer

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