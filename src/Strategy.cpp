#define STRATEGY_EXE
#define CTP_TRADEAPI
//#define KSV6T_TRADEAPI
#define KLINESTORAGE
#define MEMORYDATA
#include "Strategy.h"
#include <TradeAPITypeDefine.h>
#include <iostream>
#include <thread>
#include <DataIntoFiles\DataIntoFiles.h>

int axapi::Strategy::initializeAPI(axapi::MarketQuotationAPI *in_pMarketQuotation, axapi::TradeAPI *in_pTrade)
{
    return setAPI(in_pMarketQuotation, in_pTrade);
}

int axapi::Strategy::setAPI(axapi::MarketQuotationAPI *in_pMarketQuotation, axapi::TradeAPI *in_pTrade)
{
    if (in_pMarketQuotation == NULL || in_pTrade == NULL)
    {
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

    m_nCancelWaitSeconds = 10;
    m_nUpdateOrderTimes = 0;
    m_nUpdateTradeTimes = 0;
}

axapi::Strategy::~Strategy(void)
{
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
    ///*
    //* ����ǰ�������ݴ��������ļ�,�����ļ���ʱ������
    //*/
    //char* t_strLogFuncName = "MarketQuotationIndex::saveData";
    //char t_strLog[500];
    //sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);

    //time_t nowtime;
    //tm *curtime;
    //nowtime = time(NULL);
    //curtime = localtime(&nowtime);
    //char t_datafile[50];
    //memset(&t_datafile, '\0', sizeof(t_datafile));
    //sprintf_s(t_datafile, sizeof(t_datafile), "datafield%d%d%d%d%d%d.dat", curtime->tm_year, curtime->tm_mon + 1, curtime->tm_mday, curtime->tm_hour, curtime->tm_min, curtime->tm_sec);

    ///*
    //* д���ļ�
    //*/
    //DataIntoFiles *t_pDataIntoFiles = new DataIntoFiles(t_datafile, 50, "datafile.list");
    ///// ��¼m_hashAllOrder
    //for (std::hash_map<std::string/*UniversalChinaFutureTdOrderRefType*/, struct AllOrder>::iterator i = m_hashAllOrder.begin();
    //    i != m_hashAllOrder.end(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_hashAllOrder[%s]:OrderID(%s),OrderRef(%s),OrderType(%c),HoldTradeID(%s),updateOrderRoundSequence(%d)", i->first, i->second.OrderID, i->second.OrderRef, i->second.OrderType, i->second.HoldTradeID, i->second.updateOrderRoundSequence);
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
    ///// ��¼m_hashConfirmedOrder
    //for (std::hash_map<std::string/*UniversalChinaFutureTdOrderIDType*/, UniversalChinaFutureTdSequenceType>::iterator i = m_hashConfirmedOrder.begin();
    //    i != m_hashConfirmedOrder.end(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_hashConfirmedOrder[%s]:Sequence(%d)", i->first, i->second);
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
    ///// ��¼m_vConfirmedOrder
    //for (int i = 0; i < m_vConfirmedOrder.size(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_vConfirmedOrder[%d]:OrderID(%s),HoldTradeID(%s),OrderRef(%s),InsertTime(%s),\
    //                        OrderStatus(%c),OrderType(%c),OrderPrice(%lf),OrderVolumeTotal(%d),\
    //                        OrderVolumeTraded(%d),OrderVolumnOriginal(%d),TradeIDList.size(%d)",
    //        i, m_vConfirmedOrder[i].OrderID, m_vConfirmedOrder[i].HoldTradeID, m_vConfirmedOrder[i].OrderRef, m_vConfirmedOrder[i].InsertTime,
    //        m_vConfirmedOrder[i].OrderStatus, m_vConfirmedOrder[i].OrderType, m_vConfirmedOrder[i].OrderPrice, m_vConfirmedOrder[i].OrderVolumeTotal,
    //        m_vConfirmedOrder[i].OrderVolumeTraded, m_vConfirmedOrder[i].OrderVolumnOriginal, m_vConfirmedOrder[i].TradeIDList.size());
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
    ///// ��¼m_hashConfirmedHoldTrade
    //for (std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, UniversalChinaFutureTdSequenceType>::iterator i = m_hashConfirmedHoldTrade.begin();
    //    i != m_hashConfirmedHoldTrade.end(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_hashConfirmedHoldTrade[%s]:Sequence(%d)", i->first, i->second);
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
    ///// ��¼m_vConfirmedHoldTrade
    //for (int i = 0; i < m_vConfirmedHoldTrade.size(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_vConfirmedHoldTrade[%d]:TradeID(%s),TradeStatus(%d),InstrumentID(%s),Direction(%d),\
    //                        Price(%lf),Volumn(%d),OffsetVolumn(%d),AvailableVolumn(%d),\
    //                        SPOrderID(%s),SLOrderID.size(%d)",
    //        i, m_vConfirmedHoldTrade[i].TradeID, m_vConfirmedHoldTrade[i].TradeStatus, m_vConfirmedHoldTrade[i].InstrumentID, m_vConfirmedHoldTrade[i].Direction,
    //        m_vConfirmedHoldTrade[i].Price, m_vConfirmedHoldTrade[i].Volumn, m_vConfirmedHoldTrade[i].OffsetVolumn, m_vConfirmedHoldTrade[i].AvailableVolumn,
    //        m_vConfirmedHoldTrade[i].SPOrderID, m_vConfirmedHoldTrade[i].SLOrderID.size());
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
    ///// ��¼m_hashUnpairedOffsetOrder
    //for (std::hash_map<std::string/*UniversalChinaFutureTdTradeIDType*/, struct UnpairedOffsetOrder>::iterator i = m_hashUnpairedOffsetOrder.begin();
    //    i != m_hashUnpairedOffsetOrder.end(); i++)
    //{
    //    sprintf_s(t_strLog, 500, "m_hashUnpairedOffsetOrder[%s]:TradeID(%s),OrderRef(%s),OffsetOrderType(%c)", i->first, i->second.TradeID, i->second.OrderRef, i->second.OffsetOrderType);
    //    t_pDataIntoFiles->writeData2File(t_strLog);
    //}
}

void axapi::Strategy::startStrategy()
{
    char* t_strLogFuncName = "MarketQuotationIndex::startStrategy";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);

    /*
    * ��ֹ������е��б�
    */
    if (m_blStrategyRunning)
    {
        return;
    }

    /// ���������в���
    std::thread autorun_updateOrderInfo(&Strategy::updateOrderInfo, this);
    std::thread autorun_updateTradeInfo(&Strategy::updateTradeInfo, this);
    std::thread autorun_strategyOrder(&Strategy::strategyOrder, this);
    std::thread autorun_strategyOffset(&Strategy::strategyOffset, this);
    std::thread autorun_strategyOffsetALL(&Strategy::strategyOffsetALL, this);
    std::thread autorun_strategyCancelOrder(&Strategy::strategyCancelOrder, this);
    m_blShutdownFlag = false;
    m_blOffsetALLRunning = false;
    m_blCancelRunning = true;
    m_blOffsetRunning = true;
    m_blOpenRunning = true;
    m_blStrategyRunning = true;
    autorun_updateOrderInfo.detach();
    autorun_updateTradeInfo.detach();
    autorun_strategyOrder.detach();
    autorun_strategyOffset.detach();
    autorun_strategyOffsetALL.detach();
    autorun_strategyCancelOrder.detach();
}

