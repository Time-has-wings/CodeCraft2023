// 2023.4.5 重构版本

#include <ctype.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <vector>

using namespace std;

// 常量区定义
const double PI = acos(-1);
const double AngleEpsFree = 0.127;
const double AngleEpsCarry = 0.245;
const double MinAngle = 0.08;
const double INF = 100000.0;
const double Price[] = {0, 3000, 3200, 3400, 7100, 7800, 8300, 29000};
const double DivSpeed = 6.0;
const int PathBenchCount = 55;
const int PathRobotCount = 4;
const double MaxSpeed = 6.0;
const double ExceedSpeed = 6.6;
const double MaxAngleSpeed = PI;
const double ExceedAngleSpeed = PI * 1.1;
const double EmptyRadius = 0.45;
const double CarryRadius = 0.53;
const double CrashEps = 0.01; // 这个是干啥的
const double CrashLineSpeed = 1.0;
const int CheckFrames = 15;

// 全局变量定义
int WorkBenchCount = -1;
int RobotCount = -1;
int frameID, Money;
double RobotBenchDistance[PathRobotCount][PathBenchCount];
double CarryBenchDistance[PathBenchCount][PathBenchCount];
double EmptyBenchDistance[PathBenchCount][PathBenchCount];
double Balance[] = {0, 1, 1, 1, 1, 1, 1, 1};
int AllProduceCount[] = {0, 0, 0, 0, 0, 0, 0, 0};
bool flag7 = false;
char Map[107][107];
int MapId;                                                              // 判定地图的编号
vector<pair<double, double>> EmptyPath[PathBenchCount][PathBenchCount]; // 工作台间路径——空手
vector<pair<double, double>> CarryPath[PathBenchCount][PathBenchCount]; // 工作台间路径——携带物品
vector<pair<double, double>> RobPath[PathRobotCount][PathBenchCount];   // 机器人初始位置到工作台
int RobotColor[5];

namespace Vector {
    struct Vec {
        double x, y;
        bool operator==(const Vec a) const {
            return (fabs(a.y - y) <= 1e-6) && (fabs(a.x - x) < 1e-6);
        }
        Vec(double X = 0, double Y = 0) {
            x = X, y = Y;
        }
    };

    double dottimes(Vec a, Vec b) {
        double ret = a.x * b.x + a.y * b.y;
        return ret;
    }

    double size(Vec a) {
        double ret = a.x * a.x + a.y * a.y;
        ret = sqrt(ret);
        return ret;
    }

    double AngleCalc(Vec a, Vec b) {
        double cosval = dottimes(a, b) / (size(a) * size(b));
        double ret = acos(cosval);
        return ret;
    }

    double AngleX(Vec a) {
        Vec b;
        b.x = 1.0, b.y = 0.0;
        if (a.y > 0)
            return AngleCalc(a, b);
        else
            return -AngleCalc(a, b);
    }

    double GetAngleByPoint(double sx, double sy, double ex, double ey) {
        Vec X;
        X.x = ex - sx, X.y = ey - sy;
        return AngleX(X);
    }

    double GetAngleByPoint(double sx1, double sy1, double ex1, double ey1, double sx2, double sy2, double ex2, double ey2) {
        Vec X;
        X.x = ex1 - sx1, X.y = ey1 - sy1;
        Vec Y;
        Y.x = ex2 - sx2, Y.y = ey2 - sy2;
        return AngleCalc(X, Y);
    }

    double GetDistance(double sx, double sy, double ex, double ey) {
        double DisX = ex - sx, DisY = ey - sy;
        double power = DisX * DisX + DisY * DisY;
        double res = sqrt(power);
        return res;
    }
} // namespace Vector

