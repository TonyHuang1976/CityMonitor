/*
 * Copyright (c) 2015-2016 Swift-tech.Inc. All rights reserved.
 *
 * Swift-tech retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Swift-tech., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Swift-tech all copies of this computer program.  If you
 * are licensed by Swift-tech, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Swift-tech.
 *
 * This computer program contains trade secrets owned by Swift-tech.
 * and, unless unauthorized by Swift-tech in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * SWIFT-TECH EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */
 /*
  * File   : SwiftHikSDK.cpp
  * Author : TonyHuang
  * Date   : 2016.07
  */

#include <stdio.h>
#include <iostream>
#include <time.h>
#include "plaympeg4.h"
#include "HCNetSDK.h"
#include "SwiftHikSDK.h"
//#include "VideoRecorder.h"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <process.h>


#if 1
#define sw_err_printf(...)   do{ printf("[SW_ERR] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)

#define sw_inf_printf(...)   do{ printf("[SW_INF] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)

#define sw_deb_printf(...)   do{ printf("[SW_DEB] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

//#define _SAVE_H264_STREAM
#define _PLAY_ON_WINDOW
//#define _REAL_DATA_HANDLE_EN
//#define _TEST_TCP_C
#define _TEST_TCP_S

static LONG _g_lRealPlayHandle = 0;
static LONG _g_lPort = 0;           //ȫ�ֵĲ��ſ�port��
static LONG _g_lUserID = 0;

static SOCKET _g_tcp_c = 0;
static SOCKET _g_tcp_s = 0;
static SOCKET _g_tcp_s_cnt = 0;

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, DWORD dwUser)
{
#ifdef _REAL_DATA_HANDLE_EN

#ifdef _PLAY_ON_WINDOW
    HWND hWnd = GetConsoleWindow();
#endif
    
    //printf("lRealHandle=%d, dwDataType=%d, dwBufSize=%d, dwUser=%d\n", lRealHandle, dwDataType, dwBufSize, dwUser);

    switch (dwDataType)
    {
        case NET_DVR_SYSHEAD: //ϵͳͷ
        {
            if ( !PlayM4_GetPort(&_g_lPort) )  //��ȡ���ſ�δʹ�õ�ͨ����
            {
                break;
            }
            //m_iPort = _g_lPort; //��һ�λص�����ϵͳͷ������ȡ�Ĳ��ſ�port�Ÿ�ֵ��ȫ��port���´λص�����ʱ��ʹ�ô�port�Ų���
            if (dwBufSize > 0)
            {
                if ( !PlayM4_SetStreamOpenMode(_g_lPort, STREAME_REALTIME) )  //����ʵʱ������ģʽ
                {
                    break;
                }
                
                if ( !PlayM4_OpenStream(_g_lPort, pBuffer, dwBufSize, 1024 * 1024) ) //�����ӿ�
                {
                    break;
                }
                #ifdef _PLAY_ON_WINDOW
                if ( !PlayM4_Play(_g_lPort, hWnd) ) //���ſ�ʼ
                {
                    break;
                }
                #else
                if ( !PlayM4_Play(_g_lPort, NULL) ) //���ſ�ʼ
                {
                    break;
                }
                #endif
                
                #ifdef _PRINT_HEXDATA
                int i = 0;
                for ( i = 0; i < 32; i++ )
                {
                    if ( dwBufSize < 32 || pBuffer == NULL )
                    {
                        break;
                    }
                    printf("%d", pBuffer[i]);
                }
                #endif /* ENDIF _PRINT_HEXDATA */

                #ifdef _SAVE_H264_STREAM
                FILE*       fpsave;
                fpsave = fopen("test.record", "ab+");

                if ( fpsave == NULL )
                {
                    printf("err:open file failed\n");
                    return;
                }
                fwrite(pBuffer, dwBufSize, 1, fpsave);
                fclose(fpsave);
                #endif /* ENDIF _SAVE_H264_STREAM */
            }
        }
        
        case NET_DVR_STREAMDATA:   //��������
        {
            if ( dwBufSize > 0 && _g_lPort != -1 )
            {
                if ( !PlayM4_InputData(_g_lPort, pBuffer, dwBufSize) )
                {
                    break;
                } 
                #ifdef _SAVE_H264_STREAM
                FILE*       fpsave;            // ��д��Ƶ�ļ��������ļ�ָ��
                fpsave = fopen("test.record", "ab+");

                if ( fpsave == NULL )
                {
                    printf("err:open file failed\n");
                    return;
                }
                fwrite(pBuffer, dwBufSize, 1, fpsave);
                fclose(fpsave);
                #endif

                #ifdef _TEST_TCP_C
                send( _g_tcp_c, (char *)pBuffer, dwBufSize, 0 );
                #endif
            }
        }
        
        default:
        break;
    }
#else
    if ( !send(_g_tcp_s_cnt, (char *)pBuffer, dwBufSize, 0) )
    {
        //closesocket(_g_tcp_s_cnt);
        printf("Tcp_s send err!\n");
    }
#endif
    
}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};

    sw_deb_printf("%s\n", __FUNCTION__ );
    
    switch(dwType) 
    {
        case EXCEPTION_RECONNECT:    //Ԥ��ʱ����
        sw_inf_printf("----------reconnect--------%d\n", time(NULL));
        break;
        default:
        break;
    }
}

