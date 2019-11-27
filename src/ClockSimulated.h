#ifndef _CLOCKSIMULATED_H_
#define _CLOCKSIMULATED_H_
#pragma once

#define _MQREAD_MODE_
#include <chrono>
#include <iostream>
#include <vector>
#include "MarketQuotationAPI_HC_Defination.h"

namespace axapi
{
    /// ��������ʱ����
    struct ClockSimulatedField
    {
        /// ��Ȼ��
        char basedate[9];
        /// ʱ��hh:mm:ss
        char minute[9];
        /// ����
        int millisecond;
        /// ���
        int sequence;
    };

    class ClockSimulated
    {
    private:
        /// ���ģ��ʱ�����ݼ�
        std::vector<ClockSimulatedField> m_vClockSimulatedList;
        /// ��ſ�ʼ���е�ʱ��
        std::chrono::high_resolution_clock::time_point m_starttime;
        /// �ٶȳ��� ����Ϊ10^9��ʾδ���٣�10^6��ʶ����1000����ģ��ʱ������1000�������ʵ1�룬��ģ��ʱ�����Ϊ_MIN_SPEEDMULTIPLIER
        long m_nSpeedMultiplier;
    public:
        ClockSimulated();
        ~ClockSimulated();
        /// �����ٶȳ���������Խ��ģ��Խ�� 10^9��ʵ��1��=ģ��1�� 10^6��ʵ��1����=ģ��1�� 10^3��ʵ��1΢��=ģ��1�� 10^0��ʵ��1����=ģ��1��
        long setSpeedMultiplier(long);
        /// ���ģ��ʱ�ӵ�����ʱ���� ����Ҫģ��20190903 08:00:00-21:00:00�����ڰ���ģ�⣬��ʱ����Ӧ�ð���������ÿ���ʱ��
        void fillupClockLine(std::vector<TradingClockField>*);
        /// ��ʼ��ʱ
        void startClocking();
        /// ��õ�ǰʱ��
        ClockSimulatedField *getCurrentTime();
    };
}

#endif _CLOCKSIMULATED_H_