void axapi::Strategy::stopStrategy()
{
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
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::updateOrderInfo";

    APINamespace CThostFtdcOrderField t_objAPIOrderInfo;
    memset(&t_objAPIOrderInfo, '\0', sizeof(t_objAPIOrderInfo));

    while (!m_blShutdownFlag)
    {/*
        m_nUpdateOrderTimes += 1;
        int t_ordersize = m_pTrade->sizeOrderList();
        sprintf_s(t_strLog, "CancelOrder:sizeOrderList=%d", t_ordersize);

        for (int i = 1; i <= m_pTrade->sizeOrderList() && !m_blShutdownFlag; i++)
        {
            t_objAPIOrderInfo = m_pTrade->getOrderInfo(i);
            if (t_objAPIOrderInfo.OrderSysID[0] == '\0')
            {
                /// ������Χ
                continue;
            }

            /// ����m_hashAllOrder,m_hashConfirmedOrder,m_vConfirmedOrder
            if (m_hashConfirmedOrder.find(t_objAPIOrderInfo.OrderSysID) == m_hashConfirmedOrder.end())
                /// ��ǰί��δ��ȷ��,��m_hashAllOrder��δ���¹���Ϣ,���½�m_hashConfirmedOrder,m_vConfirmedOrder
            {
                /// ����m_hashAllOrder��,���ʾt_objAPIOrderInfo�����ڸò�����Ϊ
                if (m_hashAllOrder.find(t_objAPIOrderInfo.OrderRef) != m_hashAllOrder.end())
                {
                    /// ����m_hashAllOrder
                    strcpy_s(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID, sizeof(UniversalChinaFutureTdOrderIDType), t_objAPIOrderInfo.OrderSysID);

                    ConfirmedOrder t_objConfirmedOrder;
                    memset(&t_objConfirmedOrder, '\0', sizeof(t_objConfirmedOrder));
                    strcpy_s(t_objConfirmedOrder.OrderID, sizeof(t_objConfirmedOrder.OrderID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                    strcpy_s(t_objConfirmedOrder.HoldTradeID, sizeof(t_objConfirmedOrder.HoldTradeID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID);
                    strcpy_s(t_objConfirmedOrder.OrderRef, sizeof(t_objConfirmedOrder.OrderRef), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderRef);
                    t_objConfirmedOrder.OrderType = m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderType;
                    t_objConfirmedOrder.OrderPrice = t_objAPIOrderInfo.LimitPrice;
                    t_objConfirmedOrder.OrderVolumnOriginal = t_objAPIOrderInfo.VolumeTotalOriginal;
                    t_objConfirmedOrder.OrderVolumeTraded = t_objAPIOrderInfo.VolumeTraded;
                    t_objConfirmedOrder.OrderVolumeTotal = t_objAPIOrderInfo.VolumeTotal;
                    t_objConfirmedOrder.OrderStatus = t_objAPIOrderInfo.OrderStatus;
                    strcpy_s(t_objConfirmedOrder.InsertTime, sizeof(t_objConfirmedOrder.InsertTime), t_objAPIOrderInfo.InsertDate);
                    t_objConfirmedOrder.TradeIDList.clear();
                    /// ����m_hashConfirmedOrder
                    m_hashConfirmedOrder[t_objConfirmedOrder.OrderID] = m_vConfirmedOrder.size();
                    /// ����m_vConfirmedOrder
                    m_vConfirmedOrder.push_back(t_objConfirmedOrder);

                    /// ��ǰί�д�����m_hashUnpairedOffsetOrder��,��Ҫ����ƥ��
                    if (m_hashUnpairedOffsetOrder.find(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID) != m_hashUnpairedOffsetOrder.end())
                    {
                        /// ��ǰί�ж�Ӧ�����ѱ�ȷ��,���Խ���ƥ�����
                        if (m_hashConfirmedHoldTrade.find(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID) != m_hashConfirmedHoldTrade.end())
                        {
                            int t_HoldTradeIndex = m_hashConfirmedHoldTrade[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID];
                            /// ֹӯƽ�ֵ���Ӧ������
                            if (m_hashUnpairedOffsetOrder[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID].OffsetOrderType == OrderOffsetType_SP)
                            {
                                strcpy_s(m_vConfirmedHoldTrade[t_HoldTradeIndex].SPOrderID, sizeof(m_vConfirmedHoldTrade[t_HoldTradeIndex].SPOrderID), m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                            }
                            /// ֹ��ƽ�ֵ���Ӧ������
                            else if (m_hashUnpairedOffsetOrder[m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID].OffsetOrderType == OrderOffsetType_SL)
                            {
                                m_vConfirmedHoldTrade[t_HoldTradeIndex].SLOrderID.push_back(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].OrderID);
                            }
                            /// ����m_hashUnpairedOffsetOrder�е���Ϣ
                            m_hashUnpairedOffsetOrder.erase(m_hashAllOrder[t_objAPIOrderInfo.OrderRef].HoldTradeID);
                        }
                    }
                }
            }
            /// ��ǰί���Ѿ�ȷ�ϴ���,��m_hashAllOrder,m_hashConfirmedOrder�Ѿ����¹���Ϣ,�����m_vConfirmedOrder
            else
            {
                int t_ConfirmedOrderIndex = m_hashConfirmedOrder[t_objAPIOrderInfo.OrderSysID];
                m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderPrice = t_objAPIOrderInfo.LimitPrice;
                m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumnOriginal = t_objAPIOrderInfo.VolumeTotalOriginal;
                m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumeTraded = t_objAPIOrderInfo.VolumeTraded;
                m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderVolumeTotal = t_objAPIOrderInfo.VolumeTotal;
                m_vConfirmedOrder[t_ConfirmedOrderIndex].OrderStatus = t_objAPIOrderInfo.OrderStatus;
            }
        }*/
    }
    SetEvent(m_hUpdateOrderFinished);
}

