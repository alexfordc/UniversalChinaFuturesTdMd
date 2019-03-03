#ifndef _MQI_MACDINDEX_H_
#define _MQI_MACDINDEX_H_
#pragma once

#ifdef MQI_MACDINDEX_EXE
#define MQI_MACDINDEX_EXPORT
#else
#ifdef MQI_MACDINDEX_EXP
#define MQI_MACDINDEX_EXPORT __declspec(dllexport)
#else
#define MQI_MACDINDEX_EXPORT __declspec(dllimport)
#endif MQI_MACDINDEX_EXP
#endif MQI_MACDINDEX_EXE

#include "MarketQuotationIndex.h"

/*
* Moving Average Convergence / Divergence
��EMA1�Ĳ���Ϊ12��EMA2�Ĳ���Ϊ26�գ�DIF�Ĳ���Ϊ9��Ϊ��������MACD�ļ������
1�������ƶ�ƽ��ֵ��EMA��
12��EMA����ʽΪ
EMA��12��=ǰһ��EMA��12����11/13+�������̼ۡ�2/13
26��EMA����ʽΪ
EMA��26��=ǰһ��EMA��26����25/27+�������̼ۡ�2/27
2���������ֵ��DIF��
DIF=����EMA��12��������EMA��26��
3������DIF��9��EMA
�������ֵ������9�յ�EMA�������ƽ��ֵ���������MACDֵ��Ϊ�˲���ָ��ԭ�����������ֵ����
DEA��DEM��
����DEA��MACD��=ǰһ��DEA��8/10+����DIF��2/10��
�������DIF��DEA����ֵ��Ϊ��ֵ��ֵ��
�ã�DIF-DEA����2��ΪMACD��״ͼ��
*/
namespace axapi
{
    /// RSIָ��ṹ
    struct MQI_MACDIndexField
    {
        /// ָ��ֵ
        double indexValue;
        /// ���յڼ���K��
        int BarSerials;
        /// EMA1
        double EMA1Value;
        /// EMA2
        double EMA2Value;
        /// DIF
        double DIFValue;
        /// DEA DEM
        double DEAValue;
    };

    class MQI_MACDIndex :
        public MarketQuotationIndex
    {
    public:
        MQI_MACDIndex();
        ~MQI_MACDIndex();
        int initialize(axapi::MarketQuotationAPI*, unsigned int, unsigned int, unsigned int, std::string);
        /// ���ָ��λ�õ�ָ��ֵ ���󷵻�NULL
        double getIndexValue(int in_iCurrentOffset = 0);
    private:
        int m_n1mKBars4EMA1;
        int m_n1mKBars4EMA2;
        int m_n1mKBars4DEA;
        /// ָ������
        std::vector<MQI_MACDIndexField> m_arrayIndexValue;
        /// ������� ���¶���ΪRSI
        void caculate();
    };
}

#endif _MQI_MACDINDEX_H_