#pragma once 
// C头文件
#include <ctype.h>
// C++头文件
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>

// 常量
const double PI = acos(-1);
const double AngleEpsFree = 0.127;  // 用于Robot类
const double AngleEpsCarry = 0.245; // 用于Robot类
const double NearBenchSpeed = 0.5;  // 用于Move
const double CrashDis = 4.0;        // 用于Move
const double DecDeliverto9 = 0.5;   // 用于Value
const double DivSpeed = 6.0;        // 用于Value
const double Demand456_1 = 0.5, Demand456_2 = 0.3;              // 用于Value
const double Demand7_1 = 0.5, Demand7_2 = 0.3, Demand7_3 = 0.1; // 用于Value
const double LastLossFrame = 7800;                              // 用于Value
const double Price[] = {0, 3000, 3200, 3400, 7100, 7800, 8300, 29000};                             

// 全局变量
int RobotCount = -1;
int WorkBenchCount = -1;
int FrameId, Money;
double BenchDistance[100][100];
char Map[105][105];

// 枚举类型
enum AllTaskType
{
    NoTask = 0,
    Buy = 1, 
    Sell = 2
};