void axapi::Strategy::updateTradeInfo()
{
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::updateTradeInfo";

    TradeField t_objAPITradeInfo;
    memset(&t_objAPITradeInfo, '\0', sizeof(t_objAPITradeInfo));

    while (!m_blShutdownFlag)
    {
        //m_nUpdateTradeTimes += 1;
        //int t_tradesize = m_pTrade->sizeTradeList();
        //sprintf_s(t_strLog, "Offset:sizeTradeList=%d", t_tradesize);

        //for (int i = 1; i <= t_tradesize && m_blShutdownFlag; i++)
        //{
        //    t_objAPITradeInfo = m_pTrade->getTradeInfo(i);
        //    if (t_objAPITradeInfo.apiTradeField.TradeID[0] == '\0')
        //    {
        //        //������Χ
        //        continue;
        //    }

        //    /// ����m_vConfirmedOrder,m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
        //    if (m_hashConfirmedHoldTrade.find(t_objAPITradeInfo.apiTradeField.TradeID) == m_hashConfirmedHoldTrade.end())
        //        /*
        //        * ��ǰ�ɽ�δ��ȷ��,m_hashConfirmedHoldTrade�в�����
        //        * ���Ϊƽ����ֻ����m_vConfirmedOrder
        //        * ���Ϊ���������m_vConfirmedOrder,�½�m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
        //        */
        //    {
        //        /// ����m_hashAllOrder��,���ʾt_objAPITradeInfo�����ڸò�����Ϊ
        //        if (m_hashAllOrder.find(t_objAPITradeInfo.apiTradeField.OrderRef) != m_hashAllOrder.end())
        //        {
        //            /// m_hashConfirmedOrder��δȷ�����,���յ��˳ɽ���Ϣ,�������ȴ�m_hashConfirmedOrderȷ�����
        //            if (m_hashConfirmedOrder.find(m_hashAllOrder[t_objAPITradeInfo.apiTradeField.OrderRef].OrderID) == m_hashConfirmedOrder.end())
        //            {
        //                continue;
        //            }
        //            else
        //            {
        //                /// ����m_vConfirmedOrder,����Ѹ���������
        //                int t_index = m_hashConfirmedOrder[m_hashAllOrder[t_objAPITradeInfo.apiTradeField.OrderRef].OrderID];
        //                for (int j = 0; j < m_vConfirmedOrder[t_index].TradeIDList.size(); j++)
        //                {
        //                    /// m_vConfirmedOrder���Ѹ��¹�TradeID,������
        //                    if (strcmp(m_vConfirmedOrder[t_index].TradeIDList[j], t_objAPITradeInfo.apiTradeField.TradeID) == 0)
        //                    {
        //                        break;
        //                    }
        //                    /// m_vConfirmedOrder��δ���¹�TradeID,�����
        //                    else
        //                    {
        //                        if (j == m_vConfirmedOrder[t_index].TradeIDList.size() - 1)
        //                        {
        //                            m_vConfirmedOrder[t_index].TradeIDList.push_back(t_objAPITradeInfo.apiTradeField.TradeID);
        //                        }
        //                    }
        //                }


        //                /// ���Ϊ�������½�m_hashConfirmedHoldTrade,m_vConfirmedHoldTrade
        //                if (t_objAPITradeInfo.apiTradeField.OffsetFlag == THOST_FTDC_OF_Open)
        //                {
        //                    ConfirmedHoldTrade t_objConfirmedHoldTrade;
        //                    memset(&t_objConfirmedHoldTrade, '\0', sizeof(t_objConfirmedHoldTrade));
        //                    strcpy_s(t_objConfirmedHoldTrade.TradeID, sizeof(t_objConfirmedHoldTrade.TradeID), t_objAPITradeInfo.apiTradeField.TradeID);
        //                    strcpy_s(t_objConfirmedHoldTrade.InstrumentID, sizeof(t_objConfirmedHoldTrade.InstrumentID), t_objAPITradeInfo.apiTradeField.InstrumentID);
        //                    t_objConfirmedHoldTrade.Direction = t_objAPITradeInfo.apiTradeField.Direction;
        //                    t_objConfirmedHoldTrade.Price = t_objAPITradeInfo.apiTradeField.Price;
        //                    t_objConfirmedHoldTrade.Volumn = t_objAPITradeInfo.apiTradeField.Volume;
        //                    t_objConfirmedHoldTrade.AvailableVolumn = t_objAPITradeInfo.apiTradeField.Volume;
        //                    t_objConfirmedHoldTrade.OffsetVolumn = 0;
        //                    t_objConfirmedHoldTrade.TradeStatus = TradeStatus_Hold;
        //                    t_objConfirmedHoldTrade.SLOrderID.clear();

        //                    /// ����m_hashConfirmedHoldTrade
        //                    m_hashConfirmedHoldTrade[t_objAPITradeInfo.apiTradeField.TradeID] = m_vConfirmedHoldTrade.size();
        //                    /// ����m_vConfirmedHoldTrade
        //                    m_vConfirmedHoldTrade.push_back(t_objConfirmedHoldTrade);
        //                }
        //            }
        //        }
        //    }
        //    /*
        //    * ��ǰ�ɽ��ѱ�ȷ����ͨ��ƽ����Ϣ���³ֲ�����
        //    */
        //    else
        //    {
        //        updateHoldTrade(m_hashConfirmedHoldTrade[t_objAPITradeInfo.apiTradeField.TradeID]);
        //    }
        //}
    }
    SetEvent(m_hUpdateTradeFinished);
}

