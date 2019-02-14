#ifndef _Strategy_MaxLose_
#define _Strategy_MaxLose_
#pragma once

#include "StrategyModel.h"

namespace axapi
{
    // ֹ�����,����Լ
    class StrategyMaxLose_OneContract :
        public StrategyModel
    {
    public:
        StrategyMaxLose_OneContract();
        ~StrategyMaxLose_OneContract();

        // ����µ������Ƿ�ȫ���ṩ
        bool check(std::vector<std::string> in_parameterList);
        // �µ�����
        bool order(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI, std::vector<std::string> in_parameterList);
        // ���̲���
        bool closeMarket(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI);
        // ƽ�ֲ���
        bool offset(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI);
        // ��������
        bool cancel(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI);
    };
}

#endif //_Strategy_MaxLose_
