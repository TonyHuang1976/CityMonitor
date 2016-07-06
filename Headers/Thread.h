/**
 * 多线程模块
 * 负责人：周林、董超
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
    ulong       threadID;           // 线程ID，以后完善安全机制时使用
    bool        initDone;           // 是否完成初始化标志
    BasicTCP*   basicTCP;//TCP对象

protected:
    bool        isTerminted;        // 是否已经停止标志
    ulong       lastError;          // 线程运行至今最后一次出现的错误
  
public:
    void        Init();             // 初始化线程
    void        Stop();             // 退出线程
    ulong       GetError();         // 获取最后一次出现的错误
    bool        IsInitDone();       // 查询线程初始化是否完成
    bool        IsTerminted();      // 查询当前线程是否停止

public:
    virtual void Start();       // 执行拟在线程中计划的任务
};
#endif