void axapi::Strategy::updateHoldTrade(UniversalChinaFutureTdSequenceType in_HoldTradeIndex)
{
    //char t_strLog[500];
    //char* t_strLogFuncName = "Strategy::updateHoldTrade";

    //UniversalChinaFutureTdVolumnType t_HoldTradeOffsetVolumn = 0;

    ///// �ֲ���ȫƽ������
    //if (m_vConfirmedHoldTrade[in_HoldTradeIndex].TradeStatus == TradeStatus_OffsetALL)
    //{
    //    return;
    //}

    ///// SPƽ����Ϣ�������������
    //if (m_vConfirmedHoldTrade[in_HoldTradeIndex].SPOrderID[0] != '\0')
    //{
    //    int t_SPOrderIndex = m_hashConfirmedOrder[m_vConfirmedHoldTrade[in_HoldTradeIndex].SPOrderID];
    //    /// ����ί��������ʲôֻ��¼�Ѿ��ɽ��Ĳ���
    //    t_HoldTradeOffsetVolumn += m_vConfirmedOrder[t_SPOrderIndex].OrderVolumeTraded;
    //}

    ///// SLƽ����Ϣ�������������
    //for (int i = 0; i < m_vConfirmedHoldTrade[in_HoldTradeIndex].SLOrderID.size(); i++)
    //{
    //    int t_SLOrderIndex = m_hashConfirmedOrder[m_vConfirmedHoldTrade[in_HoldTradeIndex].SLOrderID[i]];
    //    /// ����ί��������ʲôֻ��¼�Ѿ��ɽ��Ĳ���
    //    t_HoldTradeOffsetVolumn += m_vConfirmedOrder[t_SLOrderIndex].OrderVolumeTraded;
    //}

    ///*
    //* ����OffsetVolumn,AvailableVolumn
    //* ���Volumn==OffsetVolumn,���ȫ��ƽ��,���³ֲ�״̬
    //*/
    //m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn = t_HoldTradeOffsetVolumn;
    //m_vConfirmedHoldTrade[in_HoldTradeIndex].AvailableVolumn = m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn - m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn < 0 ? 0 : m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn - m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn;
    //m_vConfirmedHoldTrade[in_HoldTradeIndex].TradeStatus = m_vConfirmedHoldTrade[in_HoldTradeIndex].Volumn == m_vConfirmedHoldTrade[in_HoldTradeIndex].OffsetVolumn ? TradeStatus_OffsetALL : TradeStatus_Hold;
}

