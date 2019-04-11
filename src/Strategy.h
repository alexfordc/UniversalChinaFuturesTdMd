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
#define MEMORYDATA
#define KLINESTORAGE

#include <MarketQuotationAPI.h>
#include <TradeAPI.h>
#include <TradeAPITypeDefine.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/sleep.h>
#include <log4cplus/loggingmacros.h>
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
        std::vector<std::string/*UniversalChinaFutureTdTradeIDType*/> TradeIDList;
    };

    /// ����ر�ȷ�Ϲ��Ŀ��ֳɽ�,������Ϊ�����гֲ���Ϣ��ѯ
    struct ConfirmedHoldTrade
    {
        UniversalChinaFutureTdTradeIDType              TradeID;
        UniversalChinaFutureTdOrderIDType              SPOrderID;
        std::vector<std::string/*UniversalChinaFutureTdOrderIDType*/> SLOrderID;
        UniversalChinaFutureTdTradeStatusType          TradeStatus;
        UniversalChinaFutureTdInstrumentIDType         InstrumentID;
        UniversalChinaFutureTdDirectionType            Direction;
        UniversalChinaFutureTdVolumnType               Volumn;
        UniversalChinaFutureTdPriceType                Price;
        UniversalChinaFutureTdTimeType                 TradeTime;
        UniversalChinaFutureTdVolumnType               AvailableVolumn;
        UniversalChinaFutureTdVolumnType               OffsetVolumn;
        UniversalChinaFutureTdPriceType                HighestProfitPrice;
    };

    /// ƽ�ֺ��¼,δ��ֲ���Ϣƥ���ƽ����Ϣ
    struct UnpairedOffsetOrder
    {
        UniversalChinaFutureTdTradeIDType         TradeID;
        UniversalChinaFutureTdOrderRefType        OrderRef;
        UniversalChinaFutureTdOffsetOrderTypeType OffsetOrderType;
    };

    /// ���Խӿ�
    class STRATEGY_EXPORT Strategy
    {
        /*
        * ������ʼ��
        */
#pragma region
    private:
        /// ������־,��ʼ����������
        log4cplus::Logger m_root;
        /// ������־,��ʼ����������
        log4cplus::Logger m_objLogger;
        /// ��ʼ����־�ļ�
        int initializeLog();
    protected:
        /// ��׼����ӿ�
        axapi::MarketQuotationAPI *m_pMarketQuotation;
        /// ��׼���׽ӿ�
        axapi::TradeAPI *m_pTrade;
        /// ���ýӿ�
        int setAPI(axapi::MarketQuotationAPI*, axapi::TradeAPI*);
#ifndef STRATEGY_EXE
        /// ���ڼ̳�����־,��ʼ����������
        log4cplus::Logger m_objLoggerSub;
        /// TODO:��ʼ����־�ļ� { m_objLoggerSub = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("SUBStrategyNAME")); }
        virtual int initializeSubLog() = 0;
#endif STRATEGY_EXE
    public:
        /// ��ʼ�������뽻�׽ӿ�
        int initializeAPI(axapi::MarketQuotationAPI*, axapi::TradeAPI*);
        /// ��ʼ��
        Strategy(void);
        ~Strategy(void);
#pragma endregion

        /*
        * ���п���
        */
#pragma region
    private:
        /// �������п���
        bool m_blStrategyRunning;
        /// ������п���
        bool m_blOffsetALLRunning;
        /// ƽ�ֲ������п���
        bool m_blOffsetRunning;
        /// ���ֲ������п���
        bool m_blOpenRunning;
        /// �������п���
        bool m_blCancelRunning;
        /// �˳�����
        bool m_blShutdownFlag;
        /// ���ֽ��̽�������
        HANDLE m_hOpenFinished;
        /// ƽ�ֽ��̽�������
        HANDLE m_hOffsetFinished;
        /// �������̽�������
        HANDLE m_hCancelFinished;
        /// ��ֽ��̽�������
        HANDLE m_hOffsetAllFinished;
        /// ί�и��½��̽�������
        HANDLE m_hUpdateOrderFinished;
        /// �ɽ����½��̽�������
        HANDLE m_hUpdateTradeFinished;
        /// ����ִ�п�ʼ
        void startStrategy();
        /// ����ִ��ֹͣ
        void stopStrategy();
        /// ����ִ����ͣ
        void pauseStrategy() {};
        /// ����ִ������
        void continueStrategy() {};
    protected:
        /// �������״̬
        bool getOpenRunning();
        bool getOffsetRunning();
    public:
        /// ����ִ��״̬�л�
        void start() { startStrategy(); };
        void stop() { stopStrategy(); };
        void pause() { pauseStrategy(); };
        void continu() { continueStrategy(); };
#pragma endregion

        /*
        * ����
        */
#pragma region
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

        /// ί�и��´���������
        unsigned int m_nUpdateOrderTimes;
        /// �ɽ����´���������
        unsigned int m_nUpdateTradeTimes;

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
        /// ���ش�����������
        void loadData();
    protected:
        /// ���Բ���:�����ȴ�����
        unsigned int m_nCancelWaitSeconds;
#ifdef STRATEGY_EXE
        int m_nOpenCount;
        /// TODO:��������
        void myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice);
        /// TODO:ƽ������
        void myOffsetStrategy(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg);
        /// TODO:Ԥ�񵥼�λ���
        void getPreOffsetPrice(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice);
#endif STRATEGY_EXE
#ifndef STRATEGY_EXE
        /// TODO:��������
        virtual void myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice) = 0;
        /// TODO:ƽ������
        virtual void myOffsetStrategy(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg) = 0;
        /// TODO:Ԥ�񵥼�λ���
        virtual void getPreOffsetPrice(struct ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice) = 0;
#endif STRATEGY_EXE
    public:
#pragma endregion

    };
}

#endif _STRATEGY_H_