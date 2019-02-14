#ifndef _RELATIVESTRENGTHINDEX_H_
#define _RELATIVESTRENGTHINDEX_H_
#pragma once

#ifdef RELATIVESTRENGTHINDEX_EXE
#define RELATIVESTRENGTHINDEX_EXPORT
#else
#ifdef RELATIVESTRENGTHINDEX_EXP
#define RELATIVESTRENGTHINDEX_EXPORT __declspec(dllexport)
#else
#define RELATIVESTRENGTHINDEX_EXPORT __declspec(dllimport)
#endif RELATIVESTRENGTHINDEX_EXP
#endif RELATIVESTRENGTHINDEX_EXE

//#define MarketQuotationAPI_EXP
#define CTP_TRADEAPI
#define KLINESTORAGE
#include <MarketQuotationAPI.h>
//#include <KSV6T_API_common/KSMarketDataAPI/KSMarketDataAPI.h>

namespace axapi
{
    class RELATIVESTRENGTHINDEX_EXPORT RelativeStrengthIndex
    {
    private:
        /// �����������
        MarketQuotationAPI* m_pMarketQuotation;
        /// 1����K�߼����ָ�� m_array1mIndexÿ����Ա��¼��ͬ��Լindex���ݵ���ʼֵ��ַ
        std::vector<double*> m_array1mIndex;
        /// ��Լ����
        std::vector<char*> m_arrayContracts;
        /// ָ�����
        int m_n1mKBars;
        /// ָ��ֵ
        double m_nIndexValue;
    public:
        /// index�������
        void caculate();
        RelativeStrengthIndex(void);
        ~RelativeStrengthIndex(void);
        /// �����������
        RelativeStrengthIndex(MarketQuotationAPI*, int);
        double getIndexValue();
    };
}

#endif _RELATIVESTRENGTHINDEX_H_