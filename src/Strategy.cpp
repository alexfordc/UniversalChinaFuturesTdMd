#define STRATEGY_EXP
#define CTP_TRADEAPI
//#define KSV6T_TRADEAPI
#define LOGGER_NAME "Strategy"
#include "Strategy.h"
#include <TradeAPITypeDefine.h>
#include <iostream>
#include <thread>
#include <DataIntoFiles\DataIntoFiles.h>

#define UnpairedOffsetOrderCheckSequenceDiff 200  ///��UnpairedOffsetOrder�еļ�¼��ɾ��ǰ����Ҫ��������С��ȷ�ϴ���(updateorderinfoִ�еĴ�����)

int axapi::Strategy::initializeLog()
{
    char *t_strLogFuncName = "axapi::Strategy::initializeLog";
    log4cplus::initialize();
    log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(true);
    m_root = log4cplus::Logger::getRoot();
    m_objLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(LOGGER_NAME));
    try {
        log4cplus::ConfigureAndWatchThread configureThread(
            LOG4CPLUS_TEXT("log4cplus.properties"), 5 * 1000);
    }
    catch (std::exception e) {
        LOG4CPLUS_FATAL(m_root, "initialLog exception");
    }
    return 0;
}

int axapi::Strategy::initializeAPI(axapi::MarketQuotationAPI *in_pMarketQuotation, axapi::TradeAPI *in_pTrade)
{
    return setAPI(in_pMarketQuotation, in_pTrade);
}

int axapi::Strategy::setAPI(axapi::MarketQuotationAPI *in_pMarketQuotation, axapi::TradeAPI *in_pTrade)
{
    char *t_strLogFuncName = "Strategy::setAPI";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    if (in_pMarketQuotation == NULL || in_pTrade == NULL)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "%s:TdAPI&MdAPI is NULL", t_strLogFuncName);
        LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
        return -100;
    }
    else
    {
        m_pTrade = in_pTrade;
        m_pMarketQuotation = in_pMarketQuotation;
    }
    return 0;
}

axapi::Strategy::Strategy(void)
{
    if (initializeLog() == 0)
    {
        LOG4CPLUS_FATAL(m_objLogger, "initialize LOG OK");
    }
    else
    {
        LOG4CPLUS_FATAL(m_objLogger, "initialize LOG ERROR");
    }
    m_pMarketQuotation = NULL;
    m_pTrade = NULL;

    m_blOffsetALLRunning = false;
    m_blOffsetRunning = false;
    m_blOpenRunning = false;
    m_blCancelRunning = false;
    m_blShutdownFlag = false;
    m_hOpenFinished = CreateEvent(NULL, true, false, NULL);;
    m_hOffsetFinished = CreateEvent(NULL, true, false, NULL);
    m_hCancelFinished = CreateEvent(NULL, true, false, NULL);
    m_hOffsetAllFinished = CreateEvent(NULL, true, false, NULL);
    m_hUpdateOrderFinished = CreateEvent(NULL, true, false, NULL);
    m_hUpdateTradeFinished = CreateEvent(NULL, true, false, NULL);

    m_nCancelWaitSeconds = 20;
    m_nOrderingCheckWaitMillseconds = 1000;
    m_nUpdateOrderTimes = 0;
    m_nUpdateTradeTimes = 0;
#ifdef STRATEGY_EXE
    m_nOpenCount = 0;
#endif STRATEGY_EXE
}

axapi::Strategy::~Strategy(void)
{
    char *t_strLogFuncName = "Strategy::~Strategy";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    stop();
    m_hashAllOrder.clear();
    m_hashConfirmedHoldTrade.clear();
    m_hashConfirmedOrder.clear();
    m_hashUnpairedOffsetOrder.clear();
    m_vConfirmedHoldTrade.clear();
    m_vConfirmedOrder.clear();
}

