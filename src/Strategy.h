#ifndef _STRATEGY_H_
#define _STRATEGY_H_
#pragma once

#ifdef STRATEGY_EXE
#define STRATEGY_EXPORT
#else
#ifdef STRATEGY_EXP
#define STRATEGY_EXPORT __declspec(dllexport)
#else
#define STRATEGY_EXPORT __declspec(dllimport)
#endif STRATEGY_EXP
#endif STRATEGY_EXE

#define _SCL_SECURE_NO_WARNINGS

#include <MarketQuotationAPI.h>
#include <TradeAPI.h>
#include <string>
#include <vector>
#include <hash_map>

namespace axapi
{
    typedef APINamespace TThostFtdcOrderRefType     UniversalChinaFutureTdOrderRefType;
    typedef APINamespace TThostFtdcOrderSysIDType   UniversalChinaFutureTdOrderIDType;
    typedef APINamespace TThostFtdcOrderStatusType  UniversalChinaFutureTdOrderStatusType;
    typedef char                                    UniversalChinaFutureTdOrderTypeType;
    typedef char                                    UniversalChinaFutureTdOffsetOrderTypeType;
    typedef APINamespace TThostFtdcTradeIDType      UniversalChinaFutureTdTradeIDType;
    typedef char                                    UniversalChinaFutureTdTradeStatusType;
    typedef unsigned int                            UniversalChinaFutureTdSequenceType;
    typedef unsigned int                            UniversalChinaFutureTdVolumnType;
    typedef double                                  UniversalChinaFutureTdPriceType;
    typedef APINamespace TThostFtdcInstrumentIDType UniversalChinaFutureTdInstrumentIDType;
    typedef APINamespace TThostFtdcDirectionType    UniversalChinaFutureTdDirectionType;
    typedef APINamespace TThostFtdcTimeType         UniversalChinaFutureTdTimeType;

    //---------------------------------------------------------------
    //OrderStatus Definition
    // ����
#define OrderStatus_Open 'a'
    // ֹӯƽ��
#define OrderStatus_SPOffset 'b'
    // ֹ��ƽ��
#define OrderStatus_SLOffset 'c'
    //---------------------------------------------------------------
    //TradeStatus Definition
    // ����
#define TradeStatus_Hold 'a'
    // ��ƽ
#define TradeStatus_OffsetALL 'b'
    //---------------------------------------------------------------
    //UnpairedOffsetOrder Definition
    // ֹӯ
#define OrderOffsetType_SP 'a'
    // ֹ��
#define OrderOffsetType_SL 'b'
    //---------------------------------------------------------------

    /// δ��ر�ȷ�Ϲ���ί����Ϣ,�Ӳ����з����ı���ί�ж��м�¼
    struct AllOrder
    {
        UniversalChinaFutureTdOrderRefType  OrderRef;
        UniversalChinaFutureTdSequenceType  updateOrderRoundSequence;
        UniversalChinaFutureTdOrderIDType   OrderID;
        UniversalChinaFutureTdOrderTypeType OrderType;
        /// ���ί��Ϊƽ��,���ʶ��Ӧ�ĳɽ�ID
        UniversalChinaFutureTdTradeIDType   HoldTradeID;
    };

    /// ����ر�ȷ�Ϲ���ί����Ϣ,������Ϊ������ί����Ϣ��ѯ
    struct ConfirmedOrder
    {
        UniversalChinaFutureTdOrderRefType    OrderRef;
        UniversalChinaFutureTdOrderIDType     OrderID;
        UniversalChinaFutureTdOrderTypeType   OrderType;
        UniversalChinaFutureTdOrderStatusType OrderStatus;
        UniversalChinaFutureTdPriceType       OrderPrice;
        UniversalChinaFutureTdVolumnType      OrderVolumnOriginal;
        UniversalChinaFutureTdVolumnType      OrderVolumeTraded;
        UniversalChinaFutureTdVolumnType      OrderVolumeTotal;
        UniversalChinaFutureTdTimeType        InsertTime;
        /// ���ί��Ϊƽ��,���ʶ��Ӧ�ĳɽ�ID
        UniversalChinaFutureTdTradeIDType     HoldTradeID;
        std::vector<UniversalChinaFutureTdTradeIDType> TradeIDList;
    };

    /// ����ر�ȷ�Ϲ��Ŀ��ֳɽ�,������Ϊ�����гֲ���Ϣ��ѯ
    struct ConfirmedHoldTrade
    {
        UniversalChinaFutureTdTradeIDType              TradeID;
        UniversalChinaFutureTdOrderIDType              SPOrderID;
        std::vector<UniversalChinaFutureTdOrderIDType> SLOrderID;
        UniversalChinaFutureTdTradeStatusType          TradeStatus;
        UniversalChinaFutureTdInstrumentIDType         InstrumentID;
        UniversalChinaFutureTdDirectionType            Direction;
        UniversalChinaFutureTdVolumnType               Volumn;
        UniversalChinaFutureTdPriceType                Price;
        UniversalChinaFutureTdVolumnType               AvailableVolumn;
        UniversalChinaFutureTdVolumnType               OffsetVolumn;
    };

