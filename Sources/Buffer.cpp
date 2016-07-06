#include "Buffer.h" 
#include "Debugger.h" 


Buffer::Buffer(uint bufLen)
{
	PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ����һ������Ϊ %d ��ѭ��������", bufLen);
	this->bufLen = bufLen;
	buffer = new byte[bufLen];
	rdIndex = 0;
	wrIndex = 0;
	status = BUFFER_EMPTY;
}
Buffer::~Buffer()
{
	PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ɾ��һ��ѭ��������");
	delete [] buffer;
}
uint Buffer::Read(byte* rdBuf, uint len)
{
	// ��ȡ����������ֹ�����̶߳Ա�������в���
	lock = true;	
	// ����Ƿ������ݿ��Զ�ȡ
	if (BUFFER_EMPTY == status) 
	{ 
		PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ���󣺻������գ���ѭ���������ж�ȡ 0 ���ֽ�");
		return 0; 
	}
	else
	{
		PRINT(DEBUG_LEVEL_5, "Buffer", __FUNCTION__, __LINE__, " ��ѭ���������ж�ȡ %d ���ֽ�", len);
	}

	for (uint i = 0; i < len; i++)
	{
		// �ӻ������ж�ȡ����
		rdBuf[i] = buffer[rdIndex++];
		// ����ָ���Ƿ��Ѿ����ﻺ�����ĵײ�
		if (rdIndex == bufLen)
		{
			// ʵ��ѭ����������ѭ������
			rdIndex = 0;
		}
		// ��黺�����������Ƿ��Ѿ�����
		if (rdIndex == wrIndex)
		{
			// �������ѿ�
			status = BUFFER_EMPTY;
			return i + 1;
		}
	}
	// ���û������ǿշ�����־
	status = BUFFER_OK;
	// �ͷŲ����������������̶߳Ա�������в���
	lock = false;	

	return len;
}
uint Buffer::Write(byte* wrBuf, uint len)
{
	// ��ȡ����������ֹ�����̶߳Ա�������в���
	lock = true;

	// ��黺�����Ƿ��пռ�д��������
	if (BUFFER_OVERFLOW == status) 
	{ 
//		PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ���󣺻�����������ѭ����������д�� 0 ���ֽ�");
		return 0; 
	}
	else
	{
//		PRINT(DEBUG_LEVEL_5, "Buffer", __FUNCTION__, __LINE__, " ��ѭ����������д�� %d ���ֽ�", len);
	}

	for (uint i = 0; i < len; i++)
	{
		// ����������д������
		buffer[wrIndex++] = wrBuf[i];
		// ���дָ���Ƿ��Ѿ����ﻺ�����Ķ���
		if (wrIndex == bufLen)
		{
			// ʵ��ѭ����������ѭ������
			wrIndex = 0;
		}
		// ��黺���������Ƿ��Ѿ�д��
		if (rdIndex == wrIndex)
		{
			// ����������
			status = BUFFER_OVERFLOW;
			return i + 1;
		}
	}
	// ���û������ǿշ�����־
	status = BUFFER_OK;
	// �ͷŲ����������������̶߳Ա�������в���
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
