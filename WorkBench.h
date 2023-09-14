#pragma once
#ifndef _WorkBench_H
#define _WorkBench_H

#include "defs.h"

class WorkBench {
private:
    int Type;                                          // 工作台类型
    double PosX, PosY;                                 // 工作台坐标
    int RemainFPS;                                     // 工作台剩余生产帧数
    bool ProduceFinished;                              // 材料格状态
    int MaterialNeed;

    // 后续新增数据成员
    int MaterialHave;
    int MaterialReserved;
    bool OutputReserved;

public:
    // 数据成员Set类
    void TypeSet(int Tp);

    void PostitionSet(double Px, double Py);

    void RemainFPSSet(int fps);

    void ProduceFinishedSet(int status);

    void MaterialHaveSet(int own);

    void OutputReservedSet(bool status);

    // 数据成员Get类
    int GetType() { return Type; }

    double GetPosX() { return PosX; }

    double GetPosY() { return PosY; }

    int GetRemainFPS() { return RemainFPS; }

    bool IsProduceFinished() { return ProduceFinished; }

    bool DoMaterialNeed(int Mtype);

    bool DoMaterialHave(int Mtype);

    bool DoMaterialReserved(int Mtype);

    bool IsOutputReserved() { return OutputReserved; }

    // 订单模块函数
    void ReserveMaterial(int Mtype) { MaterialReserved |= (1 << Mtype); }
    
    void MaterialReceived(int Mtype) { MaterialReserved ^= (1 << Mtype); }

    // 需求计算
    int GetDemandCount();

}Bench[50];

#endif