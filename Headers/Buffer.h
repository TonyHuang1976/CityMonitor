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

// ˫�˿ڶ���ѭ������������һ�˿�Ϊ��Ҫ�����˿ڣ��ڶ��˿�Ϊ��Ҫ�����˿ڡ�����ά��2����ָ�룬2��������״̬��־��
// 2���˿ڵ����ݶ�������Ӱ�죬2���˿��ڻ�������״̬Ҳ������ȷ�ģ�����������״̬������ǻ��ڵ�һ��Ҫ��ָ�������
// �ϵġ�Ҳ����˵����������������״̬ʱ���ڶ������˿ڿ��ܻ���Ϊ���ݱ����Ƕ���ʧ���ݣ�Ҳ�����ǵڶ��˿ڻ�û������
// ��һ��ȡ���ڣ�2���˿ڵ����ݶ����ٶ��Լ����ݵ�д���ٶȡ�
// ���Ե�2���˿ڶ���д���ٶȸ�����ٶȶ������ݣ���ô����������Ա��⣬������������ջ�����ʱ���������ְ�ȫ�����
//
// ��������ֻʹ��һ�������˿�ʱ������ʹ�õ�һ�����˿ڣ����������򻺳���״̬������������������������Զ������������
class Buffer : BasicObj
{
public:
    Buffer(uint bufLen, byte bufID);
    virtual ~Buffer();

private:
    byte    bufID;                          // ������ID
    uint    bufLen;                         // ���������ݵ�Ԫ����    
    uint    rdIndex;                        // ѭ����������һ��ǰ����ַ
    uint    rdIndexExt;                     // ѭ���������ڶ���ǰ����ַ
    uint    wrIndex;                        // ѭ����������ǰд��ַ
    byte*   buffer;                         // ѭ�����ݻ�����
    byte    status;                         // ѭ����������һ״̬
    byte    statusExt;                      // ѭ���������ڶ�״̬
    bool    lock;                           // ���̷߳���ʱ�Ļ���������֤���κ�ʱ��ֻ��һ���̷߳��ʸ�Buffer

public:
    uint    Read(byte* rdBuf, uint len);    // ʹ�õ�һ��ǰ����ַ�������ݡ�����ʵ�ʶ������ֽ�������
    uint    ReadExt(byte* rdBuf, uint len); // ʹ�õڶ���ǰ����ַ�������ݡ�����ʵ�ʶ������ֽ�������
    uint    Write(byte* wrBuf, uint len);   // ��ָ���������ڵ�ָ�����ݶ�д��ѭ��������������ʵ��д����ֽ�������
    uint    GetUnusedSpace();               // ��ȡ�û�����������д��������ֽ�����
    uint    GetValidDataLen();              // ʹ�õ�һ��ǰ����ַ��������ʱ�����������е���Ч�����ֽ�����
    uint    GetValidDataLenExt();           // ʹ�õڶ���ǰ����ַ��������ʱ�����������е���Ч�����ֽ�����
    bool    IsLocked();
    bool    IsEmpty();                      // ѯ�ʵ�һ�����˿��Ƿ�������
    bool    IsEmptyExt();                   // ѯ�ʵڶ������˿��Ƿ�������
    bool    IsFull();
    void    TakeLock();
    void    ReleaseLock();
};