class WorkBench {
private:
    int Type;
    double PosX, PosY;
    int RemainFPS;
    bool ProductHave;               // true -> 产品格有产品
    int MaterialNeed, MaterialHave; // Need -> 需要的类型 have ->有的类型
    int MaterialReserved;           // 原料格预定情况
    bool OutputReserved;
    int IX, IY;

public:
    // Set类
    void TypeSet(int Tp) {
        Type = Tp;
        switch (Tp) {
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
    void PositionSet(double Px, double Py) { PosX = Px, PosY = Py; }
    void PositionGridSet(int i, int j) { IX = i, IY = j; }
    void Remainset(int fps) { RemainFPS = fps; }
    void HaveSet(int own) { MaterialHave = own; } // 原料格状态设置
    void ProductHaveSet(int status) {
        if (status == 0)
            ProductHave = false;
        else
            ProductHave = true;
    }

    // Get类
    int GetType() { return Type; }
    double GetPosX() { return PosX; }
    double GetPosY() { return PosY; }
    int GetRemainFPS() { return RemainFPS; }
    bool GetProductHave() { return ProductHave; } // 获得产品格状态
    int GetPosIX() { return IX; }
    int GetPosIY() { return IY; }

    // 功能类
    bool DoMaterialNeed(int Mtype) { // 收不收购 Mtype 类型的材料
        if ((MaterialNeed >> Mtype) & 1)
            return true;
        return false;
    } 
    bool DoMaterialHave(int Mtype) { // 有没有 Mtype 类型的材料
        if (Type == 9 || Type == 8)
            return false; // 解除对8，9的束缚
        if ((MaterialHave >> Mtype) & 1)
            return true;
        return false;
    } 
    bool DoMaterialReserved(int Mtype) { // Mtype 类型的材料有没有机器人 claim 要送过来
        if (Type == 9 || Type == 8)
            return false; // 解除对8，9的束缚
        if ((MaterialReserved >> Mtype) & 1)
            return true;
        return false;
    } 
    void ReserveMaterial(int Mtype) { MaterialReserved |= (1 << Mtype); }
    void MaterialReceived(int Mtype) { MaterialReserved ^= (1 << Mtype); }
    int DemandCount() {
        int cnt = 0;
        for (int Mtype = 1; Mtype <= 7; Mtype++) {
            if (DoMaterialNeed(Mtype) && !DoMaterialHave(Mtype) && !DoMaterialReserved(Mtype))
                cnt++;
        }
        return cnt;
    }
    void OutputReservedSet(bool status) { OutputReserved = status; }
    bool isOutputReserved() { return OutputReserved; }
    bool DoMaterialNeedByFPS(int Mtype, double TIM) {
        if (0.02 * RemainFPS < TIM)
            return true;
        return false;
    }
} Bench[50];

enum AllTaskType {
    notask = 0,
    buy = 1,
    sell = 2
};

class Tasks {
private:
    double AimPointX, AimPointY;
    double AimBenchX, AimBenchY;
    int AimId;
    AllTaskType TaskType;
    vector<pair<double, double>> TaskPath;
    int NowPoint = 0;

public:
    // Set类
    void AimBenchSet(int BenchId) {
        AimBenchX = Bench[BenchId].GetPosX(), AimBenchY = Bench[BenchId].GetPosY();
        AimId = BenchId;
    }
    void AimTypeSet(AllTaskType tp) { TaskType = tp; }
    void PathSet(vector<pair<double, double>> pat) {
        int length = pat.size();
        TaskPath.clear();
        TaskPath = pat;
        NowPoint = 0;
    }
    int GetPathLen() { return TaskPath.size(); }

    void NextPointSet() {
        if (NowPoint == (int)TaskPath.size()) {
            return;
        }
        pair<double, double> NextPoint = TaskPath[NowPoint++];
        AimPointX = NextPoint.first;
        AimPointY = NextPoint.second;
    }

    // Get类
    double GetAimBenchX() { return AimBenchX; }
    double GetAimBenchY() { return AimBenchY; }
    double GetAimPointX() { return AimPointX; }
    double GetAimPointY() { return AimPointY; }
    int GetAimId() { return AimId; }
    AllTaskType GetType() { return TaskType; }
    vector<pair<double, double>> GetNextPoints() { //获得该路径之后的5个点
        vector<pair<double, double>> res;
        int limit = min((int)TaskPath.size() - 1, NowPoint + 5);
        for (int i = NowPoint; i <= limit; i++) {
            res.push_back(TaskPath[i]);
        }
        return res;
    }
    vector<pair<double, double>> GetPath() {
        vector<pair<double, double>> res;
        for (int i = NowPoint; i < (int)TaskPath.size(); i++) {
            res.push_back(TaskPath[i]);
        }
        return res;
    }
};

class Order {
private:
    Tasks toBuy, toSell;
    int CarryBotId;
    int classid;
    double Value;

public:
    // Set类
    void OrderGenerate(int BuyBenchId, int SellBenchId) {
        toBuy.AimBenchSet(BuyBenchId);
        toBuy.AimTypeSet(buy);
        toSell.AimBenchSet(SellBenchId);
        toSell.AimTypeSet(sell);
    }
    void BotIdSet(int id) { CarryBotId = id; }
    void ValSet(double val) { Value = val; }
    void ClassIdSet(int id) { classid = id; }
    void OrderPathSet(int id, int RemainBenchId) {
        if (RemainBenchId != -1) {
            toBuy.PathSet(EmptyPath[RemainBenchId][toBuy.GetAimId()]);
            toSell.PathSet(CarryPath[toBuy.GetAimId()][toSell.GetAimId()]);
        }
        else {
            toBuy.PathSet(RobPath[id][toBuy.GetAimId()]);
            toSell.PathSet(CarryPath[toBuy.GetAimId()][toSell.GetAimId()]);
        }
    }

    // Get类
    Tasks GetBuyTask() { return toBuy; }
    Tasks GetSellTask() { return toSell; }
    int GetRobotId() { return CarryBotId; }
    int GetClassId() { return classid; }
    double GetVal() { return Value; }
};

vector<Order> ordervec;

bool comp(Order a, Order b) { return a.GetVal() > b.GetVal(); }

class Robot {
private:
    int WorkBenchId;
    int CarryItem;
    double TimeVal, CollisionVal;
    double SpeedAngle;
    double SpeedX, SpeedY;
    double Orientation;
    double PosX, PosY;
    double PrePosX, PrePosY; // 预测位置坐标

    Tasks Task;
    Order order;
    bool DoingTask;
    double SpeedLine;       // 增加的线速度 用于打印
    int RemainBenchId = -1; // 始终保存最后一个WorkBenchId，防止机器人未及时接单
    int RobId;

public:
    // Set类
    // 题意
    void WorkBenchIdSet(int InBenchId) { WorkBenchId = InBenchId; }
    void CarrySet(int CarryID) { CarryItem = CarryID; }
    void TimeValSet(double fTime) { TimeVal = fTime; }
    void CollisionValSet(double fColl) { CollisionVal = fColl; }
    void SpeedSet(double SAngle, double lx, double ly) {
        SpeedAngle = SAngle, SpeedX = lx, SpeedY = ly;
    }
    void OrientationSet(double Orient) { Orientation = Orient; }
    void PositionSet(double Px, double Py) { PosX = Px, PosY = Py; }
    // Our设计
    void OrderSet(Order ord) {
        TaskSet(ord.GetBuyTask());
        Bench[ord.GetBuyTask().GetAimId()].OutputReservedSet(true);
        Task.NextPointSet(); // 设置第一个路径点
        order = ord;
        int tomodify = ord.GetSellTask().GetAimId();
        Bench[tomodify].ReserveMaterial(Bench[ord.GetBuyTask().GetAimId()].GetType());
    }
    void TaskSet(Tasks task) {
        Task = task;
        DoingTask = true;
    }
    void SpeedLineSet(double speedline) { // 设置线速度 用于打印
        SpeedLine = speedline;
    } 
    void SpeedAngleSet(double speedangle) { // 设置角速度 用于打印
        SpeedAngle = speedangle;
    } 
    void RemainBenchIdSet(int BenchId) {
        if (BenchId != -1) { //输入中-1表示不在工作台附近
            RemainBenchId = BenchId;
        }
    }
    void RobIdSet(int id) { RobId = id; }
    void PrePosSet(double preX, double preY) { PrePosX = preX, PrePosY = preY; }

    // Get类
    // 题意
    int GetCarry() { return CarryItem; }
    double GetSpeedX() { return SpeedX; }
    double GetSpeedY() { return SpeedY; }
    double GetOrientation() { return Orientation; };
    double GetX() { return PosX; }
    double GetY() { return PosY; }
    // Our设计
    Order GetOrder() { return order; }
    int GetAimBenchId() { return Task.GetAimId(); }
    AllTaskType GetTaskType() { return Task.GetType(); }
    double GetSpeedLine() { return SpeedLine; }
    double GetDist(double Ax, double Ay) {
        double ret = (Ax - PosX) * (Ax - PosX) + (Ay - PosY) * (Ay - PosY);
        ret = sqrt(ret);
        return ret;
    }
    Tasks GetTask() { return Task; }
    Tasks &GetTask(int i) { return Task; } // 设置下一个目标点时使用
    int GetRemainBenchId() { return RemainBenchId; }
    double GetPrePosX() { return PrePosX; }
    double GetPrePosY() { return PrePosY; }
    vector<pair<double, double>> GetNextPoints() {
        return Task.GetNextPoints();
    }
    double GetVal() { // IsCarry?Isfree
        if (IsCarry() == true) return order.GetVal();
        return -1.0;
    }
    // 功能类
    bool IsCarry() {
        if (CarryItem != 0) return true;
        return false;
    }
    bool isfree() {
        if (DoingTask) return false;
        return true;
    }
    bool TaskFinished() { // 是否到达指定工作台
        int AimBenchId = Task.GetAimId();
        if (WorkBenchId == AimBenchId)
            return true;
        return false;
    }
    double AngleCheck() {
        double NowAngle = Vector::GetAngleByPoint(
            PosX, PosY, Task.GetAimPointX(), Task.GetAimPointY(), 0, 0,
            cos(Orientation), sin(Orientation));
        return NowAngle;
    }
    int AdjustAngle() {
        double NowAngle = Vector::GetAngleByPoint(PosX, PosY, Task.GetAimPointX(), Task.GetAimPointY());
        double AngleSub = Orientation - NowAngle;
        if (AngleSub >= PI || (AngleSub >= -PI && AngleSub <= 0))
            return 1;
        return -1;
    } // 返回值为 -1 代表顺时针旋转，返回值为 1 代表逆时针旋转
    void sell() {
        int tomodify = order.GetSellTask().GetAimId();
        Bench[tomodify].MaterialReceived(Bench[order.GetBuyTask().GetAimId()].GetType());
        Task.AimTypeSet(notask);
        DoingTask = false;
    }
    void buy() {
        TaskSet(order.GetSellTask());
        Bench[order.GetBuyTask().GetAimId()].OutputReservedSet(false);
        Task.NextPointSet(); // 设置第一个路径点
    }
    void print(int id) {
        cout << "forward " << id << " " << SpeedLine << '\n';
        cout << "rotate " << id << " " << SpeedAngle << '\n';
    }
    double dis() { return GetDist(Task.GetAimBenchX(), Task.GetAimBenchY()); }
} Rob[4];

namespace TimeFunc {
    double TurnCostRobtoBuy(int BuyId, int SellId, int BotId) { // Rot -> Buy 花的转角时间
        double Angle = 0;
        vector<pair<double, double>> pat;
        if (Rob[BotId].GetRemainBenchId() != -1) {
            pat = EmptyPath[Rob[BotId].GetRemainBenchId()][BuyId];
        }
        else {
            pat = RobPath[BotId][BuyId];
        }
        if (!pat.size()) {
            return INF; //返回正无穷
        }
        Angle += Vector::GetAngleByPoint( // 第一个角
            pat[0].first, pat[0].second, pat[1].first, pat[1].second, 0, 0,
            cos(Rob[BotId].GetOrientation()), sin(Rob[BotId].GetOrientation()));
        for (int i = 1; i < pat.size() - 1; i++) {
            Angle += Vector::GetAngleByPoint(
                pat[i - 1].first, pat[i - 1].second, pat[i].first, pat[i].second,
                pat[i].first, pat[i].second, pat[i + 1].first, pat[i + 1].second);
        }
        return Angle / PI;
    }

    double TurnCostBuytoSell(int BuyId, int SellId, int BotId) { // Buy -> Sell 花的转角时间
        double Angle = 0; // 没有加上第一个角，太复杂而且影响较小
        vector<pair<double, double>> pat = CarryPath[BuyId][SellId];
        if (!pat.size()) {
            return INF;
        }
        for (int i = 1; i < pat.size() - 1; i++) {
            Angle += Vector::GetAngleByPoint(
                pat[i - 1].first, pat[i - 1].second, pat[i].first, pat[i].second,
                pat[i].first, pat[i].second, pat[i + 1].first, pat[i + 1].second);
        }
        return Angle / PI;
    }

    double OrderTime(int BuyId, int SellId, int BotId) { // 计算整个订单完成的时间
        double Distance1;
        if (Rob[BotId].GetRemainBenchId() != -1) {
            Distance1 = EmptyBenchDistance[Rob[BotId].GetRemainBenchId()][BuyId];
        }
        else {
            Distance1 = RobotBenchDistance[BotId][BuyId];
        }
        double Distance2 = CarryBenchDistance[BuyId][SellId];
        double Time = TurnCostRobtoBuy(BuyId, SellId, BotId) + TurnCostBuytoSell(BuyId, SellId, BotId) + (Distance1 + Distance2) / DivSpeed;
        return Time;
    }
} // namespace TimeFunc

namespace Move {
    // 参数&常量区定义
    const double WallJudge = 2.5;
    const double OrienJudge = 0.25;
    const double SlowDown = 0.5;
    const double NearBenchSpeed = 0.5;
    const double NearAimJudge = 0.6; //与目标点之间的距离
    const double CrashDis = 4.0;
    const int PredictFrame = 5; //为什么要设置为15帧 是不是太长了

    struct Point {
        double x, y;
        Point operator+(const Point &p) {
            Point sum;
            sum.x = this->x + p.x;
            sum.y = this->y + p.y;
            return sum;
        }
        Point operator-(const Point &p) {
            Point minus;
            minus.x = this->x - p.x;
            minus.y = this->y - p.y;
            return minus;
        }
        Point operator*(int p) {
            Point mult;
            mult.x = this->x * p;
            mult.y = this->y * p;
            return mult;
        }
    };

    void PredictPos(int id) {
        double preX = Rob[id].GetX() + Rob[id].GetSpeedX() * PredictFrame / 50;
        double preY = Rob[id].GetY() + Rob[id].GetSpeedY() * PredictFrame / 50;
        Rob[id].PrePosSet(preX, preY);
    }

    double isNearWall(int id) {
        double nPosX = Rob[id].GetPrePosX(), nPosY = Rob[id].GetPrePosY();
        double Orientation = Rob[id].GetOrientation();
        // 右撞墙 左撞墙 上撞墙 下撞墙 无撞墙
        if (nPosX > 50 - WallJudge && Orientation > (-PI / 2 + OrienJudge) && Orientation < (PI / 2 - OrienJudge))
            return SlowDown;
        else if (nPosX < WallJudge && Orientation > (PI / 2 + OrienJudge) && Orientation < (-PI / 2 - OrienJudge))
            return SlowDown;
        else if (nPosY > 50 - WallJudge && Orientation > OrienJudge && Orientation < PI - OrienJudge)
            return SlowDown;
        else if (nPosY < WallJudge && Orientation > -OrienJudge && Orientation < -PI + OrienJudge)
            return SlowDown;
        else
            return 1.0;
    }

    bool isNearAimPoint(int id){
        double distance = Vector::GetDistance(Rob[id].GetX(), Rob[id].GetY(), Rob[id].GetTask().GetAimPointX(), Rob[id].GetTask().GetAimPointY());
        if (distance < NearAimJudge) {
            return true;
        }
        return false;
    }

    void RobotMove(int id) {
        double AllowMoveSpeed = 5.0;
        PredictPos(id);
        if (isNearAimPoint(id)) {
            Rob[id].GetTask(1).NextPointSet(); //这个函数调的是引用
            AllowMoveSpeed = 2.5;
        }
        double NowAngle = Rob[id].AngleCheck(); // NowAngle值域[0, PI]
        double wallJudge = isNearWall(id);
        int SpanDirection = Rob[id].AdjustAngle(); // 顺 or 逆
        if (NowAngle < MinAngle) { // 如果朝向和目标点的夹角很小，直接全速前进
            Rob[id].SpeedLineSet(AllowMoveSpeed * wallJudge);
            Rob[id].SpeedAngleSet(0);
        }
        else {
            if (NowAngle > PI / 2) {
                // 角度太大，全速扭转
                // 速度控制小一点，避免靠近不了工作台
                Rob[id].SpeedLineSet(AllowMoveSpeed * 0.2 * wallJudge);
                Rob[id].SpeedAngleSet(1.0 * SpanDirection * ExceedAngleSpeed);
            }
            else {
                Rob[id].SpeedLineSet(AllowMoveSpeed * cos(NowAngle) * wallJudge);
                Rob[id].SpeedAngleSet(MaxAngleSpeed * SpanDirection * sin(NowAngle));
            }
        }
    }

    void CrashRobAdjust(int RobId1, int RobId2, int TurnDire, double dis) {
        double var1 = Rob[RobId1].GetOrder().GetVal();
        double var2 = Rob[RobId2].GetOrder().GetVal();
        if (var2 > var1)
            swap(RobId1, RobId2);
        Rob[RobId1].SpeedLineSet(2);
        Rob[RobId2].SpeedAngleSet(TurnDire * ExceedAngleSpeed);
        Rob[RobId2].SpeedLineSet(3);
        if (dis <= 1.1) { // 距离太小 表示已经撞上去了
            Rob[RobId1].SpeedLineSet(-1);
            Rob[RobId2].SpeedLineSet(-1);
        }
    }

    void Crash() {
        for (int i = 0; i <= RobotCount; i++) {
            for (int j = i + 1; j <= RobotCount; j++) {
                double dis = Rob[j].GetDist(Rob[i].GetX(), Rob[i].GetY());
                if (dis <= CrashDis) {
                    // Rob[j]相对于Rob[i]的相对坐标
                    double dx = Rob[j].GetX() - Rob[i].GetX(),
                           dy = Rob[j].GetY() - Rob[i].GetY();
                    // Rob[j]相对于Rob[i]的相对速度
                    double dxspeed = Rob[j].GetSpeedX() - Rob[i].GetSpeedX();
                    double dyspeed = Rob[j].GetSpeedY() - Rob[i].GetSpeedY();
                    // 相对坐标算的角度
                    double angle = Vector::GetAngleByPoint(dx, dy, 0, 0);
                    double anglecmp = asin(1 / dis);
                    double speedangle = Vector::GetAngleByPoint(0, 0, dxspeed, dyspeed);
                    double leftangle = angle - anglecmp, rightangle = angle + anglecmp;
                    if (leftangle < -PI) { // 这种情况下rightangle绝对不会出现任何问题
                        leftangle += 2 * PI;
                        if ((-PI <= speedangle && speedangle <= angle) ||
                            (leftangle <= speedangle && speedangle <= PI)) // 顺时针
                            CrashRobAdjust(i, j, -1, dis);
                        if (angle <= speedangle &&
                            speedangle <= angle + anglecmp) // 逆时针转动
                            CrashRobAdjust(i, j, 1, dis);
                    }
                    else {
                        if (rightangle > PI) { // 这种情况下leftangle绝对不会出现任何问题
                            rightangle -= 2 * PI;
                            if (angle - anglecmp <= speedangle &&
                                speedangle <= angle) // 顺时针
                                CrashRobAdjust(i, j, -1, dis);
                            if ((angle <= speedangle && speedangle <= PI) ||
                                (-1 * PI <= speedangle && speedangle <= rightangle))
                                CrashRobAdjust(i, j, 1, dis);
                        }
                        else { // 正常现象
                            if (angle - anglecmp <= speedangle &&
                                speedangle <= angle) // 顺时针
                                CrashRobAdjust(i, j, -1, dis);
                            if (angle <= speedangle &&
                                speedangle <= angle + anglecmp) // 逆时针转动
                                CrashRobAdjust(i, j, 1, dis);
                        }
                    }
                }
            }
        }
    }

    void isNearBench() { // 接近工作台
        for (int BotId1 = 0; BotId1 <= RobotCount; BotId1++) {
            for (int BotId2 = BotId1 + 1; BotId2 <= RobotCount; BotId2++) {
                if (Rob[BotId1].GetAimBenchId() != Rob[BotId2].GetAimBenchId()) return;
                const double NearBenchJudge = 7.5;
                const double SlowDown = 2.0;
                if (Rob[BotId1].dis() > NearBenchJudge)
                    return;
                if (Rob[BotId2].dis() > NearBenchJudge)
                    return;
                if (Rob[BotId1].dis() < Rob[BotId2].dis()) {
                    Rob[BotId2].SpeedLineSet(SlowDown);
                }
                else {
                    Rob[BotId1].SpeedLineSet(SlowDown);
                }
            }
        }
    }

    bool isObstacle(Point a) {
        //如果不考虑格点
        int J = ceil(a.x * 2), I = 100 - floor(a.y * 2);
        if(Map[I][J] == '#')
            return true;
        return false;
    }

    void ObstacleAvoid() { // 避障
        const double PredictFrameLimit = 5; //为什么要设置那么长
        const double PredictFrameStep = 1;
        const double LineSpeedSlowDown = MaxSpeed * 0.5;
        const double AngleSpeedUp = MaxAngleSpeed;
        for (int i = 0; i <= RobotCount; i++) {
            double RobotRadius = Rob[i].IsCarry() ? 0.53 : 0.45; // 半径
            double SpeedOrientation = atan2(Rob[i].GetSpeedY(), Rob[i].GetSpeedX()); // 实际速度角度
            double EdgeOrienRight = SpeedOrientation > -PI / 2 // 右边界向量，判断越界
                                        ? SpeedOrientation - PI / 2
                                        : SpeedOrientation + 3 * PI / 2;
            Point RobPos = {Rob[i].GetX(), Rob[i].GetY()};
            Point EdgeRight = {
                Rob[i].GetX() + RobotRadius * cos(EdgeOrienRight), // 右边界点
                Rob[i].GetY() + RobotRadius * sin(EdgeOrienRight)};
            Point EdgeLeft = RobPos * 2 - EdgeRight; // 左边界点
            Point IncreaseStep = {Rob[i].GetSpeedX(), Rob[i].GetSpeedY()}; // 向前拓展检查步长
            for (int j = 1; j <= PredictFrameLimit; j += PredictFrameStep) { // 开始检查，从机器人头长度开始
                // do sth?
                Point PreEdgeRight = EdgeRight + IncreaseStep * ((double)j / 50.0);
                Point PreEdgeLeft = EdgeLeft + IncreaseStep * ((double)j / 50.0);
                bool RightObst = isObstacle(PreEdgeRight);
                bool LeftObst = isObstacle(PreEdgeLeft);
                if (!RightObst && !LeftObst) { // 没有碰到障碍
                    continue;
                }
                else if (RightObst && !LeftObst) { // 右边界先碰撞
                    // do sth
                    Rob[i].SpeedLineSet(LineSpeedSlowDown);
                    Rob[i].SpeedAngleSet(AngleSpeedUp);
                    break;
                }
                else if (LeftObst && !RightObst) { // 左边界先碰撞
                    // do sth
                    Rob[i].SpeedLineSet(LineSpeedSlowDown);
                    Rob[i].SpeedAngleSet(AngleSpeedUp * -1);
                    break;
                }
                else { // 左右同时碰撞
                    // do sth
                    Rob[i].SpeedLineSet(LineSpeedSlowDown * -1); //设置为反方向速度
                    // if (Rob[i].AdjustAngle()) {  // 目标点在顺时针方向
                    //   Rob[i].SpeedAngleSet(AngleSpeedUp);
                    // } else {  // 目标点在逆时针方向
                    //   Rob[i].SpeedAngleSet(AngleSpeedUp * -1);
                    // }
                    break;
                }
            }
        }
    }

    void MovePrint() {
        for (int i = 0; i <= RobotCount; i++)
            Rob[i].print(i);
    }
} // namespace Move

namespace PathFind
{
    bool search[55][55];              // 判断这两个工作台之间的路径是否被搜索过
    map<pair<int, int>, int> IsBench; // 是工作台 //先用map来记录一下映射关系  如果觉得这里事件有点耗 再改成数组 想想其他方法来映射
    int EmptyType[105][105];          // 用来判断类型 没有障碍为1， 只有一个障碍为2， 有两个障碍为3，已经不合法了是不会被赋值的
    bool Mode = false;
    struct Point {
        double x, y;
        bool operator<(const Point &p) const {
            if (x != p.x)
                return x < p.x;
            else
                return y < p.y;
        }
    };

    bool vis[105][105];
    const int dx[] = {1, -1, 0, 0}, dy[] = {0, 0, 1, -1};   // 下 上 右 左
    const int dx2[] = {2, -2, 0, 0}, dy2[] = {0, 0, 2, -2}; // 下 上 右 左
    // 根据格子的坐标 其实就是和二维数组存储类似
    //  /*
    //  注意格子的坐标对应关系是如下的
    //  (1,1) (1,2) (1,3) (1,4)
    //  (2,1) (2,2) (2,3) (2,4)
    //  (3,1) (3,2) (3,3) (3,4)
    //  (4,1) (4,2) (4,3) (4,4)
    //  */
    // 故而 dx=1表示向下,dx=-1表示向上
    // 故而 dy=1表示向右,dy=-1表示向左

    // 没有携带物体时
    int EmptyColor[105][105];
    bool EmptyValid[105][105];
    pair<Point, Point> EmptyValidScope[105][105];

    // 携带物体时
    int CarryColor[105][105];
    bool CarryValid[105][105];
    pair<Point, Point> CarryValidScope[105][105];

    bool InMap(int x, int y) {
        if (1 <= x && x <= 100 && 1 <= y && y <= 100)
            return true;
        else
            return false;
    }

    pair<double, double> Reflect(int x, int y) { // 由格子坐标转换为坐标系坐标
        double NowX = 0.25 + 0.5 * (y - 1), NowY = 49.75 - 0.5 * (x - 1);
        return {NowX, NowY};
    }

    void ValidScopeSet(pair<Point, Point> ValidScope[][105], int i, int j, double x1, double y1, double x2, double y2) { // 设置合法范围
        pair<double, double> pr = Reflect(i, j);
        double x = pr.first, y = pr.second;
        Point TopLeft, BottomRight;
        TopLeft.x = x + x1, TopLeft.y = y + y1;
        BottomRight.x = x + x2, BottomRight.y = y + y2;
        ValidScope[i][j] = {TopLeft, BottomRight};
    }

    void EmptyValidScopeInit() {
        for (int i = 1; i <= 100; i++) {
            for (int j = 1; j <= 100; j++) {
                if (EmptyValid[i][j] == false)
                    continue; // 一开始就是'#',直接continue
                bool obstacle[4];
                memset(obstacle, false, sizeof obstacle);
                for (int k = 0; k < 4; k++)
                { // 下 上 右 左
                    int PosX = i + dx[k], PosY = j + dy[k];
                    if (!InMap(PosX, PosY))
                        continue;
                    if (Map[PosX][PosY] == '#')
                        obstacle[k] = true;
                }
                double AcceptDis = 0.25;
                double UnacceptDis = 0.18;
                if ((obstacle[0] && obstacle[1]) || (obstacle[2] && obstacle[3]))
                    EmptyValid[i][j] = false;
                else if (obstacle[0])
                { // 下面有障碍
                    if (obstacle[2])
                    { // 右边有障碍
                        ValidScopeSet(EmptyValidScope, i, j, -0.25, 0.25, -0.22, 0.22);
                        EmptyType[i][j] = 3;
                    }
                    else if (obstacle[3])
                    { // 左边有障碍
                        ValidScopeSet(EmptyValidScope, i, j, 0.22, 0.25, 0.25, 0.22);
                        EmptyType[i][j] = 3;
                    }
                    else
                    {
                        ValidScopeSet(EmptyValidScope, i, j, -0.25, 0.25, 0.25, 0.22);
                        EmptyType[i][j] = 2;
                    }
                }
                else if (obstacle[1])
                { // 上面有障碍
                    if (obstacle[2])
                    { // 右边有障碍
                        ValidScopeSet(EmptyValidScope, i, j, -0.25, -0.22, -0.22, -0.25);
                        EmptyType[i][j] = 3;
                    }
                    else if (obstacle[3])
                    { // 左边有障碍
                        ValidScopeSet(EmptyValidScope, i, j, 0.22, -0.22, 0.25, -0.25);
                        EmptyType[i][j] = 3;
                    }
                    else
                    {
                        ValidScopeSet(EmptyValidScope, i, j, -0.25, -0.22, 0.25, -0.25);
                        EmptyType[i][j] = 2;
                    }
                }
                else if (obstacle[2])
                { // 只有右边有障碍
                    ValidScopeSet(EmptyValidScope, i, j, -0.25, 0.25, -0.22, -0.25);
                    EmptyType[i][j] = 2;
                }
                else if (obstacle[3])
                { // 只有左边有障碍
                    ValidScopeSet(EmptyValidScope, i, j, 0.22, 0.25, 0.25, -0.25);
                    EmptyType[i][j] = 2;
                }
                else
                { // 没有障碍
                    ValidScopeSet(EmptyValidScope, i, j, -0.25, 0.25, 0.25, -0.25);
                    EmptyType[i][j] = 1;
                }
            }
        }
    }

    pair<double, double> ChooseVersion2(int x, int y, pair<Point, Point> ValidScope[][105], int Type[][105])
    {
        // 首先先取合法区域的中心
        double x1 = ValidScope[x][y].first.x, y1 = ValidScope[x][y].first.y;
        double x2 = ValidScope[x][y].second.x, y2 = ValidScope[x][y].second.y;
        double AnsX = (x1 + x2) / 2, AnsY = (y1 + y2) / 2;
        // 随后进行调整
        if (Type[x][y] == 1)
        { // 全是合法区域 直接选中心点返回把
            return {AnsX, AnsY};
        }
        else if (Type[x][y] == 2)
        { // 只有一个位置是'#'
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy))
                    continue;
                if (Type[xx][yy] == 1)
                { // 表示这个是合法区域
                    if (i == 0)
                    { // 下
                        AnsY = ValidScope[x][y].first.y;
                    }
                    else if (i == 1)
                    { // 上
                        AnsY = ValidScope[x][y].second.y;
                    }
                    else if (i == 2)
                    { // 左
                        AnsX = ValidScope[x][y].second.x;
                    }
                    else if (i == 3)
                    { // 右
                        AnsY = ValidScope[x][y].first.x;
                    }
                    break;
                }
            }
            return {AnsX, AnsY};
        }
        else if (Type[x][y] == 3)
        { // 有两处是'#'
          // 先不管
        }
        return {AnsX, AnsY};
    }

    void PrintValidScope(pair<Point, Point> ValidScope[][105], bool Valid[][105])
    {
        freopen("C:\\Users\\lin Guangming\\Desktop\\ValidScope.txt", "w", stdout);

        for (int i = 1; i <= 100; i++)
        {
            for (int j = 1; j <= 100; j++)
            {
                pair<Point, Point> pr = ValidScope[i][j];
                cout << "(" << i << "," << j << ")"
                     << " ";
                if (Valid[i][j] == false)
                    cout << "不合法"
                         << " ";
                else
                {
                    cout << "(" << pr.first.x << "," << pr.first.y << ")"
                         << " ";
                    cout << "(" << pr.second.x << "," << pr.second.y << ")"
                         << " ";
                }
                cout << endl
                     << endl
                     << endl;
            }
        }
        fclose(stdout);
    }

    void CarryValidScopeInit()
    {
        for (int i = 1; i <= 100; i++)
        {
            for (int j = 1; j <= 100; j++)
            {
                if (CarryValid[i][j] == false)
                    continue;

                bool obstacle[4];
                memset(obstacle, false, sizeof obstacle);
                for (int k = 0; k < 4; k++)
                {
                    int PosX = i + dx[k], PosY = j + dy[k];
                    if (!InMap(PosX, PosY))
                        continue;
                    if (Map[PosX][PosY] == '#')
                    {
                        obstacle[k] = true;
                        break; // 直接结束
                    }
                }
                if (obstacle[0] || obstacle[1] || obstacle[2] || obstacle[3])
                {
                    CarryValid[i][j] = false;
                    continue; // 注意这里要continue
                }

                int obstacleBit = 0;
                for (int k = 0; k < 4; k++)
                { // 下 上 右 左
                    int PosX = i + dx2[k], PosY = j + dy2[k];
                    if (!InMap(PosX, PosY))
                        continue;
                    if (Map[PosX][PosY] == '#')
                    {
                        obstacleBit |= (1 << k);
                    }
                }
                switch (obstacleBit)
                {
                case 0: // 没有障碍
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.25, 0.25, -0.25);
                    break;
                case 1: // 下
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.25, 0.25, -0.22);
                    break;
                case 2: // 上
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.22, 0.25, -0.25);
                    break;
                case 3: // 下 + 上
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.22, 0.25, -0.22);
                    break;
                case 4: // 右
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.25, 0.22, -0.25);
                    break;
                case 5: // 右 + 下
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.25, 0.22, -0.22);
                    break;
                case 6: // 右 + 上
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.22, 0.22, -0.25);
                    break;
                case 7: // 右 + 上 + 下
                    ValidScopeSet(CarryValidScope, i, j, -0.25, 0.22, 0.22, -0.22);
                    break;
                case 8: // 左
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.25, 0.25, -0.25);
                    break;
                case 9: // 左 + 下
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.25, 0.25, -0.22);
                    break;
                case 10: // 左 + 上
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.22, 0.25, -0.25);
                    break;
                case 11: // 左 + 上 + 下
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.22, 0.25, -0.22);
                    break;
                case 12: // 左 + 右
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.25, 0.22, -0.25);
                    break;
                case 13: // 左 + 右 + 下
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.25, 0.22, -0.22);
                    break;
                case 14: // 左 + 右 + 上
                    ValidScopeSet(CarryValidScope, i, j, -0.22, 0.22, 0.22, -0.25);
                    break;
                case 15: // 左 + 右 + 上 + 下
                    CarryValid[i][j] = false;
                    break;
                }
            }
        }
    }

    bool Check(bool Valid[][105], pair<Point, Point> ValidScope[][105], int x1,
               int y1, int x2, int y2)
    {
        if (Valid[x1][y1] == false || Valid[x2][y2] == false)
        {
            if (x1 == 47 && x2 == 48 && y1 == 62 && y2 == 62)
                cout << "here1" << endl;
            return false;
        }
        if (x1 == x2)
        { // 同一行 左右结构 (相当注意这个坐标的问题)
            if (y1 > y2)
                swap(y1, y2); // 让(x1,y1)在左边
            double LeftX = ValidScope[x1][y1].second.x,
                   LeftY1 = ValidScope[x1][y1].first.y,
                   LeftY2 = ValidScope[x1][y1].second.y;
            double RightX = ValidScope[x2][y2].first.x,
                   RigthY1 = ValidScope[x2][y2].first.y,
                   RigthY2 = ValidScope[x2][y2].second.y;
            if (LeftX != RightX)
                return false;
            // Y1会比Y2大
            if (LeftY2 >= RigthY1 || RigthY2 >= LeftY1)
                return false;
            // if(x1 == 47 && x2 == 48 && y1 == 62 && y2 == 62)
            //     cout << "here2 " << endl;
            return true;
        }
        else
        { // 同一列 上下结构
            if (x1 > x2)
                swap(x1, x2); // 让(x1,y1)在上面 注意
            double AboveY = ValidScope[x1][y1].second.y,
                   AboveX1 = ValidScope[x1][y1].first.x,
                   AboveX2 = ValidScope[x1][y1].second.x;
            double BelowY = ValidScope[x2][y2].first.y,
                   BelowX1 = ValidScope[x2][y2].first.x,
                   BelowX2 = ValidScope[x2][y2].second.x;
            // X1会比X2小
            if (AboveY != BelowY)
                return false;
            if (AboveX2 <= BelowX1 || BelowX2 <= AboveX1)
                return false;
            if (x1 == 47 && x2 == 48 && y1 == 62 && y2 == 62)
                cout << " here3" << endl;
            return true;
        }
    }

    void bfsForColor(int X, int Y, int ColorNum, int ColorArray[][105],
                     bool Valid[][105], pair<Point, Point> ValidScope[][105])
    {
        queue<pair<int, int>> q;
        q.push({X, Y});
        vis[X][Y] = true; // 表示已经染色
        while (q.size())
        {
            int x = q.front().first, y = q.front().second;
            q.pop();
            ColorArray[x][y] = ColorNum;
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy) == false)
                    continue;
                if (vis[xx][yy] == true)
                    continue;
                if (Check(Valid, ValidScope, x, y, xx, yy) == false)
                {
                    continue;
                }
                // if(x == 47 && y == 62 && xx == 47 && yy = 62) {
                //   cout << "no problem" << endl;
                // }
                if (x == 48 && y == 62 && xx == 47 && yy == 62)
                {
                    cout << "no problem" << endl;
                }
                if (x == 47 && y == 62 && xx == 48 && yy == 62)
                {
                    cout << "no problem" << endl;
                }
                vis[xx][yy] = true;
                q.push({xx, yy});
            }
        }
    }

    void ColorInit(int ColorArray[][105], bool Valid[][105],
                   pair<Point, Point> ValidScope[][105])
    {
        int ColorCount = 0;
        memset(vis, false, sizeof vis); // 初始化为false
        for (int i = 1; i <= 100; i++)
        {
            for (int j = 1; j <= 100; j++)
            {
                if (Map[i][j] == '#')
                    continue; // 这个不需要染色
                if (!vis[i][j])
                {
                    ++ColorCount;
                    bfsForColor(i, j, ColorCount, ColorArray, Valid, ValidScope);
                }
            }
        }
    }

    pair<double, double> Choose(
        int x, int y,
        pair<Point, Point> ValidScope[][105])
    { // 选择这个格子的合法区域的一个点
        // 返回
        // 这里取合法区域的中心返回
        double x1 = ValidScope[x][y].first.x, y1 = ValidScope[x][y].first.y;
        double x2 = ValidScope[x][y].second.x, y2 = ValidScope[x][y].second.y;
        double AnsX = (x1 + x2) / 2, AnsY = (y1 + y2) / 2;
        return {AnsX, AnsY};
    }

    vector<pair<double, double>> FindSinglePath(
        int IX, int IY, int JX, int JY, int ColorArray[][105],
        bool Valid[][105],
        pair<Point, Point> ValidScope[][105])
    {
        vector<pair<double, double>> path;
        map<pair<int, int>, pair<int, int>> fa; // 记录这个节点是从哪个节点过来的
        memset(vis, false, sizeof vis);
        queue<pair<int, int>> q;
        q.push({IX, IY});
        vis[IX][IY] = true;
        while (true)
        {
            int x = q.front().first, y = q.front().second;
            q.pop();
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy) == false)
                    continue;
                if (vis[xx][yy])
                    continue;
                if (ColorArray[xx][yy] != ColorArray[x][y])
                    continue;
                if (Check(Valid, ValidScope, x, y, xx, yy) == false) // 相当注意这个。。。。。。。。。。。。。。。。。。。。。。。。
                    continue;
                // 否则 push
                q.push({xx, yy});
                vis[xx][yy] = true;
                fa[{xx, yy}] = {x, y};
                // 如果找到了 记录路径 返回
                if (xx == JX && yy == JY)
                {
                    path.push_back(Reflect(xx, yy)); // 将工作台的坐标输入
                    while (true)
                    {
                        if (xx == IX && yy == IY)
                            break; // 结束条件
                        path.push_back(Choose(xx, yy, ValidScope));
                        pair<int, int> pr = fa[{xx, yy}];
                        xx = pr.first, yy = pr.second;
                    }
                    reverse(path.begin(), path.end());
                    return path;
                }
            }
        }
    }

    void FindAllPath(vector<pair<double, double>> Path[][55], int ColorArray[][105], bool Valid[][105],
                     pair<Point, Point> ValidScope[][105])
    {
        for (int i = 0; i <= WorkBenchCount; i++)
        {
            for (int j = 0; j <= WorkBenchCount; j++)
            {
                if (i == j)
                    continue;
                int IX = Bench[i].GetPosIX(), IY = Bench[i].GetPosIY();
                int JX = Bench[j].GetPosIX(), JY = Bench[j].GetPosIY();
                if (ColorArray[IX][IY] != ColorArray[JX][JY])
                    continue;
                Path[i][j] = FindSinglePath(IX, IY, JX, JY, ColorArray, Valid, ValidScope);
            }
        }
    }

    void FindRobotPath(vector<pair<double, double>> RobotPath[][55],
                       int ColorArray[][105],
                       bool Valid[][105],
                       pair<Point, Point> ValidScope[][105])
    {
        for (int i = 0; i <= WorkBenchCount; i++)
        {
            for (int j = 0; j <= RobotCount; j++)
            {
                int IX = Bench[i].GetPosIX(), IY = Bench[i].GetPosIY();
                int JX = (49.75 - Rob[j].GetY()) * 2 + 1,
                    JY = (Rob[j].GetX() - 0.25) * 2 + 1;
                if (ColorArray[IX][IY] != ColorArray[JX][JY])
                    continue;
                RobotPath[j][i] = FindSinglePath(JX, JY, IX, IY, ColorArray, Valid, ValidScope);
            }
        }
    }

    void PrintPath(int x, int y, vector<pair<double, double>> Path[][55])
    {
        vector<pair<double, double>> vec = Path[x][y];
        for (int i = 0; i < (int)vec.size(); i++)
        {
            cout << "" << vec[i].first << " " << vec[i].second << "" << endl;
        }
    }

    void RobotColorInit()
    {
        for (int i = 0; i < RobotCount; i++)
        {
            int IX = (49.75 - Rob[i].GetY()) * 2 + 1, IY = (Rob[i].GetX() - 0.25) * 2 + 1;
            RobotColor[i] = CarryColor[IX][IY];
        }
    }
    void ModifyFindPathVersion2(int IX, int IY, int ColorArray[][105], bool Valid[][105], pair<Point, Point> ValidScope[][105], vector<pair<double, double>> Path[][55])
    {
        map<pair<int, int>, pair<int, int>> fa;
        memset(vis, false, sizeof vis);
        queue<pair<int, int>> q;
        q.push({IX, IY});
        vis[IX][IY] = true;
        int NowId = IsBench[{IX, IY}]; // Id号
        while (q.size())
        {
            int x = q.front().first, y = q.front().second;
            q.pop();
            // 首先判定这四个方向
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy) == false) // 表示不在地图中
                    continue;
                if (vis[xx][yy]) // 之前搜过这个点
                    continue;
                if (ColorArray[xx][yy] != ColorArray[x][y]) // 不是一个颜色
                    continue;
                if (Check(Valid, ValidScope, x, y, xx, yy) == false) // 相当注意这个。。。。。。。。。。。。。。。。。。。。。。。。
                    continue;
                // 否则，push
                q.push({xx, yy});
                vis[xx][yy] = true;
                fa[{xx, yy}] = {x, y};
                // 判断(xx,yy)是不是一个工作台
                if (IsBench.count({xx, yy}))
                {
                    int JX = xx, JY = yy;
                    int NextId = IsBench[{xx, yy}];    // Id号
                    if (search[NowId][NextId] == true) // 如果之前搜过这样的路径 就不搜了
                        continue;
                    // 生成(IX, IY)到(JX, JY)的路径 和(JX,JY)到(IX, IY)的路径
                    vector<pair<double, double>> path;
                    path.push_back(Reflect(xx, yy)); // 将这个工作台的坐标存进去
                    while (true)
                    {
                        if (xx == IX && yy == IY)
                            break; // 结束
                        if (Mode == false)
                        {
                            path.push_back(ChooseVersion2(xx, yy, ValidScope, EmptyType));
                        }
                        else
                        {
                            path.push_back(Choose(xx, yy, ValidScope));
                        }
                        pair<int, int> pr = fa[{xx, yy}];
                        xx = pr.first;
                        yy = pr.second;
                    }
                    path.push_back(Reflect(IX, IY)); // 将初始工作台的坐标存进去
                    Path[NextId][NowId] = path;      // 第一条路径
                    reverse(path.begin(), path.end());
                    Path[NowId][NextId] = path; // 第二条路径
                    // 标记这两条路径已经搜过了
                    search[NowId][NextId] = true;
                    search[NextId][NowId] = true;
                }
            }
            // 其次再判定左上 右上 左下 右下
            if (Mode == false)
            { // 没啥意思 就是把下面的东西封起来 这里先测试empty的
                int testx[] = {123456, -1, 1, -1, 1};
                int testy[] = {123456, 1, 1, -1, -1};
                for (int i = 1; i <= 4; i++)
                {
                    int xx = x + testx[i], yy = y + testy[i];
                    if (InMap(xx, y) && InMap(x, yy) && EmptyType[xx][y] == 1 && EmptyType[x][yy] == 1 && EmptyType[xx][yy] == 1 && vis[xx][yy] == false)
                    {
                        vis[xx][yy] = true;
                        q.push({xx, yy});
                        fa[{xx, yy}] = {x, y};
                        if (IsBench.count({xx, yy}))
                        {
                            int JX = xx, JY = yy;
                            int NextId = IsBench[{xx, yy}];    // Id号
                            if (search[NowId][NextId] == true) // 如果之前搜过这样的路径 就不搜了
                                continue;
                            // 生成(IX, IY)到(JX, JY)的路径 和(JX,JY)到(IX, IY)的路径
                            vector<pair<double, double>> path;
                            path.push_back(Reflect(xx, yy)); // 将这个工作台的坐标存进去
                            while (true)
                            {
                                if (xx == IX && yy == IY)
                                    break; // 结束
                                if (Mode == false)
                                {
                                    path.push_back(ChooseVersion2(xx, yy, ValidScope, EmptyType));
                                }
                                else
                                {
                                    path.push_back(Choose(xx, yy, ValidScope));
                                }
                                pair<int, int> pr = fa[{xx, yy}];
                                xx = pr.first;
                                yy = pr.second;
                            }
                            path.push_back(Reflect(IX, IY)); // 将初始工作台的坐标存进去
                            Path[NextId][NowId] = path;      // 第一条路径
                            reverse(path.begin(), path.end());
                            Path[NowId][NextId] = path; // 第二条路径
                            // 标记这两条路径已经搜过了
                            search[NowId][NextId] = true;
                            search[NextId][NowId] = true;
                        }
                    }
                }
            }
        }
    }

    void FindPath()
    {
        // 处理不带物品
        EmptyValidScopeInit();
        ColorInit(EmptyColor, EmptyValid, EmptyValidScope);
        FindAllPath(EmptyPath, EmptyColor, EmptyValid, EmptyValidScope);

        // 处理带物品
        CarryValidScopeInit();
        ColorInit(CarryColor, CarryValid, CarryValidScope);
        FindAllPath(CarryPath, CarryColor, CarryValid, CarryValidScope);

        // 处理机器人
        FindRobotPath(RobPath, EmptyColor, EmptyValid, EmptyValidScope);
        RobotColorInit();
    }
    //************************************************************************************************************************************************************************************************************************************************

    void IsBenchInit()
    {
        for (int i = 0; i <= WorkBenchCount; i++)
        {
            pair<int, int> pr = {Bench[i].GetPosIX(), Bench[i].GetPosIY()};
            IsBench.insert({pr, i});
        }
    }

    // 相当注意找这个路径的时候 search需要进行初始化 为false
    void ModifyFindPath(int IX, int IY, int ColorArray[][105], bool Valid[][105], pair<Point, Point> ValidScope[][105], vector<pair<double, double>> Path[][55])
    {
        map<pair<int, int>, pair<int, int>> fa;
        memset(vis, false, sizeof vis);
        queue<pair<int, int>> q;
        q.push({IX, IY});
        vis[IX][IY] = true;
        int NowId = IsBench[{IX, IY}]; // Id号
        while (q.size())
        {
            int x = q.front().first, y = q.front().second;
            q.pop();
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy) == false) // 表示不在地图中
                    continue;
                if (vis[xx][yy]) // 之前搜过这个点
                    continue;
                if (ColorArray[xx][yy] != ColorArray[x][y]) // 不是一个颜色
                    continue;
                if (Check(Valid, ValidScope, x, y, xx, yy) == false) // 相当注意这个。。。。。。。。。。。。。。。。。。。。。。。。
                    continue;
                // 否则，push
                q.push({xx, yy});
                vis[xx][yy] = true;
                fa[{xx, yy}] = {x, y};
                // 判断(xx,yy)是不是一个工作台
                if (IsBench.count({xx, yy}))
                {
                    int JX = xx, JY = yy;
                    int NextId = IsBench[{xx, yy}];    // Id号
                    if (search[NowId][NextId] == true) // 如果之前搜过这样的路径 就不搜了
                        continue;
                    // 生成(IX, IY)到(JX, JY)的路径 和(JX,JY)到(IX, IY)的路径
                    vector<pair<double, double>> path;
                    path.push_back(Reflect(xx, yy)); // 将这个工作台的坐标存进去
                    while (true)
                    {
                        if (xx == IX && yy == IY)
                            break; // 结束
                        path.push_back(Choose(xx, yy, ValidScope));
                        pair<int, int> pr = fa[{xx, yy}];
                        xx = pr.first;
                        yy = pr.second;
                    }
                    path.push_back(Reflect(IX, IY)); // 将初始工作台的坐标存进去
                    Path[NextId][NowId] = path;      // 第一条路径
                    reverse(path.begin(), path.end());
                    Path[NowId][NextId] = path; // 第二条路径
                    // 标记这两条路径已经搜过了
                    search[NowId][NextId] = true;
                    search[NextId][NowId] = true;
                }
            }
        }
    }

    struct Grid
    {
        int x, y;         // Map[x][y]
        double Dis;       // 抵达距离 越小越好
        double TurnCount; // 转角次数 越小越好
        int Priority;     // 根据合法区域的大小进行优先级排序 分为4个等级 越小越好
        bool operator<(const Grid &grid) const
        { // 注意优先队列是大根堆
            if (Dis != grid.Dis)
                return Dis > grid.Dis;
            else if (Priority != grid.Priority)
                return Priority > grid.Priority;
            else
                return TurnCount > grid.TurnCount;
        }
    };

    void ModifyFindPathVersion3(int IX, int IY, int ColorArray[][105], bool Valid[][105], pair<Point, Point> ValidScope[][105], vector<pair<double, double>> Path[][55])
    {
        map<pair<int, int>, pair<int, int>> fa;
        memset(vis, false, sizeof vis);
        priority_queue<Grid> heap;
        Grid StartGrid;
        StartGrid.x = IX, StartGrid.y = IY;
        StartGrid.Dis = 0;
        StartGrid.TurnCount = 0;
        StartGrid.Priority = 0;
        heap.push(StartGrid);
        vis[IX][IY] = true;
        int NowId = IsBench[{IX, IY}]; // Id号
        while (heap.size())
        {
            Grid NowGrid = heap.top();
            heap.pop();
            int x = NowGrid.x, y = NowGrid.y;
            // 首先判定这四个方向
            for (int i = 0; i < 4; i++)
            {
                int xx = x + dx[i], yy = y + dy[i];
                if (InMap(xx, yy) == false) // 表示不在地图中
                    continue;
                if (vis[xx][yy]) // 之前搜过这个点
                    continue;
                if (ColorArray[xx][yy] != ColorArray[x][y]) // 不是一个颜色
                    continue;
                if (Check(Valid, ValidScope, x, y, xx, yy) == false) // 相当注意这个。。。。。。。。。。。。。。。。。。。。。。。。
                    continue;
                // 否则，push
                Grid NextGrid;
                NextGrid.x = xx, NextGrid.y = yy;
                NextGrid.Dis = NowGrid.Dis + 1;
                // 根据emptytype来写NextGrid的prior
                NextGrid.Priority = EmptyType[xx][yy];
                if ((xx - x) == (x - fa[{x, y}].first) && (yy - y) == (y - fa[{x, y}].second))
                    NextGrid.TurnCount = NowGrid.TurnCount;
                else
                    NextGrid.TurnCount = NowGrid.TurnCount + 1;

                heap.push(NextGrid);
                vis[xx][yy] = true;
                fa[{xx, yy}] = {x, y};
                // 判断(xx,yy)是不是一个工作台
                if (IsBench.count({xx, yy}))
                {
                    int JX = xx, JY = yy;
                    int NextId = IsBench[{xx, yy}];    // Id号
                    if (search[NowId][NextId] == true) // 如果之前搜过这样的路径 就不搜了
                        continue;
                    // 生成(IX, IY)到(JX, JY)的路径 和(JX,JY)到(IX, IY)的路径
                    vector<pair<double, double>> path;
                    path.push_back(Reflect(xx, yy)); // 将这个工作台的坐标存进去
                    while (true)
                    {
                        if (xx == IX && yy == IY)
                            break; // 结束
                        if (Mode == false)
                        {
                            path.push_back(ChooseVersion2(xx, yy, ValidScope, EmptyType));
                        }
                        else
                        {
                            path.push_back(Choose(xx, yy, ValidScope));
                        }
                        pair<int, int> pr = fa[{xx, yy}];
                        xx = pr.first;
                        yy = pr.second;
                    }
                    path.push_back(Reflect(IX, IY)); // 将初始工作台的坐标存进去
                    Path[NextId][NowId] = path;      // 第一条路径
                    reverse(path.begin(), path.end());
                    Path[NowId][NextId] = path; // 第二条路径
                    // 标记这两条路径已经搜过了
                    search[NowId][NextId] = true;
                    search[NextId][NowId] = true;
                }
            }
            // continue; //不跑下面的 省的出错
            //  其次再判定左上 右上 左下 右下
            if (Mode == false || Mode == true)
            { // 没啥意思 就是把下面的东西封起来 
                int testx[] = {123456, -1, 1, -1, 1};
                int testy[] = {123456, 1, 1, -1, -1};
                for (int i = 1; i <= 4; i++)
                {
                    int xx = x + testx[i], yy = y + testy[i];
                    if (InMap(xx, y) && InMap(x, yy) && EmptyType[xx][y] == 1 && EmptyType[x][yy] == 1 && EmptyType[xx][yy] == 1 && vis[xx][yy] == false)
                    {
                        vis[xx][yy] = true;
                        fa[{xx, yy}] = {x, y};
                        Grid NextGrid;
                        NextGrid.x = xx, NextGrid.y = yy;
                        NextGrid.Dis = NowGrid.Dis + sqrt(2); // 算成根号2
                        // 根据emptytype来写NextGrid的prior
                        NextGrid.Priority = EmptyType[xx][yy];
                        if ((xx - x) == (x - fa[{x, y}].first) && (yy - y) == (y - fa[{x, y}].second))
                            NextGrid.TurnCount = NowGrid.TurnCount;
                        else
                            NextGrid.TurnCount = NowGrid.TurnCount + 1.5;
                        heap.push(NextGrid);

                        if (IsBench.count({xx, yy}))
                        {
                            int JX = xx, JY = yy;
                            int NextId = IsBench[{xx, yy}];    // Id号
                            if (search[NowId][NextId] == true) // 如果之前搜过这样的路径 就不搜了
                                continue;
                            // 生成(IX, IY)到(JX, JY)的路径 和(JX,JY)到(IX, IY)的路径
                            vector<pair<double, double>> path;
                            path.push_back(Reflect(xx, yy)); // 将这个工作台的坐标存进去
                            while (true)
                            {
                                if (xx == IX && yy == IY)
                                    break; // 结束
                                if (Mode == false)
                                {
                                    path.push_back(ChooseVersion2(xx, yy, ValidScope, EmptyType));
                                }
                                else
                                {
                                    path.push_back(Choose(xx, yy, ValidScope));
                                }
                                pair<int, int> pr = fa[{xx, yy}];
                                xx = pr.first;
                                yy = pr.second;
                            }
                            path.push_back(Reflect(IX, IY)); // 将初始工作台的坐标存进去
                            Path[NextId][NowId] = path;      // 第一条路径
                            reverse(path.begin(), path.end());
                            Path[NowId][NextId] = path; // 第二条路径
                            // 标记这两条路径已经搜过了
                            search[NowId][NextId] = true;
                            search[NextId][NowId] = true;
                        }
                    }
                }
            }
        }
    }

    void ModifyFindAllPath(int ColorArray[][105], bool Valid[][105], pair<Point, Point> ValidScope[][105], vector<pair<double, double>> Path[][55]) {
        memset(search, false, sizeof search); // 初始化为false
        for (int i = 0; i <= WorkBenchCount; i++) {
            if (Mode == false)
                ModifyFindPathVersion3(Bench[i].GetPosIX(), Bench[i].GetPosIY(), ColorArray, Valid, ValidScope, Path);
            else
                ModifyFindPathVersion3(Bench[i].GetPosIX(), Bench[i].GetPosIY(), ColorArray, Valid, ValidScope, Path);
        }
    }

    // BRflag 用于判断 Empty(0) or Carry(1) or Robot(2)
    void CalPathDis(vector<pair<double, double>> Path[][55], int BRflag) {
        int i, j, k;
        if (BRflag == 2) {
            for (i = 0; i <= RobotCount; i++) {
                for (j = 0; j <= WorkBenchCount; j++) {
                    vector<pair<double, double>> pat = Path[i][j];
                    for (k = 1; k < pat.size(); k++) {
                        RobotBenchDistance[i][j] += Vector::GetDistance(
                            pat[k - 1].first, pat[k - 1].second, pat[k].first, pat[k].second);
                    }
                }
            }
        }
        else if (BRflag == 1) {
            for (i = 0; i < WorkBenchCount; i++) {
                for (j = i + 1; j <= WorkBenchCount; j++) {
                    vector<pair<double, double>> pat = Path[i][j];
                    for (k = 1; k < pat.size(); k++) {
                        CarryBenchDistance[i][j] += Vector::GetDistance(
                            pat[k - 1].first, pat[k - 1].second, pat[k].first, pat[k].second);
                    }
                }
                CarryBenchDistance[j][i] = CarryBenchDistance[i][j];
            }
        }
        else {
            for (i = 0; i < WorkBenchCount; i++) {
                for (j = i + 1; j <= WorkBenchCount; j++) {
                    vector<pair<double, double>> pat = Path[i][j];
                    for (k = 1; k < pat.size(); k++) {
                        EmptyBenchDistance[i][j] += Vector::GetDistance(
                            pat[k - 1].first, pat[k - 1].second, pat[k].first, pat[k].second);
                    }
                }
                EmptyBenchDistance[j][i] = EmptyBenchDistance[i][j];
            }
        }
    }

    void Modify() {
        // 初始化(x,y)到工作台的映射
        IsBenchInit();

        // 处理不带物品
        Mode = false;
        EmptyValidScopeInit();
        ColorInit(EmptyColor, EmptyValid, EmptyValidScope);
        ModifyFindAllPath(EmptyColor, EmptyValid, EmptyValidScope, EmptyPath);
        CalPathDis(EmptyPath, 0);

        // 处理带物品
        Mode = true;
        CarryValidScopeInit();
        ColorInit(CarryColor, CarryValid, CarryValidScope);
        ModifyFindAllPath(CarryColor, CarryValid, CarryValidScope, CarryPath);
        CalPathDis(CarryPath, 1);

        // 处理机器人
        FindRobotPath(RobPath, EmptyColor, EmptyValid, EmptyValidScope);
        RobotColorInit();
        CalPathDis(RobPath, 2);

        // 打印信息
        // PrintAllPathFinal();
    }

    //*******************************************************************************************************************************************************************************************************************************8
} // namespace PathFind

