#ifndef __GLOBAL_H__

#define __GLOBAL_H__

//#define __WIN32__

#define COMPANY     "Swift Technology"
#define APP_NAME    "CityMonitor"
#define VER         "V1.0.0"


#define BOARD_NO1
//#define BOARD_NO2
//#define BOARD_NO3

#ifdef  BOARD_NO1
    #define LEFTNODE_IP_ADDR "192.168.1.106"
    #define RIGHTNODE_IP_ADDR  "192.168.1.138"
#elif defined (BOARD_NO2)
    #define LEFTNODE_IP_ADDR "192.168.1.137"
    #define RIGHTNODE_IP_ADDR  "192.168.1.106"
    #define NO_RIGHT_SERVER
#elif defined (BOARD_NO3)
    #define LEFTNODE_IP_ADDR "192.168.1.137"
    #define RIGHTNODE_IP_ADDR  "192.168.1.138"
    #define NO_RIGHT_SERVER
#else
    #define LEFTNODE_IP_ADDR "192.168.1.137"
    #define RIGHTNODE_IP_ADDR  "192.168.1.138"
#endif
#define LOCAL_IP_ADDR  "192.168.1.138"

using namespace std;

#define NULL    0
#define null    0
typedef unsigned char   byte;
typedef unsigned int    uint;
typedef unsigned long   ulong;

#define MAX_HISTORY_VIDEO_PLAYERS       5       // �����ʷ��Ƶͬʱ��������
#define FRAME_LENGTH                    9000    // ��2Mbps�������ʶ��ԣ����ÿ��30֡�Ĳ����ٶȣ�ÿ֡��8333�ֽ�
#define MAX_NUM_CONN_TIME     5   //����������Դ���
#ifdef __WIN32__
#else
typedef int SOCKET;
#endif

#endif  //__GLOBAL_H__



 


