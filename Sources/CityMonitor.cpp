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
int main() 
{
    VideoManager* videoManager = new VideoManager();

    while (1)
    {
        #ifdef __WIN32__
            Sleep(1000);
        #else
            sleep(1000);
        #endif
    }

    return 0;
}
