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
    // ������Եȼ�
    debugLevel = DEBUG_LEVEL;
    // ����ÿ���ܹ���ӡ������ַ�����
    lineBuffer = new char[MAX_LINE_LENGTH+1];

    // ����ÿ�η��͵����ݵ��ܳ���
    sendlen = BUF_SEND_LEN;
    SendBuf = new char[sendlen];

    char* info = new char[128];
    // ������־�ļ�
    strcpy(info, APP_NAME);
    strcat(info, ".log");
    logger = fopen(info, "w");
    // ����־�ļ��͵��Դ��ڴ�ӡ��Ȩ��������Ϣ
    strcpy(info, COMPANY);
    strcat(info, "    ");
    strcat(info, APP_NAME);
    strcat(info, "  �汾");
    strcat(info, VER);
    strcat(info, "    ");
    strcat(info, __DATE__);
    strcat(info, "  ");
    strcat(info, __TIME__);
    strcat(info, "      All Rights Reserved\n\n");
    printf(info);
    fputs(info, logger);
    fflush(logger);
    // �ͷŶ�̬�ڴ�
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
        lineBuffer[0] = 0;  // ����ַ���
        va_list varList;    // ����б�
        va_start(varList, format); /* �ñ�����͵Ĳ���ȥ��ʼ��varList */
        ExtractArgs(lineBuffer, varList, format);   // ��ȡ���б�εĲ���ֵ
        va_end(varList);  /* �����ͷ�varList !!�� �����Ӱ�쵽��һ�ε��ô˺������ĵط�ʹ�����ֱ�����ȡʱ�Ĺ�����ȷ�ԡ��μ�����ĵ� */
        // ��ӡͨ����ʽת������ַ���
        strcat(lineBuffer, "\n");   // ��ӻ��з�
        printf(lineBuffer);
        // ���ַ���д��log�ļ�
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* fileName, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // ����ַ���
        strcpy(lineBuffer, fileName);
        strcat(lineBuffer, " - ");
        va_list varList;    // ����б�
        va_start(varList, format); /* �ñ�����͵Ĳ���ȥ��ʼ��varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // ��ȡ���б�εĲ���ֵ
        va_end(varList);  /* �����ͷ�varList !!�� �����Ӱ�쵽��һ�ε��ô˺������ĵط�ʹ�����ֱ�����ȡʱ�Ĺ�����ȷ�ԡ��μ�����ĵ� */
        // ��ӡͨ����ʽת������ַ���
        strcat(lineBuffer, "\n");   // ��ӻ��з�
        printf(lineBuffer);
        // ���ַ���д��log�ļ�
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}
void Debugger::Print(byte dbgLevel, char* fileName, byte lineNumber, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // ����ַ���
        strcpy(lineBuffer, fileName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);
        strcat(lineBuffer, "-");
        va_list varList;    // ����б�
        va_start(varList, format); /* �ñ�����͵Ĳ���ȥ��ʼ��varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // ��ȡ���б�εĲ���ֵ
        va_end(varList);  /* �����ͷ�varList !!�� �����Ӱ�쵽��һ�ε��ô˺������ĵط�ʹ�����ֱ�����ȡʱ�Ĺ�����ȷ�ԡ��μ�����ĵ� */
        // ��ӡͨ����ʽת������ַ���
        strcat(lineBuffer, "\n");   // ��ӻ��з�
        printf(lineBuffer);
        // ���ַ���д��log�ļ�
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber, char* format, ...)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // ����ַ���
        strcpy(lineBuffer, className);
        strcat(lineBuffer, "::");
        strcat(lineBuffer, funcName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);
        strcat(lineBuffer, "-");
        va_list varList;    // ����б�
        va_start(varList, format); /* �ñ�����͵Ĳ���ȥ��ʼ��varList */
        ExtractArgs(lineBuffer + strlen(lineBuffer), varList, format);  // ��ȡ���б�εĲ���ֵ
        va_end(varList);  /* �����ͷ�varList !!�� �����Ӱ�쵽��һ�ε��ô˺������ĵط�ʹ�����ֱ�����ȡʱ�Ĺ�����ȷ�ԡ��μ�����ĵ� */

        SendToMonitorServer(lineBuffer, strlen(lineBuffer));  //���͵�Զ�˷�����

        // ��ӡͨ����ʽת������ַ���
        strcat(lineBuffer, "\n");   // ��ӻ��з�
        printf(lineBuffer);
        // ���ַ���д��log�ļ�
        fputs(lineBuffer, logger);
        fflush(logger);
    }
}

