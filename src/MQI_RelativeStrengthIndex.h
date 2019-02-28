#ifndef _MQI_RELATIVESTRENGTHINDEX_H_
#define _MQI_RELATIVESTRENGTHINDEX_H_
#pragma once

#ifdef MQI_RELATIVESTRENGTHINDEX_EXE
#define MQI_RELATIVESTRENGTHINDEX_EXPORT
#else
#ifdef MQI_RELATIVESTRENGTHINDEX_EXP
#define MQI_RELATIVESTRENGTHINDEX_EXPORT __declspec(dllexport)
#else
#define MQI_RELATIVESTRENGTHINDEX_EXPORT __declspec(dllimport)
#endif MQI_RELATIVESTRENGTHINDEX_EXP
#endif MQI_RELATIVESTRENGTHINDEX_EXE

#include "MarketQuotationIndex.h"

namespace axapi
{
    /// RSIָ��ṹ
    struct MQI_RelativeStrengthIndexField
    {
        /// ָ��ֵ
        double indexValue;
        /// ���յڼ���K��
        int BarSerials;
    };

    class MQI_RelativeStrengthIndex :
        public MarketQuotationIndex
    {
    public:
        MQI_RelativeStrengthIndex();
        ~MQI_RelativeStrengthIndex();
        /// ���ָ��λ�õ�ָ��ֵ ���󷵻�NULL
        double getIndexValue(int in_iCurrentOffset = 0);
    private:
        /// ָ������
        std::vector<MQI_RelativeStrengthIndexField> m_arrayIndexValue;
        /// ������� ���¶���ΪRSI
        void caculate();
    };
}

#endif _MQI_RELATIVESTRENGTHINDEX_H_