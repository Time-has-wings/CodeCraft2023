#pragma once
#ifndef _Robot_H
#define _Robot_H

#include "defs.h"
#include "Tasks.h"
#include "Order.h"
#include "WorkBench.h" // ע��������ļ�
#include "Vector.h"

class Robot {
private:
    int WorkBenchId;       // ��������̨ID
    int CarryItem;         // Я����Ʒ����
    double TimeValue;      // ʱ���ֵϵ��         
    double CollisionValue; // ��ײ��ֵϵ��
    double AngleSpeed;     // ���ٶ�
    double SpeedX, SpeedY; // ���ٶ�(X�����ٶ� + Y�����ٶ�)
    double Orientation;    // ����
    double PosX, PosY;     // ����(PosX, PosY)

    // �������ݳ�Ա
    double LineSpeed;     // ���ٶ�ģֵ LineSpeed = sqrt(SpeedX * SpeedX + SpeedY * SpeedY) 
    Order order;          // Ŀǰ��������order
    Tasks Task;           // Ŀǰ��������task
    bool DoingTask;       // Ŀǰ�Ƿ�����task
    int RotId;            // ������Id


public:
    // ���ݳ�ԱSet��
    void WorkBenchIdSet(int InBenchId) { WorkBenchId = InBenchId; }
    
    void CarryItemSet(int CarryId) { CarryItem = CarryId; }

    void TimeValueSet(double Value) { TimeValue = Value; }

    void CollisionValueSet(double Value) {CollisionValue = Value; }

    void AngleSpeedSet(double Speed) { AngleSpeed = Speed; }

    void SpeedXSet(double XSpeed) { SpeedX = XSpeed; }

    void SpeedYSet(double YSpeed) { SpeedY = YSpeed; }

    void SpeedSet(double Angle, double X, double Y) 
    {
        AngleSpeedSet(Angle);
        SpeedXSet(X);
        SpeedYSet(Y);
    }

    void LineSpeedSet(double Speed) { LineSpeed = Speed; }

    void OrientationSet(double Orient) { Orientation = Orient; }

    void PositionSet(double Px, double Py) { PosX = Px; PosY = PosY; }

    // ���ݳ�ԱGet��
    int GetCarryItem() { return CarryItem; }

    double GetOrientation() { return Orientation; }

    double GetSpeedX() { return SpeedX; }

    double GetSpeedY() { return SpeedY; }

    double GetLineSpeed() { return LineSpeed; }
    
    double GetPosX() { return PosX; }

    double GetPosY() { return PosY; } 

    // ������
    bool IsCarry() { return CarryItem != 0; }

    void Print(int Id)
    {
        std::cout << "forward " << Id << " " << LineSpeed << '\n';
        std::cout << "rotate " << Id << " " << AngleSpeed << '\n';
    }

    // task & order����
    void TaskSet(Tasks task);
    void SetNoTask();
    bool IsFree();
    void TakeOrder(Order ord);
    void DoSellAction();
    void DoBuyAction();

    // ����ת��
    double AngleCheck();
    int AdjustAngle();

    // ������Ŀ�깤��̨�ľ���
    double DisFromAim() { return Vector::GetDistance(PosX, PosY, Task.GetAimX(), Task.GetAimY()); }

    // ���û�����Id
    void RotIdSet(int id) { RotId = id; }
}Rob[4];

#endif