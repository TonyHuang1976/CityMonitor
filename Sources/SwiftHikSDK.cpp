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
static LONG _g_lPort = 0;           //全局的播放库port号
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
        case NET_DVR_SYSHEAD: //系统头
        {
            if ( !PlayM4_GetPort(&_g_lPort) )  //获取播放库未使用的通道号
            {
                break;
            }
            //m_iPort = _g_lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
            if (dwBufSize > 0)
            {
                if ( !PlayM4_SetStreamOpenMode(_g_lPort, STREAME_REALTIME) )  //设置实时流播放模式
                {
                    break;
                }
                
                if ( !PlayM4_OpenStream(_g_lPort, pBuffer, dwBufSize, 1024 * 1024) ) //打开流接口
                {
                    break;
                }
                #ifdef _PLAY_ON_WINDOW
                if ( !PlayM4_Play(_g_lPort, hWnd) ) //播放开始
                {
                    break;
                }
                #else
                if ( !PlayM4_Play(_g_lPort, NULL) ) //播放开始
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
        
        case NET_DVR_STREAMDATA:   //码流数据
        {
            if ( dwBufSize > 0 && _g_lPort != -1 )
            {
                if ( !PlayM4_InputData(_g_lPort, pBuffer, dwBufSize) )
                {
                    break;
                } 
                #ifdef _SAVE_H264_STREAM
                FILE*       fpsave;            // 待写视频文件的索引文件指针
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
        case EXCEPTION_RECONNECT:    //预览时重连
        sw_inf_printf("----------reconnect--------%d\n", time(NULL));
        break;
        default:
        break;
    }
}

#ifdef _TEST_TCP_S
int sw_tcp_s_init( void )
{
    /* 加载socket动态链接库(dll) */ 
    WORD wVersionRequested;  
    WSADATA wsaData;            /* 这结构是用于接收Wjndows Socket的结构信息的  */ 
    int err;  
       
    wVersionRequested = MAKEWORD( 1, 1 );   /* 请求1.1版本的WinSock库 */ 
       
    err = WSAStartup( wVersionRequested, &wsaData );  
    if ( err != 0 ) 
    {  
        return -1;          /* 返回值为零的时候是表示成功申请WSAStartup */
    }  
       
    if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) 
    {  
        /* 
         * 检查这个低字节是不是1，高字节是不是1以确定是否我们所请求的1.1版本   
         * 否则的话，调用WSACleanup()清除信息，结束函数
         */
        WSACleanup( );  
        return -1;   
    }
      
    /*
     * 创建socket操作，建立流式套接字，返回套接字号sockSrv   
     * SOCKET socket(int af, int type, int protocol);   
     * 第一个参数，指定地址簇(TCP/IP只能是AF_INET，也可写成PF_INET)   
     * 第二个，选择套接字的类型(流式套接字)，第三个，特定地址家族相关协议（0为自动）
     */
    _g_tcp_s = socket(AF_INET, SOCK_STREAM, 0);     

    /*
     * 套接字sockSrv与本地地址相连   
     * int bind(SOCKET s, const struct sockaddr* name, int namelen);   
     * 第一个参数，指定需要绑定的套接字；   
     * 第二个参数，指定该套接字的本地地址信息，该地址结构会随所用的网络协议的不同而不同   
     * 第三个参数，指定该网络协议地址的长度   
     * PS: struct sockaddr{ u_short sa_family; char sa_data[14];};   
     * sa_family指定该地址家族， sa_data起到占位占用一块内存分配区的作用   
     * 在TCP/IP中，可使用sockaddr_in结构替换sockaddr，以方便填写地址信息   
     *    
     * struct sockaddr_in{ short sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8];};   
     * sin_family表示地址族，对于IP地址，sin_family成员将一直是AF_INET。   
     * sin_port指定将要分配给套接字的端口。   
     * sin_addr给出套接字的主机IP地址。   
     * sin_zero[8]给出填充数，让sockaddr_in与sockaddr结构的长度一样。   
     * 将IP地址指定为INADDR_ANY，允许套接字向任何分配给本地机器的IP地址发送或接收数据。   
     * 如果想只让套接字使用多个IP中的一个地址，可指定实际地址，用inet_addr()函数。  
     */
    SOCKADDR_IN addrSrv;  
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY); // 将INADDR_ANY转换为网络字节序，调用 htonl(long型)或htons(整型)   
    addrSrv.sin_family = AF_INET;  
    addrSrv.sin_port = htons(60003);  
  
    bind( _g_tcp_s, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR) ); /* 第二参数要强制类型转换 */

    /*
     * 将套接字设置为监听模式（连接请求）， listen()通知TCP服务器准备好接收连接   
     * int listen(SOCKET s,  int backlog);   
     * 第一个参数指定需要设置的套接字，第二个参数为（等待连接队列的最大长度）
     */
    listen( _g_tcp_s, 10 );  