namespace Value {
    // 参数&常量区定义
    const double Demand456_1 = 0.5, Demand456_2 = 0.3;
    const double Demand7_1 = 0.5, Demand7_2 = 0.3, Demand7_3 = 0.1;
    const double LastLossFrame = 15000;

    void AdjustBalance456() { // 4，5，6的权重调整函数
        if (!flag7) return; // 没有7，不需要均衡4，5，6生产
        int tot = AllProduceCount[4] + AllProduceCount[5] + AllProduceCount[6];
        if (tot == 0) return; // 防止开始时程序崩溃
        Balance[4] = 1.5 * (AllProduceCount[5] + AllProduceCount[6]) / tot;
        Balance[5] = 1.5 * (AllProduceCount[4] + AllProduceCount[6]) / tot;
        Balance[6] = 1.5 * (AllProduceCount[4] + AllProduceCount[5]) / tot;
    }

    double F(double x) { // F函数 x表示时间
        if(x > 180.0) return 0.8;
        double tmp = pow(1 - x / 180, 2);
        return (1 - sqrt(1 - tmp)) * 0.2 + 0.8;
    }

    double CalTime(int BuyId, int SellId, int BotId) { // 计算buy -> sell的时间
        double distance = CarryBenchDistance[BuyId][SellId];
        double time = distance / DivSpeed + TimeFunc::TurnCostBuytoSell(BuyId, SellId, BotId);
        return time;
    }

