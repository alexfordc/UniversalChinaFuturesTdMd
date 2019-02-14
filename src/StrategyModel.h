#ifndef _STRATEGY_MODEL_
#define _STRATEGY_MODEL_
#pragma once

#include <string>
#include <vector>
#include <../src/TradeAPI.h>
#include <../src/MarketQuotationAPI.h>

namespace axapi
{
    // ���Խӿ�
    class StrategyModel
    {
    public:
        StrategyModel() {};
        ~StrategyModel() {};

        // ����µ������Ƿ�ȫ���ṩ
        virtual bool check(std::vector<std::string> in_parameterList) = 0;
        // �µ�����
        virtual bool order(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI, std::vector<std::string> in_parameterList) = 0;
        // ���̲���
        virtual bool closeMarket(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI) = 0;
        // ƽ�ֲ���
        virtual bool offset(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI) = 0;
        // ��������
        virtual bool cancel(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI) = 0;
    };
}
#endif //_STRATEGY_MODEL_