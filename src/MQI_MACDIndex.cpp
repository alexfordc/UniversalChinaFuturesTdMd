#define MQI_MACDINDEX_EXE
#include "MQI_MACDIndex.h"
#include <iostream>
#include <thread>

axapi::MQI_MACDIndex::MQI_MACDIndex()
{
    char* t_strLogFuncName = "MQI_MACDIndex::MQI_MACDIndex";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

axapi::MQI_MACDIndex::~MQI_MACDIndex()
{
    char* t_strLogFuncName = "MQI_MACDIndex::MQI_MACDIndex";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

int axapi::MQI_MACDIndex::initialize(axapi::MarketQuotationAPI *in_pMarketQuotationAPI, std::string in_strContract)
{
    char* t_strLogFuncName = "MarketQuotationIndex::initialize";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    if (m_blAutoRun == true)
    {
        m_blAutoRun = false;
        WaitForSingleObject(m_hCaculateRuning, INFINITE);
    }
    /// ������������򷵻ش���
    if (in_pMarketQuotationAPI == NULL || in_strContract.size() == 0)
    {
        return -100;
    }
    else
    {
        m_pMarketQuotation = in_pMarketQuotationAPI;
        m_n1mKBars = 0;
        m_nBarsDIF = 9;
        m_strContract = in_strContract;
        m_arrayIndexValue.clear();
    }

    /*
    * ���̼߳���ָ��ֵ
    */
    /// ��������
    APINamespace TThostFtdcInstrumentIDType t_chContract;
    memset(t_chContract, '\0', sizeof(t_chContract));
    if (m_strContract.size() >= sizeof(t_chContract))
    {
        return -300;
    }
    else
    {
        m_strContract.copy(t_chContract, m_strContract.size());
        if (in_pMarketQuotationAPI->subMarketDataSingle(t_chContract) < 0)
        {
            return -200;
        }
    }



    m_blAutoRun = true;
    std::thread autorun(&MQI_MACDIndex::caculate, this);
    autorun.detach();
    return 0;
}

/*
* Moving Average Convergence / Divergence
��EMA1�Ĳ���Ϊ12��EMA2�Ĳ���Ϊ26�գ�DIF�Ĳ���Ϊ9��Ϊ��������MACD�ļ������
1�������ƶ�ƽ��ֵ��EMA��
12��EMA����ʽΪ
EMA��12��=ǰһ��EMA��12����11/13+�������̼ۡ�2/13
26��EMA����ʽΪ
EMA��26��=ǰһ��EMA��26����25/27+�������̼ۡ�2/27
2���������ֵ��DIF��
DIF=����EMA��12��������EMA��26��
3������DIF��9��EMA
�������ֵ������9�յ�EMA�������ƽ��ֵ���������MACDֵ��Ϊ�˲���ָ��ԭ�����������ֵ����
DEA��DEM��
����DEA��MACD��=ǰһ��DEA��8/10+����DIF��2/10��
�������DIF��DEA����ֵ��Ϊ��ֵ��ֵ��
�ã�DIF-DEA����2��ΪMACD��״ͼ��
*/
void axapi::MQI_MACDIndex::caculate()
{
    char* t_strLogFuncName = "MQI_MACDIndex::caculate";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    std::string t_strContract = m_strContract;

#pragma region
    /*
    * �Զ���ָ����㲿��
    */
    MQI_MACDIndexField t_objLatestIndexValue;
    t_objLatestIndexValue.BarSerials = NULL;
    t_objLatestIndexValue.indexValue = NULL;

    axapi::KMarketDataField *t_pMarketDataFieldLastest = NULL;

    while (m_blAutoRun)
    {
        sprintf_s(t_strLog, 500, "%s:MACD caculating%s...", t_strLogFuncName, t_strContract.c_str());
        //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

        t_pMarketDataFieldLastest = m_pMarketQuotation->getKLine(t_strContract.c_str(), 1, 0);

        if (t_pMarketDataFieldLastest == NULL)
        {
            continue;
        }
        t_objLatestIndexValue.BarSerials = t_pMarketDataFieldLastest->BarSerials;

        if (m_arrayIndexValue.size() >= 1)
        {
            // BARδǰ������һ����������Ѵ�����
            if (t_objLatestIndexValue.BarSerials == m_arrayIndexValue[m_arrayIndexValue.size() - 1].BarSerials)
            {
                m_arrayIndexValue.pop_back();
            }
        }

        /// ��ǰ���㲻�ǵ�һ��ָ��ֵ
        if (m_arrayIndexValue.size() >= 1)
        {
            t_objLatestIndexValue.EMA12Value = m_arrayIndexValue[m_arrayIndexValue.size() - 1].EMA12Value * 11 / 13 + t_pMarketDataFieldLastest->ClosePrice * 2 / 13;
            t_objLatestIndexValue.EMA26Value = m_arrayIndexValue[m_arrayIndexValue.size() - 1].EMA26Value * 25 / 27 + t_pMarketDataFieldLastest->ClosePrice * 2 / 27;
            t_objLatestIndexValue.DIFValue = t_objLatestIndexValue.EMA12Value - t_objLatestIndexValue.EMA26Value;
            for (int i = 0; i < m_nBarsDIF && i < m_arrayIndexValue.size(); i++)
            {
                t_objLatestIndexValue.DEAValue = m_arrayIndexValue[m_arrayIndexValue.size() - 1 - i].DIFValue
                    + 1 / (m_arrayIndexValue.size() < m_nBarsDIF ? m_arrayIndexValue.size() : m_nBarsDIF);
            }
            t_objLatestIndexValue.indexValue = t_objLatestIndexValue.DEAValue;
        }
        /// ��ǰ����Ϊ��һ��ָ��ֵ
        else
        {
            t_objLatestIndexValue.EMA12Value = t_pMarketDataFieldLastest->ClosePrice * 2 / 13;
            t_objLatestIndexValue.EMA26Value = t_pMarketDataFieldLastest->ClosePrice * 2 / 27;
            t_objLatestIndexValue.DIFValue = t_objLatestIndexValue.EMA12Value - t_objLatestIndexValue.EMA26Value;
            t_objLatestIndexValue.DEAValue = t_objLatestIndexValue.DIFValue;
            t_objLatestIndexValue.indexValue = t_objLatestIndexValue.DEAValue;
        }

        m_arrayIndexValue.push_back(t_objLatestIndexValue);
#pragma endregion

        Sleep(100);
    }

    SetEvent(m_hCaculateRuning);
    sprintf_s(t_strLog, 500, "%s:MACD exit caculate%s", t_strLogFuncName, t_strContract.c_str());
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

double axapi::MQI_MACDIndex::getIndexValue(int in_iCurrentOffset)
{
    char* t_strLogFuncName = "MQI_MACDIndex::getIndexValue";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    /*
    * ��������λ�ó���ָ�����з�Χ���򷵻�NULL
    */
    if (m_arrayIndexValue.size() + in_iCurrentOffset <= 0 || in_iCurrentOffset > 0)
    {
        return NULL;
    }

    /*
    * ����ָ��λ�õ�ָ��ֵ
    */
    return m_arrayIndexValue[m_arrayIndexValue.size() + in_iCurrentOffset - 1].indexValue;
}