#ifdef _TEST_TCP_S
int sw_tcp_s_init( void )
{
    /* ����socket��̬���ӿ�(dll) */ 
    WORD wVersionRequested;  
    WSADATA wsaData;            /* ��ṹ�����ڽ���Wjndows Socket�Ľṹ��Ϣ��  */ 
    int err;  
       
    wVersionRequested = MAKEWORD( 1, 1 );   /* ����1.1�汾��WinSock�� */ 
       
    err = WSAStartup( wVersionRequested, &wsaData );  
    if ( err != 0 ) 
    {  
        return -1;          /* ����ֵΪ���ʱ���Ǳ�ʾ�ɹ�����WSAStartup */
    }  
       
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) 
    {  
        /* 
         * ���������ֽ��ǲ���1�����ֽ��ǲ���1��ȷ���Ƿ������������1.1�汾   
         * ����Ļ�������WSACleanup()�����Ϣ����������
         */
        WSACleanup( );  
        return -1;   
    }
      
    /*
     * ����socket������������ʽ�׽��֣������׽��ֺ�sockSrv   
     * SOCKET socket(int af, int type, int protocol);   
     * ��һ��������ָ����ַ��(TCP/IPֻ����AF_INET��Ҳ��д��PF_INET)   
     * �ڶ�����ѡ���׽��ֵ�����(��ʽ�׽���)�����������ض���ַ�������Э�飨0Ϊ�Զ���
     */
    _g_tcp_s = socket(AF_INET, SOCK_STREAM, 0);     

    /*
     * �׽���sockSrv�뱾�ص�ַ����   
     * int bind(SOCKET s, const struct sockaddr* name, int namelen);   
     * ��һ��������ָ����Ҫ�󶨵��׽��֣�   
     * �ڶ���������ָ�����׽��ֵı��ص�ַ��Ϣ���õ�ַ�ṹ�������õ�����Э��Ĳ�ͬ����ͬ   
     * ������������ָ��������Э���ַ�ĳ���   
     * PS: struct sockaddr{ u_short sa_family; char sa_data[14];};   
     * sa_familyָ���õ�ַ���壬 sa_data��ռλռ��һ���ڴ������������   
     * ��TCP/IP�У���ʹ��sockaddr_in�ṹ�滻sockaddr���Է�����д��ַ��Ϣ   
     *    
     * struct sockaddr_in{ short sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8];};   
     * sin_family��ʾ��ַ�壬����IP��ַ��sin_family��Ա��һֱ��AF_INET��   
     * sin_portָ����Ҫ������׽��ֵĶ˿ڡ�   
     * sin_addr�����׽��ֵ�����IP��ַ��   
     * sin_zero[8]�������������sockaddr_in��sockaddr�ṹ�ĳ���һ����   
     * ��IP��ַָ��ΪINADDR_ANY�������׽������κη�������ػ�����IP��ַ���ͻ�������ݡ�   
     * �����ֻ���׽���ʹ�ö��IP�е�һ����ַ����ָ��ʵ�ʵ�ַ����inet_addr()������  
     */
    SOCKADDR_IN addrSrv;  
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // ��INADDR_ANYת��Ϊ�����ֽ��򣬵��� htonl(long��)��htons(����)   
    addrSrv.sin_family = AF_INET;  
    addrSrv.sin_port = htons(60003);  
  
    bind( _g_tcp_s, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR) ); /* �ڶ�����Ҫǿ������ת�� */

    /*
     * ���׽�������Ϊ����ģʽ���������󣩣� listen()֪ͨTCP������׼���ý�������   
     * int listen(SOCKET s,  int backlog);   
     * ��һ������ָ����Ҫ���õ��׽��֣��ڶ�������Ϊ���ȴ����Ӷ��е���󳤶ȣ�
     */
    listen( _g_tcp_s, 10 );  