void axapi::Strategy::strategyOrder()
{
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::strategyOrder";

    bool t_blOpenFlag = false;
    std::string t_strOffsetMessage;
    char t_strContract[20];
    memset(t_strContract, '\0', sizeof(t_strContract));
    int t_nDirection, t_nOffsetFlag, t_nOrderTypeFlag, t_nOrderAmount;
    double t_dOrderPrice;
    long t_plOpenOrderRef;
    UniversalChinaFutureTdOrderRefType t_objOpenOrderRef;

    while (m_blOpenRunning)
    {
        //myStrategy(&t_blOpenFlag, &t_strOffsetMessage, t_strContract, &t_nDirection, &t_nOffsetFlag, &t_nOrderTypeFlag, &t_nOrderAmount, &t_dOrderPrice);

        //if (t_blOpenFlag)
        //{
        //    sprintf_s(t_strLog, sizeof(t_strLog), "%s", t_strOffsetMessage.c_str());
        //    m_pTrade->MyOrdering(t_strContract, t_nDirection, t_nOffsetFlag, t_nOrderTypeFlag, t_nOrderAmount, t_dOrderPrice, &t_plOpenOrderRef);
        //    sprintf_s(t_objOpenOrderRef, sizeof(t_objOpenOrderRef), "%ld", t_plOpenOrderRef);

        //    /// ������Ϣ����m_hashAllOrder,�ȴ�����ȷ��
        //    AllOrder t_objAllOrder;
        //    memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
        //    strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objOpenOrderRef);
        //    t_objAllOrder.OrderType = OrderStatus_Open;
        //    t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

        //    m_hashAllOrder[t_objOpenOrderRef] = t_objAllOrder;
        //}
    }
    SetEvent(m_hOpenFinished);
}

