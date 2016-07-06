/**
 *=======================================================================================================================
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : CityMonitor.cpp
 * Author      : 董超
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *               不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : 城市视频监控系统执行程序入口
 *=======================================================================================================================
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-10  董超    新建
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