void axapi::Strategy::saveData()
{
    /*
    * ����ǰ�������ݴ��������ļ�,�����ļ���ʱ������
    */
    char *t_strLogFuncName = "Strategy::saveData";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    time_t nowtime;
    tm *curtime;
    nowtime = time(NULL);
    curtime = localtime(&nowtime);
    char t_datafile[50];
    memset(&t_datafile, '\0', sizeof(t_datafile));
    sprintf_s(t_datafile, sizeof(t_datafile), "datafield", curtime->tm_year, curtime->tm_mon + 1, curtime->tm_mday, curtime->tm_hour, curtime->tm_min, curtime->tm_sec);

    /*
    * д���ļ�
    */
    DataIntoFiles *t_pDataIntoFiles = new DataIntoFiles(t_datafile, 50, "datafile.list");
    /// ��¼m_hashAllOrder
    for (std::hash_map<std::string/*UniversalChinaFutureTdOrderRefType*/, struct AllOrder>::iterator i = m_hashAllOrder.begin();
        i != m_hashAllOrder.end(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_hashAllOrder[%s]:OrderID(%s),OrderRef(%s),OrderType(%c),HoldTradeID(%s),updateOrderRoundSequence(%d)", i->first.c_str(), i->second.OrderID, i->second.OrderRef, i->second.OrderType, i->second.HoldTradeID, i->second.updateOrderRoundSequence);
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼m_hashConfirmedOrder
    for (std::hash_map<std::string/*UniversalChinaFutureTdOrderIDType*/, UniversalChinaFutureTdSequenceType>::iterator i = m_hashConfirmedOrder.begin();
        i != m_hashConfirmedOrder.end(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_hashConfirmedOrder[%s]:Sequence(%d)", i->first.c_str(), i->second);
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼m_vConfirmedOrder
    for (int i = 0; i < m_vConfirmedOrder.size(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_vConfirmedOrder[%d]:OrderID(%s),HoldTradeID(%s),OrderRef(%s),InsertTime(%s),\
                            OrderStatus(%c),OrderType(%c),OrderPrice(%lf),OrderVolumeTotal(%d),\
                            OrderVolumeTraded(%d),OrderVolumnOriginal(%d),TradeIDList.size(%d)",
            i, m_vConfirmedOrder[i].OrderID, m_vConfirmedOrder[i].HoldTradeID, m_vConfirmedOrder[i].OrderRef, m_vConfirmedOrder[i].InsertTime,
            m_vConfirmedOrder[i].OrderStatus, m_vConfirmedOrder[i].OrderType, m_vConfirmedOrder[i].OrderPrice, m_vConfirmedOrder[i].OrderVolumeTotal,
            m_vConfirmedOrder[i].OrderVolumeTraded, m_vConfirmedOrder[i].OrderVolumnOriginal, m_vConfirmedOrder[i].TradeIDList.size());
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼m_hashConfirmedHoldTrade
    for (std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, UniversalChinaFutureTdSequenceType>::iterator i = m_hashConfirmedHoldTrade.begin();
        i != m_hashConfirmedHoldTrade.end(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_hashConfirmedHoldTrade[%s]:Sequence(%d)", i->first.c_str(), i->second);
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼m_vConfirmedHoldTrade
    for (int i = 0; i < m_vConfirmedHoldTrade.size(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_vConfirmedHoldTrade[%d]:TradeID(%s),TradeStatus(%d),InstrumentID(%s),Direction(%d),\
                            Price(%lf),Volumn(%d),OffsetVolumn(%d),AvailableVolumn(%d),\
                            SPOrderID(%s),SLOrderID.size(%d)",
            i, m_vConfirmedHoldTrade[i].TradeID, m_vConfirmedHoldTrade[i].TradeStatus, m_vConfirmedHoldTrade[i].InstrumentID, m_vConfirmedHoldTrade[i].Direction,
            m_vConfirmedHoldTrade[i].Price, m_vConfirmedHoldTrade[i].Volumn, m_vConfirmedHoldTrade[i].OffsetVolumn, m_vConfirmedHoldTrade[i].AvailableVolumn,
            m_vConfirmedHoldTrade[i].SPOrderID, m_vConfirmedHoldTrade[i].SLOrderID.size());
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼m_hashUnpairedOffsetOrder
    for (std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, struct UnpairedOffsetOrder>::iterator i = m_hashUnpairedOffsetOrder.begin();
        i != m_hashUnpairedOffsetOrder.end(); i++)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "m_hashUnpairedOffsetOrder[%s]:TradeID(%s),OrderRef(%s),OffsetOrderType(%c)", i->first.c_str(), i->second.TradeID, i->second.OrderRef, i->second.OffsetOrderType);
        t_pDataIntoFiles->writeData2File(t_strLog);
    }
    /// ��¼����
    sprintf_s(t_strLog, sizeof(t_strLog), "m_nUpdateOrderTimes:%d", m_nUpdateOrderTimes);
    t_pDataIntoFiles->writeData2File(t_strLog);
    sprintf_s(t_strLog, sizeof(t_strLog), "m_nUpdateTradeTimes:%d", m_nUpdateTradeTimes);
    t_pDataIntoFiles->writeData2File(t_strLog);
    sprintf_s(t_strLog, sizeof(t_strLog), "m_nOrderingCheckWaitMillseconds:%d", m_nOrderingCheckWaitMillseconds);
    t_pDataIntoFiles->writeData2File(t_strLog);
    sprintf_s(t_strLog, sizeof(t_strLog), "m_nCancelWaitSeconds:%d", m_nCancelWaitSeconds);
    t_pDataIntoFiles->writeData2File(t_strLog);
}

void axapi::Strategy::loadData()
{
    char *t_strLogFuncName = "Strategy::loadData";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    /*AllOrder t_objAllOrder;
    strcpy_s(t_objAllOrder.OrderID, sizeof(t_objAllOrder.OrderID), "    40000477");
    strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), "34");
    t_objAllOrder.OrderType = OrderStatus_Open;
    t_objAllOrder.updateOrderRoundSequence = 1;
    memset(t_objAllOrder.HoldTradeID, '\0', sizeof(t_objAllOrder.HoldTradeID));
    m_hashAllOrder["34"] = t_objAllOrder;

    strcpy_s(t_objAllOrder.OrderID, sizeof(t_objAllOrder.OrderID), "    40000478");
    strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), "35");
    t_objAllOrder.OrderType = OrderStatus_Open;
    t_objAllOrder.updateOrderRoundSequence = 1;
    memset(t_objAllOrder.HoldTradeID, '\0', sizeof(t_objAllOrder.HoldTradeID));
    m_hashAllOrder["35"] = t_objAllOrder;

    strcpy_s(t_objAllOrder.OrderID, sizeof(t_objAllOrder.OrderID), "    40000480");
    strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), "36");
    t_objAllOrder.OrderType = OrderStatus_Open;
    t_objAllOrder.updateOrderRoundSequence = 1;
    memset(t_objAllOrder.HoldTradeID, '\0', sizeof(t_objAllOrder.HoldTradeID));
    m_hashAllOrder["36"] = t_objAllOrder;

    ConfirmedOrder t_objConfirmedOrder;
    memset(t_objConfirmedOrder.HoldTradeID, '\0', sizeof(t_objConfirmedOrder.HoldTradeID));
    strcpy_s(t_objConfirmedOrder.InsertTime, sizeof(t_objConfirmedOrder.InsertTime), "13:50:08");
    strcpy_s(t_objConfirmedOrder.OrderID, sizeof(t_objConfirmedOrder.OrderID), "    40000477");
    strcpy_s(t_objConfirmedOrder.OrderRef, sizeof(t_objConfirmedOrder.OrderRef), "34");
    t_objConfirmedOrder.OrderPrice = 702.5;
    t_objConfirmedOrder.OrderStatus = THOST_FTDC_OST_AllTraded;
    t_objConfirmedOrder.OrderType = OrderStatus_Open;
    t_objConfirmedOrder.OrderVolumeTotal = 0;
    t_objConfirmedOrder.OrderVolumeTraded = 1;
    t_objConfirmedOrder.OrderVolumnOriginal = 1;
    t_objConfirmedOrder.TradeIDList.clear();
    m_vConfirmedOrder.push_back(t_objConfirmedOrder);
    m_hashConfirmedOrder["    40000477"] = 0;

    memset(t_objConfirmedOrder.HoldTradeID, '\0', sizeof(t_objConfirmedOrder.HoldTradeID));
    strcpy_s(t_objConfirmedOrder.InsertTime, sizeof(t_objConfirmedOrder.InsertTime), "13:50:09");
    strcpy_s(t_objConfirmedOrder.OrderID, sizeof(t_objConfirmedOrder.OrderID), "    40000478");
    strcpy_s(t_objConfirmedOrder.OrderRef, sizeof(t_objConfirmedOrder.OrderRef), "35");
    t_objConfirmedOrder.OrderPrice = 702.5;
    t_objConfirmedOrder.OrderStatus = THOST_FTDC_OST_AllTraded;
    t_objConfirmedOrder.OrderType = OrderStatus_Open;
    t_objConfirmedOrder.OrderVolumeTotal = 0;
    t_objConfirmedOrder.OrderVolumeTraded = 1;
    t_objConfirmedOrder.OrderVolumnOriginal = 1;
    t_objConfirmedOrder.TradeIDList.clear();
    m_vConfirmedOrder.push_back(t_objConfirmedOrder);
    m_hashConfirmedOrder["    40000478"] = 1;

    memset(t_objConfirmedOrder.HoldTradeID, '\0', sizeof(t_objConfirmedOrder.HoldTradeID));
    strcpy_s(t_objConfirmedOrder.InsertTime, sizeof(t_objConfirmedOrder.InsertTime), "13:50:10");
    strcpy_s(t_objConfirmedOrder.OrderID, sizeof(t_objConfirmedOrder.OrderID), "    40000480");
    strcpy_s(t_objConfirmedOrder.OrderRef, sizeof(t_objConfirmedOrder.OrderRef), "36");
    t_objConfirmedOrder.OrderPrice = 702.5;
    t_objConfirmedOrder.OrderStatus = THOST_FTDC_OST_AllTraded;
    t_objConfirmedOrder.OrderType = OrderStatus_Open;
    t_objConfirmedOrder.OrderVolumeTotal = 0;
    t_objConfirmedOrder.OrderVolumeTraded = 1;
    t_objConfirmedOrder.OrderVolumnOriginal = 1;
    t_objConfirmedOrder.TradeIDList.clear();
    m_vConfirmedOrder.push_back(t_objConfirmedOrder);
    m_hashConfirmedOrder["    40000480"] = 2;*/
}

void axapi::Strategy::startStrategy()
{
    char *t_strLogFuncName = "Strategy::startStrategy";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    /*
    * ��ֹ������е��б�
    */
    if (m_blStrategyRunning)
    {
        return;
    }

    /// ���������в���
    loadData();
    m_blShutdownFlag = false;
    m_blOffsetALLRunning = false;
    m_blCancelRunning = true;
    m_blOffsetRunning = true;
    m_blOpenRunning = true;
    m_blStrategyRunning = true;
    std::thread autorun_updateOrderInfo(&Strategy::updateOrderInfo, this);
    std::thread autorun_updateTradeInfo(&Strategy::updateTradeInfo, this);
    std::thread autorun_strategyOrder(&Strategy::strategyOrder, this);
    std::thread autorun_strategyOffset(&Strategy::strategyOffset, this);
    std::thread autorun_strategyOffsetALL(&Strategy::strategyOffsetALL, this);
    std::thread autorun_strategyCancelOrder(&Strategy::strategyCancelOrder, this);
    autorun_updateOrderInfo.detach();
    autorun_updateTradeInfo.detach();
    autorun_strategyOrder.detach();
    autorun_strategyOffset.detach();
    autorun_strategyOffsetALL.detach();
    autorun_strategyCancelOrder.detach();
}

void axapi::Strategy::stopStrategy()
{
    char *t_strLogFuncName = "Strategy::stopStrategy";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    ResetEvent(m_hOpenFinished);
    m_blOpenRunning = false;
    WaitForSingleObject(m_hOpenFinished, INFINITY);

    ResetEvent(m_hOffsetFinished);
    ResetEvent(m_hCancelFinished);
    m_blOffsetRunning = false;
    m_blCancelRunning = false;
    WaitForSingleObject(m_hOffsetFinished, INFINITY);
    WaitForSingleObject(m_hCancelFinished, INFINITY);

    ResetEvent(m_hOffsetAllFinished);
    m_blOffsetALLRunning = true;
    WaitForSingleObject(m_hOffsetAllFinished, INFINITY);

    ResetEvent(m_hUpdateOrderFinished);
    ResetEvent(m_hUpdateTradeFinished);
    m_blShutdownFlag = true;
    WaitForSingleObject(m_hUpdateOrderFinished, INFINITY);
    WaitForSingleObject(m_hUpdateTradeFinished, INFINITY);

    saveData();
    m_blStrategyRunning = false;
}

void axapi::Strategy::updateOrderInfo()
{
    char *t_strLogFuncName = "Strategy::updateOrderInfo";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    APINamespace CThostFtdcOrderField t_objAPIOrderInfo;
    memset(&t_objAPIOrderInfo, '\0', sizeof(t_objAPIOrderInfo));

    try
    {
        while (!m_blShutdownFlag)
        {
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            m_nUpdateOrderTimes += 1;
            int t_ordersize = m_pTrade->sizeOrderList();
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:sizeOrderList=%d", t_strLogFuncName, t_ordersize);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            for (int i = 1; i <= t_ordersize && !m_blShutdownFlag; i++)
            {
                t_objAPIOrderInfo = m_pTrade->getOrderInfo(i);
                if (t_objAPIOrderInfo.OrderSysID[0] == '\0')
                {
                    /// ������Χ
                    continue;
                }

                LOG4CPLUS_TRACE(m_objLogger, "-----------11111111--------------");
                /// ����m_hashAllOrder,m_hashConfirmedOrder,m_vConfirmedOrder
                if (m_hashConfirmedOrder.find(t_objAPIOrderInfo.OrderSysID) == m_hashConfirmedOrder.end())
                    /// ��ǰί��δ��ȷ��,��m_hashAllOrder��δ���¹���Ϣ,���½�m_hashConfirmedOrder,m_vConfirmedOrder
                {
                    /// ����m_hashAllOrder��,���ʾt_objAPIOrderInfo�����ڸò�����Ϊ
                    if (m_hashAllOrder.find(t_objAPIOrderInfo.OrderRef) != m_hashAllOrder.end())
                    {
                        LOG4CPLUS_TRACE(m_objLogger, "-----------33333333--------------");
                        /// ����m_hashAllOrder
                        strcpy_s(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID, sizeof(UniversalChinaFutureTdOrderIDType), t_objAPIOrderInfo.OrderSysID);

                        ConfirmedOrder t_objConfirmedOrder;
                        //memset(&t_objConfirmedOrder, '\0', sizeof(t_objConfirmedOrder));
                        strcpy_s(t_objConfirmedOrder.OrderID, sizeof(t_objConfirmedOrder.OrderID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                        strcpy_s(t_objConfirmedOrder.HoldTradeID, sizeof(t_objConfirmedOrder.HoldTradeID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID);
                        strcpy_s(t_objConfirmedOrder.OrderRef, sizeof(t_objConfirmedOrder.OrderRef), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderRef);
                        t_objConfirmedOrder.OrderType = m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderType;
                        t_objConfirmedOrder.OrderPrice = t_objAPIOrderInfo.LimitPrice;
                        t_objConfirmedOrder.OrderVolumnOriginal = t_objAPIOrderInfo.VolumeTotalOriginal;
                        t_objConfirmedOrder.OrderVolumeTraded = t_objAPIOrderInfo.VolumeTraded;
                        t_objConfirmedOrder.OrderVolumeTotal = t_objAPIOrderInfo.VolumeTotal;
                        t_objConfirmedOrder.OrderStatus = t_objAPIOrderInfo.OrderStatus;
                        strcpy_s(t_objConfirmedOrder.InsertTime, sizeof(t_objConfirmedOrder.InsertTime), t_objAPIOrderInfo.InsertTime);
                        t_objConfirmedOrder.TradeIDList.clear();
                        /// ����m_hashConfirmedOrder
                        m_hashConfirmedOrder[t_objConfirmedOrder.OrderID] = m_vConfirmedOrder.size();
                        /// ����m_vConfirmedOrder
                        m_vConfirmedOrder.push_back(t_objConfirmedOrder);
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:����ConfirmedOrder[%d]_hashmap(%s)", t_strLogFuncName, m_vConfirmedOrder.size() - 1, t_objConfirmedOrder.OrderID);
                        LOG4CPLUS_DEBUG(m_objLogger, t_strLog);

                        /// ��ǰί�д�����m_hashUnpairedOffsetOrder��,��Ҫ����ƥ��
                        if (m_hashUnpairedOffsetOrder.find(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID) != m_hashUnpairedOffsetOrder.end())
                        {
                            LOG4CPLUS_TRACE(m_objLogger, "-----------44444444--------------");
                            /// ��ǰί�ж�Ӧ�����ѱ�ȷ��,���Խ���ƥ�����
                            if (m_hashConfirmedHoldTrade.find(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID) != m_hashConfirmedHoldTrade.end())
                            {
                                LOG4CPLUS_TRACE(m_objLogger, "-----------55555555--------------");
                                int t_HoldTradeIndex = m_hashConfirmedHoldTrade[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID];
                                /// ֹӯƽ�ֵ���Ӧ������
                                if (m_hashUnpairedOffsetOrder[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID].OffsetOrderType == OrderOffsetType_SP)
                                {
                                    strcpy_s(m_vConfirmedHoldTrade[t_HoldTradeIndex].SPOrderID, sizeof(m_vConfirmedHoldTrade[t_HoldTradeIndex].SPOrderID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                                    sprintf_s(t_strLog, sizeof(t_strLog), "%s:ConfirmedHoldTrade[%d]_hashmap(%s)ֹӯƽ�ֵ�������Ϊ(%s)", t_strLogFuncName, t_HoldTradeIndex, m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID, m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                                    LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
                                }
                                /// ֹ��ƽ�ֵ���Ӧ������
                                else if (m_hashUnpairedOffsetOrder[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID].OffsetOrderType == OrderOffsetType_SL)
                                {
                                    m_vConfirmedHoldTrade[t_HoldTradeIndex].SLOrderID.push_back(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                                    sprintf_s(t_strLog, sizeof(t_strLog), "%s:ConfirmedHoldTrade[%d]_hashmap(%s)ֹ��ƽ�ֵ�(%s)����", t_strLogFuncName, t_HoldTradeIndex, m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID, m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                                    LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
                                }
                                /// ����m_hashUnpairedOffsetOrder�е���Ϣ
                                m_hashUnpairedOffsetOrder.erase(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID);

                                sprintf_s(t_strLog, sizeof(t_strLog), "%s:UnpairedOffsetOrder(%s)���޳�,ʣ��%d��δ��ȷ��", t_strLogFuncName, m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID, m_hashUnpairedOffsetOrder.size());
                                LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
                                LOG4CPLUS_TRACE(m_objLogger, "-----------5a5a5a5a--------------");
                            }
                            LOG4CPLUS_TRACE(m_objLogger, "-----------4a4a4a4a--------------");
                        }
                        LOG4CPLUS_TRACE(m_objLogger, "-----------3a3a3a3a--------------");
                    }
                    LOG4CPLUS_TRACE(m_objLogger, "-----------1a1a1a1a--------------");
                }
                /// ��ǰί���Ѿ�ȷ�ϴ���,��m_hashAllOrder,m_hashConfirmedOrder�Ѿ����¹���Ϣ,�����m_vConfirmedOrder
                else
                {
                    LOG4CPLUS_TRACE(m_objLogger, "-----------22222222--------------");
                    int t_ConfirmedOrderIndex = m_hashConfirmedOrder[t_objAPIOrderInfo.OrderSysID];
                    m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderPrice = t_objAPIOrderInfo.LimitPrice;
                    m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumnOriginal = t_objAPIOrderInfo.VolumeTotalOriginal;
                    m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumeTraded = t_objAPIOrderInfo.VolumeTraded;
                    m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumeTotal = t_objAPIOrderInfo.VolumeTotal;
                    m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderStatus = t_objAPIOrderInfo.OrderStatus;
                    LOG4CPLUS_TRACE(m_objLogger, "-----------2a2a2a2a--------------");
                }
            }
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "updateOrderInfo:%s", e.what());
        LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
    }

    sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
    LOG4CPLUS_INFO(m_objLogger, t_strLog);

    SetEvent(m_hUpdateOrderFinished);
}

void axapi::Strategy::updateTradeInfo()
{
    char *t_strLogFuncName = "Strategy::updateTradeInfo";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    TradeField t_objAPITradeInfo;
    memset(&t_objAPITradeInfo, '\0', sizeof(t_objAPITradeInfo));

    try
    {
        while (!m_blShutdownFlag)
        {
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            m_nUpdateTradeTimes += 1;
            int t_tradesize = m_pTrade->sizeTradeList();
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:sizeTradeList=%d", t_strLogFuncName, t_tradesize);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            for (int i = 1; i <= t_tradesize && !m_blShutdownFlag; i++)
            {
                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------11111111--------------");
                t_objAPITradeInfo = m_pTrade->getTradeInfo(i);
                if (t_objAPITradeInfo.apiTradeField.TradeID[0] == '\0')
                {
                    //������Χ
                    continue;
                }

                /// ����m_vConfirmedOrder,m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
                if (m_hashConfirmedHoldTrade.find(t_objAPITradeInfo.apiTradeField.TradeID) == m_hashConfirmedHoldTrade.end())
                    /*
                    * ��ǰ�ɽ�δ��ȷ��,m_hashConfirmedHoldTrade�в�����
                    * ���Ϊƽ����ֻ����m_vConfirmedOrder
                    * ���Ϊ���������m_vConfirmedOrder,�½�m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
                    */
                {
                    LOG4CPLUS_TRACE(m_objLogger, "---------------------------------33333333--------------");
                    /// ����m_hashAllOrder��,���ʾt_objAPITradeInfo�����ڸò�����Ϊ
                    if (m_hashAllOrder.find(t_objAPITradeInfo.apiTradeField.OrderRef) != m_hashAllOrder.end())
                    {
                        LOG4CPLUS_TRACE(m_objLogger, "---------------------------------44444444--------------");
                        /// m_hashConfirmedOrder��δȷ�����,���յ��˳ɽ���Ϣ,�������ȴ�m_hashConfirmedOrderȷ�����
                        if (m_hashConfirmedOrder.find(m_hashAllOrder[t_objAPITradeInfo.apiTradeField.OrderRef].OrderID) == m_hashConfirmedOrder.end())
                        {
                            continue;
                        }
                        else
                        {
                            LOG4CPLUS_TRACE(m_objLogger, "---------------------------------55555555--------------");
                            /// ����m_vConfirmedOrder,����Ѹ���������
                            int t_index = m_hashConfirmedOrder[m_hashAllOrder[t_objAPITradeInfo.apiTradeField.OrderRef].OrderID];
                            for (int j = 0; j < m_vConfirmedOrder[t_index].TradeIDList.size(); j++)
                            {
                                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------66666666--------------");
                                /// m_vConfirmedOrder���Ѹ��¹�TradeID,������
                                if (strcmp(m_vConfirmedOrder[t_index].TradeIDList[j].c_str(), t_objAPITradeInfo.apiTradeField.TradeID) == 0)
                                {
                                    break;
                                }
                                /// m_vConfirmedOrder��δ���¹�TradeID,�����
                                else
                                {
                                    if (j == m_vConfirmedOrder[t_index].TradeIDList.size() - 1)
                                    {
                                        m_vConfirmedOrder[t_index].TradeIDList.push_back(t_objAPITradeInfo.apiTradeField.TradeID);
                                    }
                                }
                                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------6a6a6a6a--------------");
                            }


                            /// ���Ϊ�������½�m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
                            if (t_objAPITradeInfo.apiTradeField.OffsetFlag == THOST_FTDC_OF_Open)
                            {
                                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------77777777--------------");
                                ConfirmedHoldTrade t_objConfirmedHoldTrade;
                                //memset(&t_objConfirmedHoldTrade, '\0', sizeof(t_objConfirmedHoldTrade));
                                strcpy_s(t_objConfirmedHoldTrade.TradeID, sizeof(t_objConfirmedHoldTrade.TradeID), t_objAPITradeInfo.apiTradeField.TradeID);
                                strcpy_s(t_objConfirmedHoldTrade.InstrumentID, sizeof(t_objConfirmedHoldTrade.InstrumentID), t_objAPITradeInfo.apiTradeField.InstrumentID);
                                t_objConfirmedHoldTrade.Direction = t_objAPITradeInfo.apiTradeField.Direction;
                                t_objConfirmedHoldTrade.Price = t_objAPITradeInfo.apiTradeField.Price;
                                t_objConfirmedHoldTrade.Volumn = t_objAPITradeInfo.apiTradeField.Volume;
                                strcpy_s(t_objConfirmedHoldTrade.TradeTime, sizeof(t_objConfirmedHoldTrade.TradeTime), t_objAPITradeInfo.apiTradeField.TradeTime);
                                t_objConfirmedHoldTrade.AvailableVolumn = t_objAPITradeInfo.apiTradeField.Volume;
                                t_objConfirmedHoldTrade.OffsetVolumn = 0;
                                t_objConfirmedHoldTrade.HighestProfitPrice = t_objAPITradeInfo.apiTradeField.Price;
                                t_objConfirmedHoldTrade.TradeStatus = TradeStatus_Hold;
                                memset(t_objConfirmedHoldTrade.SPOrderID, '\0', sizeof(t_objConfirmedHoldTrade.SPOrderID));
                                t_objConfirmedHoldTrade.SLOrderID.clear();

                                /// ����m_hashConfirmedHoldTrade
                                m_hashConfirmedHoldTrade[t_objAPITradeInfo.apiTradeField.TradeID] = m_vConfirmedHoldTrade.size();
                                /// ����m_vConfirmedHoldTrade
                                m_vConfirmedHoldTrade.push_back(t_objConfirmedHoldTrade);

                                sprintf_s(t_strLog, sizeof(t_strLog), "%s:����ConfirmedHoldTrade[%d]_hash(%s)", t_strLogFuncName, m_vConfirmedHoldTrade.size() - 1, t_objAPITradeInfo.apiTradeField.TradeID);
                                LOG4CPLUS_INFO(m_objLogger, t_strLog);
                                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------7a7a7a7a--------------");
                            }
                            LOG4CPLUS_TRACE(m_objLogger, "---------------------------------5a5a5a5a--------------");
                        }
                        LOG4CPLUS_TRACE(m_objLogger, "---------------------------------4a4a4a4a--------------");
                    }
                    LOG4CPLUS_TRACE(m_objLogger, "---------------------------------3a3a3a3a--------------");
                }
                /*
                * ��ǰ�ɽ��ѱ�ȷ����ͨ��ƽ����Ϣ���³ֲ�����
                */
                else
                {
                    LOG4CPLUS_TRACE(m_objLogger, "---------------------------------22222222--------------");
                    updateHoldTrade(m_hashConfirmedHoldTrade[t_objAPITradeInfo.apiTradeField.TradeID]);
                    LOG4CPLUS_TRACE(m_objLogger, "---------------------------------2a2a2a2a--------------");
                }
                LOG4CPLUS_TRACE(m_objLogger, "---------------------------------1a1a1a1a--------------");
            }
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "updateTradeInfo:%s", e.what());
        LOG4CPLUS_DEBUG(m_objLogger, t_strLog);
    }

    sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
    LOG4CPLUS_INFO(m_objLogger, t_strLog);

    SetEvent(m_hUpdateTradeFinished);
}

void axapi::Strategy::updateHoldTrade(UniversalChinaFutureTdSequenceType in_HoldTradeIndex)
{
    char *t_strLogFuncName = "Strategy::updateHoldTrade";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    UniversalChinaFutureTdVolumnType t_HoldTradeOffsetVolumn = 0;

    /// �ֲ���ȫƽ������
    if (m_vConfirmedHoldTrade[in_HoldTradeIndex].TradeStatus == TradeStatus_OffsetALL)
    {
        return;
    }

    /// SPƽ����Ϣ�������������
    if (m_vConfirmedHoldTrade[in_HoldTradeIndex].SPOrderID[0] != '\0')
    {
        int t_SPOrderIndex = m_hashConfirmedOrder[m_vConfirmedHoldTrade[in_HoldTradeIndex].SPOrderID];
        /// ����ί��������ʲôֻ��¼�Ѿ��ɽ��Ĳ���
        t_HoldTradeOffsetVolumn += m_vConfirmedOrder[t_SPOrderIndex].OrderVolumeTraded;
    }

    /// SLƽ����Ϣ�������������
    for (int i = 0; i < m_vConfirmedHoldTrade[in_HoldTradeIndex].SLOrderID.size(); i++)
    {
        int t_SLOrderIndex = m_hashConfirmedOrder[m_vConfirmedHoldTrade[in_HoldTradeIndex].SLOrderID[i]];
        /// ����ί��������ʲôֻ��¼�Ѿ��ɽ��Ĳ���
        t_HoldTradeOffsetVolumn += m_vConfirmedOrder[t_SLOrderIndex].OrderVolumeTraded;
    }

    /*
    * ����OffsetVolumn,AvailableVolumn
    * �������ӯ����λHighestProfitPrice
    * ���Volumn==OffsetVolumn,���ȫ��ƽ��,���³ֲ�״̬
    */
    // ������Բ�ѯ����ǰ��Լ������������ӯ����λ
    axapi::MarketDataField t_objCurrentPrice;
    axapi::MarketDataField* t_currentPrice = m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[in_HoldTradeIndex].InstrumentID);
    if (t_currentPrice != NULL)
    {
        memcpy_s(&t_objCurrentPrice, sizeof(t_objCurrentPrice), t_currentPrice, sizeof(t_objCurrentPrice));
        if ((m_vConfirmedHoldTrade[in_HoldTradeIndex].Direction == THOST_FTDC_D_Buy
            && m_vConfirmedHoldTrade[in_HoldTradeIndex].HighestProfitPrice < t_objCurrentPrice.LastPrice)
            || (m_vConfirmedHoldTrade[in_HoldTradeIndex].Direction == THOST_FTDC_D_Sell
                && m_vConfirmedHoldTrade[in_HoldTradeIndex].HighestProfitPrice > t_objCurrentPrice.LastPrice))
        {
            m_vConfirmedHoldTrade[in_HoldTradeIndex].HighestProfitPrice = t_objCurrentPrice.LastPrice;
        }
    }
    m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn = t_HoldTradeOffsetVolumn;
    m_vConfirmedHoldTrade[in_HoldTradeIndex].AvailableVolumn = m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn - m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn < 0 ? 0 : m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn - m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn;
    m_vConfirmedHoldTrade[in_HoldTradeIndex].TradeStatus = m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn == m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn ? TradeStatus_OffsetALL : TradeStatus_Hold;
}

void axapi::Strategy::strategyOrder()
{
    char *t_strLogFuncName = "Strategy::strategyOrder";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    bool t_blOpenFlag = false;
    std::string t_strOffsetMessage;
    char t_strContract[20];
    memset(t_strContract, '\0', sizeof(t_strContract));
    int t_nDirection, t_nOffsetFlag, t_nOrderTypeFlag, t_nOrderAmount;
    double t_dOrderPrice;
    long t_plOpenOrderRef;
    UniversalChinaFutureTdOrderRefType t_objOpenOrderRef;

    try
    {
        while (m_blOpenRunning)
        {
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            myStrategy(&t_blOpenFlag, &t_strOffsetMessage, t_strContract, &t_nDirection, &t_nOffsetFlag, &t_nOrderTypeFlag, &t_nOrderAmount, &t_dOrderPrice);

            if (t_blOpenFlag)
            {
                m_pTrade->MyOrdering(t_strContract, t_nDirection, t_nOffsetFlag, t_nOrderTypeFlag, t_nOrderAmount, t_dOrderPrice, &t_plOpenOrderRef);
                sprintf_s(t_objOpenOrderRef, sizeof(t_objOpenOrderRef), "%ld", t_plOpenOrderRef);

                /// ������Ϣ����m_hashAllOrder,�ȴ�����ȷ��
                AllOrder t_objAllOrder;
                memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
                strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objOpenOrderRef);
                t_objAllOrder.OrderType = OrderStatus_Open;
                t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

                m_hashAllOrder[t_objOpenOrderRef] = t_objAllOrder;

                sprintf_s(t_strLog, sizeof(t_strLog), "%s:�µ�%s_%s", t_strLogFuncName, t_objOpenOrderRef, t_strOffsetMessage.c_str());
                LOG4CPLUS_INFO(m_objLogger, t_strLog);
            }
            Sleep(m_nOrderingCheckWaitMillseconds);
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "strategyOrder:%s", e.what());
        LOG4CPLUS_ERROR(m_objLogger, t_strLog);
    }

    sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
    LOG4CPLUS_INFO(m_objLogger, t_strLog);

    SetEvent(m_hOpenFinished);
}

void axapi::Strategy::strategyOffset()
{
    char *t_strLogFuncName = "Strategy::strategyOffset";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    bool t_blOffsetFlag;
    std::string t_strOffsetMessage;

    try
    {
        while (m_blOffsetRunning)
        {
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            /// �������еĳֲ�,�ҵ���δƽ�ֵĳֲ�
            for (int i = 0; m_blOffsetRunning && i < m_vConfirmedHoldTrade.size(); i++)
            {
                LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------11111111--------------");

                t_blOffsetFlag = false;
                t_strOffsetMessage.clear();
                if (m_vConfirmedHoldTrade[i].TradeStatus == TradeStatus_Hold)
                {
                    LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------22222222--------------");
                    myOffsetStrategy(m_vConfirmedHoldTrade[i], &t_blOffsetFlag, &t_strOffsetMessage);
                    /// ��Ҫ����ƽ��
                    if (t_blOffsetFlag)
                    {
                        LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------33333333--------------");
                        /// �����ֹӯ��ֹӯ�� ��ֹ��
                        if (m_vConfirmedHoldTrade[i].SPOrderID[0] != '\0')
                        {
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------55555555--------------");
                            bool t_blCancelSPOrder = true;
                            while (t_blCancelSPOrder)
                            {
                                switch (m_vConfirmedOrder[m_hashConfirmedOrder[m_vConfirmedHoldTrade[i].SPOrderID]].OrderStatus)
                                {
                                case THOST_FTDC_OST_NoTradeQueueing:
                                case THOST_FTDC_OST_PartTradedQueueing:
                                    m_pTrade->MyCancelOrder(m_vConfirmedHoldTrade[i].SPOrderID);
                                    Sleep(100);
                                    break;
                                default:
                                    t_blCancelSPOrder = false;
                                    break;
                                }
                            }

                            m_vConfirmedHoldTrade[i].SLOrderID.push_back(m_vConfirmedHoldTrade[i].SPOrderID);
                            memset(&m_vConfirmedHoldTrade[i].SPOrderID, '\0', sizeof(m_vConfirmedHoldTrade[i].SPOrderID));
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------5a5a5a5a--------------");
                        }
                        /// ����¹�ֹӯ��������δȷ����ɾ�������Ϣ
                        else if (m_hashUnpairedOffsetOrder.find(m_vConfirmedHoldTrade[i].TradeID) != m_hashUnpairedOffsetOrder.end()
                            && m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID].OffsetOrderType == OrderOffsetType_SP)
                        {
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------66666666--------------");
                            /// UnpairedOffsetOrder�е�δƥ��ί���Ѿ�������ƥ����δƥ����,��ֱ��ɾ��
                            if (m_hashAllOrder[m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID].OrderRef].updateOrderRoundSequence + UnpairedOffsetOrderCheckSequenceDiff <= m_nUpdateOrderTimes)
                            {
                                m_hashUnpairedOffsetOrder.erase(m_vConfirmedHoldTrade[i].TradeID);
                            }
                            else
                            {
                                continue;
                            }
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------6a6a6a6a--------------");
                        }

                        /// ȷ��û��ֹӯ��,��ֱ����ֹ��
                        UniversalChinaFutureTdOrderRefType t_objSLOrderRef;
                        long t_lSLOrderRef;
                        m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID,
                            m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY,
                            ORDER_OFFSETFLAG_OFFSET_TODAY,
                            ORDER_LIMITPRICE,
                            m_vConfirmedHoldTrade[i].AvailableVolumn,
                            m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID)->BidPrice1 : m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID)->AskPrice1,
                            &t_lSLOrderRef);
                        sprintf_s(t_objSLOrderRef, sizeof(t_objSLOrderRef), "%ld", t_lSLOrderRef);

                        /// ֹ��ƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
                        UnpairedOffsetOrder t_objUnpairedOffsetOrder;
                        memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
                        strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedHoldTrade[i].TradeID);
                        strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSLOrderRef);
                        t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SL;

                        m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;

                        /// ֹӯƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
                        AllOrder t_objAllOrder;
                        memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
                        strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedHoldTrade[i].TradeID);
                        strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSLOrderRef);
                        t_objAllOrder.OrderType = OrderStatus_SLOffset;
                        t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

                        m_hashAllOrder[t_objSLOrderRef] = t_objAllOrder;
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:�µ�m_vConfirmedHoldTrade[%d](%s)ֹ��ƽ�ֵ�m_hashAllOrder(%s)", t_strLogFuncName, i, m_vConfirmedHoldTrade[i].TradeID, t_objSLOrderRef);
                        LOG4CPLUS_INFO(m_objLogger, t_strLog);
                        LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------3a3a3a3a--------------");
                    }
                    /// �������Ҫƽ��
                    else
                    {
                        LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------44444444--------------");
                        /// û��ֹӯ,��ֹӯ
                        if (m_vConfirmedHoldTrade[i].SPOrderID[0] == '\0'
                            && m_vConfirmedHoldTrade[i].SLOrderID.size() == 0
                            && m_hashUnpairedOffsetOrder.find(m_vConfirmedHoldTrade[i].TradeID) == m_hashUnpairedOffsetOrder.end())
                        {
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------77777777--------------");
                            bool t_blSPOffsetFlag = false;
                            double t_dbSPOrderPrice;
                            getPreOffsetPrice(m_vConfirmedHoldTrade[i], &t_blSPOffsetFlag, &t_dbSPOrderPrice);
                            if (t_blSPOffsetFlag)
                            {
                                UniversalChinaFutureTdOrderRefType t_objSPOrderRef;
                                long t_lSPOrderRef;

                                /// �µ����OrderRef
                                m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID,
                                    (m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY),
                                    ORDER_OFFSETFLAG_OFFSET_TODAY,
                                    ORDER_LIMITPRICE,
                                    m_vConfirmedHoldTrade[i].AvailableVolumn,
                                    t_dbSPOrderPrice,
                                    &t_lSPOrderRef);
                                sprintf_s(t_objSPOrderRef, sizeof(t_objSPOrderRef), "%ld", t_lSPOrderRef);

                                /// ֹӯƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
                                UnpairedOffsetOrder t_objUnpairedOffsetOrder;
                                memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
                                strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedHoldTrade[i].TradeID);
                                strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSPOrderRef);
                                t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SP;

                                m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;

                                /// ֹӯƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
                                AllOrder t_objAllOrder;
                                memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
                                strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedHoldTrade[i].TradeID);
                                strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSPOrderRef);
                                t_objAllOrder.OrderType = OrderStatus_SPOffset;
                                t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

                                m_hashAllOrder[t_objSPOrderRef] = t_objAllOrder;

                                sprintf_s(t_strLog, sizeof(t_strLog), "%s:�µ�m_vConfirmedHoldTrade[%d](%s)ֹӯƽ�ֵ�m_hashAllOrder(%s)", t_strLogFuncName, i, m_vConfirmedHoldTrade[i].TradeID, t_objSPOrderRef);
                                LOG4CPLUS_INFO(m_objLogger, t_strLog);
                            }
                            LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------7a7a7a7a--------------");
                        }
                        LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------4a4a4a4a--------------");
                    }
                    LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------2a2a2a2a--------------");
                }
                LOG4CPLUS_TRACE(m_objLogger, "------------------------------------------------------------------1a1a1a1a--------------");
            }
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "strategyOffset:%s", e.what());
        LOG4CPLUS_ERROR(m_objLogger, t_strLog);
    }

    sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
    LOG4CPLUS_INFO(m_objLogger, t_strLog);

    SetEvent(m_hOffsetFinished);
}