#if 1
    SOCKADDR_IN  addrClient;  
    int len = sizeof(SOCKADDR);  
    
    while ( true )
    {
        // ���ϵȴ��ͻ�������ĵ���   
        _g_tcp_s_cnt = accept(_g_tcp_s, (SOCKADDR *)&addrClient, &len);  

        #if 0
        char sendBuf[100];  
        sprintf(sendBuf, "Welcome %s to the server program~ \nNow, let's start talking...\n", inet_ntoa(addrClient.sin_addr));  
        send(_g_tcp_s_cnt, sendBuf, strlen(sendBuf)+1, 0);  // ������ʾ��ӭ��Ϣ   
        
        char recvBuf[100];  
        recv(_g_tcp_s_cnt, recvBuf, 100, 0);  
        printf("%s\n", recvBuf);        // ���յ�һ����Ϣ   
  
        char * sockConnName = "Client";  
        printf("���ǿ�������仰");  
        int n = 5;  
        while(n--)
        {  
            printf("��ʣ%d�Σ�\n", n+1);  
            char recvBuf[100];  
            recv(_g_tcp_s_cnt, recvBuf, 100, 0);  
            printf("%s Says: %s\n", sockConnName, recvBuf);     // ������Ϣ   
              
            char talk[100];  
            printf("Please enter what you want to say next(\"quit\"to exit):");  
            gets(talk);  
            send(_g_tcp_s_cnt, talk, strlen(talk)+1, 0);            // ������Ϣ   
            printf("\n");  
        }         
        
        closesocket(_g_tcp_s_cnt);
        #endif
    }  
#endif
    return 0; 
}
#endif

#ifdef _TEST_TCP_C
void sw_tcp_c_init( void )
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested,&wsaData); //����Winsocket DLL
    if ( err != 0 )
    {
        return;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return;
    }

    _g_tcp_c = socket(AF_INET, SOCK_STREAM, 0);    //�����׽���
    SOCKADDR_IN addrSrv;                           //socketAddress socket�˿�
    
    //�������˿�����
    addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.1.153");
    
    ////��Ϊ�ͻ��ˣ���Ҫ���ӡ�connect����Զ�˵ķ�������Ҳ��Ҫָ��Զ�˷������ģ�ip, port���ԡ�
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6001);

    connect( _g_tcp_c, (SOCKADDR *) &addrSrv, sizeof(SOCKADDR) );
}

void sw_tcp_c_close( void )
{
    //�ر��׽���
    closesocket(_g_tcp_c);
    
    //��ֹʹ�� DLL
    WSACleanup();
}
#endif