    double CalCurrentValue(int BuyId, int SellId, int BotId) { // 计算固有价值
        int MaterialType = Bench[BuyId].GetType();
        double val = Price[MaterialType];
        double distance = CarryBenchDistance[BuyId][SellId];
        double time = distance / DivSpeed + TimeFunc::TurnCostBuytoSell(BuyId, SellId, BotId);
        val *= F(time);
        return val;
    }

    double CalPotentialPrice(int SellId, int BuyId, int BotId) { // 计算潜在价值
        double PotentialValue = 0.0;
        int SellType = Bench[SellId].GetType();
        int DemandCount = Bench[SellId].DemandCount();
        switch (SellType) {
        case 9:
        case 8:
            PotentialValue = 0;
            break;
        case 7:
            if (DemandCount == 3)
                PotentialValue = Price[SellType] * Demand7_3;
            else if (DemandCount == 2)
                PotentialValue = Price[SellType] * Demand7_2;
            else
                PotentialValue = Price[SellType] * Demand7_1;
            break;
        case 6:
        case 5:
        case 4:
            if (DemandCount == 2)
                PotentialValue = Price[SellType] * Demand456_2;
            else
                PotentialValue = Price[SellType] * Demand456_1;
            break;
        }
        if (SellType == 4 || SellType == 5 || SellType == 6) { // 在这里对4，5，6的权重进行调整
            PotentialValue *= Balance[SellType];
        }
        return PotentialValue;
    }