void axapi::Strategy::strategyCancelOrder()
{
    char *t_strLogFuncName = "Strategy::strategyCancelOrder";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    //���ڻ�ȡʱ��
    time_t nowtime;
    tm *curtime;
    int curMinutes, t_OrderInsertTime;
    char t_OrderInfoHour[3];
    char t_OrderInfoMinutes[3];
    char t_OrderInfoSeconds[3];
    //�Ƿ񳷵���־
    bool t_CancelAction;

    try
    {
        while (m_blCancelRunning)
        {
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
            LOG4CPLUS_TRACE(m_objLogger, t_strLog);

            for (int i = 0; m_blCancelRunning && i < m_vConfirmedOrder.size(); i++)
            {
                switch (m_vConfirmedOrder[i].OrderStatus)
                {
                case THOST_FTDC_OST_PartTradedQueueing:
                case THOST_FTDC_OST_NoTradeQueueing:
                {
                    /// Ԥ��ֹӯ������
                    if (m_vConfirmedOrder[i].OrderType == OrderStatus_SPOffset)
                    {
                        continue;
                    }
                    /// ֹ�������µ�
                    else if (m_vConfirmedOrder[i].OrderType == OrderStatus_SLOffset || m_vConfirmedOrder[i].OrderType == OrderStatus_Open)
                    {
                        /*
                        * �Ƿ񳷵�
                        * 1.n�볷��
                        */
#pragma region
                        nowtime = time(NULL);
                        curtime = localtime(&nowtime);
                        t_OrderInfoHour[0] = m_vConfirmedOrder[i].InsertTime[0];
                        t_OrderInfoHour[1] = m_vConfirmedOrder[i].InsertTime[1];
                        t_OrderInfoHour[2] = '\0';
                        t_OrderInfoMinutes[0] = m_vConfirmedOrder[i].InsertTime[3];
                        t_OrderInfoMinutes[1] = m_vConfirmedOrder[i].InsertTime[4];
                        t_OrderInfoMinutes[2] = '\0';
                        t_OrderInfoSeconds[0] = m_vConfirmedOrder[i].InsertTime[6];
                        t_OrderInfoSeconds[1] = m_vConfirmedOrder[i].InsertTime[7];
                        t_OrderInfoSeconds[2] = '\0';
                        if (curtime->tm_hour < 20)
                        {
                            curMinutes = (curtime->tm_hour + 24) * 3600 + curtime->tm_min * 60 + curtime->tm_sec * 1;
                        }
                        else
                        {
                            curMinutes = (curtime->tm_hour + 0) * 3600 + curtime->tm_min * 60 + curtime->tm_sec * 1;
                        }
                        if (atoi(t_OrderInfoHour) < 20)
                        {
                            t_OrderInsertTime = (atoi(t_OrderInfoHour) + 24) * 3600 + atoi(t_OrderInfoMinutes) * 60 + atoi(t_OrderInfoSeconds);
                        }
                        else
                        {
                            t_OrderInsertTime = (atoi(t_OrderInfoHour) + 0) * 3600 + atoi(t_OrderInfoMinutes) * 60 + atoi(t_OrderInfoSeconds);
                        }
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:curMinutes:%d,|%d:%d:%d", t_strLogFuncName, curMinutes, curtime->tm_hour, curtime->tm_min, curtime->tm_sec);
                        LOG4CPLUS_TRACE(m_objLogger, t_strLog);
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:m_vConfirmedOrder[%d]:%d,%s|%s|%s|%s:%s:%s", t_strLogFuncName, i, t_OrderInsertTime, m_vConfirmedOrder[i].OrderID, m_vConfirmedOrder[i].OrderRef, m_vConfirmedOrder[i].InsertTime, t_OrderInfoHour, t_OrderInfoMinutes, t_OrderInfoSeconds);
                        LOG4CPLUS_TRACE(m_objLogger, t_strLog);
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:m_vConfirmedOrder[%d](%s):curMinutes:%d,t_OrderInsertTime:%d,m_nCancelWaitSeconds:%d", t_strLogFuncName, i, m_vConfirmedOrder[i].OrderID, curMinutes, t_OrderInsertTime, m_nCancelWaitSeconds);
                        LOG4CPLUS_DEBUG(m_objLogger, t_strLog);

                        if (curMinutes - t_OrderInsertTime >= m_nCancelWaitSeconds)
                        {
                            t_CancelAction = true;
                            sprintf_s(t_strLog, sizeof(t_strLog), "%s:����m_vConfirmedOrder[%d](%s)", t_strLogFuncName, i, m_vConfirmedOrder[i].OrderID);
                            LOG4CPLUS_INFO(m_objLogger, t_strLog);
                        }
                        else
                        {
                            t_CancelAction = false;
                        }
#pragma endregion
                        if (t_CancelAction)
                        {
                            /// ����
                            bool t_blCanelNOSPOrder = true;
                            while (t_blCanelNOSPOrder)
                            {
                                switch (m_vConfirmedOrder[i].OrderStatus)
                                {
                                case THOST_FTDC_OST_PartTradedQueueing:
                                case THOST_FTDC_OST_NoTradeQueueing:
                                    m_pTrade->MyCancelOrder(m_vConfirmedOrder[i].OrderID);
                                    Sleep(100);
                                    break;
                                default:
                                    t_blCanelNOSPOrder = false;
                                    break;
                                }
                            }

                            /// ERROR ���²���ʱ�����³����ɳ�������������ƽ����ֲ�

                            /// �����ֹ��ƽ�ֱ������������,���ֲ�������
                            if (m_vConfirmedOrder[i].OrderType == OrderStatus_SLOffset)
                            {
                                UniversalChinaFutureTdOrderRefType t_objSLOrderRef;
                                long t_lSLOrderRef;
                                m_pTrade->MyOrdering(m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].InstrumentID,
                                    m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY,
                                    ORDER_OFFSETFLAG_OFFSET_TODAY,
                                    ORDER_AGAINSTPRICE,
                                    m_vConfirmedOrder[i].OrderVolumeTotal,
                                    m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].InstrumentID)->AskPrice1,
                                    &t_lSLOrderRef);
                                sprintf_s(t_objSLOrderRef, sizeof(t_objSLOrderRef), "%ld", t_lSLOrderRef);

                                /// ֹ��ƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
                                UnpairedOffsetOrder t_objUnpairedOffsetOrder;
                                memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
                                strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedOrder[i].HoldTradeID);
                                strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSLOrderRef);
                                t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SL;

                                m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;

                                /// ֹ��ƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
                                AllOrder t_objAllOrder;
                                memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
                                strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedOrder[i].HoldTradeID);
                                strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSLOrderRef);
                                t_objAllOrder.OrderType = OrderStatus_SLOffset;
                                t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

                                m_hashAllOrder[t_objSLOrderRef] = t_objAllOrder;
                                sprintf_s(t_strLog, sizeof(t_strLog), "%s:ֹ��ƽ�ֳ����󲹱�(%s)", t_strLogFuncName, t_objSLOrderRef);
                                LOG4CPLUS_INFO(m_objLogger, t_strLog);
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "%s:%s", t_strLogFuncName, e.what());
        LOG4CPLUS_ERROR(m_objLogger, t_strLog);
    }

    sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
    LOG4CPLUS_INFO(m_objLogger, t_strLog);

    SetEvent(m_hCancelFinished);
}