void axapi::Strategy::strategyOffset()
{
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::strategyOffset";

    bool t_blOffsetFlag;
    std::string t_strOffsetMessage;

    while (m_blOffsetRunning)
    {
        ///// �������еĳֲ�,�ҵ���δƽ�ֵĳֲ�
        //for (int i = 0; m_blOffsetRunning && i < m_vConfirmedHoldTrade.size(); i++)
        //{
        //    t_blOffsetFlag = false;
        //    t_strOffsetMessage.clear();
        //    if (m_vConfirmedHoldTrade[i].TradeStatus == TradeStatus_Hold)
        //    {
        //        myOffsetStrategy(m_vConfirmedHoldTrade[i], &t_blOffsetFlag, &t_strOffsetMessage);
        //        /// ��Ҫ����ƽ��
        //        if (t_blOffsetFlag)
        //        {
        //            /// �����ֹӯ��ֹӯ�� ��ֹ��
        //            if (m_vConfirmedHoldTrade[i].SPOrderID[0] != '\0')
        //            {
        //                m_pTrade->MyCancelOrder(m_vConfirmedHoldTrade[i].SPOrderID);

        //                m_vConfirmedHoldTrade[i].SLOrderID.push_back(m_vConfirmedHoldTrade[i].SPOrderID);
        //                memset(&m_vConfirmedHoldTrade[i].SPOrderID, '\0', sizeof(m_vConfirmedHoldTrade[i].SPOrderID));
        //            }
        //            /// ����¹�ֹӯ��������δȷ����ɾ�������Ϣ
        //            else if (m_hashUnpairedOffsetOrder.find(m_vConfirmedHoldTrade[i].TradeID) != m_hashUnpairedOffsetOrder.end()
        //                && m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID].OffsetOrderType == OrderOffsetType_SP)
        //            {
        //                /// UnpairedOffsetOrder�е�δƥ��ί���Ѿ�������ƥ����δƥ����,��ֱ��ɾ��
        //                if (m_hashAllOrder[m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID].OrderRef].updateOrderRoundSequence >= m_nUpdateOrderTimes + 2)
        //                {
        //                    m_hashUnpairedOffsetOrder.erase(m_vConfirmedHoldTrade[i].TradeID);
        //                }
        //                else
        //                {
        //                    continue;
        //                }
        //            }

        //            /// ȷ��û��ֹӯ��,��ֱ����ֹ��
        //            UniversalChinaFutureTdOrderRefType t_objSLOrderRef;
        //            long t_lSLOrderRef;
        //            m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID,
        //                m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY,
        //                ORDER_OFFSETFLAG_OFFSET_TODAY,
        //                ORDER_LIMITPRICE,
        //                m_vConfirmedHoldTrade[i].AvailableVolumn,
        //                m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID)->BidPrice1 : m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID)->AskPrice1,
        //                &t_lSLOrderRef);
        //            sprintf_s(t_objSLOrderRef, sizeof(t_objSLOrderRef), "%ld", t_lSLOrderRef);

        //            /// ֹ��ƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
        //            UnpairedOffsetOrder t_objUnpairedOffsetOrder;
        //            memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
        //            strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedHoldTrade[i].TradeID);
        //            strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSLOrderRef);
        //            t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SL;

        //            m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;

        //            /// ֹӯƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
        //            AllOrder t_objAllOrder;
        //            memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
        //            strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedHoldTrade[i].TradeID);
        //            strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSLOrderRef);
        //            t_objAllOrder.OrderType = OrderStatus_SLOffset;
        //            t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

        //            m_hashAllOrder[t_objSLOrderRef] = t_objAllOrder;
        //        }
        //        /// �������Ҫƽ��
        //        else
        //        {
        //            /// û��ֹӯ,��ֹӯ
        //            if (m_vConfirmedHoldTrade[i].SPOrderID[0] == '\0'
        //                && m_vConfirmedHoldTrade[i].SLOrderID.size() == 0
        //                && m_hashUnpairedOffsetOrder.find(m_vConfirmedHoldTrade[i].TradeID) == m_hashUnpairedOffsetOrder.end())
        //            {
        //                bool t_blSPOffsetFlag = false;
        //                double t_dbSPOrderPrice;
        //                getPreOffsetPrice(m_vConfirmedHoldTrade[i], &t_blSPOffsetFlag, &t_dbSPOrderPrice);
        //                if (t_blSPOffsetFlag)
        //                {
        //                    UniversalChinaFutureTdOrderRefType t_objSPOrderRef;
        //                    long t_lSPOrderRef;

        //                    /// �µ����OrderRef
        //                    m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID,
        //                        (m_vConfirmedHoldTrade[i].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY),
        //                        ORDER_OFFSETFLAG_OFFSET_TODAY,
        //                        ORDER_LIMITPRICE,
        //                        m_vConfirmedHoldTrade[i].AvailableVolumn,
        //                        t_dbSPOrderPrice,
        //                        &t_lSPOrderRef);
        //                    sprintf_s(t_objSPOrderRef, sizeof(t_objSPOrderRef), "%ld", t_lSPOrderRef);

        //                    /// ֹӯƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
        //                    UnpairedOffsetOrder t_objUnpairedOffsetOrder;
        //                    memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
        //                    strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedHoldTrade[i].TradeID);
        //                    strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSPOrderRef);
        //                    t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SP;

        //                    m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;

        //                    /// ֹӯƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
        //                    AllOrder t_objAllOrder;
        //                    memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
        //                    strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedHoldTrade[i].TradeID);
        //                    strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSPOrderRef);
        //                    t_objAllOrder.OrderType = OrderStatus_SPOffset;
        //                    t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;

        //                    m_hashAllOrder[t_objSPOrderRef] = t_objAllOrder;
        //                }
        //            }
        //        }
        //    }
        //}
    }
    SetEvent(m_hOffsetFinished);
}