    double ValueCalculation(int BuyId, int SellId, int BotId) {
        double Value = 0;
        if (frameID > LastLossFrame)
            Value = CalCurrentValue(BuyId, SellId, BotId);
        else
            Value = CalCurrentValue(BuyId, SellId, BotId) + CalPotentialPrice(SellId, BuyId, BotId);
        double Time = TimeFunc::OrderTime(BuyId, SellId, BotId);
        double AveValue = Value / Time;
        // 判断最后帧数 最后一段时间不进行购买了
        if (frameID * 0.02 + Time >= 300)
            AveValue = 0.0;
        return AveValue;
    }
} // namespace Value

namespace NoDoubleRobotInOneWay { // 避免两个机器人怼在一起
    pair<double, double> HistroyPosition[4][CheckFrames], HistroyV[4][CheckFrames];
    int HistroyPos;
    vector<int> RobotTurn;
    vector<pair<double, double>> RobNextPoints[4];
    double KeepDistance[4];
    map<pair<double, double>, pair<int, double>> ColorizeMap;

    bool SortRobotByVal(int a, int b) {
        return Rob[a].GetVal() > Rob[b].GetVal();
    }

    bool CheckCoin(int r1, int r2) {
        int len1 = RobNextPoints[r1].size(), len2 = RobNextPoints[r2].size();
        for (int i = 0; i < len1; i++) {
            for (int j = 0; j < len2; j++) {
                Vector::Vec qwq(RobNextPoints[r1][i].first - RobNextPoints[r2][j].first, RobNextPoints[r1][i].second - RobNextPoints[r2][j].second);
                if (Vector::size(qwq) <= KeepDistance[r1] + KeepDistance[r2]) {
                    return true;
                }
                // 希望这里不要有霸哥 [精神小泥.jpg]
            }
        }
        return false;
    }