void axapi::Strategy::strategyOffsetALL()
{
    char *t_strLogFuncName = "Strategy::strategyOffsetALL";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    while (true)
    {
        sprintf_s(t_strLog, sizeof(t_strLog), "%s:Running", t_strLogFuncName);
        LOG4CPLUS_TRACE(m_objLogger, t_strLog);

        if (m_blOffsetALLRunning)
        {
            /// �ȳ��ҵ�
            int t_nUnCanceledOrder = 1;
            while (t_nUnCanceledOrder != 0)
            {
                t_nUnCanceledOrder = 0;
                for (int i = 0; i < m_vConfirmedOrder.size(); i++)
                {
                    switch (m_vConfirmedOrder[i].OrderStatus)
                    {
                    case THOST_FTDC_OST_NoTradeQueueing:
                    case THOST_FTDC_OST_PartTradedQueueing:
                        m_pTrade->MyCancelOrder(m_vConfirmedOrder[i].OrderID);
                        t_nUnCanceledOrder++;
                        break;
                    default:
                        break;
                    }
                }
            }
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:��ּ����ֲּ�¼%d��", t_strLogFuncName, m_vConfirmedHoldTrade.size());
            LOG4CPLUS_DEBUG(m_objLogger, t_strLog);

            for (int i = 0; i < m_vConfirmedHoldTrade.size(); i++)
            {
                if (m_vConfirmedHoldTrade[i].TradeStatus == TradeStatus_Hold)
                {
                    axapi::MarketDataField* t_currentPrice = m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID);
                    // û������ʱ������
                    if (t_currentPrice == NULL)
                    {
                        continue;
                    }

                    switch (m_vConfirmedHoldTrade[i].Direction)
                    {
                    case THOST_FTDC_D_Buy:
                    {
                        //std::cout << "���:��ƽ�ֵ�" << m_vConfirmedHoldTrade[i].InstrumentID << "|" << m_vConfirmedHoldTrade[i].AvailableVolumn << "|" << t_currentPrice->LastPrice << "|" << t_currentPrice->AskPrice1 << "|" << t_currentPrice->BidPrice1 << std::endl;
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:�����ƽ��TradeID(%s),InstrumentID(%s),AvailableVolumn(%d),LastPrice(%lf),AskPrice1(%lf),BidPrice1(%lf)", t_strLogFuncName, m_vConfirmedHoldTrade[i].TradeID, m_vConfirmedHoldTrade[i].InstrumentID, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->LastPrice, t_currentPrice->AskPrice1, t_currentPrice->BidPrice1);
                        LOG4CPLUS_INFO(m_objLogger, t_strLog);
                        if (m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID, ORDER_DIRECTION_SELL, ORDER_OFFSETFLAG_OFFSET_TODAY, ORDER_AGAINSTPRICE, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->BidPrice1) >= 0)
                        {
                        }

                    }
                    break;
                    case THOST_FTDC_D_Sell:
                    {
                        //std::cout << "���:��ƽ�ֵ�" << m_vConfirmedHoldTrade[i].InstrumentID << "|" << m_vConfirmedHoldTrade[i].AvailableVolumn << "|" << t_currentPrice->LastPrice << "|" << t_currentPrice->AskPrice1 << "|" << t_currentPrice->BidPrice1 << std::endl;
                        sprintf_s(t_strLog, sizeof(t_strLog), "%s:�����ƽ��TradeID(%s),InstrumentID(%s),AvailableVolumn(%d),LastPrice(%lf),AskPrice1(%lf),BidPrice1(%lf)", t_strLogFuncName, m_vConfirmedHoldTrade[i].TradeID, m_vConfirmedHoldTrade[i].InstrumentID, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->LastPrice, t_currentPrice->AskPrice1, t_currentPrice->BidPrice1);
                        LOG4CPLUS_INFO(m_objLogger, t_strLog);
                        if (m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID, ORDER_DIRECTION_BUY, ORDER_OFFSETFLAG_OFFSET_TODAY, ORDER_AGAINSTPRICE, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->AskPrice1) >= 0)
                        {
                        }
                    }
                    break;
                    }
                }
            }
            sprintf_s(t_strLog, sizeof(t_strLog), "%s:stopped", t_strLogFuncName);
            LOG4CPLUS_INFO(m_objLogger, t_strLog);

            SetEvent(m_hOffsetAllFinished);
            return;
        }
        Sleep(100);
    }
}

