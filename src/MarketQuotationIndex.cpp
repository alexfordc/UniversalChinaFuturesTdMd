#define MARKETQUOTATIONINDEX_EXP
#define CTP_TRADEAPI
#include "MarketQuotationIndex.h"
#include <thread>
#include <iostream>

axapi::MarketQuotationIndex::MarketQuotationIndex(void)
{
    m_hCaculateRuning = CreateEvent(NULL, true, false, NULL);
    m_blAutoRun = false;
    m_strContract = "";
    m_n1mKBars = 0;
    m_arrayIndexValue.clear();
    m_pMarketQuotation = NULL;
}

axapi::MarketQuotationIndex::~MarketQuotationIndex(void)
{
    m_blAutoRun = false;
    m_strContract = "";
    m_n1mKBars = 0;
    m_arrayIndexValue.clear();
    m_pMarketQuotation = NULL;
}

/// ����������ݳ�ʼ�������ú�ʼ����ָ��ֵ
int axapi::MarketQuotationIndex::initialize(axapi::MarketQuotationAPI *in_pMarketQuotationAPI, unsigned int in_n1mKBars, std::string in_strContract)
{
    if (m_blAutoRun == true)
    {
        m_blAutoRun = false;
        WaitForSingleObject(m_hCaculateRuning, INFINITE);
    }
    /// ������������򷵻ش���
    /*if (in_pMarketQuotationAPI == NULL || in_n1mKBars <= 0 || in_strContract.size() == 0)
    {
        return -100;
    }
    else*/
    {
        m_pMarketQuotation = in_pMarketQuotationAPI;
        m_n1mKBars = in_n1mKBars;
        m_strContract = in_strContract;
        m_arrayIndexValue.clear();
    }

    /*
    * ���̼߳���ָ��ֵ
    */
    /// ��������
    APINamespace TThostFtdcInstrumentIDType t_chContract;
    m_strContract.copy(t_chContract, sizeof(t_chContract));
    /*if (in_pMarketQuotationAPI->subMarketDataSingle(t_chContract) <= 0)
    {
        return -200;
    }*/



    m_blAutoRun = true;
    std::thread autorun(&MarketQuotationIndex::caculate, this);
    autorun.detach();
    return 0;
}

/// ���ָ��λ�õ�ָ��ֵ ���󷵻�NULL
double axapi::MarketQuotationIndex::getIndexValue(int in_iCurrentOffset)
{
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
    return m_arrayIndexValue[m_arrayIndexValue.size() + in_iCurrentOffset - 1];
}

/// ������� �̳к����û��Լ�����
void axapi::MarketQuotationIndex::caculate()
{
    std::string t_strContract = m_strContract;
    while (m_blAutoRun)
    {
        std::cout << "caculating " << t_strContract.c_str() << "..." << std::endl;
        Sleep(1000);
    }
    SetEvent(m_hCaculateRuning);
    std::cout << "exit caculate " << t_strContract.c_str() << std::endl;
}