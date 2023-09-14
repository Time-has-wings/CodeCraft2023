#include <ctype.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

const double PI = acos(-1);
const double AngleEpsFree = 0.127;
const double AngleEpsCarry = 0.245;
const double INF = 100000.0;
const double Price[] = { 0, 3000, 3200, 3400, 7100, 7800, 8300, 29000 };

int WorkBenchCount = -1;  // ����̨�ı�����ֵ
int RobotCount = -1;
int frameID, Money;
double BenchDistance[100][100];

namespace Vector {
    struct Vec {
        double x, y;
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
    double GetAngleByPoint(double sx1, double sy1, double ex1, double ey1,
        double sx2, double sy2, double ex2, double ey2) {
        Vec X;
        X.x = ex1 - sx1, X.y = ey1 - sy1;
        Vec Y;
        Y.x = ex2 - sx2, Y.y = ey2 - sy2;
        return AngleCalc(X, Y);
    }
    double GetDistance(double sx, double sy, double ex, double ey) {
        double DisX = ex - sx, DisY = ey - sy;
        double power = pow(DisX, 2) + pow(DisY, 2);
        double res = sqrt(power);
        return res;
    }
}  // namespace Vector

class WorkBench {
private:
    int Type;
    double PosX, PosY;
    int RemainFPS;
    bool ProduceFinished;  // true -> �ù���̨�в�Ʒ�ȴ��۳���false ->
                           // �ù���̨û�в�Ʒ�ȴ��۳�
    int MaterialNeed, MaterialReserved,
        MaterialHave;  // MaterialNeed Ϊ��Ҫ��ԭ��������Ķ����Ʊ�ʾ��
                       // MaterialHave Ϊ�Ѿ��е�ԭ���ϵĶ����Ʊ�ʾ
    int MaterialDistribute;
    bool IsDistributeBuy;
    bool outputReserved;

public:
    //
    void PositionSet(double Px, double Py) { PosX = Px, PosY = Py; }
    // 
    void Remainset(int fps) { RemainFPS = fps; }
    // 
    void HaveSet(int own) { MaterialHave = own; }
    // 
    void ProduceSet(int status) {
        if (status == 0)
            ProduceFinished = false;
        else
            ProduceFinished = true;
    }
    // 
    double GetPosX() { return PosX; }
    // 
    double GetPosY() { return PosY; }
    //
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
    //
    int GetType() { return Type; }
    //
    bool DoMaterialNeed(int Mtype) {
        if ((MaterialNeed >> Mtype) & 1) return true;
        return false;
    }  // �ղ��չ� Mtype ���͵Ĳ���
    //
    bool DoMaterialHave(int Mtype) {
        if ((MaterialHave >> Mtype) & 1) return true;
        return false;
    }  // ��û�� Mtype ���͵Ĳ���
    //
    bool DoMaterialReserved(int Mtype) {
        if ((MaterialReserved >> Mtype) & 1) return true;
        return false;
    }  // Mtype ���͵Ĳ�����û�л����� claim Ҫ�͹���
    // 
    bool GetProduceFinished() { return ProduceFinished; }
    // 
    void ReserveMaterial(int Mtype) { MaterialReserved |= (1 << Mtype); }
    // 
    void MaterialReceived(int Mtype) { MaterialReserved ^= (1 << Mtype); }

