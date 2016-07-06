#include "BasicObj.h" 
#include "Debugger.h" 

Debugger db;
BasicObj::BasicObj()
{
    SetDebugger(&db);
}
BasicObj::~BasicObj()
{
}
void BasicObj::SetDebugger(Debugger* debugger)
{
    this->debugger = debugger;
}
void BasicObj::VcdDump()
{
}
