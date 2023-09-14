#pragma once
#ifndef _Robot_H
#define _Robot_H

#include "defs.h"
#include "Tasks.h"
#include "Order.h"
#include "WorkBench.h" // 注意包含此文件
#include "Vector.h"

class Robot {
private:
    int WorkBenchId;       // 所处工作台ID
    int CarryItem;         // 携带物品类型
    double TimeValue;      // 时间价值系数         
    double CollisionValue; // 碰撞价值系数
    double AngleSpeed;     // 角速度
    double SpeedX, SpeedY; // 线速度(X方向速度 + Y方向速度)
    double Orientation;    // 朝向
    double PosX, PosY;     // 坐标(PosX, PosY)

    // 新增数据成员
    double LineSpeed;     // 线速度模值 LineSpeed = sqrt(SpeedX * SpeedX + SpeedY * SpeedY) 
    Order order;          // 目前正在做的order
    Tasks Task;           // 目前正在做的task
    bool DoingTask;       // 目前是否在做task
    int RotId;            // 机器人Id


public:
    // 数据成员Set类
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

    // 数据成员Get类
    int GetCarryItem() { return CarryItem; }

    double GetOrientation() { return Orientation; }

    double GetSpeedX() { return SpeedX; }

    double GetSpeedY() { return SpeedY; }

    double GetLineSpeed() { return LineSpeed; }
    
    double GetPosX() { return PosX; }

    double GetPosY() { return PosY; } 

    // 功能类
    bool IsCarry() { return CarryItem != 0; }

    void Print(int Id)
    {
        std::cout << "forward " << Id << " " << LineSpeed << '\n';
        std::cout << "rotate " << Id << " " << AngleSpeed << '\n';
    }

    // task & order函数
    void TaskSet(Tasks task);
    void SetNoTask();
    bool IsFree();
    void TakeOrder(Order ord);
    void DoSellAction();
    void DoBuyAction();

    // 调整转向
    double AngleCheck();
    int AdjustAngle();

    // 计算与目标工作台的距离
    double DisFromAim() { return Vector::GetDistance(PosX, PosY, Task.GetAimX(), Task.GetAimY()); }

    // 设置机器人Id
    void RotIdSet(int id) { RotId = id; }
}Rob[4];

#endif