/**
 ************************************************************************************************************************
 * Swift Technology 重庆思委夫特科技有限公司 城市视频监控系统软件
 * Name        : ObjBuffer.h
 * Author      : 董超
 * Version     : V1.0.0
 * Copyright   : 本软件由重庆思委夫特科技有限公司开发并拥有所有权利，在无思委夫特书面授权许可的情况下，任何其他团体或个人
 *               不得对本文件进行部分或全部的拷贝，否则，本公司将依法追究相应的法律责任。
 * Description : 视频单元管理模块。
 ************************************************************************************************************************
 * Revision History:
 * 修改时间    修改人  修改内容
 * 2016-06-11  董超    新建
 * 2016-06-22  董超    完成Linux 及 Windows版本 V1.0.0
 ************************************************************************************************************************/
#pragma once
#include "Global.h" 
#include "BasicObj.h"    

class Debugger;

/**
 * 说明：具有任意位置对象指针读取及对象指针循环写入功能的对象指针缓冲器，对象可以是任意对象。本模块是一个多线程操作安全模块。
 * 功能：
 * 1. 缓冲器的写入（AddObject）是循环的而且是覆盖性质的，如果写入单元已经存有对象指针，caller必须删除欲覆盖的指针所指向的对象。
 *    写入完成后，写指针自动加一指向下一个单元。
 * 2. 缓冲器内对象指针的读出可以是随机的，由caller在GetObjectAt的入参中指定
 * 3. 缓冲器内对象指针可以使用GetNextObject顺序读出，读出内容是当前读指针所指向的单元的下一个单元。调用结束后，读指针指向实际读出的单元。
 * 4. 缓冲器循环长度可以动态设置，但不得超过创建缓冲器对象时所给的最大长度。
 */
class ObjBuffer : BasicObj
{
public:
    ObjBuffer(uint bufLen);
    virtual ~ObjBuffer();

private:
    uint    bufLen;                         // 缓冲器对象指针个数   
    uint    circleLen;                      // 缓冲器循环长度   
    uint    rdIndex;                        // 循环缓冲器当前读地址
    uint    wrIndex;                        // 循环缓冲器当前写地址
    void**  buffer;                         // C++对象指针循环缓冲器
    bool    lock;                           // 多线程访问时的互斥锁，保证在任何时刻只有一个线程访问该Buffer
    uint    numObjects;                     // 当前有效对象指针数量
public:
    void*   GetObjectAt(uint index);        // 对象读取指针将被重置到指定位置，然后获取指定位置的对象指针
    void*   GetNextObject();                // 读指针加一指向下一个位置，并获取新的读指针所指向的对象指针
    uint    GetWriteIndex();                // 获取写指针
    void    AddObject(void* obj);           // 以覆盖的方式将指定的对象指针写入到当前写指针指向的位置
    void    SetCircleLen(uint circleLen);   // 设置循环缓冲器的循环长度
    uint    GetCircleLen();
    bool    IsLocked();
    void    TakeLock();
    void    ReleaseLock();
    uint    GetNumEntries();
};