#include "Buffer.h" 
#include "Debugger.h" 

// ˫�˿ڶ���ѭ������������һ�˿�Ϊ��Ҫ�����˿ڣ��ڶ��˿�Ϊ��Ҫ�����˿ڡ�����ά��2����ָ�룬2��������״̬��־��
// 2���˿ڵ����ݶ�������Ӱ�죬2���˿��ڻ�������״̬Ҳ������ȷ�ģ�����������״̬������ǻ��ڵ�һ��Ҫ��ָ�������
// �ϵġ�Ҳ����˵����������������״̬ʱ���ڶ������˿ڿ��ܻ���Ϊ���ݱ����Ƕ���ʧ���ݣ�Ҳ�����ǵڶ��˿ڻ�û������
// ��һ��ȡ���ڣ�2���˿ڵ����ݶ����ٶ��Լ����ݵ�д���ٶȡ�
// ���Ե�2���˿ڶ���д���ٶȸ�����ٶȶ������ݣ���ô����������Ա��⣬������������ջ�����ʱ���������ְ�ȫ�����
//
// ��������ֻʹ��һ�������˿�ʱ������ʹ�õ�һ�����˿ڣ����������򻺳���״̬������������������������Զ������������

Buffer::Buffer(uint bufLen, byte bufID)
{
    PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ����һ�� len = %d��ID = %d ��ѭ��������", bufLen, bufID);
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
    PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ɾ�� ID = %d ��ѭ��������", bufID);
    delete [] buffer;
}
// ͨ����һ�����˿ڶ�ȡ����
uint Buffer::Read(byte* rdBuf, uint len)
{
    // ��ȡ����������ֹ�����̶߳Ա�������в���
    lock = true;    
    // ����Ƿ������ݿ��Զ�ȡ
    if (BUFFER_EMPTY == status) 
    { 
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ���󣺻������գ���ѭ�������� %d ��ͨ����һ�����˿ڶ�ȡ 0 ���ֽ�", bufID);
      return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " ��ѭ�������� %d ��ͨ����һ�����˿ڶ�ȡ %d ���ֽ�", bufID, len);
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
// ͨ���ڶ������˿ڶ�ȡ����
uint Buffer::ReadExt(byte* rdBuf, uint len)
{
    // ��ȡ����������ֹ�����̶߳Ա�������в���
    lock = true;    
    // ����Ƿ������ݿ��Զ�ȡ
    if (BUFFER_EMPTY == statusExt) 
    { 
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ����:�������գ���ѭ�������� %d ��ͨ���ڶ������˿ڶ�ȡ 0 ���ֽ�", bufID);
        return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " ��ѭ�������� %d ��ͨ���ڶ������˿ڶ�ȡ %d ���ֽ�", bufID, len);
    }

    for (uint i = 0; i < len; i++)
    {
        // �ӻ������ж�ȡ����
        rdBuf[i] = buffer[rdIndexExt++];
        // ����ָ���Ƿ��Ѿ����ﻺ�����ĵײ�
        if (rdIndexExt == bufLen)
        {
            // ʵ��ѭ����������ѭ������
            rdIndexExt = 0;
        }
        // ��黺�����������Ƿ��Ѿ�����
        if (rdIndexExt == wrIndex)
        {
            // �������ѿ�
            statusExt = BUFFER_EMPTY;
            return i + 1;
        }
    }
    // ���û������ǿշ�����־
    statusExt = BUFFER_OK;
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
        PRINT(ALWAYS_PRINT, "Buffer", __FUNCTION__, __LINE__, " ����:������ %d ������ѭ����������д�� 0 ���ֽ�", bufID);
      return 0; 
    }
    else
    {
        PRINT(DEBUG_LEVEL_9, "Buffer", __FUNCTION__, __LINE__, " ��ѭ�������� %d ��д�� %d ���ֽ�", bufID, len);
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
    statusExt = BUFFER_OK;
    // �ͷŲ����������������̶߳Ա�������в���
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
    return 0;   // �������е������ƭCompiler
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
    return 0;   // �������е������ƭCompiler
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
