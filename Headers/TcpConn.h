/**
 *=======================================================================================================================
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : TcpConn.h
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
#pragma once
#include "Global.h" 
#include "BasicTCP.h" 


class VideoManager;
class Buffer;

class TcpConn : public BasicTCP
{
public:
    TcpConn(byte mode, byte connectType, VideoManager* manager, Buffer* readBuffer, Buffer* writeBuffer);
    TcpConn(byte mode, byte connectType, VideoManager* manager);
    virtual ~TcpConn();

private:
    byte            connectType;    // ���屾���ӵ���;�����ӱ������������һ���������������һ����Ƶ����Ԫ
    VideoManager*   manager;        // ����Ƶ�ڵ����Ƶ����ģ��

public:
    void SetLocalInfo(char* ipAddr, uint port);     // �����������ӵı��ص�ַ�Ͷ˿�
    void SetLocalInfo(char* ipAddr);                // �����������ӵı��ص�ַ�Ͷ˿�
    void SetRemoteInfo(char* ipAddr, uint port);    // �����������ӵ�Զ�̵�ַ�Ͷ˿�
    void Rebind();                                  // ����������ɾ�����������������������ӵĶ˿ڼ�IP��ַ�İ�
};
