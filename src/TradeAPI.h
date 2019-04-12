/*
* version 1.0.0
* created by niuchao
* on Apr. 13th 2018
* �ð汾�ϳ�BaseAPI��BaseAPI_CTP,ּ��ͨ�����ÿ������л���ͬ��̨�İ汾,
* ����ʵ��ͬһ���µ��������ͬʱ�ԽӲ�ͬ�Ĺ�̨,���磺�µ��ڽ��˴�,��������CTP
*  ����Ϊ KSV6T_TRADEAPI, CTP_TRADEAPI
*/
#ifndef _TRADEAPI_H_
#define _TRADEAPI_H_
#pragma once

#ifdef TradeAPI_EXE
#define TradeAPI_EXPORT
#else
#ifdef TradeAPI_EXP
#define TradeAPI_EXPORT __declspec(dllexport)
#else
#define TradeAPI_EXPORT __declspec(dllimport)
#endif TradeAPI_EXP
#endif TradeAPI_EXE

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/sleep.h>
#include <log4cplus/loggingmacros.h>
#include <Windows.h>
#include <vector>

#ifdef SQLITE3DATA
#include    "CppSQLite3/CppSQLite3.h"
#endif  SQLITE3DATA

#ifndef KSV6T_TRADEAPI
#ifndef CTP_TRADEAPI
#define     APINamespace
#else   CTP_TRADEAPI
#include    <CTPTdMd_API_common/ThostFtdcTraderApi.h>
#define     APINamespace
#endif  CTP_TRADEAPI
#else   KSV6T_TRADEAPI
#include    <KSv6tTdMd_API_common/KSTradeAPI.h>
#define     APINamespace KingstarAPI::
#endif  KSV6T_TRADEAPI

namespace axapi
{
    /*
    * �Զ������ݽṹ
    */
#pragma region
    /// �ɽ���¼�ṹ 
    struct TradeField
    {
        /// API�ӿڹ淶
        APINamespace CThostFtdcTradeField apiTradeField;
        /// ʣ������
        int Volumn;
        /// ���ӯ����
        double Price;
    };
#pragma endregion

    class TradeAPI_EXPORT TradeAPI : public APINamespace CThostFtdcTraderSpi
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
        /// �˺���Ϣ ������ID
        APINamespace TThostFtdcBrokerIDType m_strBrokerID;
        /// �˺���Ϣ �ͻ���
        APINamespace TThostFtdcUserIDType m_strUserID;
        /// �˺���Ϣ �ͻ�����
        APINamespace TThostFtdcPasswordType m_strPassword;
        /// ��ǰ������
        APINamespace TThostFtdcDateType m_strTradingDay;
        /// ������ʼ������־�������
        int initialLog();
        /// �����û���Ϣ
        int setUserInfo(APINamespace TThostFtdcBrokerIDType in_strBrokerID, APINamespace TThostFtdcUserIDType in_strUserID, APINamespace TThostFtdcPasswordType in_strPassword);
        /// ���ӷ�����
        int initiate(char* in_strFrontAddr);
    public:
        /// �ô˺�����ʼ��,��״̬������,����ʹ������һ�����ع��ĳ�ʼ������
        TradeAPI();
        /// ��ʼ����־,���ݿ�,��������
        TradeAPI(APINamespace TThostFtdcBrokerIDType in_strBrokerID, APINamespace TThostFtdcUserIDType in_strUserID, APINamespace TThostFtdcPasswordType in_strPassword, char* in_strFrontAddr);
        /// TradeAPI(char* in_strBrokerID, char* in_strUserID, char* in_strPassword, char* in_strFrontAddr);
        /// �Ͽ�������,�Ͽ����ݿ�
        ~TradeAPI();
#pragma endregion

        /*
        * ��ǰAPI�ӿ�״̬
        * �ӿ����б���
        */
#pragma region
    private:
        /// API�ӿ�����״̬
        char m_chStatus;
        /// a pointer of CThostFtdcMduserApi instance
        APINamespace CThostFtdcTraderApi* m_pUserApi;
        /// �¼�,������
        HANDLE m_hInitEvent;
        /// ��󱨵�����orderef,����ʱ�豣֤��ֵ���ظ�
        /// ����ʱ���¸ñ���,�յ�ί�лر�ʱ���¸ñ���p
        APINamespace TThostFtdcOrderRefType m_nOrderRef;
        /// ����ID,����ID���ظ�
        int m_nRequestID;
        /// �������ɿͻ��˲�ѯ�����Ӧ��RequestID
        std::vector<int> m_nCompleted_Query_RequestID;
        /// ״̬����,���ڳ�ʼ��ʱ
        int setStatus(char);
        /// ����ɲ�ѯ����ID�������ID�б�
        void setCompletedQueryRequestID(int);
    public:
        /// ����״̬,�����ж��µ�����״̬
        char getStatus();
        /// �鿴���ṩ��ѯID�Ƿ����
        bool checkCompletedQueryRequestID(int);
#pragma endregion

