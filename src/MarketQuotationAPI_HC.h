#ifndef _MARKETQUOTATIONAPI_HC_H_
#define _MARKETQUOTATIONAPI_HC_H_
#pragma once

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#ifdef MarketQuotationAPI_HC_EXE
#define MarketQuotationAPI_HC_EXPORT
#else
#ifdef MarketQuotationAPI_HC_EXP
#define MarketQuotationAPI_HC_EXPORT __declspec(dllexport)
#else
#define MarketQuotationAPI_HC_EXPORT __declspec(dllimport)
#endif MarketQuotationAPI_HC_EXP
#endif MarketQuotationAPI_HC_EXE

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/sleep.h>
#include <log4cplus/loggingmacros.h>
#include <Windows.h>
#include <time.h>
#include <vector>
#include <hash_map>
#include <string>
#ifdef DBDataSource
#include <otlv4.h>
#endif DBDataSource

#include <CTPTdMd_API_common/ThostFtdcMdApi.h>
#include "MarketQuotationAPI_HC_Defination.h"
#include "ClockSimulated.h"
#define  APINamespace

#ifdef HQDATAFILE
#define DATAFILE
#endif HQDATAFILE
#ifdef KDATAFILE
#define DATAFILE
#endif KDATAFILE

#ifndef WAITFORCLOSEFILE
#define WAITFORCLOSEFILE 2000
#endif  WAITFORCLOSEFILE

namespace axapi
{
    typedef int APIStatus;
#define APISTATUS_DISCONNECTED  9999
#define APISTATUS_CONNECTED     1000

    struct Contract
    {
        APINamespace TThostFtdcInstrumentIDType InstrumentID;
    };

    struct MarketDataField
    {
        ///������
        APINamespace TThostFtdcDateType	TradingDay;
        ///��Լ����
        APINamespace TThostFtdcInstrumentIDType	InstrumentID;
        ///����������
        APINamespace TThostFtdcExchangeIDType	ExchangeID;
        ///��Լ�ڽ������Ĵ���
        APINamespace TThostFtdcExchangeInstIDType	ExchangeInstID;
        ///���¼�
        APINamespace TThostFtdcPriceType	LastPrice;
        ///�ϴν����
        APINamespace TThostFtdcPriceType	PreSettlementPrice;
        ///������
        APINamespace TThostFtdcPriceType	PreClosePrice;
        ///��ֲ���
        APINamespace TThostFtdcLargeVolumeType	PreOpenInterest;
        ///����
        APINamespace TThostFtdcPriceType	OpenPrice;
        ///��߼�
        APINamespace TThostFtdcPriceType	HighestPrice;
        ///��ͼ�
        APINamespace TThostFtdcPriceType	LowestPrice;
        ///����
        APINamespace TThostFtdcVolumeType	Volume;
        ///�ɽ����
        APINamespace TThostFtdcMoneyType	Turnover;
        ///�ֲ���
        APINamespace TThostFtdcLargeVolumeType	OpenInterest;
        ///������
        APINamespace TThostFtdcPriceType	ClosePrice;
        ///���ν����
        APINamespace TThostFtdcPriceType	SettlementPrice;
        ///��ͣ���
        APINamespace TThostFtdcPriceType	UpperLimitPrice;
        ///��ͣ���
        APINamespace TThostFtdcPriceType	LowerLimitPrice;
        ///����ʵ��
        APINamespace TThostFtdcRatioType	PreDelta;
        ///����ʵ��
        APINamespace TThostFtdcRatioType	CurrDelta;
        ///����޸�ʱ��
        APINamespace TThostFtdcTimeType	UpdateTime;
        ///����޸ĺ���
        APINamespace TThostFtdcMillisecType	UpdateMillisec;
        ///�����һ
        APINamespace TThostFtdcPriceType	BidPrice1;
        ///������һ
        APINamespace TThostFtdcVolumeType	BidVolume1;
        ///������һ
        APINamespace TThostFtdcPriceType	AskPrice1;
        ///������һ
        APINamespace TThostFtdcVolumeType	AskVolume1;
        ///����۶�
        APINamespace TThostFtdcPriceType	BidPrice2;
        ///��������
        APINamespace TThostFtdcVolumeType	BidVolume2;
        ///�����۶�
        APINamespace TThostFtdcPriceType	AskPrice2;
        ///��������
        APINamespace TThostFtdcVolumeType	AskVolume2;
        ///�������
        APINamespace TThostFtdcPriceType	BidPrice3;
        ///��������
        APINamespace TThostFtdcVolumeType	BidVolume3;
        ///��������
        APINamespace TThostFtdcPriceType	AskPrice3;
        ///��������
        APINamespace TThostFtdcVolumeType	AskVolume3;
        ///�������
        APINamespace TThostFtdcPriceType	BidPrice4;
        ///��������
        APINamespace TThostFtdcVolumeType BidVolume4;
        ///��������
        APINamespace TThostFtdcPriceType	 AskPrice4;
        ///��������
        APINamespace TThostFtdcVolumeType	AskVolume4;
        ///�������
        APINamespace TThostFtdcPriceType	BidPrice5;
        ///��������
        APINamespace TThostFtdcVolumeType	BidVolume5;
        ///��������
        APINamespace TThostFtdcPriceType	AskPrice5;
        ///��������
        APINamespace TThostFtdcVolumeType	AskVolume5;
        ///���վ���
        APINamespace TThostFtdcPriceType	AveragePrice;
        ///ҵ������
        APINamespace TThostFtdcDateType	ActionDay;
    };

