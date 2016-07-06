/**
 * 文件管理模块。功能如下：
 * 1. 创建新视频文件，文件的最大字节数量限制在100MB，文件格式为H264。存满100MB后立即创建新文件。
 * 2. 以先存先删的方式删除一个文件
 * 3. 获取历史视频文件清单
 * 4. 按指定的速度读取视频流
 * 负责人：周林
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 
#include "HistoryVideo.h"
#include <list>
#include "ObjBuffer.h"
class historyvideo; 
class DateTime;

/** 
 * 视频录像索引文件数据存储格式：

数据结构 开始时间 + 结束时间 + 数据长度 + 文件名长度 + 文件名
字节数       6         6         4           4        由文件名长度指定

其中：
1．开始时间 – 监控中心指定的历史视频播放开始时间，6字节（分别为年、月、日、时、分、秒）
2．结束时间 – 监控中心指定的历史视频播放结束时间，6字节（分别为年、月、日、时、分、秒）

 */


#define MAX_FILE_PATH               256
#define MAX_FILE_NUM            6
#define RECORD_DATA_PATH        "g:\\data\\"
#define MAX_FILE_LEN            (10240/5)

class VideoRecorder : BasicObj
{
public:
    VideoRecorder();
    virtual ~VideoRecorder();

private:
    char fileindexpath[MAX_FILE_PATH];                                      // 文件索引路径
    ObjBuffer               *recordedVideoList;                         // 本地视频节点的历史视频清单列表  

public:
    bool                    SaveLiveVideo(byte* data, uint dataLen);    // 存储本地摄像机视频数据
    int                     ReadVideo(byte* buf, byte speed);           // 按指定的速度读取视频文件
    void                    AddRecodedVideo(HistoryVideo* historyVideo);    // 添加欲播放的历史视频信息进播放清单
    void                    AddRecodedVideo(byte* historyVideo, int len);// 添加欲播放的历史视频信息进播放清单,该数据流为从日志文件中读出
    HistoryVideo*           FindHistoryVideo(DateTime startTime);       // 寻找包含指定时间的视频
    ObjBuffer*              GetHistoryVideos();                         // 获取历史视频文件清单
    void                    PrintVideoRecords();                        // 写入视频记录文件
    void                    ReadVideoRecordsFromFile();                 // 从索引读出日志文件 
    
    int                     CheckFileDir(char* dir);                    // 检查文件夹是否存在，不存在就新建   
    bool                    CreateFile(char* path);                     // 创建该路径下的文件

    bool                    CreateHistoryVideo();                       // 创建新的索引文件并添加到历史视频列表中   
    bool                    RefreshCurHistoryVideo();                   // 视频流文件读取保存完毕，更新该视频流记录信息  

protected:
    //读取索引文件所需要的缓存
    DateTime* bgtime;      // 读索引开始时间缓存
    DateTime* edtime;      // 读索引结束时间缓存
    uint filelen;          // 文件长度缓存
    uint pathlen;          // 路径长度缓存
    std::string path;      // 路径缓存
    int  state;            // 读取函数当前读取文件流处于当前索引文件位置  
    FILE *fp;              // 读索引文件时的视频索引文件指针

    //写入视频数据的缓存
    DateTime*   startTime;          // 本段视频的起始时间
    DateTime*   endTime;            // 本段视频的结束时间
    std::string pathfile;           // 本段视频的存放位置及文件名
    uint        videoLength;        // 本段视频的字节长度  
    FILE*       fpsave;            // 待写视频文件的索引文件指针
    HistoryVideo*  hVideo;          // 
};
