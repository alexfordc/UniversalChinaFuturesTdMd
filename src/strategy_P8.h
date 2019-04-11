#ifndef _STRATEGY_P8_H_
#define _STRATEGY_P8_H_
#pragma once

#include <Strategy.h>

class strategy_P8 :
    public axapi::Strategy
{
private:
    axapi::MarketDataField m_objFormerPrice1, m_objFormerPrice2;
    APINamespace CThostFtdcInstrumentField m_objInstrumentInfo;
    axapi::MarketDataField *m_pCurrentPrice;
    axapi::UniversalChinaFutureTdInstrumentIDType m_chInstrument;

    /// �����µ�����
    char m_chStrategy_PPP[5];
    char m_chStrategy_PPN[5];
    char m_chStrategy_PNP[5];
    char m_chStrategy_PNN[5];
    char m_chStrategy_NPP[5];
    char m_chStrategy_NPN[5];
    char m_chStrategy_NNP[5];
    char m_chStrategy_NNN[5];
    int m_nOffsetInterval;
    /// ֹ���λ
    int m_nOffsetPriceDiff;
    /// ֹӯ��λ
    int m_nOffsetPriceDiff2;
    /// �س�ǿƽ���Է�ֵ
    int m_nProfitFallOffsetValve;
    /// �س�ǿƽ�����������س���
    double m_dbProfitFallRate;

    /// ������־��ʼ��
    int initializeSubLog();

protected:
    /// TODO:��������
    void myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice);
    /// TODO:ƽ������
    void myOffsetStrategy(struct axapi::ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg);
    /// TODO:Ԥ�񵥼�λ���
    void getPreOffsetPrice(struct axapi::ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice);
public:
    strategy_P8(char *in_chInstrument, int in_nCancelWaitSeconds, char *in_chPPPDirection, char *in_chPPNDirection, char *in_chPNPDirection, char *in_chPNNDirection, char *in_chNPPDirection, char *in_chNPNDirection, char *in_chNNPDirection, char *in_chNNNDirection, int in_nOffsetInterval = INT_MAX, int in_nOffsetPriceDiff = INT_MAX, int in_nOffsetPriceDiff2 = INT_MAX, int in_nProfitFallOffsetValve = INT_MAX, double in_dbProfitFallRate = 1);
    ~strategy_P8();
};

#endif _STRATEGY_P8_H_