    /// K�ߴ洢���ݽṹ
    struct KMarketDataField
    {
        APINamespace TThostFtdcInstrumentIDType Contract;
        APINamespace TThostFtdcDateType TradingDay;
        /// K�����ڱ�ʶ60s=1m
        int SecondsPeriod;
        /// ���յڼ���K��
        int BarSerials;
        /// ��
        double OpenPrice;
        /// ��
        double ClosePrice;
        /// ��
        double HighestPrice;
        /// ��
        double LowestPrice;
        /// �ɽ���
        double Volume;
        /// �ɽ���
        double Turnover;
        long begintime;
        long endtime;
    };

    /// ����ʱ����
    struct TradingMinuteField
    {
        /// ����������
        int  CurrentMinute;
        APINamespace TThostFtdcDateType TradingDay;
        /// �Ƿ���ʱ��
        bool isTradeFlag;
        /// 1��K��Bar���
        int  BarSerials_1Min;
        /// 3��K��Bar���
        int  BarSerials_3Min;
        /// 5��K��Bar���
        int  BarSerials_5Min;
        /// 10��K��Bar���
        int  BarSerials_10Min;
        /// 15��K��Bar���
        int  BarSerials_15Min;
        /// 30��K��Bar���
        int  BarSerials_30Min;
        /// 60��K��Bar���
        int  BarSerials_60Min;
        /// ƫ����
        int  OffsetValue;
    };

    class MarketQuotationAPI_HC_EXPORT MarketQuotationAPI_HC : public APINamespace CThostFtdcMdSpi
    {
        /*
        * ��ʼ��
        */
#pragma region
    private:
        /// ������־,��ʼ����������
        log4cplus::Logger m_root;
        /// ������־,��ʼ����������
        log4cplus::Logger m_objLogger;
#ifdef DBDataSource
        /// ���ݿ�����
        otl_connect m_DBConnector;
#endif DBDataSource
        /// �˺���Ϣ ������ID
        ///APINamespace TThostFtdcBrokerIDType m_strBrokerID;
        /// �˺���Ϣ �ͻ���
        ///APINamespace TThostFtdcUserIDType m_strUserID;
        /// �˺���Ϣ �ͻ�����
        ///APINamespace TThostFtdcPasswordType m_strPassword;
        /// �������������
        ///int initializeConnection(APINamespace TThostFtdcBrokerIDType, APINamespace TThostFtdcUserIDType, APINamespace TThostFtdcPasswordType, char*);
        /// ��ʼ����־�ļ�
        int initializeLog();
    public:
        MarketQuotationAPI_HC(void);
        //MarketQuotationAPI_HC(APINamespace TThostFtdcBrokerIDType, APINamespace TThostFtdcUserIDType, APINamespace TThostFtdcPasswordType, char*);
        ~MarketQuotationAPI_HC(void);
#pragma endregion