    void FindANewRoute(int id) {
    }

    void RobotRouteDoNotCrash() {
        for (int i = 0; i <= RobotCount; i++) {
            RobNextPoints[i] = Rob[i].GetNextPoints();
            if (Rob[i].IsCarry())
                KeepDistance[i] = CarryRadius;
            else
                KeepDistance[i] = EmptyRadius;
        }
        for (int i = 0; i <= RobotCount; i++) {
            bool NeedToAct = false;
            for (auto it : ColorizeMap) {
                if (it.second.first == i)
                    continue;

                for (int j = 0; j < (int)RobNextPoints[i].size(); j++) {
                    Vector::Vec qwq(it.first.first - RobNextPoints[i][j].first, it.first.second - RobNextPoints[i][j].second);
                    if (Vector::size(qwq) <= it.second.second) {
                        NeedToAct = true;
                        break;
                    }
                }
            }
            if (NeedToAct == true) {
                FindANewRoute(i);
            }
            for (int j = 0; j < (int)RobNextPoints[i].size(); j++) {
                ColorizeMap[RobNextPoints[i][j]] = make_pair(i, KeepDistance[i]);
            }
        }
    }

    void RobotSort() {
        RobotTurn.clear();
        for (int i = 0; i <= RobotCount; i++) {
            RobotTurn.push_back(i);
        }
        sort(RobotTurn.begin(), RobotTurn.end(), SortRobotByVal);
    }