void axapi::Strategy::strategyCancelOrder()
{
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::strategyCancelOrder";

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
//            for (int i = 0; m_blOffsetRunning && i < m_vConfirmedOrder.size(); i++)
//            {
//                switch (m_vConfirmedOrder[i].OrderStatus)
//                {
//                case THOST_FTDC_OST_PartTradedQueueing:
//                case THOST_FTDC_OST_NoTradeQueueing:
//                {
//                    /// Ԥ��ֹӯ������
//                    if (m_vConfirmedOrder[i].OrderType == OrderStatus_SPOffset)
//                    {
//                        continue;
//                    }
//                    /// ֹ�������µ�
//                    else if (m_vConfirmedOrder[i].OrderType == OrderStatus_SLOffset || m_vConfirmedOrder[i].OrderType == OrderStatus_Open)
//                    {
//                        /*
//                        * �Ƿ񳷵�
//                        * 1.n�볷��
//                        */
//#pragma region
//                        nowtime = time(NULL);
//                        curtime = localtime(&nowtime);
//                        t_OrderInfoHour[0] = m_vConfirmedOrder[i].InsertTime[0];
//                        t_OrderInfoHour[1] = m_vConfirmedOrder[i].InsertTime[1];
//                        t_OrderInfoHour[2] = '\0';
//                        t_OrderInfoMinutes[0] = m_vConfirmedOrder[i].InsertTime[3];
//                        t_OrderInfoMinutes[1] = m_vConfirmedOrder[i].InsertTime[4];
//                        t_OrderInfoMinutes[2] = '\0';
//                        t_OrderInfoSeconds[0] = m_vConfirmedOrder[i].InsertTime[6];
//                        t_OrderInfoSeconds[1] = m_vConfirmedOrder[i].InsertTime[7];
//                        t_OrderInfoSeconds[2] = '\0';
//                        if (curtime->tm_hour < 20)
//                        {
//                            curMinutes = (curtime->tm_hour) * 3600 + curtime->tm_min * 60 + curtime->tm_sec * 1;
//                            t_OrderInsertTime = atoi(t_OrderInfoHour) * 3600 + atoi(t_OrderInfoMinutes) * 60 + atoi(t_OrderInfoSeconds);
//                        }
//                        else
//                        {
//                            curMinutes = (curtime->tm_hour + 0) * 3600 + curtime->tm_min * 60 + curtime->tm_sec * 1;
//                            t_OrderInsertTime = atoi(t_OrderInfoHour) * 3600 + atoi(t_OrderInfoMinutes) * 60 + atoi(t_OrderInfoSeconds);
//                        }
//                        sprintf_s(t_strLog, "curMinutes:%d,|%d:%d:%d", curMinutes, curtime->tm_hour, curtime->tm_min, curtime->tm_sec);
//                        //LOG4CPLUS_DEBUG(g_objLogger_DEBUG, t_strLog);
//                        sprintf_s(t_strLog, "m_vConfirmedOrder[%d]:%d,%s|%s|%s|%s:%s:%s", i, t_OrderInsertTime, m_vConfirmedOrder[i].OrderID, m_vConfirmedOrder[i].OrderRef, m_vConfirmedOrder[i].InsertTime, t_OrderInfoHour, t_OrderInfoMinutes, t_OrderInfoSeconds);
//                        //LOG4CPLUS_DEBUG(g_objLogger_DEBUG, t_strLog);
//                        sprintf_s(t_strLog, "curMinutes:%d,t_OrderInsertTime:%d", curMinutes, t_OrderInsertTime);
//                        //LOG4CPLUS_DEBUG(g_objLogger_DEBUG, t_strLog);
//
//                        if (curMinutes - t_OrderInsertTime >= m_nCancelWaitSeconds)
//                        {
//                            t_CancelAction = true;
//                        }
//                        else
//                        {
//                            t_CancelAction = false;
//                        }
//#pragma endregion
//                        if (t_CancelAction)
//                        {
//                            /// ����
//                            m_pTrade->MyCancelOrder(m_vConfirmedOrder[i].OrderID);
//
//                            /// ERROR ���²���ʱ�����³����ɳ�������������ƽ����ֲ�
//
//                            //�����ֹ��ƽ�ֱ������������
//                            if (m_vConfirmedOrder[i].OrderType == OrderStatus_SLOffset)
//                            {
//                                UniversalChinaFutureTdOrderRefType t_objSLOrderRef;
//                                long t_lSLOrderRef;
//                                m_pTrade->MyOrdering(m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].InstrumentID,
//                                    m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].Direction == THOST_FTDC_D_Buy ? ORDER_DIRECTION_SELL : ORDER_DIRECTION_BUY,
//                                    ORDER_OFFSETFLAG_OFFSET_TODAY,
//                                    ORDER_AGAINSTPRICE,
//                                    m_vConfirmedOrder[i].OrderVolumeTotal,
//                                    m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[m_hashConfirmedHoldTrade[m_vConfirmedOrder[i].HoldTradeID]].InstrumentID)->AskPrice1,
//                                    &t_lSLOrderRef);
//                                sprintf_s(t_objSLOrderRef, sizeof(t_objSLOrderRef), "%ld", t_lSLOrderRef);
//
//                                /// ֹ��ƽ����Ϣ����m_hashUnpairedOffsetOrder,�ȴ�����ƥ��
//                                UnpairedOffsetOrder t_objUnpairedOffsetOrder;
//                                memset(&t_objUnpairedOffsetOrder, '\0', sizeof(t_objUnpairedOffsetOrder));
//                                strcpy_s(t_objUnpairedOffsetOrder.TradeID, sizeof(t_objUnpairedOffsetOrder.TradeID), m_vConfirmedOrder[i].HoldTradeID);
//                                strcpy_s(t_objUnpairedOffsetOrder.OrderRef, sizeof(t_objUnpairedOffsetOrder.OrderRef), t_objSLOrderRef);
//                                t_objUnpairedOffsetOrder.OffsetOrderType = OrderOffsetType_SL;
//
//                                m_hashUnpairedOffsetOrder[m_vConfirmedHoldTrade[i].TradeID] = t_objUnpairedOffsetOrder;
//
//                                /// ֹ��ƽ����Ϣ����m_hashAllOrder,�ȴ�����ȷ��
//                                AllOrder t_objAllOrder;
//                                memset(&t_objAllOrder, '\0', sizeof(t_objAllOrder));
//                                strcpy_s(t_objAllOrder.HoldTradeID, sizeof(t_objAllOrder.HoldTradeID), m_vConfirmedOrder[i].HoldTradeID);
//                                strcpy_s(t_objAllOrder.OrderRef, sizeof(t_objAllOrder.OrderRef), t_objSLOrderRef);
//                                t_objAllOrder.OrderType = OrderStatus_SLOffset;
//                                t_objAllOrder.updateOrderRoundSequence = m_nUpdateOrderTimes;
//
//                                m_hashAllOrder[t_objSLOrderRef] = t_objAllOrder;
//                            }
//                        }
//                    }
//                    break;
//                }
//                default:
//                    break;
//                }
//            }
        }
    }
    catch (std::exception e)
    {
        sprintf_s(t_strLog, "CancelOrder:%s", e.what());
    }

    SetEvent(m_hCancelFinished);
}

