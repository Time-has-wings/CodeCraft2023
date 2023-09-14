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
    // ���ݳ�ԱSet��
    void AimSet (double PosX, double PosY, double BenchId)
    {  
        AimX = PosX;
        AimY = PosY;
        AimId = BenchId;
    }

    void TaskTypeSet(AllTaskType Tp) { TaskType = Tp; }

    // ���ݳ�ԱGet��
    int GetAimId() { return AimId; }

    double GetAimX() { return AimX; }

    double GetAimY() { return AimY; }
    
    AllTaskType GetTaskType() { return TaskType; }
};

#endif