    void GetRadius() {
        for (int i = 0; i <= RobotCount; i++) {
            if (Rob[i].IsCarry())
                KeepDistance[i] = CarryRadius;
            else
                KeepDistance[i] = EmptyRadius;
        }
    }

    int lastfollow[5];

    void FollowPath(int follower, int followee) { // 跟随者，被跟随者
        if (frameID - lastfollow[follower] < 1000)
            return;
        lastfollow[follower] = frameID;
        vector<pair<double, double>> toadd = Rob[followee].GetTask().GetPath(),
                                     nowa = Rob[follower].GetTask().GetPath();
        vector<pair<double, double>> NewRoute;
        for (auto item : toadd) {
            NewRoute.push_back(item);
        }
        reverse(toadd.begin(), toadd.end());
        for (auto item : toadd) {
            NewRoute.push_back(item);
        }
        for (auto item : nowa) {
            NewRoute.push_back(item);
        }
        Rob[follower].GetTask(follower).PathSet(NewRoute);
    }

    void CheckisCrash() {
        for (int i = 0; i <= RobotCount; i++) {
            HistroyPosition[i][HistroyPos] = {Rob[i].GetX(), Rob[i].GetY()};
            HistroyV[i][HistroyPos] = {Rob[i].GetSpeedX(), Rob[i].GetSpeedY()};
        }

        bool Moving[4];
        double DistanceToMove = 1.0 * CheckFrames * 2.0 / 50.0;
        for (int i = 0; i <= RobotCount; i++) {
            Moving[i] = false;
            int fir = (HistroyPos + 1) % CheckFrames;
            Vector::Vec weiyi(HistroyPosition[i][HistroyPos].first - HistroyPosition[i][fir].first,
                              HistroyPosition[i][HistroyPos].second - HistroyPosition[i][fir].second);
            if (Vector::size(weiyi) >= DistanceToMove) {
                Moving[i] = true;
            }
        }
        for (int idi = 0; idi <= RobotCount; idi++) {
            int i = RobotTurn[idi];
            if (Moving[i] == true)
                continue;
            for (int idj = 0; idj < idi; idj++) {
                int j = RobotTurn[idj];
                if (Moving[j])
                    continue;
                Vector::Vec juli(HistroyPosition[i][HistroyPos].first - HistroyPosition[j][HistroyPos].first,
                                 HistroyPosition[i][HistroyPos].second - HistroyPosition[j][HistroyPos].second);
                if (Vector::size(juli) <= KeepDistance[i] + KeepDistance[j] + CrashEps) {
                    FollowPath(i, j);
                }
            }
        }
        ++HistroyPos;
        HistroyPos %= CheckFrames;
    }

