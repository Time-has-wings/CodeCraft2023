#pragma once
#ifndef _Tasks_H
#define _Tasks_H

#include "defs.h"

class Tasks {
private:
    double AimX, AimY;
    int AimId;
    AllTaskType TaskType;

public:
    // 数据成员Set类
    void AimSet (double PosX, double PosY, double BenchId)
    {  
        AimX = PosX;
        AimY = PosY;
        AimId = BenchId;
    }

    void TaskTypeSet(AllTaskType Tp) { TaskType = Tp; }

    // 数据成员Get类
    int GetAimId() { return AimId; }

    double GetAimX() { return AimX; }

    double GetAimY() { return AimY; }
    
    AllTaskType GetTaskType() { return TaskType; }
};

#endif