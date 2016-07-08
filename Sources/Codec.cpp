#include "Codec.h"
#include <assert.h>
#include <cstring>
#include <malloc.h>
#include "DateTime.h"
#include "VideoManager.h"
#include "Buffer.h"
#include "Debugger.h"
#ifdef __WIN32__
#include "Windows.h"
#else
#include <unistd.h>
#endif
Codec::Codec(VideoManager* videoManager, Buffer* cmdBuffer)
{   
    this->videoManager = videoManager;
    this->cmdBuffer = cmdBuffer;
    state = DECODE_STATE_CMD_START;
#if defined(BOARD_NO1)
    siteId = 1;//manger->GetSiteId();
    cameraId = 1;//manger->GetCameraId();
#elif defined(BOARD_NO2)
    siteId = 1;
    cameraId = 2;
#elif defined(BOARD_NO3)
    siteId = 1;
    cameraId = 3;
#endif
}

Codec::~Codec(void)
{
}
void Codec::Start()
{
    PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__,"Codec Start!");
    byte tmpBuffer[CMD_BUFFER_LEN];
    byte count = 0;
    byte cmd_len = 0;
    byte checksum = 0;
    byte cmd_type = 0;
    byte *cmdInfo;

    while (true)
    {
        // 检查循环缓冲器中是否有下行命令信息。使用循环缓冲器的第二读出端口。第一端口被命令转发单元所用。
        uint availLen = cmdBuffer->GetValidDataLenExt();
        if (availLen == 0)
        {
            // 没有命令就等一会儿再检查
#ifdef __WIN32__
            Sleep(100);
#else
            usleep(100000);
#endif
            continue;
        }
        // 否则就从循环缓冲器中读取命令信息。
        cmdBuffer->ReadExt(tmpBuffer, availLen);
        // 对命令信息进行译码
        for (int i = 0; i < availLen; i++)
        {
            switch (state)
            {
            case DECODE_STATE_CMD_START:    // 命令头译码
                if (0 == count)
                {
                    if (((DOWN_COMMEND >> 8) & 0xFF) == tmpBuffer[i])
                    {
                        // 消息标识符的高8位正确
                        count++;
                        checksum = tmpBuffer[i];
                    }
                }
                else if (1 == count)
                {
                    if ((DOWN_COMMEND & 0xFF) == tmpBuffer[i])
                    {
                        // 消息标识符的低8位正确
                        count = 0;
                        state = DECODE_STATE_CMD_TYPE;
                        // 计算校验和
                        checksum += tmpBuffer[i];
                    }
                    else
                    {
                        // 校验和清零
                        checksum = 0;
                    }
                }
                break;

            case DECODE_STATE_CMD_TYPE: // 命令类型译码
                cmd_type = tmpBuffer[i];
                state = DECODE_STATE_SITE_ID;
                checksum += tmpBuffer[i];
                break;
            case DECODE_STATE_SITE_ID:
                // 参照DECODE_STATE_CMD_START状态译码
                // TODO
                if (0 == count)
                {
                    siteId = (tmpBuffer[i] << 8);
                    count++;
                }
                else if (1 == count)
                {
                    siteId |= tmpBuffer[i];
                    count = 0;
                    state = DECODE_STATE_CAM_ID;
                }
                checksum += tmpBuffer[i];
                break;
            case DECODE_STATE_CAM_ID:
                // 参照DECODE_STATE_CMD_START状态译码
                // TODO
                if (0 == count)
                {
                    cameraId = (tmpBuffer[i] << 8);
                    count++;
                }
                else if (1 == count)
                {
                    cameraId |= tmpBuffer[i];
                    count = 0;
                    state = DECODE_STATE_CMD_LEN;
                }
                checksum += tmpBuffer[i];
                break;
            case DECODE_STATE_CMD_LEN:      // 获取命令参数长度
                cmd_len = tmpBuffer[i];
                if (0 == cmd_len)
                {
                    state = DECODE_STATE_CMD_END;
                }
                else 
                {
                    state = DECODE_STATE_CMD_INFO;
                    cmdInfo = new byte[cmd_len];
                    count=0;
                }
                checksum += tmpBuffer[i];
                break;
            case DECODE_STATE_CMD_INFO:     // 获取命令参数
                // TODO
                cmdInfo[count] = tmpBuffer[i];
                checksum += tmpBuffer[i];
                count++;
                if (count >= cmd_len)
                {
                    state = DECODE_STATE_CMD_END;
                    count=0;
                }
                break;
            case DECODE_STATE_CMD_END:      // 命令结束符译码
                // 参照DECODE_STATE_CMD_START状态译码
                // TODO
                if (0 == count)
                {
                    if (((END_MESSAGE_FLAG >> 8) & 0xFF) == tmpBuffer[i])
                    {
                        // 消息标识符的高8位正确
                        count++;
                        checksum += tmpBuffer[i];
                    }
                }
                else if (1 == count)
                {
                    if ((END_MESSAGE_FLAG & 0xFF) == tmpBuffer[i])
                    {
                        // 消息标识符的低8位正确
                        state = DECODE_STATE_CHECKSUM;
                        // 计算校验和
                        checksum += tmpBuffer[i];
                    }
                    else
                    {
                        // 校验和清零
                        checksum = 0;
                    }
                    count = 0;
                }
                break;
            case DECODE_STATE_CHECKSUM:     // 检查校验和
                if ( (checksum & 0xff) == tmpBuffer[i])
                {
                    PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__," 校验和正确");
                    //state = DECODE_STATE_EXECUTE;
                }
                else
                {
                    PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__," 校验和错误：计算值 = %d，传输值 = %d!", checksum, tmpBuffer[i]);
                    state = DECODE_STATE_CMD_START;
                    break;
                }
                count = 0;
                checksum = 0;
                //break;

            //case DECODE_STATE_EXECUTE:        // 执行命令
            //  PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__," cmd_type = %d", cmd_type);
                switch (cmd_type)
                {
                case DOWN_COMMEND_RESTART:                  // 这是系统重启命令
                    videoManager->SystemRestart();
                    break;
                case DOWN_COMMEND_REALVIDEO:                // 这是实时视频直播命令
                    videoManager->PlayRealTimeVideo();
                    break;
                case DOWN_COMMEND_HISTORYVIDEO:             // 这是录像点播命令
                    videoManager->PlayHistoryVideo(&historyVideoStTime, &historyVideoEndTime);
                    break;
                case DOWN_COMMEND_HISTORYLIST:              // 这是获取录像列表命令
                    videoManager->SendHistoyVideoList();
                    break;
                default:
                    PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__," 译码错误：非法命令码 - %d!", cmd_type);
                    state = DECODE_STATE_CMD_START;     // 退出本次命令译码，状态复位，以防止死锁
                    break;
                }
                state = DECODE_STATE_CMD_START;
                break;
            default:
                PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__," 译码错误：非法状态 - %d!", state);
                state = DECODE_STATE_CMD_START;     // 状态复位，以防止死锁
                count = 0;
                checksum = 0;
                break;
            }
        }
    }
}

