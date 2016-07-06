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
	SOCKET   socketID;       //socket号
	Sender   *sender;        //socket号下sender的发送线程
	Receiver *receiver;		 //socket号下pReceiver的接收线程
	char	 ip[20];		 //该SOCKET号的ip地址
	int		 port;           //该SOCKET号的端口地址
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