        /*
        * ��������
        */
#pragma region
    private:
        /// ί�б����ṹ
        APINamespace CThostFtdcInputOrderField m_objOrder;
        /// ���¼�,�����µ�������ȡ�۸�֤�ɽ�ʹ��
        APINamespace CThostFtdcDepthMarketDataField m_LatestPrice;
        /// �����µ��ṹ
        void setOrderInfo(char* in_strContract, int in_nDirection, int in_nOffsetFlag, int in_nOrderTypeFlag, int in_nOrderAmount, double in_dOrderPrice);
        /// ��ú�Լ��ǰ��,�����µ�������ȡ�۸�
        double getMarketData(char* in_strContract);
    public:
        /// ��ѯָ����Լ������,��������ڴ���,��ͨ��MarketQuotationAPI�滻�ù��� 
        int queryMarketData(char* in_strContract);
        /// ��ȡqueryMarketData����ѯ�õ��ĺ�Լ����������
        APINamespace CThostFtdcDepthMarketDataField getLatestPrice();
        /// �µ� ����Requestid,��������Ϊin_strContract��Լ��in_nDirection����in_nOffsetFlag��ƽ��־��in_nOrderTypeFlag�������͡�in_nOrderAmount����������in_dOrderPrice�����۸�in_plOrderRef����ָ��
        int MyOrdering(char* in_strContract, int in_nDirection, int in_nOffsetFlag, int in_nOrderTypeFlag, int in_nOrderAmount, double in_dOrderPrice, long *in_plOrderRef = NULL);
        /// ���� in_OrderSysIDΪί�к�,��Ҫͨ����ѯm_vOrderList���
        int MyCancelOrder(char* in_OrderSysID);
#pragma endregion

        /*
        * ��ѯ����
        *   �ⲿ��ѯ�ӿ�,����Requestid,�Ƿ���ɲ�ѯ��Ҫ��m_nCompleted_Query_RequestID
        */
#pragma region
    private:
    public:
        /// ��ѯ�ɽ�,��������߽����ڴ���,ret:Requestid
        int queryCustDone();
        /// ��ѯ�ʽ�,��������߽����ڴ���,ret:Requestid
        int queryCustFund();
        /// ��ѯί��,��������߽����ڴ���,ret:Requestid
        int queryCustOrder();
        /// ��ѯ�ֲ�,��������߽����ڴ���,ret:Requestid
        int queryCustSTKHoldDetail();
        /// ��ѯ�ֲ���ϸ,��������߽����ڴ���,ret:Requestid
        int queryCustSTKHold();
        /// ��ѯ��Լ��Ϣ,��������߽����ڴ���,ret:Requestid
        int queryInstrument();
#pragma endregion

        /*
        * ���ݴ洢
        *   �������ݵ�ָ���洢��Ԫ,sqlite3DB����memory
        */
#pragma region
    private:
        /// �������в�����߸���cust_order�Ĳ���,ret:0 ����
        int insertorUpdateOrder(APINamespace CThostFtdcOrderField *pOrder);
        /// �������в�����߸���cust_done�Ĳ���,ret:0 ����
        int insertorUpdateTrade(APINamespace CThostFtdcTradeField *pTrade);
        /// �������в�����߸���cust_fund�Ĳ���,ret:0 ����
        int insertorUpdateFund(APINamespace CThostFtdcTradingAccountField *pTradingAccount);
        /// �������в�����߸���cust_stock_hold_detail�Ĳ���,ret:0 ����
        int insertorUpdateSTKHoldDetail(APINamespace CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail);
        /// �������в�����߸���cust_stock_hold_detail�Ĳ���,ret:0 ����
        int insertorUpdateSTKHold(APINamespace CThostFtdcInvestorPositionField *pInvestorPosition);
        /// �������в�����߸���market_data�Ĳ���,ret:0 ����
        int insertorUpdateMarketData(APINamespace CThostFtdcDepthMarketDataField *pDepthMarketData);
        /// �������в�����߸���instrument�Ĳ���,ret:0 ����
        int insertorUpdateInstrument(APINamespace CThostFtdcInstrumentField *pInstrument);
        /// �������в�����߸���instrumentStatus�Ĳ���,ret:0 ����
        int insertorUpdateInstrumentStatus(APINamespace CThostFtdcInstrumentStatusField *pInstrumentStatus);
    public:
#pragma endregion

