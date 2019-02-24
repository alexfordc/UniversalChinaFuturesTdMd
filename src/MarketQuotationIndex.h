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

#define CTP_TRADEAPI
#define KLINESTORAGE
#include <MarketQuotationAPI.h>
#include <vector>

namespace axapi
{
    class MarketQuotationIndex
    {
    public:

        /// index�������
        void caculate();
        double getIndexValue();
        /// �����������
        MarketQuotationIndex(axapi::MarketQuotationAPI*, int);
        void initialize(axapi::MarketQuotationAPI*, int);
        MarketQuotationIndex(void);
        ~MarketQuotationIndex(void);

    private:
        /// �����������
        axapi::MarketQuotationAPI* m_pMarketQuotation;
        /// 1����K�߼����ָ�� m_array1mIndexÿ����Ա��¼��ͬ��Լindex���ݵ���ʼֵ��ַ
        std::vector<double*> m_array1mIndex;
        /// ��Լ����
        std::vector<char*> m_arrayContracts;
        /// ָ�����
        int m_n1mKBars;
        /// ָ��ֵ
        double m_nIndexValue;

    };
}

#endif _RELATIVESTRENGTHINDEX_H_