bool axapi::Strategy::strategyHoldCompare()
{
    char *t_strLogFuncName = "Strategy::strategyHoldCompare";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    std::hash_map<std::string, UniversalChinaFutureTdVolumnType> t_hashAPIPosition;
    std::hash_map<std::string, UniversalChinaFutureTdVolumnType> t_hashPosition;
    APINamespace CThostFtdcInvestorPositionDetailField t_objPositionDetailInfo;

    int t_nRequestID = m_pTrade->queryCustSTKHoldDetail();
    while (!m_pTrade->checkCompletedQueryRequestID(t_nRequestID))
    {
        std::cout << "waiting for position detail infomation" << std::endl;
        Sleep(100);
    }
    int t_positiondetailsize = m_pTrade->sizePositionDetailList();
    std::cout << "OffsetALL:sizePositionDetailList=" << t_positiondetailsize << std::endl;
    for (int i = 1; i <= t_positiondetailsize; i++)
    {
        t_objPositionDetailInfo = m_pTrade->getPositionDetailInfo(i);
        if (t_hashAPIPosition.find(t_objPositionDetailInfo.InstrumentID) == t_hashAPIPosition.end())
        {
            t_hashAPIPosition[t_objPositionDetailInfo.InstrumentID] = t_objPositionDetailInfo.Volume;
        }
        else
        {
            t_hashAPIPosition[t_objPositionDetailInfo.InstrumentID] += t_objPositionDetailInfo.Volume;
        }
    }

    for (int i = 0; i < m_vConfirmedHoldTrade.size(); i++)
    {
        if (m_vConfirmedHoldTrade[i].TradeStatus == TradeStatus_Hold)
        {
            if (t_hashPosition.find(m_vConfirmedHoldTrade[i].InstrumentID) == t_hashPosition.end())
            {
                t_hashPosition[m_vConfirmedHoldTrade[i].InstrumentID] = m_vConfirmedHoldTrade[i].AvailableVolumn;
            }
            else
            {
                t_hashPosition[m_vConfirmedHoldTrade[i].InstrumentID] += m_vConfirmedHoldTrade[i].AvailableVolumn;
            }
        }
    }

    /// �����һ��
    for (std::hash_map<std::string, UniversalChinaFutureTdVolumnType>::iterator i = t_hashPosition.begin(); i != t_hashPosition.end(); i++)
    {
        if (t_hashAPIPosition.find(i->first) == t_hashAPIPosition.end())
        {
            std::cout << i->first.c_str() << ":" << i->second << "<->" << 0 << std::endl;
        }
        else
        {
            std::cout << i->first.c_str() << ":" << i->second << "<->" << t_hashAPIPosition[i->first] << std::endl;
        }
    }
    return true;
}