void Debugger::Print(byte dbgLevel, char* className, const char* funcName, byte lineNumber)
{
    if (dbgLevel <= debugLevel) 
    { 
        lineBuffer[0] = 0;  // ����ַ���
        strcpy(lineBuffer, className);
        strcat(lineBuffer, "::");
        strcat(lineBuffer, funcName);
        strcat(lineBuffer, ":");
        sprintf(lineBuffer + strlen(lineBuffer), "L%d", lineNumber);

        SendToMonitorServer(lineBuffer, strlen(lineBuffer));  //���͵�Զ�˷�����

        // ��ӡͨ����ʽת������ַ���
        strcat(lineBuffer, "\n");   // ��ӻ��з�
        printf(lineBuffer);
        // ���ַ���д��log�ļ�
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
                
    for (; *formatPtr != 0; formatPtr++)  // ����������ֹͣѭ��
    {
        /* ������ǿ����ַ� */
        if (*formatPtr != '%')
        {
            if (index >= MAX_LINE_LENGTH)
            {
                // ����ӡ�����ݳ��ȳ������д�ӡ����������󳤶ȣ�����������Ϣ����ǰ��ֹ���������򽫷�������ָ��ѰַԽ��ĳ����쳣
                strBuffer[MAX_LINE_LENGTH] = 0;
                return;
            }
            strBuffer[index++] = *formatPtr;
            continue;
        }
        /* ����ǿ����ַ����鿴��һ�ַ� */
        formatPtr++;
        if ('\0' == *formatPtr) /* ����ǽ����� */
        {
            strBuffer[index] = 0;
            break;
        }
        switch (*formatPtr)
        {
            case '%': /* ����������'%' */
                break;
            case 'd': /* ��ȡ32λ���ͱ���ֵ */
                /* �ӱ���б�����ȡһ��32λ���α��� */
                i = va_arg(varList, int);                           
                sprintf(formatBuf, "%d", i); // ������iת��Ϊ�ַ��������Windowsƽ̨�µ�itoa()������ͬ�Ĺ���
                if ((index + strlen(formatBuf)) >= MAX_LINE_LENGTH)
                {
                    // ����ӡ�����ݳ��ȳ������д�ӡ����������󳤶ȣ�����������Ϣ����ǰ��ֹ���������򽫷�������ָ��ѰַԽ��ĳ����쳣
                    strBuffer[index-1] = 0;
                    return;
                }
                strcpy(strBuffer + index, formatBuf);
                // ��indexֵ�������ַ���β��
                index = strlen(strBuffer);
                break;
            case 's': /* ��ȡ�ַ��� */               
                fmtChar = va_arg(varList ,char*);
                strcpy(strBuffer + index, fmtChar);
                index = strlen(strBuffer);
                break;
            case 'c': /* ��ȡ�ַ� */
                /*  ��ô���� */
                /*  ��������ȡ���� */
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

        SendBuf[1] = packet_count++;                // ֡ͬ������������ʱʹ��
        SendBuf[2] = DATA_TYPE_FREE_TEXT;           // ��������Ϊ��������    
        SendBuf[3] = len;       

        byte checksum = 0;
        int i = 0;
        for(; i < len; i++)
        {
            SendBuf[i + 4] = buf[i];
            checksum += (byte)SendBuf[i + 4];
        }   

        SendBuf[i + 4] = checksum;          // ����У���룺ע�⣬������Ϊ�㣬���Բ��ܶ��Ѿ�������У�����packet�����ַ�������       

        //buffwrite->Write((byte*)SendBuf, i + 5);
    }
#endif
}
