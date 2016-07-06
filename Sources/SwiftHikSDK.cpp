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
using namespace std;

#if 1
#define sw_err_printf(...)   do{ printf("[SW_ERR] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)

#define sw_inf_printf(...)   do{ printf("[SW_ERR] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)

#define sw_deb_printf(...)   do{ printf("[SW_ERR] "); \
    fprintf(stderr,  __VA_ARGS__); } while(0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define _SAVE_H264_STREAM

static LONG _g_lRealPlayHandle = 0;
static LONG _g_lPort = 0;           //ȫ�ֵĲ��ſ�port��
static LONG _g_lUserID = 0;

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, DWORD dwUser)
{
#ifdef _PLAY_ON_WINDOW
    HWND hWnd = GetConsoleWindow();
#endif

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
            }
        }
        
        default:
        break;
    }
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

    sw_hik_realplay_start();

    system("PAUSE");
    
    sw_hik_realplay_stop();
    
    /* ע���û� */
    NET_DVR_Logout_V30(_g_lUserID);
    NET_DVR_Cleanup();
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