void axapi::Strategy::strategyOffsetALL()
{
    char t_strLog[500];
    char* t_strLogFuncName = "Strategy::strategyOffsetALL";

    while (true)
    {
        if (m_blOffsetALLRunning)
        {

            //for (int i = 0; i < m_vConfirmedHoldTrade.size(); i++)
            //{
            //    if (m_vConfirmedHoldTrade[i].TradeStatus == TradeStatus_Hold)
            //    {
            //        axapi::MarketDataField* t_currentPrice = m_pMarketQuotation->getCurrentPrice(m_vConfirmedHoldTrade[i].InstrumentID);
            //        // û������ʱ������
            //        if (t_currentPrice == NULL)
            //        {
            //            continue;
            //        }

            //        switch (m_vConfirmedHoldTrade[i].Direction)
            //        {
            //        case THOST_FTDC_D_Buy:
            //        {
            //            std::cout << "���:��ƽ�ֵ�" << m_vConfirmedHoldTrade[i].InstrumentID << "|" << m_vConfirmedHoldTrade[i].AvailableVolumn << "|" << t_currentPrice->LastPrice << "|" << t_currentPrice->AskPrice1 << "|" << t_currentPrice->BidPrice1 << std::endl;
            //            if (m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID, ORDER_DIRECTION_SELL, ORDER_OFFSETFLAG_OFFSET_TODAY, ORDER_AGAINSTPRICE, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->BidPrice1) >= 0)
            //            {
            //            }

            //        }
            //        break;
            //        case THOST_FTDC_D_Sell:
            //        {
            //            std::cout << "���:��ƽ�ֵ�" << m_vConfirmedHoldTrade[i].InstrumentID << "|" << m_vConfirmedHoldTrade[i].AvailableVolumn << "|" << t_currentPrice->LastPrice << "|" << t_currentPrice->AskPrice1 << "|" << t_currentPrice->BidPrice1 << std::endl;
            //            if (m_pTrade->MyOrdering(m_vConfirmedHoldTrade[i].InstrumentID, ORDER_DIRECTION_BUY, ORDER_OFFSETFLAG_OFFSET_TODAY, ORDER_AGAINSTPRICE, m_vConfirmedHoldTrade[i].AvailableVolumn, t_currentPrice->AskPrice1) >= 0)
            //            {
            //            }
            //        }
            //        break;
            //        }
            //    }
            //}
            SetEvent(m_hOffsetAllFinished);
            return;
        }
        Sleep(100);
    }
}

bool axapi::Strategy::strategyHoldCompare()
{
    /*char t_strLog[500];
    char* t_strLogFuncName = "Strategy::strategyHoldCompare";

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
    }*/
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

void axapi::Strategy::myStrategy(bool *ot_blOpenFlag, std::string *ot_strOpenMsg, char *ot_strContract, int *ot_nDirection, int *ot_nOffsetFlag, int *ot_nOrderTypeFlag, int *ot_nOrderAmount, double *ot_dOrderPrice)
{
    *ot_blOpenFlag = false;
}

void axapi::Strategy::myOffsetStrategy(ConfirmedHoldTrade in_objHoldTrade, bool *ot_blOffsetFlag, std::string *ot_strOffsetMsg)
{
    *ot_blOffsetFlag = false;
}

void axapi::Strategy::getPreOffsetPrice(ConfirmedHoldTrade in_objHoldTrade, bool *ot_blSPOffsetFlag, double *ot_dbSPOffsetPrice)
{
    *ot_blSPOffsetFlag = false;
}