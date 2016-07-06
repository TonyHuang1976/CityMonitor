/**
 ************************************************************************************************************************
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : ObjBuffer.cpp
 * Author      : 董超
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *				 不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : 视频单元管理模块。
 ************************************************************************************************************************
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-11  董超    新建
 * 2016-06-22  董超    完成Linux 及 Windows版本 V1.0.0
 ************************************************************************************************************************/
#include "ObjBuffer.h" 
#include "Debugger.h" 
#include <string.h>


/**
 * 说明：具有任意位置对象指针读取及对象指针循环写入功能的对象指针缓冲器，对象可以是任意对象。本模块是一个多线程操作安全模块。
 * 功能：
 * 1. 缓冲器的写入（AddObject）是循环的而且是覆盖性质的，如果写入单元已经存有对象指针，caller必须删除欲覆盖的指针所指向的对象。
 *    写入完成后，写指针自动加一指向下一个单元。
 * 2. 缓冲器内对象指针的读出可以是随机的，由caller在GetObjectAt的入参中指定
 * 3. 缓冲器内对象指针可以使用GetNextObject顺序读出，读出内容是当前读指针所指向的单元的下一个单元。调用结束后，读指针指向实际读出的单元。
 * 4. 缓冲器循环长度可以动态设置，但不得超过创建缓冲器对象时所给的最大长度。
 */
ObjBuffer::ObjBuffer(uint bufLen)
{
	PRINT(ALWAYS_PRINT, "ObjBuffer", __FUNCTION__, __LINE__, " 创建一个长度为 %d 的对象指针循环缓冲器", bufLen);
	
	this->bufLen = bufLen;
	circleLen = bufLen;
	buffer = new void*[bufLen];
	memset(buffer, NULL, sizeof(void*) * bufLen);

	rdIndex = 0;
	wrIndex = 0;
	numObjects = 0;
}
ObjBuffer::~ObjBuffer()
{
	PRINT(ALWAYS_PRINT, "ObjBuffer", __FUNCTION__, __LINE__, " 删除一个对象指针循环缓冲器");

	delete [] buffer;
}
/**
 * 随机读取缓冲器内所存储的对象指针，欲读出的对象指针由caller在入参中指定
 */
void* ObjBuffer::GetObjectAt(uint index)
{
	// 获取操作锁，阻止其它线程对本对象缓冲器进行操作
	lock = true;	
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " 获取位于单元 %d 的ObjBuffer对象指针", index);	

	if (index >= circleLen)
	{
		return NULL;
	}
	rdIndex = index;
	// 释放操作锁，允许其它线程对本对象进行操作
	lock = false;	
	return buffer[rdIndex];
}
/**
 * 获取缓冲器内下一个单元存储的对象指针，读出内容是当前读指针所指向的单元的下一个单元。调用结束后，读指针指向实际读出的单元。
 */
void* ObjBuffer::GetNextObject()
{
	// 获取操作锁，阻止其它线程对本对象缓冲器进行操作
	lock = true;	
	rdIndex++;
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " 获取位于单元 %d 的ObjBuffer对象指针", rdIndex);


	if (rdIndex >= circleLen)
	{
		// 读指针越界，恢复原值
		rdIndex--;
		// 释放操作锁，允许其它线程对本对象进行操作
		lock = false;
		return NULL;
	}
	// 释放操作锁，允许其它线程对本对象进行操作
	lock = false;	
	return buffer[rdIndex];
}
/**
 * 添加指定的对象指针到缓冲器当前写指针所指向的单元。写操作完成后，写指针将被修改为指向下一个单元，如果修改后的写指针超出了缓冲器的循环长度，
 * 写指针将被置零，指向缓冲器的第一个单元。如果写入单元已经存有对象指针，caller必须删除欲覆盖的指针所指向的对象。
 */
void ObjBuffer::AddObject(void* obj)
{
	// 获取操作锁，阻止其它线程对本对象缓冲器进行操作
	lock = true;	
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " 将指定对象指针存入ObjBuffer单元 %d 中", wrIndex);
 
	// 然后将新对象写入到当前位置
	buffer[wrIndex++] = obj;

	// 检查写指针是否已经到达缓冲器的顶部
	if (wrIndex == circleLen)
	{
		// 实现循环缓冲器的循环功能
		wrIndex = 0;
	}
	// 释放操作锁，允许其它线程对本对象进行操作
	lock = false;	

	if (numObjects < circleLen) { numObjects++; }
}
/**
 * 设置缓冲器循环长度，入参值不得超过创建缓冲器对象时所给的最大长度。当缓冲器写指针达到循环长度时写指针将被归零。
 */
void ObjBuffer::SetCircleLen(uint len)
{
	// 获取操作锁，阻止其它线程对本对象缓冲器进行操作
	lock = true;	
	PRINT(DEBUG_LEVEL_1, "ObjBuffer", __FUNCTION__, __LINE__, " 设置对象指针循环缓冲器的循环长度为 %d", wrIndex);
	if (len <= bufLen) { circleLen = len; }
	// 释放操作锁，允许其它线程对本对象进行操作
	lock = false;	
}
bool ObjBuffer::IsLocked()
{
	return lock;
}
void ObjBuffer::TakeLock()
{
	lock = true;
}
void ObjBuffer::ReleaseLock()
{
	lock = false;
}
uint ObjBuffer::GetCircleLen()
{
	return circleLen;
}
uint ObjBuffer::GetWriteIndex()
{
	return wrIndex;
}

uint ObjBuffer::GetNumEntries()
{
	return numObjects;
}