//监控中心下行命令
//接收到了数据调用该函数去解析
void Codec::ParseDataPackage(VideoManager *manger,byte * buffer,int buflen)
{
    int message=0;  
    byte cmdLen=0;  
    //int endFlag;
    byte checksum=0;    
    int calcchecksum=0;
    byte len=0;
    int i=0;
    int site_id=0;
    int camera_id=0;

    assert(buffer != NULL);

    PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__);
    byte *data=buffer;
    if(buflen <2)
    {
        PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__,"input data error!");
        return;
    }   

    message = (((*data<<8) | *(data+1)) & 0xffff);  
    
    cmdLen = *(data+7);

    len = buflen-1;
    checksum = *(data+len);

    //check checksum 
    for(i=0;i<len;i++)
    {
        calcchecksum += *(data+i);
    }
    

    if((calcchecksum & 0xff) != checksum)
    {
        PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__,"check sum error!");
        return;
    }
    ////////////
    
    site_id = (*(data+3)<<8) | *(data+4);
    camera_id =( *(data+5)<<8) | *(data+6);
    
    //if(message == 0xcc33) //监控中心下行命令
    if ( message == DOWN_COMMEND ) //监控中心下行命令
    {
        if ( siteId != site_id )
        {
            //manger->SendToNextSite(buffer);
            return;
        }
        if ( cameraId != camera_id )
        {
            //manger->SendToNextCamera(buffer);
            return;
        }

        ParseDownPackage(manger,buffer,buflen);
    }
    else if ( message == UP_COMMEND )//上行消息
    {
        //判断是否还需上传

        //manger->SendToPrev(buffer,buflen);
    }
    else if(message == UP_VIDEO)//上行视频
    {
        //判断是否还需上传

        //manger->SendToPrev(buffer,buflen);
    }
}

