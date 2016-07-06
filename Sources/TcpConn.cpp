/**
 *=======================================================================================================================
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : TcpConn.cpp
 * Author      : ���� ���� �Ž���
 * Version     : V1.0.0
 * Copyright   : �����������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ��ɵ�����£��κ�������������
 *               ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : TCP/IP ͨ��ģ��
 *=======================================================================================================================
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-11  ����    �½�
 * 2016-06-xx  ����    ��ɼܹ�
 * 2016-06-xx  ����    ���Windows�汾V1.0.0
 * 2016-06-xx  �Ž���  ���Linux�汾V1.0.0
 *=======================================================================================================================
 */
#include "TcpConn.h" 
#include "VideoManager.h" 
#include "Buffer.h" 
#include "Debugger.h" 

TcpConn::TcpConn(byte mode, VideoManager* manager, Buffer* readBuffer, Buffer* writeBuffer)
{
    this->mode = mode;
    this->manager = manager;
    this->readBuffer = readBuffer;
    this->writeBuffer = writeBuffer;
}

TcpConn::TcpConn(byte mode)
{
    this->mode = mode;
}

TcpConn::~TcpConn()
{
}
// ����������ɾ�����������������������ӵĶ˿ڼ�IP��ַ�İ�
void TcpConn::Rebind()
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__);
}                       
// �����������ӵı��ص�ַ�Ͷ˿�
void TcpConn::SetLocalInfo(char* ipAddr)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s", ipAddr);
    localIpAddr = ipAddr;
}                           
// �����������ӵı��ص�ַ�Ͷ˿�
void TcpConn::SetLocalInfo(char* ipAddr, uint port)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s, port = %d", ipAddr, port);
    localIpAddr = ipAddr;
    localPort = port;
}                           
// �����������ӵ�Զ�̵�ַ�Ͷ˿�
void TcpConn::SetRemoteInfo(char* ipAddr, uint port)
{
    PRINT(ALWAYS_PRINT, "TcpConn", __FUNCTION__, __LINE__, "ipAddr = %s, port = %d", ipAddr, port);
    remoteIpAddr = ipAddr;
    remotePort = port;
}                       
