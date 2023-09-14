#pragma once
#ifndef _Decision_H
#define _Decision_H
// #include "Robot.h"
// #include "Move.h"
// #include "Value.h"

// std::vector<Order> Ordervec;

// namespace Decision {
    
//     void AllOrderGenerate() 
//     {
//         Ordervec.clear();
//         int classcount = 0;
//         for (int k = 0; k <= RobotCount; k++) 
//         {
//             //if(Rob[k].isfree() == false) //小车不空闲 直接不管有关这个小车的订单  但是修改这里之前出现过bug
//             //   continue;
//             for (int i = 0; i <= WorkBenchCount; i++) 
//             {
//                 double TimeGo = 0.0;
//                 if(Rob[k].GetRemainBenchId() == -1) {
//                     TimeGo = RobotBenchDistance[k][i] / 6.0;
//                 }
//                 else {
//                     TimeGo = EmptyBenchDistance[Rob[k].GetRemainBenchId()][i] / 6.0;
//                 }
//                 if (Bench[i].GetProductHave() == true || (Bench[i].GetRemainFPS() != -1 && Bench[i].GetRemainFPS() * 0.02 <= TimeGo)) {
//                     int ProduceType = Bench[i].GetType();
//                     if (Bench[i].GetProductHave())
//                         AllProduceCount[ProduceType]++;
//                     for (int j = 0; j <= WorkBenchCount; j++) {
//                         if (Bench[j].DoMaterialNeed(ProduceType) == false) {
//                             double TIM = Value::CalTime(i, j, k);
//                             if (Bench[j].DoMaterialNeedByFPS(ProduceType, TIM) == false) {
//                                 continue;
//                             }
//                         }
//                         if (Bench[j].DoMaterialHave(ProduceType) == true)
//                             continue;
//                         if (Bench[j].DoMaterialReserved(ProduceType) == true)
//                             continue;
//                         ++classcount;
//                         Order ord;
//                         ord.OrderGenerate(i, j);
//                         ord.RoBotIdSet(k);
//                         double val = Value::ValueCalculation(i, j, k);
//                         ord.ValueSet(val);
//                         ord.ClassIdSet(classcount);
//                         Ordervec.push_back(ord);
//                     }
//                 }
//             }
//         }
//         sort(Ordervec.begin(), Ordervec.end());
//     }
// }

#endif