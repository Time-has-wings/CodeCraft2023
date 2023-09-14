#pragma once
#ifndef _Function_H
#define _Function_H

#include "defs.h"
#include "WorkBench.h"
#include "Tasks.h"
#include "Order.h"
#include "Vector.h"
#include "Robot.h"
#include "Value.h"
#include "Move.h"
#include "Decision.h"

/**
 * @brief 地图初始化
*/
void MapInit()
{
    for (int i = 1; i <= 100; i++)
    {
        for (int j = 1; j <= 100; j++)
        {
            std::cin >> Map[i][j];
            if (Map[i][j] == '.')
                continue;
            else if (isdigit(Map[i][j]))
            {
                ++WorkBenchCount;
                double NowX = 0.25 + 0.5 * (j - 1);
                double NowY = 49.75 - 0.5 * (i - 1);
                Bench[WorkBenchCount].PostitionSet(NowX, NowY);
                Bench[WorkBenchCount].TypeSet((int)(Map[i][j] - '0'));
            }
            else if (Map[i][j] == '#')
            {

            }
            else 
            {
                ++RobotCount;
                double NowX = 0.25 + 0.5 * (j - 1);
                double NowY = 49.75 - 0.5 * (i - 1);
                Rob[RobotCount].PositionSet(NowX, NowY);
                Rob[RobotCount].RotIdSet(RobotCount);
            }
        }
    }
    std::string RubbishBin;
    std::cin >> RubbishBin;
}

/**
 * @brief 准备阶段
*/
void PreProcess()
{
    MapInit(); 
    std::cout << "OK" << '\n';
}

/**
 * @brief 读入每一帧
*/
bool InputOfFrame(void) 
{ 
    int irubbishbin;
    double drubbishbin;
    std::cin >> FrameId;
    if (FrameId == -1)
        return false;
    std::cin >> Money;
    std::cin >> irubbishbin;
    for (int i = 0; i <= WorkBenchCount; i++) 
    {
        std::cin >> irubbishbin;
        std::cin >> drubbishbin;
        std::cin >> drubbishbin;
        int fps;
        std::cin >> fps;
        Bench[i].RemainFPSSet(fps);
        int own;
        std::cin >> own;
        Bench[i].MaterialHaveSet(own);
        int status;
        std::cin >> status;
        Bench[i].ProduceFinishedSet(status);
    }
    for (int i = 0; i <= RobotCount; i++) 
    {
        int InBenchId;
        std::cin >> InBenchId;
        Rob[i].WorkBenchIdSet(InBenchId);
        // Rob[i].RemainBenchIdSet(InBenchId);
        int CarryId;
        std::cin >> CarryId;
        Rob[i].CarryItemSet(CarryId);
        double fTime;
        std::cin >> fTime;
        Rob[i].TimeValueSet(fTime);
        double fColl;
        std::cin >> fColl;
        Rob[i].CollisionValueSet(fColl);
        double SAngle, Sx, Sy;
        std::cin >> SAngle >> Sx >> Sy;
        Rob[i].SpeedSet(SAngle, Sx, Sy);
        double orient;
        std::cin >> orient;
        Rob[i].OrientationSet(orient);
        double px, py;
        std::cin >> px >> py;
        Rob[i].PositionSet(px, py);
    }
    std::string srubbishbin;
    std::cin >> srubbishbin;
    return true;
}

/**
 * @brief 每一帧
*/
void ProcessByFrame()
{
    while (true)
    {
        bool status = InputOfFrame();
        if (status == false) 
            break;
        std::cout << FrameId << '\n';

    }
}

#endif

