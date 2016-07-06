#ifndef __GLOBAL_H__

#define __GLOBAL_H__

#define __WIN32__

#define COMPANY     "Swift Technology"
#define APP_NAME    "CityMonitor"
#define VER         "V1.0.0"

#define LEFTNODE_IP_ADDR            "192.168.1.153"
#define LOCAL_IP_ADDR               "192.168.1.153"
#define RIGHTNODE_IP_ADDR           "192.168.1.153"

using namespace std;

#define NULL    0
#define null    0
typedef unsigned char   byte;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define MAX_HISTORY_VIDEO_PLAYERS       5       // 最大历史视频同时播放数量
#define FRAME_LENGTH                    9000    // 对2Mbps传输速率而言，最大每秒30帧的播放速度，每帧有8333字节
#define MAX_NUM_CONN_TIME     5   //连接最大重试次数
#ifdef __WIN32__
#else
typedef int SOCKET;
#endif

#endif  //__GLOBAL_H__



 


