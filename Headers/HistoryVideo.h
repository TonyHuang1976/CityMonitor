/**
 * ��ʷ��Ƶ��¼ģ��
 * �����ˣ�����
 */
#pragma once
#include "Global.h" 
#include "DateTime.h" 
#include "BasicObj.h" 
#include <string>


class HistoryVideo : BasicObj
{
public:
	HistoryVideo();
	virtual ~HistoryVideo();

private:
	DateTime*	startTime;			// ������Ƶ����ʼʱ��
	DateTime*	endTime;			// ������Ƶ�Ľ���ʱ��
	DateTime*	curPlayTime;		// ������Ƶ�ĵ�ǰ����ʱ��
	std::string	path;				// ������Ƶ�Ĵ��λ�ü��ļ���
	uint		videoLength;		// ������Ƶ���ֽڳ���  
public:
	void SaveRecord(FILE *findex);             // ��ָ���ļ��±����¼
	void SetStartTime(DateTime* pDateTime);    // ���ÿ�ʼʱ��
	void SetEndTime(DateTime* pDateTime);      // ���ý���ʱ��  
	void SetPath(std::string path_);           // ����·��
	bool IsInTimeRange(DateTime time);         // ��ʱ����Ƿ��ڼ�¼���� 
	void SetVideoLen(uint videoLength_);       // ������Ƶ���� 
	std::string GetPath();					   // ��ȡ�ļ�·��	

};