//上行消息
//message 1--心跳   2--视频节点报名  3--视频站点报名  4--工作异常 5--历史视频
void Codec::SendUpPackage(VideoManager *manger,byte message)
{
    switch(message)
    {
    case UP_COMMEND_HEARTBEAT:
        SendHeartbeatPacket(manger);
        break;
    case UP_COMMEND_VIDEOCAMEAR:
        SendCameraPos(manger);
        break;
    case UP_COMMEND_VIDEOSITE:
        SendSitePos(manger);
        break;
    case UP_COMMEND_ERRORMESSAGE:
        SendErrorMessage(manger);
        break;
    case UP_COMMEND_HISTORYVIDEO:
        SendHistoryVideoList(manger);
        break;
    case UP_COMMEND_SYSTEMCONTROL:
        SendSystemControl(manger);
        break;
    default:
        break;
    }
}

//上行视频数据
void Codec::SendUpVideoPackage(VideoManager *manger,byte *buffer,int len)
{
    byte *buf = new byte[len+26];
    int checksum=0; 
    int datalen=len;
    
    buf[0] = (UP_VIDEO >> 8)&0xff;//0x55;
    buf[1] = (UP_VIDEO)&0xff;//0xaa;
    buf[2] = (siteId>>8)&0xff;
    buf[3] = (siteId)&0xff;
    buf[4] = (cameraId>>8)&0xff;
    buf[5] = (cameraId)&0xff;

    buf[6] = historyVideoStTime.GetYear();
    buf[7] = historyVideoStTime.GetMouth();
    buf[8] = historyVideoStTime.GetDay();
    buf[9] = historyVideoStTime.GetHour();
    buf[10] = historyVideoStTime.GetMin();
    buf[11] = historyVideoStTime.GetSec();
    buf[12] = historyVideoStTime.GetYear();
    buf[13] = historyVideoStTime.GetMouth();
    buf[14] = historyVideoStTime.GetDay();
    buf[15] = historyVideoStTime.GetHour();
    buf[16] = historyVideoStTime.GetMin();
    buf[17] = historyVideoStTime.GetSec();

    buf[18] = (datalen>>24)&0xff;
    buf[19] = (datalen>>16)&0xff;
    buf[20] = (datalen>>8)&0xff;
    buf[21] = (datalen)&0xff;

    memcpy(buf+22,buffer,datalen);
    

    buf[22+datalen] = (END_UPVIDEO_FLAG>>8)&0xff;//0x33;
    buf[23+datalen] = (END_UPVIDEO_FLAG)&0xff;//0xcc;
    //checksum calc
    for(int i=0;i<23+datalen+1;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xffff;
    ////
    buf[23+datalen+1] = (checksum>>8)&0xff;
    buf[23+datalen+2] = checksum&0xff;

    //send

}

void Codec::ParseDownPackage(VideoManager *manger,byte * buffer,int buflen)
{
    int message=0;
    byte cmd=0;
    int site_Id=0;
    int camera_Id=0;
    byte cmdLen=0;
    byte *cmdbuf;
    int endFlag=0;
    short sum=0;
    HistoryVideoManager *hvm;
    
    assert(buffer != NULL);
    
    byte *data=buffer;
    
    message = ((*data<<8) | *(data+1))&0xffff;
    cmd = *(data+2);
    site_Id = (*(data+3)<<8) | *(data+4);
    camera_Id =( *(data+5)<<8) | *(data+6);
    cmdLen = *(data+7);

    cmdbuf = new byte[cmdLen+1];
    memcpy((void *)cmdbuf,data+8,cmdLen);
    cmdbuf[cmdLen]=0;
    
    endFlag = ((*(data+8+cmdLen) << 8) | (*(data+8+cmdLen+1)))&0xffff;
    if(endFlag != END_MESSAGE_FLAG)
    {
        PRINT(ALWAYS_PRINT, "Codec", __FUNCTION__, __LINE__,"end flag error!");
        return;
    }


    PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__, " get message=%d,cmd=%d,site id=%d,camera id=%d,cmd len=%d", \
        message,cmd,site_Id,camera_Id,cmdLen);  

    if ( message == DOWN_COMMEND )  //监控中心下行命令
    {
        switch(cmd)
        {
        case DOWN_COMMEND_RESTART://系统重启
            if(strcmp((char *)cmdbuf,"System Restart") == 0)
            {
                //manger->ReBoot();
                PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__,"System Restart Go!");
            }
            break;
        case DOWN_COMMEND_REALVIDEO://实时点播
            PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__,"Play Real Video!");
            historyVideoStTime.SetYear(0);
            historyVideoStTime.SetMouth(0);
            historyVideoStTime.SetDay(0);
            historyVideoStTime.SetHour(0);
            historyVideoStTime.SetMin(0);
            historyVideoStTime.SetSec(0);

            historyVideoEndTime.SetYear(0);
            historyVideoEndTime.SetMouth(0);
            historyVideoEndTime.SetDay(0);
            historyVideoEndTime.SetHour(0);
            historyVideoEndTime.SetMin(0);
            historyVideoEndTime.SetSec(0);
            //get video

            //char buf[32];
            //strcpy(buf,"123456789asdfghjk");
            //SendUpVideoPackage((byte *)buf,strlen(buf)-1);
            //SendUpVideoPackage(buffer, buflen);
        
            break;
        case DOWN_COMMEND_HISTORYVIDEO://录像点播
            PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__,"Play History Video go");
            hvm=new HistoryVideoManager();
            historyVideoStTime.SetYear(*cmdbuf);
            historyVideoStTime.SetMouth(*(cmdbuf+1));
            historyVideoStTime.SetDay(*(cmdbuf+2));
            historyVideoStTime.SetHour(*(cmdbuf+3));
            historyVideoStTime.SetMin(*(cmdbuf+4));
            historyVideoStTime.SetSec(*(cmdbuf+5));

            historyVideoEndTime.SetYear(*(cmdbuf+6));
            historyVideoEndTime.SetMouth(*(cmdbuf+7));
            historyVideoEndTime.SetDay(*(cmdbuf+8));
            historyVideoEndTime.SetHour(*(cmdbuf+9));
            historyVideoEndTime.SetMin(*(cmdbuf+10));
            historyVideoEndTime.SetSec(*(cmdbuf+11));

            hvm->PlayHistoryVideo(historyVideoStTime,historyVideoEndTime);

            //SendUpVideoPackage(manger,buffer,len);

            break;
        case DOWN_COMMEND_HISTORYLIST://获取录像列表
            PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__,"Get History Video");
            
            SendHistoryVideoList();
            break;
        default:
            break;
        }
    }

    delete [] cmdbuf;
}

