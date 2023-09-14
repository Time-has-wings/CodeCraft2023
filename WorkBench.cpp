#include "WorkBench.h"

// ���ݳ�ԱSet��
void WorkBench::TypeSet(int Tp) 
{
    Type = Tp;
    switch(Tp) {
        case 1:
            MaterialNeed = 0;
            break;
        case 2:
            MaterialNeed = 0;
            break;
        case 3:
            MaterialNeed = 0;
            break;
        case 4:
            MaterialNeed = 2 + 4;
            break;
        case 5:
            MaterialNeed = 2 + 8;
            break;
        case 6:
            MaterialNeed = 4 + 8;
            break;
        case 7:
            MaterialNeed = 16 + 32 + 64;
            break;
        case 8:
            MaterialNeed = 128;
            break;
        case 9:
            MaterialNeed = 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128;
            break;
    }
}

void WorkBench::PostitionSet(double Px, double Py) 
{
    PosX = Px;
    PosY = Py;
}

void WorkBench::RemainFPSSet(int fps)
{
    RemainFPS = fps;
}

void WorkBench::ProduceFinishedSet(int status)
{
    if (status == 0)
        ProduceFinished = false;
    else 
        ProduceFinished = true;
}

void WorkBench::MaterialHaveSet(int own)
{
    MaterialHave = own;
}

void WorkBench::OutputReservedSet(bool status)
{
    OutputReserved = status;
}

// ���ݳ�ԱGet��
bool WorkBench::DoMaterialNeed(int Mtype)
{
    if ((MaterialNeed >> Mtype) & 1) 
        return true;
    return false;
}

bool WorkBench::DoMaterialHave(int Mtype)
{
    if ((MaterialHave >> Mtype) & 1)
        return true;
    return false;
}

bool WorkBench::DoMaterialReserved(int Mtype)
{
    if ((MaterialReserved >> Mtype) & 1)
        return true;
    return false;
}

// �������
int WorkBench::GetDemandCount()
{
    int res = 0;
    for (int Mtype = 1; Mtype <= 7; Mtype ++)
    {
        if (DoMaterialNeed(Mtype) && !DoMaterialHave(Mtype) && !DoMaterialReserved(Mtype))
            res ++;
    }
    return res;
}