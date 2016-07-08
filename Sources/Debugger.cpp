#include "Debugger.h"
#include <iostream>
#include "stdlib.h"
#include <cstring>
#include "TcpConn.h"

#define DATA_PACKET_HEADER              0x5C
#define BUF_SEND_LEN                    256
#define DATA_TYPE_FREE_TEXT             3  


Debugger::Debugger()
{
    // 定义调试等级
    debugLevel = DEBUG_LEVEL;
    // 定义每行能够打印的最大字符数量
    lineBuffer = new char[MAX_LINE_LENGTH+1];

    // 定义每次发送的数据的总长度
    sendlen = BUF_SEND_LEN;
    SendBuf = new char[sendlen];

    char* info = new char[128];
    // 创建日志文件
    strcpy(info, APP_NAME);
    strcat(info, ".log");
    logger = fopen(info, "w");
    // 在日志文件和调试窗口打印版权声明等信息
    strcpy(info, COMPANY);
    strcat(info, "    ");
    strcat(info, APP_NAME);
    strcat(info, "  版本");
    strcat(info, VER);
    strcat(info, "    ");
    strcat(info, __DATE__);
    strcat(info, "  ");
    strcat(info, __TIME__);
    strcat(info, "      All Rights Reserved\n\n");
    printf(info);
    fputs(info, logger);
    fflush(logger);
    // 释放动态内存
    delete [] info;
/*
    buffread  = new Buffer(BUF_SEND_LEN,1);
    buffwrite = new Buffer(BUF_SEND_LEN,2);

    TcpConnect = new TcpConn(MODE_CLIENT, 0,NULL, buffread, buffwrite);
    TcpConnect->SetRemoteInfo("192.168.1.153", 5002);
    TcpConnect = NULL;
*/
}

Debugger::~Debugger()
{
    delete [] lineBuffer;
}

