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

namespace axapi
{
    /// RSIָ��ṹ
    struct MQI_KDJRandomIndexField
    {
        /// ָ��ֵ
        double indexValue;
        /// ���յڼ���K��
        int BarSerials;
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