        /*
        * ����
        */
#pragma region
    private:
        /// ģ��ʱ��
        axapi::ClockSimulated *m_pClockSimulated;
        /// ��������洢�ṹ
        std::hash_map<std::string, struct MarketDataField> m_hashMarketDataList;
        std::vector<struct MarketDataField> m_vMarketData;
        std::vector<TradingClockField> m_vTradingClock;
        /// װ�����ݿ���ָ����Լ����������
        int loadMarketData(const char*, const char*);
        /// װ��ʱ����
        int loadTimeline();
#ifdef KLINESTORAGE
        /// 1����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ1����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array1mKLine;
        /// 3����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ3����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array3mKLine;
        /// 5����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ5����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array5mKLine;
        /// 10����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ10����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array10mKLine;
        /// 15����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ15����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array15mKLine;
        /// 30����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ30����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array30mKLine;
        /// 60����K�ߴ洢�ṹ,eg.����2����Լ,�洢2����Լ60����K�ߵ�ͷ��ַ
        std::vector<KMarketDataField*> m_array60mKLine;
        /// ��Լ����
        std::vector<Contract> m_arrayContracts;
        /// ʱ�������ݣ���¼����ʱ����K�����еĶ�Ӧ
        TradingMinuteField **m_TradingTimeLine;
        /// ��ʼ��K��ʱ��������
        void initiateTradingTimeLine(char**, int) {};
        /// �����º�Լ��ʼ��K�ߴ洢
        int initialKMarketDataSingle(const char*) {};
        /// ���Һ�Լ�Ƿ��Ѷ��ģ�����ȡ����Լ����������
        int findMarketDataIndex(const char*) {};
        /// (����)���ҵ�ǰʱ���Ƿ����ڽ���ʱ�����Լ�ʱ������λ��
        int findCurrentTradingTimeLineOffset() {};
        /// ���ҵ�ǰʱ���Ƿ�����ָ����Լ����ʱ�����Լ�ʱ������λ��
        int findCurrentTradingTimeLineOffset(const char*) {};
        /// ��¼K������
        void recordKData(const char*) {};
#endif KLINESTORAGE
    public:
        /// ���Ķ������
        void subMarketData(char *pInstrumentList[], int in_nInstrumentCount, char* pTradingday);
        /// ����һ������
        int subMarketDataSingle(char *in_strInstrument, char *pTradingday);
        /// �õ�ָ����Լ������
        double getCurrPrice(const char *in_strInstrument) {};
        MarketDataField *getCurrentPrice(const char *in_strInstrument);
        /// ���ڲ������ģ���ٶ� �ڶ����������ڻ�дģ����������к�
        MarketDataField *getCurrentPrice(const char *in_strInstrument, int *);
        /// ��ʼ��ģ��ʱ��,���Ѿ����غõ�ʱ�������ݳ�ʼ���ⲿ��ʱ��
        int initialClockSimulated(ClockSimulated*);
#ifdef KLINESTORAGE
        /// �õ�ָ����Լ��K������,ָ��������K�ߵ�ƫ����,Ĭ�ϵõ����µ�1����K������
        KMarketDataField *getKLine(const char *in_strInstrument, int in_iSecondsPeriod = 1, int in_iCurrentOffset = 0) {};
        /// �õ�ָ����Լ��K������,ָ����������K�ߵ�λ��,Ĭ�ϵõ���һ���ӵ�1����K������
        KMarketDataField *getKLineBySerials(const char *in_strInstrument, int in_iSecondsPeriod = 1, int in_iPosition = 1) {};
#endif KLINESTORAGE
#pragma endregion

        /*
        * ������ʱ��
        */
#pragma region
    private:
        /*
        * �뽻����ʱ���
        */
        double m_nDCETimeDiff;
        double m_nCZCETimeDiff;
        double m_nSHFETimeDiff;
        double m_nCFFEXTimeDiff;
        double m_nIMETimeDiff;
        /// ͨ�����������ֻ�ȡ��������ʱ�䣨����ʱ��+/-������ʱ��
        double getExchangeTime(char*) {};
        /// ��ȡ����ʱ��
        double getLocalTime() {};
        /// ͨ��ʱ���ʽ�ַ���(hh24:mi:ss)��ȡʱ��
        double getTimebyFormat(char*) {};
    public:
        /// �жϴ���ʱ���Ƿ����ָ��������ʱ��
        bool overTime(char*, tm*) {};
#pragma endregion

    };
}

#endif _MARKETQUOTATIONAPI_HC_H_