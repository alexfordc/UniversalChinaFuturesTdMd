#ifndef _MQI_KDJRANDOMINDEX_H_
#define _MQI_KDJRANDOMINDEX_H_
#pragma once

#ifdef MQI_KDJRANDOMINDEX_EXE
#define MQI_KDJRANDOMINDEX_EXPORT
#else
#ifdef MQI_KDJRANDOMINDEX_EXP
#define MQI_KDJRANDOMINDEX_EXPORT __declspec(dllexport)
#else
#define MQI_KDJRANDOMINDEX_EXPORT __declspec(dllimport)
#endif MQI_KDJRANDOMINDEX_EXP
#endif MQI_KDJRANDOMINDEX_EXE

#include "MarketQuotationIndex.h"

/*
* KDJ�ļ���Ƚϸ��ӣ�����Ҫ�������ڣ�n�ա�n�ܵȣ���RSVֵ����δ�������ָ��ֵ��Ȼ���ټ���Kֵ��Dֵ��Jֵ�ȡ���n��KDJ��ֵ�ļ���Ϊ��������㹫ʽΪ
n��RSV=��Cn��Ln��/��Hn��Ln����100
��ʽ�У�CnΪ��n�����̼ۣ�LnΪn���ڵ���ͼۣ�HnΪn���ڵ���߼ۡ�
��Σ�����Kֵ��Dֵ��
����Kֵ=2/3��ǰһ��Kֵ+1/3������RSV
����Dֵ=2/3��ǰһ��Dֵ+1/3������Kֵ
����ǰһ��K ֵ��Dֵ����ɷֱ���50�����档
Jֵ=3*����Kֵ-2*����Dֵ
��9��Ϊ���ڵ�KD��Ϊ������δ�������ֵ�����㹫ʽΪ
9��RSV=��C��L9���£�H9��L9����100
��ʽ�У�CΪ��9�յ����̼ۣ�L9Ϊ9���ڵ���ͼۣ�H9Ϊ9���ڵ���߼ۡ�
Kֵ=2/3����8��Kֵ+1/3����9��RSV
Dֵ=2/3����8��Dֵ+1/3����9��Kֵ
Jֵ=3*��9��Kֵ-2*��9��Dֵ
����ǰһ��K
ֵ��Dֵ������Էֱ���50���档
*/

namespace axapi
{
    /// RSIָ��ṹ
    struct MQI_KDJRandomIndexField
    {
        /// ָ��ֵ
        double indexValue;
        /// ���յڼ���K��
        int BarSerials;
        /// RSVֵ
        int RSVValue;
        /// K
        double KValue;
        /// D
        double DValue;
        /// J
        double JValue;
    };

    class MQI_KDJRandomIndex :
        public MarketQuotationIndex
    {
    public:
        MQI_KDJRandomIndex();
        ~MQI_KDJRandomIndex();
        /// ���ָ��λ�õ�ָ��ֵ ���󷵻�NULL
        double getIndexValue(int in_iCurrentOffset = 0);
    private:
        /// ָ������
        std::vector<MQI_KDJRandomIndexField> m_arrayIndexValue;
        /// ������� ���¶���ΪRSI
        void caculate();
    };
}

#endif _MQI_KDJRANDOMINDEX_H_