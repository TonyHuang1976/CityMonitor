/**
 *=======================================================================================================================
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : BasicTCP.h
 * Author      : ����
 * Version     : V1.0.0
 * Copyright   : �����������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ��ɵ�����£��κ�������������
 *               ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : TCP/IP ͨ��ģ��
 *=======================================================================================================================
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-11  ����    �½�
 * 2016-06-xx  ����    ��ɰ汾V1.0.0
 *=======================================================================================================================
 */
#ifndef __BASICTCP_H__

#define __BASICTCP_H__

#include "Global.h" 
#include "BasicObj.h" 
#include <errno.h>

#ifdef __WIN32__
#include <WinSock2.h>
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#define SOCKET int
#endif

#define     MODE_CLIENT     0
#define     MODE_SERVER     1
#define     MAX_RECV_BUF    1024

#define     COMMAND_CHAN_CONNECTION             0
#define     LOCAL_CAMERA_CONNECTION             1
#define     LEFT_NODE_CONNECTION                2
#define     RIGHT_NODE_CONNECTION               3

class Acceptor;
class Buffer;

class BasicTCP : public BasicObj
{
public:
    BasicTCP();
    BasicTCP(byte connectType);
    virtual ~BasicTCP();
    virtual int Receive(SOCKET socket);         // ���������ӵ���һ�˽�������
    virtual int Send(SOCKET socket);            // ���������ӵ���һ�˷�������
private:
    Acceptor*   acceptor;           // ����������ʱ��Ҫͨ��Accept������һ���ͻ������ӣ����������һ���������߳�������

protected:
    char        connTypeStr[4][10]; // �������������ַ���
    char        modeStr[2][10];     // ����ģʽ�����ַ���
    Buffer*     readBuffer;         // һ��TCP/IP���ӷ���������������ݶ���������
    Buffer*     writeBuffer;        // һ��TCP/IP���ӽ���������������ݴ洢������
    byte        status;             // ��TCP/IP���ӵĹ���״̬
    byte        mode;               // ����ģʽ���������Ƿ��������ǿͻ���
    byte        connectType;        // �������ͣ�����ͨ���������������������Ƶ��Ԫ��������Ƶ��Ԫ
    uint        localPort;          // ��TCP/IPͨ�����ӵı��ض˿�
    char*       localIpAddr;        // ��TCP/IPͨ�����ӵı���IP��ַ
    uint        remotePort;         // ��TCP/IPͨ�����ӵ�Զ�̶˿�
    char*       remoteIpAddr;       // ��TCP/IPͨ�����ӵ�Զ��IP��ַ
    SOCKET      sockID;

private:
    void Init();                                // ����һ��TCP/IP��������ĳ�ʼ��
    void Bind();                                // ���������ӵĶ˿ڼ�IP��ַ�İ�
    void Bind2();
    bool Connect();                             // �����ڿͻ���ģʽʱ����Զ�̷�����
    void Listen();                              // �����ڷ�����ģʽʱ��ʼ�ȴ�Զ�̿ͻ��˵�����
    //virtual int Receive(SOCKET socket);           // ���������ӵ���һ�˽�������
    //virtual int Send(SOCKET socket);          // ���������ӵ���һ�˷�������
    void Close();                               // �رշ�������ͻ�������
    void Elapse(int timeout);                   // ��ʱ����
    
public:
    void CreateServer4Cam();                    // �������ӱ���������ķ�����
    void CreateServer();                        // ��������������
    void CreateServerOld();                     // ��������������
    void CreateClient();                        // �����ͻ�������
    SOCKET Accept();                            // �����ڷ�����ģʽʱ����Զ�̿ͻ��˵�����
    SOCKET GetSockID();                         // ��ȡ�����׽���
    void SetReadBuffer(Buffer* readBuffer);     // ����һ��TCP/IP���ӷ���������������ݶ���������
    void SetWriteBuffer(Buffer* writeBuffer);   // ����һ��TCP/IP���ӽ���������������ݴ洢������
};

#endif  // __BASICTCP_H__
