/**
 * 调试管理模块。按照预设的调试等级进行有条件文字打印和VCD故障诊断信息创建和记录，然后通过TCP/IP的方式将这些调试信息传回给开发者。
 * 负责人：董超
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
	TcpConn*		TcpConnect;		// 本视频节点调试器用TCP/IP通信模块
	byte			localPort;		// 本视频节点调试器的本地TCP/IP通信端口
	char*			localIpAddr;	// 本视频节点调试器的本地IP地址
	byte			remotePort;		// 本视频节点调试器的远程连接TCP/IP通信端口
	char*			remoteIpAddr;	// 本视频节点调试器的远程连接IP地址
	byte			debugLevel;		// 调试等级
	Buffer*			buffread;       // 读数据buff 
	Buffer*			buffwrite;      // 写数据buff

public:
	void			ExtractArgs(char* strBuffer, va_list varList, char* format);
	void			Print(byte dbgLevel, char* format, ...);		// 打印调试信息
	void			Print(byte dbgLevel, char* VideoRecorderName, char* format, ...);		// 打印调试信息
	void			Print(byte dbgLevel, char* VideoRecorderName, byte lineNumber, char* format, ...);		// 打印调试信息
	void			Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber, char* format, ...);		// 打印调试信息
	void			Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber);
	void			SetDebugLevel(byte debugLevel);		// 设置调试等级
public:
	void            SendToMonitorServer(char* buf, int len);
//protected:
	char *          SendBuf;            // 发往监控服务器的数据长度  
	int             sendlen;            // 整个发往远端的数据长度 

};