#if 1
    SOCKADDR_IN  addrClient;  
    int len = sizeof(SOCKADDR);  
    
    while ( true )
    {
        // 不断等待客户端请求的到来   
        _g_tcp_s_cnt = accept(_g_tcp_s, (SOCKADDR *)&addrClient, &len);  

        #if 0
        char sendBuf[100];  
        sprintf(sendBuf, "Welcome %s to the server program~ \nNow, let's start talking...\n", inet_ntoa(addrClient.sin_addr));  
        send(_g_tcp_s_cnt, sendBuf, strlen(sendBuf)+1, 0);  // 发送显示欢迎信息   
        
        char recvBuf[100];  
        recv(_g_tcp_s_cnt, recvBuf, 100, 0);  
        printf("%s\n", recvBuf);        // 接收第一次信息   
  
        char * sockConnName = "Client";  
        printf("我们可以聊五句话");  
        int n = 5;  
        while(n--)
        {  
            printf("还剩%d次：\n", n+1);  
            char recvBuf[100];  
            recv(_g_tcp_s_cnt, recvBuf, 100, 0);  
            printf("%s Says: %s\n", sockConnName, recvBuf);     // 接收信息   
              
            char talk[100];  
            printf("Please enter what you want to say next(\"quit\"to exit):");  
            gets(talk);  
            send(_g_tcp_s_cnt, talk, strlen(talk)+1, 0);            // 发送信息   
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
    err = WSAStartup(wVersionRequested,&wsaData); //加载Winsocket DLL
    if ( err != 0 )
    {
        return;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return;
    }

    _g_tcp_c = socket(AF_INET, SOCK_STREAM, 0);    //创建套接字
    SOCKADDR_IN addrSrv;                           //socketAddress socket端口
    
    //服务器端口配置
    addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.1.153");
    
    ////作为客户端，你要连接【connect】到远端的服务器，也是要指定远端服务器的（ip, port）对。
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6001);

    connect( _g_tcp_c, (SOCKADDR *) &addrSrv, sizeof(SOCKADDR) );
}

void sw_tcp_c_close( void )
{
    //关闭套接字
    closesocket(_g_tcp_c);
    
    //终止使用 DLL
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
        sw_deb_printf("获取通道号失败!");
        system("PAUSE");
        return;
    }
    
    if ( !PlayM4_SetStreamOpenMode( _g_lPort, STREAME_FILE) )  //设置实时流播放模式
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

    if ( !PlayM4_Play(_g_lPort, hWnd) ) //播放开始
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
    /* 初始化海康摄像头SDK */
    NET_DVR_Init();

    /* 注册设备 */
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

    /* 设置异常消息回调函数 */
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
    
    /* 注销用户 */
    NET_DVR_Logout_V30(_g_lUserID);
    NET_DVR_Cleanup();
#endif
    return 0;
}

void sw_hik_realplay_start( void )
{
    /* 启动预览并设置回调数据流 */
    NET_DVR_CLIENTINFO ClientInfo = {0};
    ClientInfo.hPlayWnd = NULL;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    ClientInfo.lChannel     = 1;       //预览通道号
    ClientInfo.lLinkMode    = 0;       //最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
    ClientInfo.sMultiCastIP = NULL;   //多播地址，需要多播预览时配置
    
    BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是
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
    /* 关闭预览 */
    NET_DVR_StopRealPlay(_g_lRealPlayHandle);
}

#ifdef __cplusplus
}
#endif /* ENDIF __cplusplus */


