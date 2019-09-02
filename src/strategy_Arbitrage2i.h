#ifndef _STRATEGY_ARBITRAGE2I_H_
#define _STRATEGY_ARBITRAGE2I_H_
#pragma once

#include <Strategy.h>

struct Arbitrage2i_Combination_Plan
{
    axapi::UniversalChinaFutureTdInstrumentIDType Instrument_left;
    axapi::UniversalChinaFutureTdOrderRefType     OrderRef_left;
    std::string                                   CombinationRef_left;
    axapi::UniversalChinaFutureTdInstrumentIDType Instrument_right;
    axapi::UniversalChinaFutureTdOrderRefType     OrderRef_right;
    std::string                                   CombinationRef_right;
    /// ��ƽ�ּ۲�,PriceDiff=instrument_left.currentprice * ordervolumn_left - instrument_right.currentprice * ordervolumn_right
    axapi::UniversalChinaFutureTdPriceType        OpenPriceDiff_Min;
    axapi::UniversalChinaFutureTdPriceType        OpenPriceDiff_Max;
    axapi::UniversalChinaFutureTdPriceType        OffsetPriceDiff_Min;
    axapi::UniversalChinaFutureTdPriceType        OffsetPriceDiff_Max;
};

struct Arbitrage2i_Combination_Paired
{
    axapi::UniversalChinaFutureTdInstrumentIDType Instrument_left;
    axapi::UniversalChinaFutureTdTradeIDType      TradeID_left;
    axapi::UniversalChinaFutureTdOrderRefType     OrderRef_left;
    std::string                                   CombinationRef_left;
    axapi::UniversalChinaFutureTdInstrumentIDType Instrument_right;
    axapi::UniversalChinaFutureTdTradeIDType      TradeID_right;
    axapi::UniversalChinaFutureTdOrderRefType     OrderRef_right;
    std::string                                   CombinationRef_right;
    /// ��ƽ�ּ۲�,PriceDiff=instrument_left.currentprice * ordervolumn_left - instrument_right.currentprice * ordervolumn_right
    axapi::UniversalChinaFutureTdPriceType        OpenPriceDiff_Min;
    axapi::UniversalChinaFutureTdPriceType        OpenPriceDiff_Max;
    axapi::UniversalChinaFutureTdPriceType        OffsetPriceDiff_Min;
    axapi::UniversalChinaFutureTdPriceType        OffsetPriceDiff_Max;
};

class strategy_Arbitrage2i
    : public axapi::Strategy
{
private:
    /// ���ڼ�¼��Ҫ�µ����������
    std::vector<Arbitrage2i_Combination_Plan> m_vCombinationPlan;
    /// ���ڼ�¼�����(�µ��ɹ�)���������
    std::vector<Arbitrage2i_Combination_Paired> m_vCombinationPaired;
    /// ���ڼ�¼��Լ��Ӧ����Ϣ����������к�
    std::hash_map<std::string/*axapi::UniversalChinaFutureTdInstrumentIDType*/, axapi::UniversalChinaFutureTdSequenceType> m_hashInstruments;
    /// ���ڼ�¼�����õ������к�Լ�ĵ�ǰ����
    std::vector<axapi::MarketDataField*> m_vInstrumentsCurrentPrice;
    /// ���ڼ�¼�����õ������к�Լ�ĺ�Լ��Ϣ
    std::vector<APINamespace CThostFtdcInstrumentField> m_vInstrumentsInfo;

    /// ������־��ʼ��
    int initializeSubLog();
protected:
    /// TODO:��������
    void myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice);
    /// TODO:ƽ������
    void myOffsetStrategy(struct axapi::ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg);
    /// TODO:Ԥ�񵥼�λ���
    void getPreOffsetPrice(struct axapi::ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice);
    /// TODO:������������
    void myCancelStrategy(struct ConfirmedOrder in_objOrder, bool *ot_blCancelFlag, std::string *ot_strCancelMsg);
public:
    strategy_Arbitrage2i(std::vector<std::string> in_vInstruments , unsigned int in_nCancelWaitSeconds, unsigned int in_nOrderingCheckWaitMillseconds);
    ~strategy_Arbitrage2i();
    /// ��ʼ�������뽻�׽ӿ�
    int initializeAPISub(axapi::MarketQuotationAPI*, axapi::TradeAPI*);
};

#endif _STRATEGY_ARBITRAGE2I_H_