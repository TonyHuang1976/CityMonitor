/**
 * ���߳�ģ��
 * �����ˣ����֡�����
 */
#ifndef _THREAD_H_
#define _THREAD_H_
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

class  BasicTCP;

class Thread : public BasicObj
{
public:
    Thread();
    Thread(BasicTCP* tcpInstance) ;
    virtual ~Thread();

private:
    ulong       threadID;           // �߳�ID���Ժ����ư�ȫ����ʱʹ��
    bool        initDone;           // �Ƿ���ɳ�ʼ����־
    BasicTCP*   basicTCP;//TCP����

protected:
    bool        isTerminted;        // �Ƿ��Ѿ�ֹͣ��־
    ulong       lastError;          // �߳������������һ�γ��ֵĴ���
  
public:
    void        Init();             // ��ʼ���߳�
    void        Stop();             // �˳��߳�
    ulong       GetError();         // ��ȡ���һ�γ��ֵĴ���
    bool        IsInitDone();       // ��ѯ�̳߳�ʼ���Ƿ����
    bool        IsTerminted();      // ��ѯ��ǰ�߳��Ƿ�ֹͣ

public:
    virtual void Start();       // ִ�������߳��мƻ�������
};
#endif