    int DemandCount() {
        int cnt = 0;
        for (int Mtype = 1; Mtype <= 7; Mtype++) {
            if (DoMaterialNeed(Mtype) && !DoMaterialHave(Mtype) &&
                !DoMaterialReserved(Mtype))
                cnt++;
        }
        return cnt;
    }
    // 
    int GetRemainFPS() { return RemainFPS; }
     // 
    void OutputReservedSet(bool status) {
        outputReserved = status;
    }
    // 
    bool isoutputreserved() {
        return outputReserved;
    }
} Bench[50];

// �㶨
enum AllTaskType { notask = 0, buy = 1, sell = 2 };

// �㶨
class Tasks {
private:
    double AimX, AimY;
    int AimId;
    AllTaskType TaskType;

public:
    void AimSet(int BenchId) {
        AimX = Bench[BenchId].GetPosX(), AimY = Bench[BenchId].GetPosY();
        AimId = BenchId;
    }
    int GetAimId() { return AimId; }
    AllTaskType GetType() { return TaskType; }
    double GetAimX() { return AimX; }
    double GetAimY() { return AimY; }
    void AimTypeSet(AllTaskType tp) { TaskType = tp; }
};

class Order {
private:
    Tasks tobuy, tosell;
    int CarryBotId;
    int classid;
    double ValCalc;

public:
    void OrderGenerate(int BuyBenchId, int SellBenchId) {
        tobuy.AimSet(BuyBenchId);
        tobuy.AimTypeSet(buy);
        tosell.AimSet(SellBenchId);
        tosell.AimTypeSet(sell);
    }
    void BotIdSet(int id) { CarryBotId = id; }
    void ValSet(double val) { ValCalc = val; }
    double GetVal() { return ValCalc; }
    int GetRobotId() { return CarryBotId; }
    Tasks GetBuyTask() { return tobuy; }
    Tasks GetSellTask() { return tosell; }
    void ClassIdSet(int id) { classid = id; }
    int GetClassId() { return classid; }
};
vector<Order> ordervec;
bool comp(Order a, Order b) { return a.GetVal() > b.GetVal(); }

class Robot {
private:
    int CarryItem;
    Tasks Task;
    Order order;
    bool DoingTask;
    int WorkBenchId;
    double PosX, PosY;
    double Orientation;
    double SpeedX, SpeedY;
    double SpeedAngle;
    double TimeVal, CollisionVal;
    double SpeedLine;  //************************���� ���ٶ�

public:
    void TaskSet(Tasks task) {
        Task = task;
        DoingTask = true;
    }
    void TaskGenerate(int BenchId,
        AllTaskType tp) {  // ֮������������Ǹ�����TaskSet
        Task.AimSet(BenchId);
        Task.AimTypeSet(tp);
        DoingTask = true;
    }
    void SetNoTask() {
        Task.AimTypeSet(notask);
        DoingTask = false;
    }
    bool isfree() {
        if (DoingTask) return false;
        return true;
    }
    void TakeOrder(Order ord) {
        TaskSet(ord.GetBuyTask());
        Bench[ord.GetBuyTask().GetAimId()].OutputReservedSet(true);
        order = ord;
        int tomodify = ord.GetSellTask().GetAimId();
        Bench[tomodify].ReserveMaterial(
            Bench[ord.GetBuyTask().GetAimId()].GetType());
    }
    void PositionSet(double Px, double Py) { PosX = Px, PosY = Py; }
    void WorkBenchIdSet(int InBenchId) { WorkBenchId = InBenchId; }
    void CarrySet(int CarryID) { CarryItem = CarryID; }
    void TimeValSet(double fTime) { TimeVal = fTime; }
    void CollisionValSet(double fColl) { CollisionVal = fColl; }
    void SpeedSet(double SAngle, double lx, double ly) {
        SpeedAngle = SAngle, SpeedX = lx, SpeedY = ly;
    }
    void OrientationSet(double Orient) { Orientation = Orient; }
    double GetDist(double Ax, double Ay) {
        double ret = (Ax - PosX) * (Ax - PosX) + (Ay - PosY) * (Ay - PosY);
        ret = sqrt(ret);
        return ret;
    }
    AllTaskType GetTaskType() { return Task.GetType(); }
    bool TaskFinished() {  // ����ָ������̨
        int AimBenchId = Task.GetAimId();
        if (WorkBenchId == AimBenchId) return true;
        return false;
    }
    double AngleCheck() {
        double NowAngle =
            Vector::GetAngleByPoint(PosX, PosY, Task.GetAimX(), Task.GetAimY(), 0,
                0, cos(Orientation), sin(Orientation));
        if (!IsCarry()) {
            if (NowAngle > AngleEpsFree) return NowAngle;
        }
        else {
            if (NowAngle > AngleEpsCarry) return NowAngle;
        }
        return -1;
    }
    int AdjustAngle() {
        double NowAngle =
            Vector::GetAngleByPoint(PosX, PosY, Task.GetAimX(), Task.GetAimY());
        double AngleSub = Orientation - NowAngle;
        if (AngleSub >= PI || (AngleSub >= -PI && AngleSub <= 0)) return 1;
        return -1;
    }  // ����ֵΪ -1 ����˳ʱ����ת������ֵΪ 1 ������ʱ����ת
    void sell() {
        int tomodify = order.GetSellTask().GetAimId();
        Bench[tomodify].MaterialReceived(
            Bench[order.GetBuyTask().GetAimId()].GetType());
        Task.AimTypeSet(notask);
        DoingTask = false;
    }
    void buy() {
        TaskSet(order.GetSellTask());
        Bench[order.GetBuyTask().GetAimId()].OutputReservedSet(false);
    }
    bool IsCarry() {
        if (CarryItem != 0) return true;
        return false;
    }
    int GetCarry() {  // ��ȡЯ����Ʒ
        return CarryItem;
    }
    double GetX() { return PosX; }
    double GetY() { return PosY; }
    double GetOrientation() { return Orientation; };
    //******************************************************���� �������
    void SpeedLineSet(double speedline) { SpeedLine = speedline; }
    void SpeedAngleSet(double speedangle) { SpeedAngle = speedangle; }
    void print(int id) {
        cout << "forward " << id << " " << SpeedLine << '\n';
        cout << "rotate " << id << " " << SpeedAngle << '\n';
    }
    double dis() {
        return GetDist(Task.GetAimX(), Task.GetAimY());  // ������Ŀ�깤��̨�ľ���
    }

