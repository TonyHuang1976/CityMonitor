#include "Thread.h"
#include "BasicTCP.h" 
#include "Debugger.h" 

#ifdef __WIN32__
#include <Windows.h>
#else
#include <pthread.h> 
#include <assert.h>
// 在Linux下定义WINAPI为空
#define     WINAPI      
#endif

ulong WINAPI ThreadFunc(void* param) 
{ 
    Thread* thread = (Thread*) param;
    thread->Start(); 
    return 1;
} 
void*  ThreadHandler(void* arg){
    ThreadFunc(arg);
    return NULL;
}
Thread::Thread()
{
    isTerminted = true;
    lastError = 0;
    threadID = 0;

}
Thread::Thread(BasicTCP* tcpInstance)
{
    isTerminted = true;
    lastError = 0;
    threadID = 0;
    basicTCP = tcpInstance ;

}
Thread::~Thread()
{
}
void Thread::Init()
{
#ifdef __WIN32__
    CreateThread( 
        NULL,               // no security attributes 
        0,                  // use default stack size  
        ThreadFunc,         // thread function 
        this,               // argument to thread function 
        0,                  // use default creation flags 
        &threadID);         // returns the thread identifier 
#else
int result=pthread_create(&threadID,NULL,ThreadHandler,this);
assert( 0 == result );
#endif
}

ulong Thread::GetError()
{
    return lastError;
}

void Thread::Stop()
{
    isTerminted = true;
}

bool Thread::IsTerminted()
{
    return isTerminted;
}

bool Thread::IsInitDone()
{
    return initDone;
}

void Thread::Start() 
{
}
