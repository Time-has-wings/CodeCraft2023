#pragma once
#ifndef _Move_H
#define _Move_H

#include "defs.h"
#include "Robot.h"

namespace Move {
    double isNearWall(int id) 
    {
        const double WallJudge = 2.5;
        const double OrienJudge = 0.25;
        double PosX = Rob[id].GetPosX(), PosY = Rob[id].GetPosY();
        double Orientation = Rob[id].GetOrientation();
        if (PosX > 50 - WallJudge && Orientation > -PI / 2 + OrienJudge && Orientation < PI / 2 - OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        else if (PosX < WallJudge && Orientation > PI / 2 + OrienJudge && Orientation < -PI / 2 - OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        else if (PosY > 50 - WallJudge && Orientation > OrienJudge && Orientation < PI - OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        else if (PosY < WallJudge && Orientation > -OrienJudge && Orientation < -PI + OrienJudge) {  // ��ײǽ
            return 0.5;
        }
        return 1.0;
    }

    void RobotMove(int id) 
    {
        double NowAngle = Rob[id].AngleCheck();  // NowAngleȡֵ0~PI
        double SlowDown = isNearWall(id);
        if (NowAngle != -1) {                         // û�ж�׼
            int SpanDirection = Rob[id].AdjustAngle();  // ��ת��ת
            if (NowAngle <= 1) {
                Rob[id].LineSpeedSet(6.0 * SlowDown);
                Rob[id].AngleSpeedSet(1.0 * SpanDirection * PI);
            }
            else if (NowAngle <= 2) {
                Rob[id].LineSpeedSet(3.0 * SlowDown);
                Rob[id].AngleSpeedSet(1.0 * SpanDirection * PI);
            }
            else {
                Rob[id].LineSpeedSet(1.0 * SlowDown);
                Rob[id].AngleSpeedSet(1.0 * SpanDirection * PI);
            }
            if (Rob[id].DisFromAim() <= 1.0) {  // û��׼ �Ҿ����㹻��
                Rob[id].LineSpeedSet(NearBenchSpeed);
                Rob[id].AngleSpeedSet(SpanDirection * PI);
            }
        }
        else {
            Rob[id].LineSpeedSet(6.0 * SlowDown);
            Rob[id].AngleSpeedSet(0.0);
        }
    }

    void Crash() 
    {
        for (int i = 0; i <= RobotCount; i++) 
        {
            for (int j = i + 1; j <= RobotCount; j++) 
            {
                double dis = Vector::GetDistance(Rob[j].GetPosX(), Rob[j].GetPosY(), Rob[i].GetPosX(), Rob[i].GetPosY());
                if (dis <= CrashDis) {
                    //����Rob[j]�����Rob[i]���������
                    double dx = Rob[j].GetPosX() - Rob[i].GetPosX(), dy = Rob[j].GetPosY() - Rob[i].GetPosY();
                    //����Rob[j]�����Rob[i]�����x��y�����ٶ�
                    double dxspeed = Rob[j].GetSpeedX() - Rob[i].GetSpeedX();
                    double dyspeed = Rob[j].GetSpeedY() - Rob[i].GetSpeedY();
                    //�����������ĽǶ� �� �ȽϵĽǶ�
                    double angle = Vector::GetAngleByPoint(dx, dy, 0, 0);
                    double anglecmp = asin(1.22 / dis);
                    double speedangle = Vector::GetAngleByPoint(0, 0, dxspeed, dyspeed);

                    double leftangle = angle - anglecmp, rightangle = angle + anglecmp;
                    if (leftangle < -PI) 
                    { //���������rightangle���Բ�������κ�����
                        leftangle += 2 * PI;
                        if (-PI <= speedangle && speedangle <= angle || leftangle <= speedangle && speedangle <= PI) 
                        { //˳ʱ��
                            Rob[j].AngleSpeedSet(-1 * PI);
                            Rob[j].LineSpeedSet(2);
                            Rob[i].AngleSpeedSet(-1 * PI);
                            Rob[i].LineSpeedSet(2);
                        }
                        if (angle <= speedangle && speedangle <= angle + anglecmp)
                         { //��ʱ��ת��
                            Rob[j].AngleSpeedSet(PI);
                            Rob[j].LineSpeedSet(2);
                            Rob[i].AngleSpeedSet(PI);
                            Rob[i].LineSpeedSet(2);
                        }
                    }
                    else 
                    {
                        if (rightangle > PI)
                        {
                            rightangle -= 2 * PI;
                            if (angle - anglecmp <= speedangle && speedangle <= angle) 
                            { //˳ʱ��
                                Rob[j].AngleSpeedSet(-1 * PI);
                                Rob[j].LineSpeedSet(2);
                                Rob[i].AngleSpeedSet(-1 * PI);
                                Rob[i].LineSpeedSet(2);
                            }
                            if (angle <= speedangle && speedangle <= PI || -1 * PI <= speedangle && speedangle <= rightangle)
                            {
                                Rob[j].AngleSpeedSet(PI);
                                Rob[j].LineSpeedSet(2);
                                Rob[i].AngleSpeedSet(PI);
                                Rob[i].LineSpeedSet(2);
                            }
                        }
                        else 
                        { //��������
                            if (angle - anglecmp <= speedangle && speedangle <= angle) 
                            { //˳ʱ��
                                Rob[j].AngleSpeedSet(-1 * PI);
                                Rob[j].LineSpeedSet(2);
                                Rob[i].AngleSpeedSet(-1 * PI);
                                Rob[i].LineSpeedSet(2);
                            }
                            if (angle <= speedangle && speedangle <= angle + anglecmp) 
                            { //��ʱ��ת��
                                Rob[j].AngleSpeedSet(PI);
                                Rob[j].LineSpeedSet(2);
                                Rob[i].AngleSpeedSet(PI);
                                Rob[i].LineSpeedSet(2);
                            }
                        }
                    }
                }
            }
        }
    }

    void Print()
    {
        for (int id = 0; id <= RobotCount; id++) 
            Rob[id].Print(id);
    }
}


/*
Crash�ж������汾����

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

    // void crashversion3() {  // FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    //     const double CrashDis = 2;
    //     const double AngleReset = PI;
    //     const double SpeedReset = 2;
    //     const double HeadOnJudge1 = 5.0 / 6 * PI, HeadOnJudge2 = 7.0 / 6 * PI;
    //     for (int i = 0; i <= RobotCount; i++) {
    //         for (int j = i + 1; j <= RobotCount; j++) {
    //             if (Rob[i].GetDist(Rob[j].GetX(), Rob[j].GetY()) <=
    //                 CrashDis) {  // �����㹻��
    //                 double angle = fabs(Rob[i].GetOrientation() - Rob[j].GetOrientation());
    //                 if (HeadOnJudge1 <= angle && angle <= HeadOnJudge2) {
    //                     if (Rob[i].GetOrientation() >=
    //                         Vector::GetAngleByPoint(Rob[i].GetX(), Rob[i].GetY(),
    //                             Rob[j].GetX(), Rob[j].GetY()))
    //                         Rob[i].SpeedAngleSet(AngleReset);
    //                     else
    //                         Rob[i].SpeedAngleSet(-1 * AngleReset);
    //                     if (Rob[j].GetOrientation() >=
    //                         Vector::GetAngleByPoint(Rob[j].GetX(), Rob[j].GetY(),
    //                             Rob[i].GetX(), Rob[i].GetY()))
    //                         Rob[j].SpeedAngleSet(AngleReset);
    //                     else
    //                         Rob[j].SpeedAngleSet(-1 * AngleReset);
    //                     Rob[i].SpeedLineSet(SpeedReset);
    //                     Rob[j].SpeedLineSet(SpeedReset);
    //                 }
    //             }
    //         }
    //     }
    // }
    
    // void crashversion4() {
    //     const double CrashDis = 4.0;
    //     for (int i = 0; i <= RobotCount; i++) {
    //         for (int j = i + 1; j <= RobotCount; j++) {
    //             double dis = Rob[j].GetDist(Rob[i].GetX(), Rob[i].GetY());
    //             if (dis <= CrashDis) {
    //                 //����Rob[j]�����Rob[i]���������
    //                 double dx = Rob[j].GetX() - Rob[i].GetX(), dy = Rob[j].GetY() - Rob[i].GetY();
    //                 //����Rob[j]�����Rob[i]�����x��y�����ٶ�
    //                 double dxspeed = Rob[j].GetSpeedX() - Rob[i].GetSpeedX();
    //                 double dyspeed = Rob[j].GetSpeedY() - Rob[i].GetSpeedY();
    //                 //�����������ĽǶ� �� �ȽϵĽǶ�
    //                 double angle = Vector::GetAngleByPoint(dx, dy, 0, 0);
    //                 double anglecmp = asin(1.22 / dis);
    //                 double speedangle = Vector::GetAngleByPoint(0, 0, dxspeed, dyspeed);

    //                 double leftangle = angle - anglecmp, rightangle = angle + anglecmp;
    //                 if (leftangle < -PI) { //���������rightangle���Բ�������κ�����
    //                     leftangle += 2 * PI;
    //                     if (-PI <= speedangle && speedangle <= angle || leftangle <= speedangle && speedangle <= PI) { //˳ʱ��
    //                         Rob[j].SpeedAngleSet(-1 * PI);
    //                         Rob[j].SpeedLineSet(2);
    //                         Rob[i].SpeedAngleSet(-1 * PI);
    //                         Rob[i].SpeedLineSet(2);
    //                     }
    //                     if (angle <= speedangle && speedangle <= angle + anglecmp) { //��ʱ��ת��
    //                         Rob[j].SpeedAngleSet(PI);
    //                         Rob[j].SpeedLineSet(2);
    //                         Rob[i].SpeedAngleSet(PI);
    //                         Rob[i].SpeedLineSet(2);
    //                     }
    //                 }
    //                 else {
    //                     if (rightangle > PI) {
    //                         rightangle -= 2 * PI;
    //                         if (angle - anglecmp <= speedangle && speedangle <= angle) { //˳ʱ��
    //                             Rob[j].SpeedAngleSet(-1 * PI);
    //                             Rob[j].SpeedLineSet(2);
    //                             Rob[i].SpeedAngleSet(-1 * PI);
    //                             Rob[i].SpeedLineSet(2);
    //                         }
    //                         if (angle <= speedangle && speedangle <= PI || -1 * PI <= speedangle && speedangle <= rightangle) {
    //                             Rob[j].SpeedAngleSet(PI);
    //                             Rob[j].SpeedLineSet(2);
    //                             Rob[i].SpeedAngleSet(PI);
    //                             Rob[i].SpeedLineSet(2);
    //                         }
    //                     }
    //                     else { //��������
    //                         if (angle - anglecmp <= speedangle && speedangle <= angle) { //˳ʱ��
    //                             Rob[j].SpeedAngleSet(-1 * PI);
    //                             Rob[j].SpeedLineSet(2);
    //                             Rob[i].SpeedAngleSet(-1 * PI);
    //                             Rob[i].SpeedLineSet(2);
    //                         }
    //                         if (angle <= speedangle && speedangle <= angle + anglecmp) { //��ʱ��ת��
    //                             Rob[j].SpeedAngleSet(PI);
    //                             Rob[j].SpeedLineSet(2);
    //                             Rob[i].SpeedAngleSet(PI);
    //                             Rob[i].SpeedLineSet(2);
    //                         }
    //                     }
    //                 }

    //             }
    //         }
    //     }
    // }
    
*/

#endif