        /*
        * ���ݿ�����
        */
#pragma region
#ifdef SQLITE3DATA
    private:
        /// ������Ϣ�����SQLite���ݿ���
        CppSQLite3DB m_objDB;
        /// ���ѷ������������⴫�ݲ�ѯ���
        CppSQLite3Query m_objQryResult;
        /// �����ļ������ڴ���������ؽṹ,ret:0 ����
        int createDB();
        /// ��ʼ��ʱ����������ʷ��¼,ret:0 ����
        int clearDB();
        /// ��ʼ��ʱ����������ʷ��¼,ret:0 ����
        int clearDB(char* in_strTablename);
        /// ������ʼ�������ݿ�Ȳ���
        int initialDB();
    public:
        /// �������ⲿ���ݲ�ѯ��� p_SQLiteQueryResultΪ��ѯ��� in_pSQLStatementΪ��ѯ���
        void queryDB(CppSQLite3Query* p_SQLiteQueryResult, char* in_pSQLStatement);
#endif SQLITE3DATA
#pragma endregion

        /*
        * �ڴ�����
        */
#pragma region
#ifdef MEMORYDATA
    private:
        /// �ֲ���Ϣ
        std::vector<APINamespace CThostFtdcInvestorPositionField> m_vInvestorPositionList;
        /// �ֲ���ϸ��Ϣ
        std::vector<APINamespace CThostFtdcInvestorPositionDetailField> m_vInvestorPositionDetailList;
        /// ί����Ϣ
        std::vector<APINamespace CThostFtdcOrderField> m_vOrderList;
        /// �ɽ���Ϣ,�����ղ�λ
        std::vector<TradeField> m_vTradeList;
        /// ��Լ��Ϣ�ṹ
        std::vector<APINamespace CThostFtdcInstrumentField> m_vInstrumentList;
        /// Ʒ�ֽ���״̬
        std::vector<APINamespace CThostFtdcInstrumentStatusField> m_vInstrumentStatusList;
    public:
        /// ���ί�����ݼ��ϵĸ���
        int sizeOrderList();
        /// ��óɽ����ݼ��ϵĸ���
        int sizeTradeList();
        /// ��ú�Լ��Ϣ���ݼ��ϵĸ���
        int sizeInstrumentList();
        /// ��óֲ���ϸ��Ϣ���ݼ��ϵĸ���
        int sizePositionDetailList();
        /// ��ú�Լ��Ϣ���ݼ��ϵĸ���
        int sizeInstrumentStatusList();
        /// ͨ��ί�б���λ�û��ί����Ϣ,in_OrderListPostionί�б���λ��
        APINamespace CThostFtdcOrderField getOrderInfo(int in_nOrderListPosition);
        /// ͨ���ɽ�����λ�û�óɽ���Ϣ,in_TradeListPostion�ɽ�����λ��
        TradeField getTradeInfo(int in_nTradeListPosition);
        /// ͨ����Լ��Ϣ����λ�û�ú�Լ��Ϣ,in_nInstrumentListPostion��Լ����λ��
        APINamespace CThostFtdcInstrumentField getInstrumentInfo(int in_nInstrumentListPosition);
        /// ͨ����Լ�����ú�Լ��Ϣ,in_strContract��Լ����
        APINamespace CThostFtdcInstrumentField getInstrumentInfo(char* in_strContract);
        /// ͨ���ֲ���ϸ��Ϣ����λ�û�óֲ���ϸ��Ϣ,in_nPositionDetailListPosition�ֲ���ϸ����λ��
        APINamespace CThostFtdcInvestorPositionDetailField getPositionDetailInfo(int in_nPositionDetailListPosition);
        /// ���óɽ���Ϣ,��Ҫ�������óɱ��۸���ʣ������,Ŀǰֻ֧��Volumn��Priceset,ret:0 ����
        int setTradeInfo(int in_TradeListPostion, char* in_Type, double in_dbvalue=0, int in_nvalue=0);
        /// ͨ����Լ����״̬����λ�û�ú�Լ����״̬,in_nInstrumentStatusListPostion��Լ����λ��
        APINamespace CThostFtdcInstrumentStatusField getInstrumentStatusInfo(int in_nInstrumentStatusListPostion);
        /// ͨ����Լ�����ú�Լ����״̬
        APINamespace CThostFtdcInstrumentStatusField getInstrumentStatusInfo(char* in_strContract);
#endif MEMORYDATA
#pragma endregion

