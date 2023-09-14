#pragma once
#ifndef _Order_H
#define _Order_H

#include "defs.h"
#include "Tasks.h"

class Order {
private:
    Tasks ToBuy, ToSell;
    int CarryRobotId;
    int ClassId;
    double Value;

public:
    // 数据成员Set类
    void ToBuySet(double PosX, double PosY, int BuyBenchId)
    {
        ToBuy.AimSet(PosX, PosY, BuyBenchId);
        ToBuy.TaskTypeSet(Buy);
    }

    void ToSellSet(double PosX, double PosY, int SellBenchId)
    {
        ToSell.AimSet(PosX, PosY, SellBenchId);
        ToSell.TaskTypeSet(Sell);
    }

    void CarryRobotSet(int RobotId) { CarryRobotId = RobotId; }

    void ClassIdSet(int Id) { ClassId = Id; }

    void ValueSet(double val) { Value = val; }

    // 数据成员Get类
    Tasks GetBuyTask() { return ToBuy; }

    Tasks GetSellTask() { return ToSell; }

    int GetCarryRobotId() { return CarryRobotId; }

    int GetClassId() { return ClassId; }

    double GetValue() { return Value; }

    // 运算符重载(此处 未写成 bool operator < (const Order& Another) const )
    bool operator < (Order& Another) 
    { 
        return Value < Another.GetValue();
    }
};

#endif