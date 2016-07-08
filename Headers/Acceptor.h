#pragma once
#include "Thread.h"

#ifdef __WIN32__
#include <WinSock2.h>
#else
#define SOCKET int
#endif

class Acceptor : public Thread
{
public:
    Acceptor(BasicTCP* basicTCP);
    virtual ~Acceptor();

private:
    SOCKET      sockID;

public:
    virtual void Start();   // 执行拟在线程中计划的任务
    SOCKET  Accept();
};

