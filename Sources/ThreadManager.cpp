#include "ThreadManager.h"
#include "Debugger.h"
#include <cstring>
ThreadManager *ThreadManager::_instance = NULL;  

ThreadManager::ThreadManager()
{
}
ThreadManager::ThreadManager(const ThreadManager& t)
{
}
ThreadManager& ThreadManager::operator=(const ThreadManager& t)
{
    return *this;
}

ThreadManager::~ThreadManager()
{

}

bool ThreadManager::AddSocketThread(SOCKET socketID, Sender *sender, Receiver *receiver)
{
    SocketNode socketnode_;
    socketnode_.socketID  = socketID;
    socketnode_.sender    = sender;
    socketnode_.receiver = receiver;

    struct sockaddr_in peeraddr;
    int len = sizeof(peeraddr);
#ifdef __WIN32__
    int ret = getpeername(socketID, (struct sockaddr *)&peeraddr, &len);
#else
    int ret = getpeername(socketID, (struct sockaddr *)&peeraddr, (socklen_t *)&len);
#endif
    if(ret < 0)
    {
        return false;
    }

    socketnode_.port = ntohs(peeraddr.sin_port);
    strcpy(socketnode_.ip, inet_ntoa(peeraddr.sin_addr));

    socketList.push_back(socketnode_);

    PRINT(ALWAYS_PRINT, "ThreadManager", __FUNCTION__, __LINE__, "添加ip = %s, 端口 = %d 进入管理器 , pSender = %d, pReceiver = %d", socketnode_.ip, socketnode_.port, sender, receiver);

    return true;
}

bool ThreadManager::AddSocketThread(SOCKET socketID, Receiver *receiver)
{
    std::list<SocketNode>::iterator itor;
    itor = socketList.begin();
    for (itor = socketList.begin(); itor != socketList.end(); itor++)
    {
        if (itor->socketID == socketID)
        {
            itor->receiver = receiver;
            PRINT(ALWAYS_PRINT, "ThreadManager", __FUNCTION__, __LINE__, "修改ip = %s, 端口 = %d的socket， pReceiver = %d", itor->ip, itor->port, receiver);
            return true;
        }
    }

    return AddSocketThread(socketID, NULL, receiver);
}

bool ThreadManager::AddSocketThread(SOCKET socketID, Sender *sender)
{
    std::list<SocketNode>::iterator itor;
    itor = socketList.begin();
    for (itor = socketList.begin(); itor != socketList.end(); itor++)
    {
        if (itor->socketID == socketID)
        {
            itor->sender = sender;
            PRINT(ALWAYS_PRINT, "ThreadManager", __FUNCTION__, __LINE__, "修改ip = %s, 端口 = %d的socket， pSender = %d", itor->ip, itor->port, sender);
            return true;
        }
    }

    return AddSocketThread(socketID, sender, NULL);
}

ThreadManager* ThreadManager::GetInstance()  
{  
    if(_instance == NULL)  
    {  
        _instance = new ThreadManager();  
    }  
    return _instance;  
}  