    /// ƽ�ֺ��¼,δ��ֲ���Ϣƥ���ƽ����Ϣ
    struct UnpairedOffsetOrder
    {
        UniversalChinaFutureTdTradeIDType         TradeID;
        UniversalChinaFutureTdOrderRefType        OrderRef;
        UniversalChinaFutureTdOffsetOrderTypeType OffsetOrderType;
    };

    ///

    class STRATEGY_EXPORT Strategy
    {
    public:
        /// ��ʼ�������뽻�׽ӿ�
        int initializeAPI(axapi::MarketQuotationAPI*, axapi::TradeAPI*);
        /// ����ִ��״̬�л�
        void start() { startStrategy(); };
        void stop() { stopStrategy(); };
        void pause() { pauseStrategy(); };
        void continu() { continueStrategy(); };
        /// ��ʼ��
        Strategy(void);
        ~Strategy(void);

    protected:
        /// �����������
        axapi::MarketQuotationAPI *m_pMarketQuotation;
        axapi::TradeAPI *m_pTrade;
        /// ���ýӿ�
        int setAPI(axapi::MarketQuotationAPI*, axapi::TradeAPI*);
        /// �������״̬
        bool getOpenRunning();
        bool getOffsetRunning();

        /// TODO:��������
        void myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice);
        /// TODO:ƽ������
        void myOffsetStrategy(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg);
        /// TODO:Ԥ�񵥼�λ���
        void getPreOffsetPrice(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice);

    private:
        /// ��¼�ѱ�����ί��,����ʧ�ܵ�Ҳ������
        std::hash_map<std::string/*UniversalChinaFutureTdOrderRefType*/, struct AllOrder> m_hashAllOrder;
        /// ��¼��ȷ���ѱ����ί��,m_hashConfirmedOrder����m_hashAllOrder���Ӽ�,m_hashConfirmedOrderΪm_vConfirmedOrder������
        std::hash_map<std::string/*UniversalChinaFutureTdOrderIDType*/, UniversalChinaFutureTdSequenceType>  m_hashConfirmedOrder;
        /// ��¼��ȷ���ѱ����ί��
        std::vector<struct ConfirmedOrder> m_vConfirmedOrder;
        /// ��¼���ֵĳɽ�,��Ϊ�ֲ���Ϣ��ʹ��,m_hashConfirmedHoldTradeΪm_vConfirmedHoldTrade������
        std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, UniversalChinaFutureTdSequenceType> m_hashConfirmedHoldTrade;
        /// ��¼���ֵĳɽ�,��Ϊ�ֲ���Ϣ��ʹ��
        std::vector<struct ConfirmedHoldTrade> m_vConfirmedHoldTrade;
        /// ��¼δ��ֲ���Ϣƥ���ƽ����Ϣ,��ʱ��תʹ��,��ȷ�Ϻ�ɾ��
        std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, struct UnpairedOffsetOrder> m_hashUnpairedOffsetOrder;

        /// ִ��״̬
        bool m_blStrategyRunning;
        bool m_blOffsetALLRunning;
        bool m_blOffsetRunning;
        bool m_blOpenRunning;
        bool m_blCancelRunning;
        bool m_blShutdownFlag;
        HANDLE m_hOpenFinished;
        HANDLE m_hOffsetFinished;
        HANDLE m_hCancelFinished;
        HANDLE m_hOffsetAllFinished;
        HANDLE m_hUpdateOrderFinished;
        HANDLE m_hUpdateTradeFinished;

        /// ���Բ���
        unsigned int m_nCancelWaitSeconds;

        /// ί�и��´���
        unsigned int m_nUpdateOrderTimes;
        /// �ɽ����´���
        unsigned int m_nUpdateTradeTimes;

        /// ����ִ�п�ʼ
        void startStrategy();
        /// ����ִ��ֹͣ
        void stopStrategy();
        /// ����ִ����ͣ
        void pauseStrategy() {};
        /// ����ִ������
        void continueStrategy() {};

        /// ����ί�лر�
        void updateOrderInfo();
        /// �����ɽ��ر�
        void updateTradeInfo();
        /// ����ƽ��ί��������³ֲ����
        void updateHoldTrade(UniversalChinaFutureTdSequenceType);
        /// �µ�����,�����Զ������
        void strategyOrder();
        /// ƽ�ֽ���,�����Զ���ƽ�ֲ���,�Լ�ִ��Ĭ�ϵĲ���
        void strategyOffset();
        /// ��ֽ���
        void strategyOffsetALL();
        /// ��������
        void strategyCancelOrder();
        /// �ֱֲȽ�
        bool strategyHoldCompare();
        /// �������ݱ���
        void saveData();

    };
}

#endif _STRATEGY_H_