    double GetSpeedLine() {
        return SpeedLine;
    }

    double GetSpeedX() {
        return SpeedX;
    }

    double GetSpeedY() {
        return SpeedY;
    }
    //*****************************************************����

} Rob[4];

namespace Move {
    double isNearWall(int id) {
        const double WallJudge = 2.5;
        const double OrienJudge = 0.25;
        double PosX = Rob[id].GetX(), PosY = Rob[id].GetY();
        double Orientation = Rob[id].GetOrientation();
        if (PosX > 50 - WallJudge && Orientation > (-PI / 2 + OrienJudge) &&
            Orientation < (PI / 2 - OrienJudge)) {  // ��ײǽ
            return 0.5;
        }
        else if (PosX < WallJudge && Orientation >(PI / 2 + OrienJudge) &&
            Orientation < (-PI / 2 - OrienJudge)) {  // ��ײǽ
            return 0.5;
        }
        else if (PosY > 50 - WallJudge && Orientation > OrienJudge &&
            Orientation < PI - OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        else if (PosY < WallJudge && Orientation > -OrienJudge &&
            Orientation < -PI + OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        return 1.0;
    }

    void RobotMove(int id) {
        const double NearBenchSpeed = 0.5;
        double NowAngle = Rob[id].AngleCheck();  // NowAngleȡֵ0~PI
        double SlowDown = isNearWall(id);
        if (NowAngle != -1) {                         // û�ж�׼
            int SpanDirection = Rob[id].AdjustAngle();  // ��ת��ת
            if (NowAngle <= 1) {
                Rob[id].SpeedLineSet(6.0 * SlowDown);
                Rob[id].SpeedAngleSet(1.0 * SpanDirection * PI);
            }
            else if (NowAngle <= 2) {
                Rob[id].SpeedLineSet(3.0 * SlowDown);
                Rob[id].SpeedAngleSet(1.0 * SpanDirection * PI);
            }
            else {
                Rob[id].SpeedLineSet(1.0 * SlowDown);
                Rob[id].SpeedAngleSet(1.0 * SpanDirection * PI);
            }
            if (Rob[id].dis() <= 1.0) {  // û��׼ �Ҿ����㹻��
                Rob[id].SpeedLineSet(NearBenchSpeed);
                Rob[id].SpeedAngleSet(SpanDirection * PI);
            }
        }
        else {
            Rob[id].SpeedLineSet(6.0 * SlowDown);
            Rob[id].SpeedAngleSet(0.0);
        }
    }

    // void Crash() {  // ��ֱ�ߵ������
    //   for (int i = 0; i <= RobotCount; i++) {
    //     for (int j = i + 1; j <= RobotCount; j++) {
    //       if (Rob[i].GetDist(Rob[j].GetX(), Rob[j].GetY()) <=
    //           1.2375) {  // �����㹻��
    //         double angle = fabs(Rob[i].GetOrientation() -
    //         Rob[j].GetOrientation()); if (5.0 / 6 * PI <= angle && angle <= 7.0 /
    //         6 * PI) {
    //           if (-PI <= Rob[i].GetOrientation() &&
    //                   Rob[i].GetOrientation() <= -PI / 2 ||
    //               0 <= Rob[i].GetOrientation() && Rob[i].GetOrientation() <= PI /
    //               2)
    //             Rob[i].SpeedAngleSet(PI);
    //           else
    //             Rob[i].SpeedAngleSet(-1 * PI);
    //           Rob[i].SpeedLineSet(3);
    //           if (-PI <= Rob[j].GetOrientation() &&
    //                   Rob[j].GetOrientation() <= -PI / 2 ||
    //               0 <= Rob[j].GetOrientation() && Rob[j].GetOrientation() <= PI /
    //               2)
    //             Rob[i].SpeedAngleSet(PI);
    //           else
    //             Rob[j].SpeedAngleSet(-1 * PI);
    //           Rob[j].SpeedLineSet(3);
    //         }
    //       }
    //     }
    //   }
    // }

    // void crashVersion2() {
    //   for (int i = 0; i <= RobotCount; i++) {
    //     for (int j = i + 1; j <= RobotCount; j++) {
    //       if (Rob[i].GetDist(Rob[j].GetX(), Rob[j].GetY()) <=
    //           1.2375) {  // �����㹻��
    //         double angle = fabs(Rob[i].GetOrientation() -
    //         Rob[j].GetOrientation()); if (5.0 / 6 * PI <= angle && angle <= 7.0 /
    //         6 * PI) {
    //           if (Rob[i].GetX() <= Rob[j].GetX()) {
    //             Rob[i].SpeedAngleSet(PI);
    //             Rob[j].SpeedAngleSet(-1 * PI);
    //           } else {
    //             Rob[i].SpeedAngleSet(-1 * PI);
    //             Rob[j].SpeedAngleSet(PI);
    //           }
    //           Rob[i].SpeedLineSet(3);
    //           Rob[j].SpeedLineSet(3);
    //         }
    //       }
    //     }
    //   }
    // }

    void crashversion3() {  // FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
        const double CrashDis = 2;
        const double AngleReset = PI;
        const double SpeedReset = 2;
        const double HeadOnJudge1 = 5.0 / 6 * PI, HeadOnJudge2 = 7.0 / 6 * PI;
        for (int i = 0; i <= RobotCount; i++) {
            for (int j = i + 1; j <= RobotCount; j++) {
                if (Rob[i].GetDist(Rob[j].GetX(), Rob[j].GetY()) <=
                    CrashDis) {  // �����㹻��
                    double angle = fabs(Rob[i].GetOrientation() - Rob[j].GetOrientation());
                    if (HeadOnJudge1 <= angle && angle <= HeadOnJudge2) {
                        if (Rob[i].GetOrientation() >=
                            Vector::GetAngleByPoint(Rob[i].GetX(), Rob[i].GetY(),
                                Rob[j].GetX(), Rob[j].GetY()))
                            Rob[i].SpeedAngleSet(AngleReset);
                        else
                            Rob[i].SpeedAngleSet(-1 * AngleReset);
                        if (Rob[j].GetOrientation() >=
                            Vector::GetAngleByPoint(Rob[j].GetX(), Rob[j].GetY(),
                                Rob[i].GetX(), Rob[i].GetY()))
                            Rob[j].SpeedAngleSet(AngleReset);
                        else
                            Rob[j].SpeedAngleSet(-1 * AngleReset);
                        Rob[i].SpeedLineSet(SpeedReset);
                        Rob[j].SpeedLineSet(SpeedReset);
                    }
                }
            }
        }
    }

    void crashversion4() {
        const double CrashDis = 4.0;
        for (int i = 0; i <= RobotCount; i++) {
            for (int j = i + 1; j <= RobotCount; j++) {
                double dis = Rob[j].GetDist(Rob[i].GetX(), Rob[i].GetY());
                if (dis <= CrashDis) {
                    //����Rob[j]�����Rob[i]���������
                    double dx = Rob[j].GetX() - Rob[i].GetX(), dy = Rob[j].GetY() - Rob[i].GetY();
                    //����Rob[j]�����Rob[i]�����x��y�����ٶ�
                    double dxspeed = Rob[j].GetSpeedX() - Rob[i].GetSpeedX();
                    double dyspeed = Rob[j].GetSpeedY() - Rob[i].GetSpeedY();
                    //�����������ĽǶ� �� �ȽϵĽǶ�
                    double angle = Vector::GetAngleByPoint(dx, dy, 0, 0);
                    double anglecmp = asin(1.22 / dis);
                    double speedangle = Vector::GetAngleByPoint(0, 0, dxspeed, dyspeed);

                    double leftangle = angle - anglecmp, rightangle = angle + anglecmp;
                    if (leftangle < -PI) { //���������rightangle���Բ�������κ�����
                        leftangle += 2 * PI;
                        if (-PI <= speedangle && speedangle <= angle || leftangle <= speedangle && speedangle <= PI) { //˳ʱ��
                            Rob[j].SpeedAngleSet(-1 * PI);
                            Rob[j].SpeedLineSet(2);
                            Rob[i].SpeedAngleSet(-1 * PI);
                            Rob[i].SpeedLineSet(2);
                        }
                        if (angle <= speedangle && speedangle <= angle + anglecmp) { //��ʱ��ת��
                            Rob[j].SpeedAngleSet(PI);
                            Rob[j].SpeedLineSet(2);
                            Rob[i].SpeedAngleSet(PI);
                            Rob[i].SpeedLineSet(2);
                        }
                    }
                    else {
                        if (rightangle > PI) {
                            rightangle -= 2 * PI;
                            if (angle - anglecmp <= speedangle && speedangle <= angle) { //˳ʱ��
                                Rob[j].SpeedAngleSet(-1 * PI);
                                Rob[j].SpeedLineSet(2);
                                Rob[i].SpeedAngleSet(-1 * PI);
                                Rob[i].SpeedLineSet(2);
                            }
                            if (angle <= speedangle && speedangle <= PI || -1 * PI <= speedangle && speedangle <= rightangle) {
                                Rob[j].SpeedAngleSet(PI);
                                Rob[j].SpeedLineSet(2);
                                Rob[i].SpeedAngleSet(PI);
                                Rob[i].SpeedLineSet(2);
                            }
                        }
                        else { //��������
                            if (angle - anglecmp <= speedangle && speedangle <= angle) { //˳ʱ��
                                Rob[j].SpeedAngleSet(-1 * PI);
                                Rob[j].SpeedLineSet(2);
                                Rob[i].SpeedAngleSet(-1 * PI);
                                Rob[i].SpeedLineSet(2);
                            }
                            if (angle <= speedangle && speedangle <= angle + anglecmp) { //��ʱ��ת��
                                Rob[j].SpeedAngleSet(PI);
                                Rob[j].SpeedLineSet(2);
                                Rob[i].SpeedAngleSet(PI);
                                Rob[i].SpeedLineSet(2);
                            }
                        }
                    }

                }
            }
        }
    }

    void Print() {
        for (int i = 0; i <= RobotCount; i++) Rob[i].print(i);
    }
}  // namespace Move

// ����������
double Balance[] = { 0, 1, 1, 1, 1,
                    1, 1, 1 };  // ����1-7�ľ���Ȩ�� �ȿ�����4��5��6�ľ���������
int AllProduceCount[] = { 0, 0, 0, 0, 0, 0, 0, 0 };  // ���ʼ����1-7������
bool flag7 = false;

// ����4��5��6Ȩ�صĺ���
void AdjustBalance456() {
    if (!flag7) return;
    int tot = AllProduceCount[4] + AllProduceCount[5] + AllProduceCount[6];
    if (tot == 0) return;  // ��ֹ�ʼʱ�������
    Balance[4] = 1.5 * (AllProduceCount[5] + AllProduceCount[6]) / tot;
    Balance[5] = 1.5 * (AllProduceCount[4] + AllProduceCount[6]) / tot;
    Balance[6] = 1.5 * (AllProduceCount[4] + AllProduceCount[5]) / tot;
}

namespace Value {
    //************************************************************************************************���в���
    const double DecDeliverto9 = 0.5;
    const double DivSpeed = 6.0;
    const double Demand456_1 = 0.5, Demand456_2 = 0.3;
    const double Demand7_1 = 0.5, Demand7_2 = 0.3, Demand7_3 = 0.1;
    const double LastLossFrame = 7800;
    //************************************************************************************************���в���

    double TurnCostRobToBuy(int BuyId, int SellId,
        int BotId) {  // ����Rot -> buy��ת�������ǼӰ�
        double Angle = Vector::GetAngleByPoint(
            Rob[BotId].GetX(), Rob[BotId].GetY(), Bench[BuyId].GetPosX(),
            Bench[BuyId].GetPosY(), 0, 0, cos(Rob[BotId].GetOrientation()),
            sin(Rob[BotId].GetOrientation()));  // ��������˵�����̨��ת��
        return Angle / PI;
    }

    double TurnCostBuyToSell(int BuyId, int SellId,
        int BotId) {  // ����Buy -> Sell��ת������ʱ��
        double Angle = Vector::GetAngleByPoint(
            Rob[BotId].GetX(), Rob[BotId].GetY(), Bench[BuyId].GetPosX(),
            Bench[BuyId].GetPosY(), Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY(),
            Bench[SellId].GetPosX(),
            Bench[SellId].GetPosY());  // ��������̨��������̨��ת��
        return Angle / PI;
    }

    double F(double x) {  // x��ʾʱ�� F���� ���ڼ���ʱ���ֵϵ������ײ��ֵϵ��
        double tmp = pow(1 - x / 180, 2);  // 180��ʾ9000֡����3����
        return (1 - sqrt(1 - tmp)) * 0.2 + 0.8;
    }

    double CalCurrentValue(int BuyId, int SellId,
        int BotId) {  // ���������Ĺ�������
        int MaterialType = Bench[BuyId].GetType();
        double val = Price[MaterialType];
        double distance = BenchDistance[BuyId][SellId];
        double time = distance / DivSpeed + TurnCostBuyToSell(BuyId, SellId, BotId);
        val *= F(time);
        return val;
    }

    double CalPotentialPrice(int SellId, int BuyId, int BotId) {  // ����Ǳ�ڼ�ֵ
        double PotentialValue = 0.0;
        int SellType = Bench[SellId].GetType();
        int DemandCount = Bench[SellId].DemandCount();
        switch (SellType) {
        case 9:
        case 8:
            PotentialValue = 0;  // Ǳ�ڼ�ֵΪ0
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
        if (SellType == 4 || SellType == 5 ||
            SellType == 6) {  // 4��5��6��Ǳ�ڼ�ֵ��������
            PotentialValue *= Balance[SellType];
        }
        return PotentialValue;
    }

    double ValueCalculation(int BuyId, int SellId, int BotId) {
        double Value = 0;
        if (frameID > LastLossFrame) {  // FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
            Value = CalCurrentValue(BuyId, SellId, BotId);
        }
        else {
            Value = CalCurrentValue(BuyId, SellId, BotId) +
                CalPotentialPrice(SellId, BuyId, BotId);
        }
        double Distance1 =
            Vector::GetDistance(Rob[BotId].GetX(), Rob[BotId].GetY(),
                Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY());
        double Distance2 = BenchDistance[BuyId][SellId];
        double Time = TurnCostRobToBuy(BuyId, SellId, BotId) +
            TurnCostBuyToSell(BuyId, SellId, BotId) +
            (Distance1 + Distance2) / DivSpeed;
        // if (Bench[BuyId].GetRemainFPS() != -1) Time -= Bench[BuyId].GetRemainFPS()
        // * 0.02;
        double AveValue = Value / Time;  // ���ʱ���ϵ�ƽ������
        // ���һ��ʱ�䲻���й�����
        if (frameID * 0.02 + Time >= 180) AveValue = 0.0;
        return AveValue;
    }
}  // namespace Value

bool vis[100000];

void AllOrderGenerate() {
    memset(vis, false, sizeof vis);
    AdjustBalance456();
    ordervec.clear();
    int classcount = 0;
    for (int i = 0; i <= WorkBenchCount; i++) {
        double TimeGo = INF;
        for (int j = 0; j <= RobotCount; j++) {
            TimeGo = min(
                TimeGo, Rob[j].GetDist(Bench[i].GetPosX(), Bench[i].GetPosY()) / 6.0);
        }
        // if (Bench[i].GetProduceFinished() == false ) {
        //     continue;
        // }
        if(Bench[i].GetProduceFinished() == true) {
        //if (Bench[i].GetProduceFinished() == true || Bench[i].GetProduceFinished() == false && Bench[i].GetRemainFPS() != -1 && Bench[i].GetRemainFPS() * 0.02 <= TimeGo) {
            int ProduceType = Bench[i].GetType();
            AllProduceCount
                [ProduceType]++;  //**********************************************************************����
                                  // �����ʼ�������������Ʒ������
            for (int j = 0; j <= WorkBenchCount; j++) {
                if (Bench[j].DoMaterialNeed(ProduceType) == false) continue;
                if (Bench[j].DoMaterialHave(ProduceType) == true) continue;
                if (Bench[j].DoMaterialReserved(ProduceType) == true) continue;
                ++classcount;  // classcount��ʾ����������
                vis[classcount] = true;
                for (int k = 0; k <= RobotCount; k++) {
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
        if (Rob[i].isfree() == false) continue;
        for (auto ord : ordervec) {
            if (ord.GetRobotId() != i) continue;
            int itemtype = Bench[ord.GetBuyTask().GetAimId()].GetType();
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialNeed(itemtype) == false)
                continue;
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialHave(itemtype) == true)
                continue;
            if (Bench[ord.GetSellTask().GetAimId()].DoMaterialReserved(itemtype) ==
                true)
                continue;
            if (Bench[ord.GetBuyTask().GetAimId()].isoutputreserved() == true) continue;
            bool ok = true;
            for (int j = 0; j < i; j++)
                if (takeclass[j] == ord.GetClassId()) ok = false;
            if (ok == false) continue;
            Rob[i].TakeOrder(ord);
            takeclass[i] = ord.GetClassId();
            break;
        }
    }
}

void SingleRobotFindOrder(int id) {
    for (auto ord : ordervec) {  // ����Ѿ��ź�����
        if (ord.GetRobotId() != id) continue;
        int itemtype = Bench[ord.GetBuyTask().GetAimId()].GetType();
        if (Bench[ord.GetSellTask().GetAimId()].DoMaterialNeed(itemtype) == false)
            continue;
        if (Bench[ord.GetSellTask().GetAimId()].DoMaterialHave(itemtype) == true)
            continue;
        if (Bench[ord.GetSellTask().GetAimId()].DoMaterialReserved(itemtype) ==
            true)
            continue;
        bool ok = true;
        if (!vis[ord.GetClassId()]) {  // ���з���
            vis[ord.GetClassId()] = true;
            Rob[id].TakeOrder(ord);
            break;
        }
        break;
    }
}

void CarryTasksByFrame(void) {
    AllOrderGenerate();
    RobotFindOrder();
    for (int i = 0; i <= RobotCount; i++) {
        // �ʼ����ʱ
        if (Rob[i].isfree() == true) continue;
        if (Rob[i].TaskFinished() == true) {
            AllTaskType type = Rob[i].GetTaskType();
            if (type == sell) {
                cout << "sell " << i << '\n';
                Rob[i].sell();
                //SingleRobotFindOrder(i);
            }
            else if (type == buy) {
                cout << "buy " << i << '\n';
                Rob[i].buy();
            }
        }
        // û�е��� �����˶�����
        else {
            Move::RobotMove(i);
        }
    }
    // Move::Crash();
    // Move::crashVersion2();
    //Move::crashversion3();
    Move::crashversion4();
    Move::Print();
    cout << "OK" << endl;
}

void MapInit(void) {
    for (int i = 1; i <= 100; i++) {
        for (int j = 1; j <= 100; j++) {
            char mapchar;
            cin >> mapchar;
            if (mapchar == '.')
                continue;
            else if (isdigit(mapchar)) {
                if (mapchar == '7') flag7 = true;
                ++WorkBenchCount;
                double Nowx = 0.25 + 0.5 * (j - 1),
                    Nowy = 49.75 - 0.5 * (i - 1);  // ���㵱ǰ����
                Bench[WorkBenchCount].PositionSet(Nowx, Nowy);
                Bench[WorkBenchCount].TypeSet((int)(mapchar - '0'));
            }
            else {
                ++RobotCount;
                double Nowx = 0.25 + 0.5 * (j - 1),
                    Nowy = 49.75 - 0.5 * (i - 1);  // ���㵱ǰ����
                Rob[RobotCount].PositionSet(Nowx, Nowy);
            }
        }
        for (int i = 0; i <= WorkBenchCount - 1; i++) {
            for (int j = i + 1; j <= WorkBenchCount; j++) {
                double x1 = Bench[i].GetPosX(), y1 = Bench[i].GetPosY();
                double x2 = Bench[j].GetPosX(), y2 = Bench[j].GetPosY();
                BenchDistance[i][j] = BenchDistance[j][i] =
                    Vector::GetDistance(x1, y1, x2, y2);
            }
        }
    }
    string srubbishbin;
    cin >> srubbishbin;
}

bool InputOfFrame(void) {
    int irubbishbin;
    double drubbishbin;
    cin >> frameID;
    if (frameID == -1) return false;
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
        Bench[i].ProduceSet(status);
    }
    for (int i = 0; i <= RobotCount; i++) {
        int InBenchId;
        cin >> InBenchId;
        Rob[i].WorkBenchIdSet(InBenchId);
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

void ProcessByFrame() {
    while (true) {
        bool status = InputOfFrame();
        if (status == false) break;
        cout << frameID << '\n';
        CarryTasksByFrame();
    }
}

void Preprocess(void) {
    MapInit();
    cout << "OK" << endl;
}

int main() {
    Preprocess();
    ProcessByFrame();
    return 0;
}
