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

// 双端口读出循环缓冲器：第一端口为主要读出端口，第二端口为次要读出端口。这里维持2个读指针，2个缓冲器状态标志。
// 2个端口的数据读出互不影响，2个端口在缓冲器空状态也都是正确的，但缓冲器满状态则仅仅是基于第一主要读指针进行判
// 断的。也就是说，当缓冲器出现满状态时，第二读出端口可能会因为数据被覆盖而丢失数据，也可能是第二端口还没有满。
// 这一切取决于，2个端口的数据读出速度以及数据的写入速度。
// 所以当2个端口都以写入速度更快的速度读出数据，那么上述问题可以避免，如用于命令接收缓冲器时就属于这种安全的情况
//
// ！！！当只使用一个读出端口时，必须使用第一读出端口！！！！否则缓冲器状态将不可能正常，即缓冲器永远不会满！！！
class Buffer : BasicObj
{
public:
    Buffer(uint bufLen, byte bufID);
    virtual ~Buffer();

private:
    byte    bufID;                          // 缓冲器ID
    uint    bufLen;                         // 缓冲器数据单元个数    
    uint    rdIndex;                        // 循环缓冲器第一当前读地址
    uint    rdIndexExt;                     // 循环缓冲器第二当前读地址
    uint    wrIndex;                        // 循环缓冲器当前写地址
    byte*   buffer;                         // 循环数据缓冲器
    byte    status;                         // 循环缓冲器第一状态
    byte    statusExt;                      // 循环缓冲器第二状态
    bool    lock;                           // 多线程访问时的互斥锁，保证在任何时刻只有一个线程访问该Buffer

public:
    uint    Read(byte* rdBuf, uint len);    // 使用第一当前读地址读出数据。返回实际读出的字节数量。
    uint    ReadExt(byte* rdBuf, uint len); // 使用第二当前读地址读出数据。返回实际读出的字节数量。
    uint    Write(byte* wrBuf, uint len);   // 将指定缓冲器内的指定数据段写入循环缓冲器。返回实际写入的字节数量。
    uint    GetUnusedSpace();               // 获取该缓冲器还可以写入的数据字节数量
    uint    GetValidDataLen();              // 使用第一当前读地址读出数据时缓冲器中仍有的有效数据字节数量
    uint    GetValidDataLenExt();           // 使用第二当前读地址读出数据时缓冲器中仍有的有效数据字节数量
    bool    IsLocked();
    bool    IsEmpty();                      // 询问第一读出端口是否无数据
    bool    IsEmptyExt();                   // 询问第二读出端口是否无数据
    bool    IsFull();
    void    TakeLock();
    void    ReleaseLock();
};