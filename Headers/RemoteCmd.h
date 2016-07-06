/**
 * Զ���������ģ��
 * �����ˣ�����
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
    VideoManager*   manager;                    // ����Ƶ�ڵ����Ƶ����ģ��

public:
    void    Decode(byte* command);              // Զ��������������
};