        /*
        * ����ʹ��
        */
#pragma region
#ifdef TEST
    private:
        void test();
        void test2();
        void test3();
    public:
#endif TEST
#pragma endregion

        /*
        * ��̨�ӿ��ṩ
        */
#pragma region
    private:
        /// ����
        virtual void OnFrontConnected();
        /// When the connection between client and the CTP server	disconnected,the follwing function will be called.
        virtual void OnFrontDisconnected(int nReason);
        /// ������
        virtual void OnRtnInstrumentStatus(APINamespace CThostFtdcInstrumentStatusField *pInstrumentStatus);
        /// ����¼�����ر�
        virtual void OnErrRtnOrderInsert(APINamespace CThostFtdcInputOrderField *pInputOrder, APINamespace CThostFtdcRspInfoField *pRspInfo);
        /// ������������ر�
        virtual void OnErrRtnOrderAction(APINamespace CThostFtdcOrderActionField *pOrderAction, APINamespace CThostFtdcRspInfoField *pRspInfo);
        /// After receiving the login request from the client,the CTP server will send the following response to notify the client whether the login success or not.
        virtual void OnRspUserLogin(APINamespace CThostFtdcRspUserLoginField *pRspUserLogin, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// investor response
        virtual void OnRspQryInvestor(APINamespace CThostFtdcInvestorField *pInvestor, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// tradeaccount response
        virtual void OnRspQryTradingAccount(APINamespace CThostFtdcTradingAccountField *pTradingAccount, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// RspQryExchange
        virtual void OnRspQryExchange(APINamespace CThostFtdcExchangeField *pExchange, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// RspQryInstrument
        virtual void OnRspQryInstrument(APINamespace CThostFtdcInstrumentField *pInstrument, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// QryInvestorPositionDetail response
        virtual void OnRspQryInvestorPositionDetail(APINamespace CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// QryInstrumentMarginRate response
        virtual void OnRspQryInstrumentMarginRate(APINamespace CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// QryInstrumentCommissionRate response
        virtual void OnRspQryInstrumentCommissionRate(APINamespace CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// output the DepthMarketData result 
        virtual void OnRspQryDepthMarketData(APINamespace CThostFtdcDepthMarketDataField *pDepthMarketData, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// order insertion response 
        virtual void OnRspOrderInsert(APINamespace CThostFtdcInputOrderField *pInputOrder, APINamespace CThostFtdcRspInfoField *pRspInfo, int  nRequestID, bool bIsLast);
        /// order insertion return 
        virtual void OnRtnOrder(APINamespace CThostFtdcOrderField *pOrder);
        ///trade return
        virtual void OnRtnTrade(APINamespace CThostFtdcTradeField *pTrade);
        /// the error notification caused by client request
        virtual void OnRspError(APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// output the order action result 
        virtual void OnRspOrderAction(APINamespace CThostFtdcInputOrderActionField *pInputOrderAction, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// qryorder return
        virtual void OnRspQryOrder(APINamespace CThostFtdcOrderField *pOrder, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// qrytrade return
        virtual void OnRspQryTrade(APINamespace CThostFtdcTradeField *pTrade, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// QryInvestorPosition return
        virtual void OnRspQryInvestorPosition(APINamespace CThostFtdcInvestorPositionField *pInvestorPosition, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        /// logout return
        virtual void OnRspUserLogout(APINamespace CThostFtdcUserLogoutField *pUserLogout, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        // ���㵥��ѯ������Ӧ
        virtual void OnRspQrySettlementInfo(APINamespace CThostFtdcSettlementInfoField *pSettlementInfo, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        // ���㵥ȷ��ʱ���ѯ
        virtual void OnRspQrySettlementInfoConfirm(APINamespace CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
        // ���㵥ȷ��
        virtual void OnRspSettlementInfoConfirm(APINamespace CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, APINamespace CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
#ifdef CTP_TRADEAPI
        ///����������֪ͨ
        virtual void OnRtnBulletin(APINamespace CThostFtdcBulletinField *pBulletin);
        ///����֪ͨ
        virtual void OnRtnTradingNotice(APINamespace CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo);
#endif CTP_TRADEAPI
        ///ִ������֪ͨ
        virtual void OnRtnExecOrder(APINamespace CThostFtdcExecOrderField *pExecOrder);
    public:
#pragma endregion
    };
}
#endif _TRADEAPI_H_