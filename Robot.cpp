#include "Robot.h"

/**
 * @brief 接单模型
*/
void Robot::TaskSet(Tasks task)
{
    Task = task;
    DoingTask = true;
}

void Robot::SetNoTask() 
{
    Task.TaskTypeSet(NoTask);
    DoingTask = false;
}

bool Robot::IsFree() 
{
    if (DoingTask) 
        return false;
    else   
        return true;
}

void Robot::TakeOrder(Order ord)
{
    order = ord;
    TaskSet(ord.GetBuyTask());
    // 设置Bench数据成员
    Bench[order.GetBuyTask().GetAimId()].OutputReservedSet(true);
    int ToModify = order.GetSellTask().GetAimId();
    Bench[ToModify].ReserveMaterial(Bench[order.GetBuyTask().GetAimId()].GetType());
}

void Robot::DoBuyAction()
{
    TaskSet(order.GetSellTask());
    Bench[order.GetBuyTask().GetAimId()].OutputReservedSet(false);
}

void Robot::DoSellAction()
{   
    int ToModify = order.GetSellTask().GetAimId();
    Bench[ToModify].MaterialReceived(Bench[order.GetBuyTask().GetAimId()].GetType());
    Task.TaskTypeSet(NoTask);
    DoingTask = false;
}

/**
 * @brief 转向调整模型
*/

int Robot::AdjustAngle() 
{
    double NowAngle = Vector::GetAngleByPoint(PosX, PosY, Task.GetAimX(), Task.GetAimY());
    double AngleSub = Orientation - NowAngle;
    if (AngleSub >= PI || (AngleSub >= -PI && AngleSub <= 0)) 
        return 1;
    else 
        return -1;
}

double Robot::AngleCheck() 
{
    double NowAngle = Vector::GetAngleByPoint(PosX, PosY, Task.GetAimX(), Task.GetAimY(), 0, 0, cos(Orientation), sin(Orientation));
    if (IsCarry() == false && NowAngle > AngleEpsFree) return NowAngle;
    else if (IsCarry() == true && NowAngle > AngleEpsCarry) return NowAngle;
    else return -1;
}