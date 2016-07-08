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
    virtual void Start();   // ִ�������߳��мƻ�������
    SOCKET  Accept();
};