bool axapi::Strategy::getOpenRunning()
{
    return m_blOpenRunning;
}

bool axapi::Strategy::getOffsetRunning()
{
    return m_blOffsetRunning;
}

#ifdef STRATEGY_EXE
void axapi::Strategy::myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice)
{
    char *t_strLogFuncName = "Strategy::myStrategy";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    *ot_blOpenFlag = false;

    char *t_instrument = "i1906";

    m_pMarketQuotation->subMarketDataSingle(t_instrument);
    if (m_nOpenCount < 3)
    {
        Sleep(1000);
        *ot_strOpenMsg = "test open";
        sprintf_s(ot_strContract, 10, t_instrument);
        *ot_nDirection = ORDER_DIRECTION_SELL;
        *ot_nOffsetFlag = ORDER_OFFSETFLAG_OPEN;
        *ot_nOrderTypeFlag = ORDER_LIMITPRICE;
        *ot_nOrderAmount = 1;
        if (m_pMarketQuotation->getCurrentPrice(t_instrument) == NULL)
        {
            return;
        }
        MarketDataField *p_price = m_pMarketQuotation->getCurrentPrice(t_instrument);
        *ot_dOrderPrice = p_price->BidPrice1;
        if (*ot_dOrderPrice <= 0)
        {
            return;
        }
        *ot_blOpenFlag = true;
        m_nOpenCount++;
    }
}
#endif STRATEGY_EXE

#ifdef STRATEGY_EXE
void axapi::Strategy::myOffsetStrategy(ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg)
{
    char *t_strLogFuncName = "Strategy::myOffsetStrategy";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    *ot_blOffsetFlag = false;
    if (in_objHoldTrade.SPOrderID[0] != '\0')
    {
        *ot_strOffsetMsg = "SPOrader���趨";
        *ot_blOffsetFlag = true;
    }
}
#endif STRATEGY_EXE

#ifdef STRATEGY_EXE
void axapi::Strategy::getPreOffsetPrice(ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice)
{
    char *t_strLogFuncName = "Strategy::getPreOffsetPrice";
    char t_strLog[500];
    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strLogFuncName);
    LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    *ot_blSPOffsetFlag = false;

    *ot_dbSPOffsetPrice = in_objHoldTrade.Price - 10;
    *ot_blSPOffsetFlag = true;
}
#endif STRATEGY_EXE