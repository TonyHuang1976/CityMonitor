#ifndef __BASICOBJ_H__

#define __BASICOBJ_H__
/**
 * ����������ģ�飬���ڹ�����չ
 * �����ˣ�����
 */
#include "Global.h" 

class Debugger;

class BasicObj
{
public:
	BasicObj();
	virtual ~BasicObj();

public:
	Debugger*	debugger;			// ������

protected:
	void		SetDebugger(Debugger* debugger);	// ���õ�����
	void		VcdDump();	// ����VCD���������Ϣ

};

#endif  //__BASICOBJ_H__