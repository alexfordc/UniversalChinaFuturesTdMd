#include "StrategyMaxLose.h"

axapi::StrategyMaxLose_OneContract::StrategyMaxLose_OneContract()
{
}

axapi::StrategyMaxLose_OneContract::~StrategyMaxLose_OneContract()
{
}

// ����µ������Ƿ�ȫ���ṩ
bool axapi::StrategyMaxLose_OneContract::check(std::vector<std::string> in_parameterList)
{
    return true;
}

// �µ�����
bool axapi::StrategyMaxLose_OneContract::order(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI, std::vector<std::string> in_parameterList)
{
    /*TODO:ʹ��API�ӿ��µ�MyOrdering*/
    return true;
}

// ���̲���
bool axapi::StrategyMaxLose_OneContract::closeMarket(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI)
{
    /*TODO:���ﵽ����������������̲���*/
    return true;
}

// ƽ�ֲ���
bool axapi::StrategyMaxLose_OneContract::offset(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI)
{
    /*TODO:ʹ��API�ӿ�ƽ��*/
    return true;
}

// ��������
bool axapi::StrategyMaxLose_OneContract::cancel(axapi::TradeAPI *in_tradeAPI, axapi::MarketQuotationAPI *in_marketquotationAPI)
{
    /*TODO:ʹ��API�ӿڳ���*/
    return true;
}