void Debugger::Print(byte dbgLevel, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // 清空字符串
        va_list varList;    // 变参列表
        va_start(varList, format); /* 用变参类型的参数去初始化varList */
        ExtractArgs(lineBuffer, varList, format);   // 提取所有变参的参数值
        va_end(varList);  /* 必须释放varList !!！ 否则会影响到下一次调用此函数或别的地方使用这种变量提取时的功能正确性。参见相关文档 */
        // 打印通过格式转换后的字符串
        strcat(lineBuffer, "\n");   // 添加换行符
        printf(lineBuffer);
        // 将字符串写入log文件
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* fileName, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // 清空字符串
        strcpy(lineBuffer, fileName);
        strcat(lineBuffer, " - ");
        va_list varList;    // 变参列表
        va_start(varList, format); /* 用变参类型的参数去初始化varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // 提取所有变参的参数值
        va_end(varList);  /* 必须释放varList !!！ 否则会影响到下一次调用此函数或别的地方使用这种变量提取时的功能正确性。参见相关文档 */
        // 打印通过格式转换后的字符串
        strcat(lineBuffer, "\n");   // 添加换行符
        printf(lineBuffer);
        // 将字符串写入log文件
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}
void Debugger::Print(byte dbgLevel, char* fileName, byte lineNumber, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // 清空字符串
        strcpy(lineBuffer, fileName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);
        strcat(lineBuffer, "-");
        va_list varList;    // 变参列表
        va_start(varList, format); /* 用变参类型的参数去初始化varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // 提取所有变参的参数值
        va_end(varList);  /* 必须释放varList !!！ 否则会影响到下一次调用此函数或别的地方使用这种变量提取时的功能正确性。参见相关文档 */
        // 打印通过格式转换后的字符串
        strcat(lineBuffer, "\n");   // 添加换行符
        printf(lineBuffer);
        // 将字符串写入log文件
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // 清空字符串
        strcpy(lineBuffer, className);
        strcat(lineBuffer, "::");
        strcat(lineBuffer, funcName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);
        strcat(lineBuffer, "-");
        va_list varList;    // 变参列表
        va_start(varList, format); /* 用变参类型的参数去初始化varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // 提取所有变参的参数值
        va_end(varList);  /* 必须释放varList !!！ 否则会影响到下一次调用此函数或别的地方使用这种变量提取时的功能正确性。参见相关文档 */

        SendToMonitorServer(lineBuffer, strlen(lineBuffer));  //发送到远端服务器

        // 打印通过格式转换后的字符串
        strcat(lineBuffer, "\n");   // 添加换行符
        printf(lineBuffer);
        // 将字符串写入log文件
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // 清空字符串
        strcpy(lineBuffer, className);
        strcat(lineBuffer, "::");
        strcat(lineBuffer, funcName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);

        SendToMonitorServer(lineBuffer, strlen(lineBuffer));  //发送到远端服务器

        // 打印通过格式转换后的字符串
        strcat(lineBuffer, "\n");   // 添加换行符
        printf(lineBuffer);
        // 将字符串写入log文件
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::SetDebugLevel(byte debugLevel) 
{
    this->debugLevel = debugLevel;
}
void Debugger::ExtractArgs(char* strBuffer, va_list varList, char* format)
{
    int i;
    char* formatPtr = format;
    char formatBuf[MAX_FORMAT_LENGTH];  
    int index = 0;  
    char* fmtChar = 0 ;
                
    for (; *formatPtr != 0; formatPtr++)  // 遇到结束符停止循环
    {
        /* 如果不是控制字符 */
        if (*formatPtr != '%')
        {
            if (index >= MAX_LINE_LENGTH)
            {
                // 欲打印的数据长度超过了行打印缓冲器的最大长度：抛弃该行信息、提前终止解析，否则将发生数组指针寻址越界的程序异常
                strBuffer[MAX_LINE_LENGTH] = 0;
                return;
            }
            strBuffer[index++] = *formatPtr;
            continue;
        }
        /* 如果是控制字符，查看下一字符 */
        formatPtr++;
        if ('\0' == *formatPtr) /* 如果是结束符 */
        {
            strBuffer[index] = 0;
            break;
        }
        switch (*formatPtr)
        {
            case '%': /* 跳过连续的'%' */
                break;
            case 'd': /* 提取32位整型变量值 */
                /* 从变参列表中提取一个32位整形变量 */
                i = va_arg(varList, int);                           
                sprintf(formatBuf, "%d", i); // 将整数i转换为字符串，完成Windows平台下的itoa()函数相同的功能
                if ((index + strlen(formatBuf)) >= MAX_LINE_LENGTH)
                {
                    // 欲打印的数据长度超过了行打印缓冲器的最大长度：抛弃该行信息、提前终止解析，否则将发生数组指针寻址越界的程序异常
                    strBuffer[index-1] = 0;
                    return;
                }
                strcpy(strBuffer + index, formatBuf);
                // 将index值调整到字符串尾部
                index = strlen(strBuffer);
                break;
            case 's': /* 提取字符串 */               
                fmtChar = va_arg(varList ,char*);
                strcpy(strBuffer + index, fmtChar);
                index = strlen(strBuffer);
                break;
            case 'c': /* 提取字符 */
                /*  怎么做？ */
                /*  可以这样取出吗？ */
                //  char c = va_arg(varList,char);
                break;
            default:;
        }
    }   
    strBuffer[index] = 0;
}

void Debugger::SendToMonitorServer(char* buf, int len)
{
#if 0
    static uint packet_count = 0;
    if (TcpConnect != NULL )
    {
        SendBuf[0] = DATA_PACKET_HEADER;

        SendBuf[1] = packet_count++;                // 帧同步计数：测试时使用
        SendBuf[2] = DATA_TYPE_FREE_TEXT;           // 数据类型为自由文字    
        SendBuf[3] = len;       

        byte checksum = 0;
        int i = 0;
        for(; i < len; i++)
        {
            SendBuf[i + 4] = buf[i];
            checksum += (byte)SendBuf[i + 4];
        }   

        SendBuf[i + 4] = checksum;          // 设置校验码：注意，它可能为零，所以不能对已经计算了校验码的packet进行字符串操作       

        //buffwrite->Write((byte*)SendBuf, i + 5);
    }
#endif
}
