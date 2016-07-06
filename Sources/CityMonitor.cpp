/**
 *=======================================================================================================================
 * Swift Technology ����˼ί���ؿƼ����޹�˾ ������Ƶ���ϵͳ���
 * Name        : CityMonitor.cpp
 * Author      : ����
 * Version     : V1.0.0
 * Copyright   : �����������˼ί���ؿƼ����޹�˾������ӵ������Ȩ��������˼ί����������Ȩ��ɵ�����£��κ�������������
 *               ���öԱ��ļ����в��ֻ�ȫ���Ŀ��������򣬱���˾������׷����Ӧ�ķ������Ρ�
 * Description : ������Ƶ���ϵͳִ�г������
 *=======================================================================================================================
 * Revision History:
 * �޸�ʱ��    �޸���  �޸�����
 * 2016-06-10  ����    �½�
 *=======================================================================================================================
 */
#include "VideoManager.h"
#include <time.h>
#ifdef __WIN32__
#include <windows.h>
#endif
#include "VideoRecorder.h"
#include "Debugger.h"
#include <cstring>
#ifndef __WIN32__
#include <unistd.h>
#endif

#include "SwiftHikSDK.h"

int main() 
{
    sw_hik_sdk_init();
#if 0
    VideoManager* videoManager = new VideoManager();

    while (1)
    {
        #ifdef __WIN32__
            Sleep(1000);
        #else
            sleep(1000);
        #endif
        videoManager->Run();
    }

    VideoRecorder vrd;

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    while (1)
    {
        vrd.SaveLiveVideo((byte*)buf, 1024);
#ifdef __WIN32__
        Sleep(1000);
#else
        sleep(1000);
#endif
    }
#endif
    return 0;
}

