#pragma once
#include "Global.h" 
#include "BasicObj.h" 
#include <errno.h>
#include "Sender.h"
#include "Receiver.h"
#include <list>

#ifdef __WIN32__

#include <winsock.h> 
#define bzero(a, b)     memset(a, 0, b)
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/types.h>    // for socket
	#include <netinet/in.h>
	#include <signal.h>
	#include <assert.h>
	#define SOCKET int
#endif

typedef struct SocketNode
{
	SOCKET   socketID;       //socket��
	Sender   *sender;        //socket����sender�ķ����߳�
	Receiver *receiver;		 //socket����pReceiver�Ľ����߳�
	char	 ip[20];		 //��SOCKET�ŵ�ip��ַ
	int		 port;           //��SOCKET�ŵĶ˿ڵ�ַ
};

class ThreadManager : public BasicObj
{
public:
	virtual ~ThreadManager();
	static ThreadManager* GetInstance();

public:
	bool AddSocketThread(SOCKET socketID, Sender *sender, Receiver *receiver);
	bool AddSocketThread(SOCKET socketID, Receiver *receiver);
	bool AddSocketThread(SOCKET socketID, Sender *sender);

protected:
	std::list<SocketNode>   socketList;

private:
	static ThreadManager *_instance;
private:
	ThreadManager();
	ThreadManager(const ThreadManager& t);
	ThreadManager& operator=(const ThreadManager& t);
};