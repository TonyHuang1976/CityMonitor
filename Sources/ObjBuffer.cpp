/**
 ************************************************************************************************************************
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : ObjBuffer.cpp
 * Author      : ����
 * Version     : V1.0.0
 * Copyright   : �����������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ��ɵ�����£��κ�������������
 *				 ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : ��Ƶ��Ԫ����ģ�顣
 ************************************************************************************************************************
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-11  ����    �½�
 * 2016-06-22  ����    ���Linux �� Windows�汾 V1.0.0
 ************************************************************************************************************************/
#include "ObjBuffer.h" 
#include "Debugger.h" 
#include <string.h>


/**
 * ˵������������λ�ö���ָ���ȡ������ָ��ѭ��д�빦�ܵĶ���ָ�뻺���������������������󡣱�ģ����һ�����̲߳�����ȫģ�顣
 * ���ܣ�
 * 1. ��������д�루AddObject����ѭ���Ķ����Ǹ������ʵģ����д�뵥Ԫ�Ѿ����ж���ָ�룬caller����ɾ�������ǵ�ָ����ָ��Ķ���
 *    д����ɺ�дָ���Զ���һָ����һ����Ԫ��
 * 2. �������ڶ���ָ��Ķ�������������ģ���caller��GetObjectAt�������ָ��
 * 3. �������ڶ���ָ�����ʹ��GetNextObject˳����������������ǵ�ǰ��ָ����ָ��ĵ�Ԫ����һ����Ԫ�����ý����󣬶�ָ��ָ��ʵ�ʶ����ĵ�Ԫ��
 * 4. ������ѭ�����ȿ��Զ�̬���ã������ó�����������������ʱ��������󳤶ȡ�
 */
ObjBuffer::ObjBuffer(uint bufLen)
{
	PRINT(ALWAYS_PRINT, "ObjBuffer", __FUNCTION__, __LINE__, " ����һ������Ϊ %d �Ķ���ָ��ѭ��������", bufLen);
	
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
	PRINT(ALWAYS_PRINT, "ObjBuffer", __FUNCTION__, __LINE__, " ɾ��һ������ָ��ѭ��������");

	delete [] buffer;
}
/**
 * �����ȡ�����������洢�Ķ���ָ�룬�������Ķ���ָ����caller�������ָ��
 */
void* ObjBuffer::GetObjectAt(uint index)
{
	// ��ȡ����������ֹ�����̶߳Ա����󻺳������в���
	lock = true;	
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " ��ȡλ�ڵ�Ԫ %d ��ObjBuffer����ָ��", index);	

	if (index >= circleLen)
	{
		return NULL;
	}
	rdIndex = index;
	// �ͷŲ����������������̶߳Ա�������в���
	lock = false;	
	return buffer[rdIndex];
}
/**
 * ��ȡ����������һ����Ԫ�洢�Ķ���ָ�룬���������ǵ�ǰ��ָ����ָ��ĵ�Ԫ����һ����Ԫ�����ý����󣬶�ָ��ָ��ʵ�ʶ����ĵ�Ԫ��
 */
void* ObjBuffer::GetNextObject()
{
	// ��ȡ����������ֹ�����̶߳Ա����󻺳������в���
	lock = true;	
	rdIndex++;
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " ��ȡλ�ڵ�Ԫ %d ��ObjBuffer����ָ��", rdIndex);


	if (rdIndex >= circleLen)
	{
		// ��ָ��Խ�磬�ָ�ԭֵ
		rdIndex--;
		// �ͷŲ����������������̶߳Ա�������в���
		lock = false;
		return NULL;
	}
	// �ͷŲ����������������̶߳Ա�������в���
	lock = false;	
	return buffer[rdIndex];
}
/**
 * ���ָ���Ķ���ָ�뵽��������ǰдָ����ָ��ĵ�Ԫ��д������ɺ�дָ�뽫���޸�Ϊָ����һ����Ԫ������޸ĺ��дָ�볬���˻�������ѭ�����ȣ�
 * дָ�뽫�����㣬ָ�򻺳����ĵ�һ����Ԫ�����д�뵥Ԫ�Ѿ����ж���ָ�룬caller����ɾ�������ǵ�ָ����ָ��Ķ���
 */
void ObjBuffer::AddObject(void* obj)
{
	// ��ȡ����������ֹ�����̶߳Ա����󻺳������в���
	lock = true;	
	PRINT(DEBUG_LEVEL_3, "ObjBuffer", __FUNCTION__, __LINE__, " ��ָ������ָ�����ObjBuffer��Ԫ %d ��", wrIndex);
 
	// Ȼ���¶���д�뵽��ǰλ��
	buffer[wrIndex++] = obj;

	// ���дָ���Ƿ��Ѿ����ﻺ�����Ķ���
	if (wrIndex == circleLen)
	{
		// ʵ��ѭ����������ѭ������
		wrIndex = 0;
	}
	// �ͷŲ����������������̶߳Ա�������в���
	lock = false;	

	if (numObjects < circleLen) { numObjects++; }
}
/**
 * ���û�����ѭ�����ȣ����ֵ���ó�����������������ʱ��������󳤶ȡ���������дָ��ﵽѭ������ʱдָ�뽫�����㡣
 */
void ObjBuffer::SetCircleLen(uint len)
{
	// ��ȡ����������ֹ�����̶߳Ա����󻺳������в���
	lock = true;	
	PRINT(DEBUG_LEVEL_1, "ObjBuffer", __FUNCTION__, __LINE__, " ���ö���ָ��ѭ����������ѭ������Ϊ %d", wrIndex);
	if (len <= bufLen) { circleLen = len; }
	// �ͷŲ����������������̶߳Ա�������в���
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