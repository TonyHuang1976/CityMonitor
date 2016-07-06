/**
 * ��Ƶ�ڵ����ģ��
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

class VideoManager;
class Buffer;
class TcpConn;

class VideoChannel : public BasicObj
{
public:
	VideoChannel();
	VideoChannel(VideoManager* videoManager, char* localIpAddr, byte localPort, char* remoteIpAddr, byte remotePort);
	virtual ~VideoChannel();

private:
	bool			alive;						// ��Ƶ�������־: TRUE - ���ڴ���; FALSE - û�д���
	VideoManager*	manager;					// ����Ƶ�ڵ����Ƶ����ģ��
	TcpConn*		TcpConnect;					// ����Ƶ�ڵ��TCP/IPͨ��ģ��
	byte			localPort;					// ����Ƶ�ڵ�ı���TCP/IPͨ�Ŷ˿�
	char*			localIpAddr;				// ����Ƶ�ڵ�ı���IP��ַ
	byte			remotePort;					// ����Ƶ�ڵ��Զ������TCP/IPͨ�Ŷ˿�
	char*			remoteIpAddr;				// ����Ƶ�ڵ��Զ������IP��ַ
	Buffer*			videoData;					// ��Ƶ��������������һ֡��Ƶ����
	uint			validDataLength;			// �������е���Ч���ݳ���
	byte			playSpeed;					// �����ٶ�: 1 - 30���ֱ��Ӧÿ�� 1 - 30 ֡

public:	
	void	CreateBuffer();											// ������Ƶ������
	bool	IsAlive();												// ������Ƶ�������־: TRUE - ���ڴ���; FALSE - û�д���
	Buffer*	PlayVideo();											// ����Զ���������ǰ��ƵƬ�Ρ����������VideoManager::SetLocalLiveVideo()��VideoManager::SetRemoteLiveVideo()
	uint	GetVideoDataLength();									// ��ȡԶ���������ǰ��ƵƬ�ε����ݳ���
	void	SetLocalIpAddr(char* ipAddr, byte localPort);		// ���ñ�����Ƶ������IP��ַ�Ͷ˿�
	void	SetRemoteIpAddr(char* ipAddr, byte remotePort);	// ���������ӵ��������IP��ַ�Ͷ˿�
	void	ConnectVideoSource();									// ����ָ��IP��ַ�Ͷ˿ڵ������������Ѿ�������һ������Ӧ���ȶϿ�������
	void	SetPlaySpeed(byte playSpeed);							// ������Ƶ�����ٶ�
};
