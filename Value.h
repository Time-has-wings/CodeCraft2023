#pragma once
#ifndef _Value_H
#define _Value_H

#include "Robot.h"

namespace Value 
{
    /**
     * @brief ����Robot -> Buy ��ת������ʱ��
    */
    double TurnCostRobToBuy(int BuyId, int SellId, int BotId) 
    {  
        double Angle = Vector::GetAngleByPoint(
            Rob[BotId].GetPosX(), Rob[BotId].GetPosY(), 
            Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY(), 
            0, 0, 
            cos(Rob[BotId].GetOrientation()),
            sin(Rob[BotId].GetOrientation()));  // ��������˵�����̨��ת��
        return Angle / PI;
    }


    /**
     * @brief: ����Buy -> Sell ��ת������ʱ��
    */
    double TurnCostBuyToSell(int BuyId, int SellId, int BotId) 
    {  
        double Angle = Vector::GetAngleByPoint(
            Rob[BotId].GetPosX(), Rob[BotId].GetPosY(), 
            Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY(), 
            Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY(),
            Bench[SellId].GetPosX(), Bench[SellId].GetPosY());  // ��������̨��������̨��ת��
        return Angle / PI;
    }

    // x��ʾʱ�� F���� ���ڼ���ʱ���ֵϵ������ײ��ֵϵ��
    double F(double x) 
    {  
        double tmp = pow(1 - x / 180, 2);  // 180��ʾ9000֡����3����
        return (1 - sqrt(1 - tmp)) * 0.2 + 0.8;
    }

    // ���������Ĺ�������
    double CalCurrentValue(int BuyId, int SellId, int BotId) 
    {  
        int MaterialType = Bench[BuyId].GetType();
        double val = Price[MaterialType];
        double distance = BenchDistance[BuyId][SellId];
        double time = distance / DivSpeed + TurnCostBuyToSell(BuyId, SellId, BotId);
        val *= F(time);
        return val;
    }

    // ����������Ǳ�ڼ�ֵ
    double CalPotentialPrice(int SellId, int BuyId, int BotId) {  // ����Ǳ�ڼ�ֵ
        double PotentialValue = 0.0;
        int SellType = Bench[SellId].GetType();
        int DemandCount = Bench[SellId].GetDemandCount();
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
        // if (SellType == 4 || SellType == 5 ||
        //     SellType == 6) {  // 4��5��6��Ǳ�ڼ�ֵ��������
        //     PotentialValue *= Balance[SellType];
        // }
        return PotentialValue;
    }

    double ValueCalculation(int BuyId, int SellId, int BotId) {
        double Value = 0;
        if (FrameId > LastLossFrame) 
        {  
            Value = CalCurrentValue(BuyId, SellId, BotId);
        }
        else 
        {
            Value = CalCurrentValue(BuyId, SellId, BotId) + CalPotentialPrice(SellId, BuyId, BotId);
        }
        double Distance1 = Vector::GetDistance(Rob[BotId].GetPosX(), Rob[BotId].GetPosY(), Bench[BuyId].GetPosX(), Bench[BuyId].GetPosY());
        double Distance2 = BenchDistance[BuyId][SellId];
        double Time = TurnCostRobToBuy(BuyId, SellId, BotId) + TurnCostBuyToSell(BuyId, SellId, BotId) + (Distance1 + Distance2) / DivSpeed;
        double AveValue = Value / Time;  // ���ʱ���ϵ�ƽ������
        // ���һ��ʱ�䲻���й�����
        if (FrameId * 0.02 + Time >= 180) AveValue = 0.0;
        return AveValue;
    }
}

#endif