    void CarryEveryFrame() {
        RobotSort();
        GetRadius();
        // RobotRouteDoNotCrash();
        // 上面那个不要了
        CheckisCrash();
    }
}

void AllOrderGenerate() {
    Value::AdjustBalance456();
    ordervec.clear();
    int classcount = 0;
    for (int k = 0; k <= RobotCount; k++) {
        //if(Rob[k].isfree() == false) //小车不空闲 直接不管有关这个小车的订单  但是修改这里之前出现过bug
         //   continue;
        for (int i = 0; i <= WorkBenchCount; i++) {
            double TimeGo = 0.0;
            if(Rob[k].GetRemainBenchId() == -1) {
                TimeGo = RobotBenchDistance[k][i] / 6.0;
            }
            else {
                TimeGo = EmptyBenchDistance[Rob[k].GetRemainBenchId()][i] / 6.0;
            }
            if (Bench[i].GetProductHave() == true || (Bench[i].GetRemainFPS() != -1 && Bench[i].GetRemainFPS() * 0.02 <= TimeGo)) {
                int ProduceType = Bench[i].GetType();
                if (Bench[i].GetProductHave())
                    AllProduceCount[ProduceType]++;
                for (int j = 0; j <= WorkBenchCount; j++) {
                    if (Bench[j].DoMaterialNeed(ProduceType) == false) {
                        double TIM = Value::CalTime(i, j, k);
                        if (Bench[j].DoMaterialNeedByFPS(ProduceType, TIM) == false) {
                            continue;
                        }
                    }
                    if (Bench[j].DoMaterialHave(ProduceType) == true)
                        continue;
                    if (Bench[j].DoMaterialReserved(ProduceType) == true)
                        continue;
                    ++classcount;
                    Order ord;
                    ord.OrderGenerate(i, j);
                    ord.BotIdSet(k);
                    double val = Value::ValueCalculation(i, j, k);
                    ord.ValSet(val);
                    ord.ClassIdSet(classcount);
                    ordervec.push_back(ord);
                }
            }
        }
    }
    sort(ordervec.begin(), ordervec.end(), comp);
}

void RobotFindOrder() {
    int takeclass[5];
    memset(takeclass, 0, sizeof(takeclass));
    for (int i = 0; i <= RobotCount; i++) {
        if (Rob[i].isfree() == false)
            continue;
        for (auto ord : ordervec) {
            if (ord.GetRobotId() != i)
                continue;
            int itemtype = Bench[ord.GetBuyTask().GetAimId()].GetType();
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialNeed(itemtype) == false)
                continue;
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialHave(itemtype) == true)
                continue;
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialReserved(itemtype) == true)
                continue;
            if (Bench[ord.GetBuyTask().GetAimId()].isOutputReserved() == true)
                continue;
            bool ok = true;
            for (int j = 0; j < i; j++)
                if (takeclass[j] == ord.GetClassId())
                    ok = false;
            if (ok == false)
                continue;
            ord.OrderPathSet(i, Rob[i].GetRemainBenchId());
            if (ord.GetBuyTask().GetPathLen() == 0)
                continue;
            if (ord.GetSellTask().GetPathLen() == 0)
                continue;
            Rob[i].OrderSet(ord);
            takeclass[i] = ord.GetClassId();
            break;
        }
    }
}

void CarryTasksByFrame(void) { //*************************************PPPPPPP***需要修改一些逻辑
    // 运动&决策顺序
    bool IsOrderNeed = false;
    for (int i = 0; i <= RobotCount; i++) {
        IsOrderNeed |= Rob[i].isfree();
    }
    if (IsOrderNeed) {
        AllOrderGenerate();
        RobotFindOrder();
    }
    for (int i = 0; i <= RobotCount; i++) {
        // 最开始空闲时
        if (Rob[i].isfree() == true)
            continue;
        if (Rob[i].TaskFinished() == true) {
            AllTaskType type = Rob[i].GetTaskType();
            if (type == sell) {
                cout << "sell " << i << '\n';
                Rob[i].sell();
            }
            else if (type == buy) {
                // 再一次判断最后的帧数 判断最后一段时间到底需不需要购买
                double TimetoSell =
                    EmptyBenchDistance[i][Rob[i].GetOrder().GetBuyTask().GetAimId()] +
                    CarryBenchDistance[Rob[i].GetOrder().GetBuyTask().GetAimId()][Rob[i].GetOrder().GetSellTask().GetAimId()] / 6.0;
                if (frameID * 0.02 + TimetoSell >= 300)
                    continue;
                cout << "buy " << i << '\n';
                Rob[i].buy();
            }
        }
        else { // 没有到达，进行运动调整
            Move::RobotMove(i);
        }
    }
    
    Move::isNearBench();
    Move::ObstacleAvoid();
    Move::Crash();
    
    //   if(frameID % 100 == 0) { NoDoubleRobotInOneWay::CarryEveryFrame();}
    Move::MovePrint();
    cout << "OK" << endl;
}

void MapInit() {
    for (int i = 1; i <= 100; i++) {
        for (int j = 1; j <= 100; j++) {
            cin >> Map[i][j];
            PathFind::EmptyValid[i][j] = true; // 先默认赋值为true
            PathFind::CarryValid[i][j] = true; // 先默认赋值为true
            if (Map[i][j] == '.')
                continue;
            else if (isdigit(Map[i][j])) {
                if (Map[i][j] == '7')
                    flag7 = true;
                ++WorkBenchCount;
                double Nowx = 0.25 + 0.5 * (j - 1), Nowy = 49.75 - 0.5 * (i - 1);
                Bench[WorkBenchCount].PositionSet(Nowx, Nowy);
                Bench[WorkBenchCount].PositionGridSet(i, j);
                Bench[WorkBenchCount].TypeSet((int)(Map[i][j] - '0'));
            }
            else if (Map[i][j] == '#') {
                PathFind::EmptyValid[i][j] = false; // 设置为false
                PathFind::CarryValid[i][j] = false; // 设置为false
            }
            else {
                ++RobotCount;
                double Nowx = 0.25 + 0.5 * (j - 1), Nowy = 49.75 - 0.5 * (i - 1);
                Rob[RobotCount].PositionSet(Nowx, Nowy);
                Rob[RobotCount].RobIdSet(RobotCount);
            }
        }
    }
    string srubbishbin;
    cin >> srubbishbin;
}

bool InputOfFrame(void) { //读入每一帧
    int irubbishbin;
    double drubbishbin;
    cin >> frameID;
    if (frameID == -1)
        return false;
    cin >> Money;
    cin >> irubbishbin;
    for (int i = 0; i <= WorkBenchCount; i++) {
        cin >> irubbishbin;
        cin >> drubbishbin;
        cin >> drubbishbin;
        int fps;
        cin >> fps;
        Bench[i].Remainset(fps);
        int own;
        cin >> own;
        Bench[i].HaveSet(own);
        int status;
        cin >> status;
        Bench[i].ProductHaveSet(status);
    }
    for (int i = 0; i <= RobotCount; i++) {
        int InBenchId;
        cin >> InBenchId;
        Rob[i].WorkBenchIdSet(InBenchId);
        Rob[i].RemainBenchIdSet(InBenchId);
        int CarryId;
        cin >> CarryId;
        Rob[i].CarrySet(CarryId);
        double fTime;
        cin >> fTime;
        Rob[i].TimeValSet(fTime);
        double fColl;
        cin >> fColl;
        Rob[i].CollisionValSet(fColl);
        double SAngle, Sx, Sy;
        cin >> SAngle >> Sx >> Sy;
        Rob[i].SpeedSet(SAngle, Sx, Sy);
        double orient;
        cin >> orient;
        Rob[i].OrientationSet(orient);
        double px, py;
        cin >> px >> py;
        Rob[i].PositionSet(px, py);
    }
    string srubbishbin;
    cin >> srubbishbin;
    return true;
}

void Preprocess(void) {
    MapInit(); //读入地图
    PathFind::Modify(); //寻找路径
    cout << "OK" << endl;
}

void ProcessByFrame() {
    while (true) {
        bool status = InputOfFrame();
        if (status == false)
            break;
        cout << frameID << '\n';
        CarryTasksByFrame();
    }
}

int main() {
    Preprocess();
    ProcessByFrame();
    return 0;
}