void Codec::SendHeartbeatPacket(VideoManager *manger)
{
    byte buf[30];
    int checksum=0;
    
    buf[0] = (UP_COMMEND>>8)&0xff;//0x5a;
    buf[1] = (UP_COMMEND)&0xff;//0xc3;
    buf[2] = UP_COMMEND_HEARTBEAT;
    buf[3] = (siteId>>8)&0xff;
    buf[4] = (siteId)&0xff;
    buf[5] = (cameraId>>8)&0xff;
    buf[6] = (cameraId)&0xff;
    buf[7] = (END_UPMESSAGE_FLAG>>8)&0xff;//0x3c;
    buf[8] = (END_UPMESSAGE_FLAG)&0xff;//0xa5;
    //checksum calc
    for(int i=0;i<9;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xff;
    ////
    buf[9] = checksum;
    

    //send

}

void Codec::SendCameraPos(VideoManager *manger)
{
    byte buf[30];
    int checksum=0;
    uint longitude=0; //经度
    uint latitude=0;  //纬度
    
    //longitude = manger->GetLong();
    //latitude = manger->GetLat();

    buf[0] = (UP_COMMEND>>8)&0xff;//0x5a;
    buf[1] = (UP_COMMEND)&0xff;//0xc3;
    buf[2] = UP_COMMEND_VIDEOCAMEAR;
    buf[3] = (siteId>>8)&0xff;
    buf[4] = (siteId)&0xff;
    buf[5] = (cameraId>>8)&0xff;
    buf[6] = (cameraId)&0xff;

    buf[7] = 8;
    buf[8] = (longitude>>24)&0xff;
    buf[9] = (longitude>>16)&0xff;
    buf[10] = (longitude>>8)&0xff;
    buf[11] = (longitude)&0xff;
    buf[12] = (latitude>>24)&0xff;
    buf[13] = (latitude>>16)&0xff;
    buf[14] = (latitude>>8)&0xff;
    buf[15] = (latitude)&0xff;

    buf[16] = (END_UPMESSAGE_FLAG>>8)&0xff;//0x3c;
    buf[17] = (END_UPMESSAGE_FLAG)&0xff;//0xa5;
    //checksum calc
    for(int i=0;i<18;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xff;
    ////
    buf[18] = checksum;
    

    //send
}
void Codec::SendSitePos(VideoManager *manger)
{
    byte buf[30];
    int checksum=0;
    uint longitude=0; //经度
    uint latitude=0;  //纬度

    //longitude = manger->GetLong();
    //latitude = manger->GetLat();

    buf[0] = (UP_COMMEND>>8)&0xff;//0x5a;
    buf[1] = (UP_COMMEND)&0xff;//0xc3;
    buf[2] = UP_COMMEND_VIDEOSITE;
    buf[3] = (siteId>>8)&0xff;
    buf[4] = (siteId)&0xff;
    buf[5] = (cameraId>>8)&0xff;
    buf[6] = (cameraId)&0xff;

    buf[7] = 8;
    buf[8] = (longitude>>24)&0xff;
    buf[9] = (longitude>>16)&0xff;
    buf[10] = (longitude>>8)&0xff;
    buf[11] = (longitude)&0xff;
    buf[12] = (latitude>>24)&0xff;
    buf[13] = (latitude>>16)&0xff;
    buf[14] = (latitude>>8)&0xff;
    buf[15] = (latitude)&0xff;

    buf[16] = (END_UPMESSAGE_FLAG>>8)&0xff;//0x3c;
    buf[17] = (END_UPMESSAGE_FLAG)&0xff;//0xa5;
    //checksum calc
    for(int i=0;i<18;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xff;
    ////
    buf[18] = checksum;
    

    //send
}

void Codec::SendErrorMessage(VideoManager *manger)
{
    byte buf[30];
    int checksum=0;
    byte message=0;
    //message = manger->GetErrorMessage();

    buf[0] = (UP_COMMEND>>8)&0xff;//0x5a;
    buf[1] = (UP_COMMEND)&0xff;//0xc3;
    buf[2] = UP_COMMEND_ERRORMESSAGE;
    buf[3] = (siteId>>8)&0xff;
    buf[4] = (siteId)&0xff;
    buf[5] = (cameraId>>8)&0xff;
    buf[6] = (cameraId)&0xff;

    buf[7] = 1;
    buf[8] = message;
    

    buf[9] = (END_UPMESSAGE_FLAG>>8)&0xff;//0x3c;
    buf[10] = (END_UPMESSAGE_FLAG)&0xff;//0xa5;
    //checksum calc
    for(int i=0;i<11;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xff;
    ////
    buf[11] = checksum;
    

    //send
}

void Codec::SendHistoryVideoList(VideoManager *manger)
{
    SendHistoryVideoList();
}

void Codec::SendSystemControl(VideoManager *manger)
{
        byte buf[30];
    int checksum=0;
    int message=0;
    //message = manger->GetMessage();

    buf[0] = (UP_COMMEND>>8)&0xff;//0x5a;
    buf[1] = (UP_COMMEND)&0xff;//0xc3;
    buf[2] = UP_COMMEND_SYSTEMCONTROL;
    buf[3] = (siteId>>8)&0xff;
    buf[4] = (siteId)&0xff;
    buf[5] = (cameraId>>8)&0xff;
    buf[6] = (cameraId)&0xff;

    buf[7] = 2;
    buf[8] = (message>>8)&0xff;
    buf[9] = message&0xff;

    buf[10] = (END_UPMESSAGE_FLAG>>8)&0xff;//0x3c;
    buf[11] = (END_UPMESSAGE_FLAG)&0xff;//0xa5;
    //checksum calc
    for(int i=0;i<12;i++)
    {
        checksum += buf[i];
    }
    checksum=checksum&0xff;
    ////
    buf[12] = checksum;

    //send
}

void Codec::SendHistoryVideoList(void)
{
#if 0
    VideoRecorder *vrec;//=new VideoRecorder();
    ObjBuffer *obj;
    std::string path;
    byte *strbuff;
    int len=0;

    vrec=new VideoRecorder();           
    obj=vrec->GetHistoryVideos();
    len=obj->GetCircleLen();
    HistoryVideo* historyVideo;         
    for(int i=0;i<len;i++)
    {
        historyVideo=(HistoryVideo*)obj->GetObjectAt(i);
        path += historyVideo->GetPath();
    }
    len=path.length();
    strbuff = new byte[len+1];
    strcpy((char *)strbuff,path.c_str());           
    PRINT(DEBUG_LEVEL_3, "Codec", __FUNCTION__, __LINE__,"Get History Video str=%s,%d",strbuff,len);

    //SendToPrev(strbuff,len);
    delete [] strbuff;
#endif
}
