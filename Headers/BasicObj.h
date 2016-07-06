#ifndef __BASICOBJ_H__

#define __BASICOBJ_H__
/**
 * 基本对象类模块，用于功能扩展
 * 负责人：董超
 */
#include "Global.h" 

class Debugger;

class BasicObj
{
public:
	BasicObj();
	virtual ~BasicObj();

public:
	Debugger*	debugger;			// 调试器

protected:
	void		SetDebugger(Debugger* debugger);	// 设置调试器
	void		VcdDump();	// 创建VCD故障诊断信息

};

#endif  //__BASICOBJ_H__