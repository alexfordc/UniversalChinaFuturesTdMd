#ifndef _MARKETQUOTATIONINDEX_H_
#define _MARKETQUOTATIONINDEX_H_
#pragma once

#ifdef MARKETQUOTATIONINDEX_EXE
#define MARKETQUOTATIONINDEX_EXPORT
#else
#ifdef MARKETQUOTATIONINDEX_EXP
#define MARKETQUOTATIONINDEX_EXPORT __declspec(dllexport)
#else
#define MARKETQUOTATIONINDEX_EXPORT __declspec(dllimport)
#endif MARKETQUOTATIONINDEX_EXP
#endif MARKETQUOTATIONINDEX_EXE

#define _SCL_SECURE_NO_WARNINGS
#define CTP_TRADEAPI
#define KLINESTORAGE
#include <MarketQuotationAPI.h>
#include <string>
#include <vector>

namespace axapi
{
    class MARKETQUOTATIONINDEX_EXPORT MarketQuotationIndex
    {
    public:
        /// ���ָ��λ�õ�ָ��ֵ ���󷵻�NULL
        double getIndexValue(int in_iCurrentOffset = 0);
        /// ����������ݳ�ʼ�������ú�ʼ����ָ��ֵ
        int initialize(axapi::MarketQuotationAPI*, unsigned int, std::string);
        MarketQuotationIndex(void);
        ~MarketQuotationIndex(void);

    protected:
        /// �����������
        axapi::MarketQuotationAPI* m_pMarketQuotation;
        /// ��ǰָ���Լ
        std::string m_strContract;
        /// ָ����� һ��ָ��ʹ�ü���1m����KBar
        unsigned int m_n1mKBars;
        /// ָ������
        std::vector<double> m_arrayIndexValue;
        /// ������� �̳к����û��Լ�����
        void caculate();
        /// ����ֹͣ��־
        bool m_blAutoRun;
        HANDLE m_hCaculateRuning;
    };
}

#endif _MARKETQUOTATIONINDEX_H_