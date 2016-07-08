#include "Buffer.h" 
#include "Debugger.h" 

// 双端口读出循环缓冲器：第一端口为主要读出端口，第二端口为次要读出端口。这里维持2个读指针，2个缓冲器状态标志。
// 2个端口的数据读出互不影响，2个端口在缓冲器空状态也都是正确的，但缓冲器满状态则仅仅是基于第一主要读指针进行判
// 断的。也就是说，当缓冲器出现满状态时，第二读出端口可能会因为数据被覆盖而丢失数据，也可能是第二端口还没有满。
// 这一切取决于，2个端口的数据读出速度以及数据的写入速度。
// 所以当2个端口都以写入速度更快的速度读出数据，那么上述问题可以避免，如用于命令接收缓冲器时就属于这种安全的情况
//
// ！！！当只使用一个读出端口时，必须使用第一读出端口！！！！否则缓冲器状态将不可能正常，即缓冲器永远不会满！！！

Buffer::Buffer(uint bufLen, byte bufID)
{
    PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 创建一个 len = %d、ID = %d 的循环缓冲器", bufLen, bufID);
 this->bufLen = bufLen;
    this->bufID = bufID;
    buffer = new byte[bufLen];
    rdIndex = 0;
    rdIndexExt = 0;
    wrIndex = 0;
    status = BUFFER_EMPTY;
    statusExt = BUFFER_EMPTY;
}
Buffer::~Buffer()
{
    PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 删除 ID = %d 的循环缓冲器", bufID);
    delete [] buffer;
}
// 通过第一读出端口读取数据
uint Buffer::Read(byte* rdBuf, uint len)
{
    // 获取操作锁，阻止其它线程对本对象进行操作
    lock = true;    
    // 检查是否有数据可以读取
    if (BUFFER_EMPTY == status) 
    { 
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 错误：缓冲器空，从循环缓冲器 %d 中通过第一读出端口读取 0 个字节", bufID);
      return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " 从循环缓冲器 %d 中通过第一读出端口读取 %d 个字节", bufID, len);
   }

    for (uint i = 0; i < len; i++)
    {
        // 从缓冲器中读取数据
        rdBuf[i] = buffer[rdIndex++];

        // 检查读指针是否已经到达缓冲器的底部
        if (rdIndex == bufLen)
        {
            // 实现循环缓冲器的循环功能
            rdIndex = 0;
        }
        // 检查缓冲器中数据是否已经读完
        if (rdIndex == wrIndex)
        {
            // 缓冲器已空
            status = BUFFER_EMPTY;
            return i + 1;
        }
    }
    // 设置缓冲器非空非满标志
    status = BUFFER_OK;
    // 释放操作锁，允许其它线程对本对象进行操作
    lock = false;   

    return len;
}
// 通过第二读出端口读取数据
uint Buffer::ReadExt(byte* rdBuf, uint len)
{
    // 获取操作锁，阻止其它线程对本对象进行操作
    lock = true;    
    // 检查是否有数据可以读取
    if (BUFFER_EMPTY == statusExt) 
    { 
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 错误:缓冲器空，从循环缓冲器 %d 中通过第二读出端口读取 0 个字节", bufID);
        return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " 从循环缓冲器 %d 中通过第二读出端口读取 %d 个字节", bufID, len);
    }

    for (uint i = 0; i < len; i++)
    {
        // 从缓冲器中读取数据
        rdBuf[i] = buffer[rdIndexExt++];
        // 检查读指针是否已经到达缓冲器的底部
        if (rdIndexExt == bufLen)
        {
            // 实现循环缓冲器的循环功能
            rdIndexExt = 0;
        }
        // 检查缓冲器中数据是否已经读完
        if (rdIndexExt == wrIndex)
        {
            // 缓冲器已空
            statusExt = BUFFER_EMPTY;
            return i + 1;
        }
    }
    // 设置缓冲器非空非满标志
    statusExt = BUFFER_OK;
    // 释放操作锁，允许其它线程对本对象进行操作
    lock = false;   

    return len;
}

uint Buffer::Write(byte* wrBuf, uint len)
{
    // 获取操作锁，阻止其它线程对本对象进行操作
    lock = true;

    // 检查缓冲器是否有空间写入新数据
    if (BUFFER_OVERFLOW == status) 
    { 
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 错误:缓冲器 %d 满。向循环缓冲器中写入 0 个字节", bufID);
      return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " 向循环缓冲器 %d 中写入 %d 个字节", bufID, len);
   }

    for (uint i = 0; i < len; i++)
    {
        // 往缓冲器中写入数据
        buffer[wrIndex++] = wrBuf[i];
        // 检查写指针是否已经到达缓冲器的顶部
        if (wrIndex == bufLen)
        {
            // 实现循环缓冲器的循环功能
            wrIndex = 0;
        }
        // 检查缓冲器数据是否已经写满
        if (rdIndex == wrIndex)
        {
            // 缓冲器已满
            status = BUFFER_OVERFLOW;
            return i + 1;
        }
    }
    // 设置缓冲器非空非满标志
    status = BUFFER_OK;
    statusExt = BUFFER_OK;
    // 释放操作锁，允许其它线程对本对象进行操作
    lock = false;   

    return len;
}
uint Buffer::GetUnusedSpace()
{
    switch (status)
    {
        case BUFFER_OK: 
            if (wrIndex > rdIndex)  { return bufLen - (wrIndex - rdIndex);  }
            else { return rdIndex - wrIndex;}

        case BUFFER_EMPTY: 
            return bufLen;

        case BUFFER_OVERFLOW: 
            return 0;
    }
    return 0;   // 不会运行到这里，欺骗Compiler
}
uint Buffer::GetValidDataLen()
{
    return bufLen - GetUnusedSpace();
}
uint Buffer::GetValidDataLenExt()
{
    switch (statusExt)
    {
        case BUFFER_OK: 
            if (wrIndex > rdIndexExt)   { return wrIndex - rdIndexExt;  }
            else { return bufLen - (rdIndexExt - wrIndex);}

        case BUFFER_EMPTY: 
            return 0;

        case BUFFER_OVERFLOW: 
            return bufLen;
    }
    return 0;   // 不会运行到这里，欺骗Compiler
}
bool Buffer::IsLocked()
{
    return lock;
}
bool Buffer::IsEmpty()
{
    return (BUFFER_EMPTY == status);
}
bool Buffer::IsEmptyExt()
{
    return (BUFFER_EMPTY == statusExt);
}
bool Buffer::IsFull()
{
    return (BUFFER_OVERFLOW == status);
}
void Buffer::TakeLock()
{
    lock = true;
}
void Buffer::ReleaseLock()
{
    lock = false;
}