#define STREAM_BUF_SIZE (1024 * 1024)
#define PER_FRAME (1024 * 5)
void sw_test_play_local_file( void )
{
    //BYTE Buffer[STREAM_BUF_SIZE] = {0};
    //DWORD dwBufSize = 0;
    BYTE *Buffer = (BYTE *)malloc( sizeof(BYTE) * STREAM_BUF_SIZE );
    memset( Buffer, 0, STREAM_BUF_SIZE );

    //FILE*       fpsave;

    HWND hWnd = GetConsoleWindow();
    
    if(!PlayM4_GetPort(&_g_lPort))
    {
        sw_deb_printf("��ȡͨ����ʧ��!");
        system("PAUSE");
        return;
    }
    
    if ( !PlayM4_SetStreamOpenMode( _g_lPort, STREAME_FILE) )  //����ʵʱ������ģʽ
    {
        sw_deb_printf("Set stream mode failed!\n");
        system("PAUSE");
        return;
    }
    
    if ( !PlayM4_OpenFile(_g_lPort, "test.record" ) )
    {
        sw_deb_printf("Open stream file failed!\n");
        system("PAUSE");
        return;
    }

    if ( !PlayM4_Play(_g_lPort, hWnd) ) //���ſ�ʼ
    {
        sw_deb_printf("Start play failed!\n");
        system("PAUSE");
        return;
    }
    PlayM4_CloseFile(_g_lPort);
    system("PAUSE");
}

int sw_hik_sdk_init( void )
{
    /* ��ʼ����������ͷSDK */
    NET_DVR_Init();

    /* ע���豸 */
    NET_DVR_DEVICEINFO_V30 struDeviceInfo;
    
    _g_lUserID = NET_DVR_Login_V30("192.168.1.65", 8000, "admin", "admin0123", &struDeviceInfo);
    
    if (_g_lUserID < 0)
    {
       sw_err_printf("Login error, %d\n", NET_DVR_GetLastError());
       NET_DVR_Cleanup();
       return -1;
    }
    else
    {
        sw_deb_printf("Login success!\n");
    }

    /* �����쳣��Ϣ�ص����� */
    NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);

#ifdef _TEST_TCP_C
    sw_tcp_c_init();
#endif

    sw_hik_realplay_start();

#ifdef _TEST_TCP_C
    system("PAUSE");
    sw_tcp_c_close();
#endif

#ifdef _TEST_TCP_S
    sw_tcp_s_init();
#endif
    
#if 0
    //system("PAUSE");
    
    sw_hik_realplay_stop();
    
    /* ע���û� */
    NET_DVR_Logout_V30(_g_lUserID);
    NET_DVR_Cleanup();
#endif
    return 0;
}

void sw_hik_realplay_start( void )
{
    /* ����Ԥ�������ûص������� */
    NET_DVR_CLIENTINFO ClientInfo = {0};
    ClientInfo.hPlayWnd = NULL;         //��ҪSDK����ʱ�����Ϊ��Чֵ����ȡ��������ʱ����Ϊ��
    ClientInfo.lChannel     = 1;       //Ԥ��ͨ����
    ClientInfo.lLinkMode    = 0;       //���λ(31)Ϊ0��ʾ��������Ϊ1��ʾ������0��30λ��ʾ���ӷ�ʽ��0��TCP��ʽ��1��UDP��ʽ��2���ಥ��ʽ��3��RTP��ʽ;
    ClientInfo.sMultiCastIP = NULL;   //�ಥ��ַ����Ҫ�ಥԤ��ʱ����
    
    BOOL bPreviewBlock = false;       //�������������Ƿ�������0����1����
    _g_lRealPlayHandle = NET_DVR_RealPlay_V30(_g_lUserID, &ClientInfo, NULL, NULL, 0);

    if (_g_lRealPlayHandle < 0)
    {
        sw_err_printf("NET_DVR_RealPlay_V30 error\n");
        NET_DVR_Logout(_g_lUserID);
        NET_DVR_Cleanup();
        return;
    }

    if ( !NET_DVR_SetRealDataCallBack(_g_lRealPlayHandle, g_RealDataCallBack_V30, 0) )
    {
        sw_err_printf("NET_DVR_SetRealDataCallBack error\n");
    }
}

void sw_hik_realplay_stop( void )
{
    sw_deb_printf("%s\n", __FUNCTION__ );
    /* �ر�Ԥ�� */
    NET_DVR_StopRealPlay(_g_lRealPlayHandle);
}

#ifdef __cplusplus
}
#endif /* ENDIF __cplusplus */


