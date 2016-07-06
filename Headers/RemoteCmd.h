/**
 * 远程命令管理模块
 * 负责人：董超
 */
#pragma once
#include "Global.h" 
#include "BasicObj.h" 

class VideoManager;

class RemoteCmd : BasicObj
{
public:
    RemoteCmd();
    RemoteCmd(VideoManager* manager);
    virtual ~RemoteCmd();

private:
    VideoManager*   manager;                    // 本视频节点的视频管理模块

public:
    void    Decode(byte* command);              // 远程下行命令译码
};
