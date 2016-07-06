/**
 ************************************************************************************************************************
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ����
 * Name        : ObjBuffer.h
 * Author      : ����
 * Version     : V1.0.0
 * Copyright   : ������������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ���ɵ�����£��κ�������������
 *				 ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : ��Ƶ��Ԫ����ģ�顣
 ************************************************************************************************************************
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-11  ����    �½�
 * 2016-06-22  ����    ���Linux �� Windows�汾 V1.0.0
 ************************************************************************************************************************/
#pragma once
#include "Global.h" 
#include "BasicObj.h"    

class Debugger;

/**
 * ˵������������λ�ö���ָ���ȡ������ָ��ѭ��д�빦�ܵĶ���ָ�뻺���������������������󡣱�ģ����һ�����̲߳�����ȫģ�顣
 * ���ܣ�
 * 1. ��������д�루AddObject����ѭ���Ķ����Ǹ������ʵģ����д�뵥Ԫ�Ѿ����ж���ָ�룬caller����ɾ�������ǵ�ָ����ָ��Ķ���
 *    д����ɺ�дָ���Զ���һָ����һ����Ԫ��
 * 2. �������ڶ���ָ��Ķ�������������ģ���caller��GetObjectAt�������ָ��
 * 3. �������ڶ���ָ�����ʹ��GetNextObject˳����������������ǵ�ǰ��ָ����ָ��ĵ�Ԫ����һ����Ԫ�����ý����󣬶�ָ��ָ��ʵ�ʶ����ĵ�Ԫ��
 * 4. ������ѭ�����ȿ��Զ�̬���ã������ó�����������������ʱ��������󳤶ȡ�
 */
class ObjBuffer : BasicObj
{
public:
	ObjBuffer(uint bufLen);
	virtual ~ObjBuffer();

private:
	uint	bufLen;							// ����������ָ�����	
	uint	circleLen;						// ������ѭ������	
	uint	rdIndex;						// ѭ����������ǰ����ַ
	uint	wrIndex;						// ѭ����������ǰд��ַ
	void**	buffer;							// C++����ָ��ѭ��������
	bool	lock;							// ���̷߳���ʱ�Ļ���������֤���κ�ʱ��ֻ��һ���̷߳��ʸ�Buffer
	uint    numObjects;                     // ��ǰ��Ч����ָ������
public:
	void*	GetObjectAt(uint index);		// �����ȡָ�뽫�����õ�ָ��λ�ã�Ȼ���ȡָ��λ�õĶ���ָ��
	void*	GetNextObject();				// ��ָ���һָ����һ��λ�ã�����ȡ�µĶ�ָ����ָ��Ķ���ָ��
	uint    GetWriteIndex();                // ��ȡдָ��
	void	AddObject(void* obj);			// �Ը��ǵķ�ʽ��ָ���Ķ���ָ��д�뵽��ǰдָ��ָ���λ��
	void	SetCircleLen(uint circleLen);	// ����ѭ����������ѭ������
	uint	GetCircleLen();
	bool	IsLocked();
	void	TakeLock();
	void	ReleaseLock();
	uint	GetNumEntries();
};