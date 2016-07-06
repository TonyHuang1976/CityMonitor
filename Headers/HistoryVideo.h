/**
 * 历史视频记录模块
 * 负责人：周林
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
	DateTime*	startTime;			// 本段视频的起始时间
	DateTime*	endTime;			// 本段视频的结束时间
	DateTime*	curPlayTime;		// 本段视频的当前播放时间
	std::string	path;				// 本段视频的存放位置及文件名
	uint		videoLength;		// 本段视频的字节长度  
public:
	void SaveRecord(FILE *findex);             // 在指定文件下保存记录
	void SetStartTime(DateTime* pDateTime);    // 设置开始时间
	void SetEndTime(DateTime* pDateTime);      // 设置结束时间  
	void SetPath(std::string path_);           // 设置路径
	bool IsInTimeRange(DateTime time);         // 该时间点是否在记录区间 
	void SetVideoLen(uint videoLength_);       // 设置视频长度 
	std::string GetPath();					   // 获取文件路径	

};
