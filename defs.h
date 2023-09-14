#pragma once 
// Cͷ�ļ�
#include <ctype.h>
// C++ͷ�ļ�
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>

// ����
const double PI = acos(-1);
const double AngleEpsFree = 0.127;  // ����Robot��
const double AngleEpsCarry = 0.245; // ����Robot��
const double NearBenchSpeed = 0.5;  // ����Move
const double CrashDis = 4.0;        // ����Move
const double DecDeliverto9 = 0.5;   // ����Value
const double DivSpeed = 6.0;        // ����Value
const double Demand456_1 = 0.5, Demand456_2 = 0.3;              // ����Value
const double Demand7_1 = 0.5, Demand7_2 = 0.3, Demand7_3 = 0.1; // ����Value
const double LastLossFrame = 7800;                              // ����Value
const double Price[] = {0, 3000, 3200, 3400, 7100, 7800, 8300, 29000};                             

// ȫ�ֱ���
int RobotCount = -1;
int WorkBenchCount = -1;
int FrameId, Money;
double BenchDistance[100][100];
char Map[105][105];

// ö������
enum AllTaskType
{
    NoTask = 0,
    Buy = 1, 
    Sell = 2
};