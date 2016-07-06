/**
 * ���Թ���ģ�顣����Ԥ��ĵ��Եȼ��������������ִ�ӡ��VCD���������Ϣ�����ͼ�¼��Ȼ��ͨ��TCP/IP�ķ�ʽ����Щ������Ϣ���ظ������ߡ�
 * �����ˣ�����
 */
#pragma once
#include "Global.h" 
#include "stdarg.h"
#include "stdio.h"
#include "Buffer.h"

#define		PRINT				debugger->Print		

#define		ALWAYS_PRINT		0
#define		DEBUG_LEVEL_0		0
#define		DEBUG_LEVEL_1		1
#define		DEBUG_LEVEL_2		2
#define		DEBUG_LEVEL_3		3
#define		DEBUG_LEVEL_4		4
#define		DEBUG_LEVEL_5		5
#define		DEBUG_LEVEL_6		6
#define		DEBUG_LEVEL_7		7
#define		DEBUG_LEVEL_8		8
#define		DEBUG_LEVEL_9		9

#define		DEBUG_LEVEL		DEBUG_LEVEL_6

#define		MAX_FORMAT_LENGTH	32		
#define		MAX_LINE_LENGTH		512		
#define		RADIX_16			16		
#define		RADIX_10			10		
#define		RADIX_2				2		

class TcpConn;
class Buffer;
class Debugger
{
public:
	Debugger();
	virtual ~Debugger();

private:
	FILE*			logger;
	char*			lineBuffer;
	TcpConn*		TcpConnect;		// ����Ƶ�ڵ��������TCP/IPͨ��ģ��
	byte			localPort;		// ����Ƶ�ڵ�������ı���TCP/IPͨ�Ŷ˿�
	char*			localIpAddr;	// ����Ƶ�ڵ�������ı���IP��ַ
	byte			remotePort;		// ����Ƶ�ڵ��������Զ������TCP/IPͨ�Ŷ˿�
	char*			remoteIpAddr;	// ����Ƶ�ڵ��������Զ������IP��ַ
	byte			debugLevel;		// ���Եȼ�
	Buffer*			buffread;       // ������buff 
	Buffer*			buffwrite;      // д����buff

public:
	void			ExtractArgs(char* strBuffer, va_list varList, char* format);
	void			Print(byte dbgLevel, char* format, ...);		// ��ӡ������Ϣ
	void			Print(byte dbgLevel, char* VideoRecorderName, char* format, ...);		// ��ӡ������Ϣ
	void			Print(byte dbgLevel, char* VideoRecorderName, byte lineNumber, char* format, ...);		// ��ӡ������Ϣ
	void			Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber, char* format, ...);		// ��ӡ������Ϣ
	void			Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber);
	void			SetDebugLevel(byte debugLevel);		// ���õ��Եȼ�
public:
	void            SendToMonitorServer(char* buf, int len);
//protected:
	char *          SendBuf;            // ������ط����������ݳ���  
	int             sendlen;            // ��������Զ�˵����ݳ��� 

};
