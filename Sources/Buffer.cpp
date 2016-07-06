#include "Buffer.h" 
#include "Debugger.h" 


Buffer::Buffer(uint bufLen)
{
	PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 创建一个长度为 %d 的循环缓冲器", bufLen);
	this->bufLen = bufLen;
	buffer = new byte[bufLen];
	rdIndex = 0;
	wrIndex = 0;
	status = BUFFER_EMPTY;
}
Buffer::~Buffer()
{
	PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 删除一个循环缓冲器");
	delete [] buffer;
}
uint Buffer::Read(byte* rdBuf, uint len)
{
	// 获取操作锁，阻止其它线程对本对象进行操作
	lock = true;	
	// 检查是否有数据可以读取
	if (BUFFER_EMPTY == status) 
	{ 
		PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 错误：缓冲器空，从循环缓冲器中读取 0 个字节");
		return 0; 
	}
	else
	{
		PRINT(DEBUG_LEVEL_5, "Buffer", __FUNCTION__, __LINE__, " 从循环缓冲器中读取 %d 个字节", len);
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
uint Buffer::Write(byte* wrBuf, uint len)
{
	// 获取操作锁，阻止其它线程对本对象进行操作
	lock = true;

	// 检查缓冲器是否有空间写入新数据
	if (BUFFER_OVERFLOW == status) 
	{ 
//		PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " 错误：缓冲器满。向循环缓冲器中写入 0 个字节");
		return 0; 
	}
	else
	{
//		PRINT(DEBUG_LEVEL_5, "Buffer", __FUNCTION__, __LINE__, " 向循环缓冲器中写入 %d 个字节", len);
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
	// 释放操作锁，允许其它线程对本对象进行操作
	lock = false;	

	return len;
}
uint Buffer::GetUnusedSpace()
{
		switch (status)
	{
		case BUFFER_OK: 
			if (wrIndex > rdIndex)	{ return bufLen - (wrIndex - rdIndex);	}
			else { return rdIndex - wrIndex;}

		case BUFFER_EMPTY: 
			return bufLen;

		case BUFFER_OVERFLOW: 
			return 0;
	}
	return 1;
}
uint Buffer::GetValidDataLen()
{
	return bufLen - GetUnusedSpace();
}
bool Buffer::IsLocked()
{
	return lock;
}
bool Buffer::IsEmpty()
{
	return (BUFFER_EMPTY == status);
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
