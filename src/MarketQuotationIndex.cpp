#define _SCL_SECURE_NO_WARNINGS
#define RELATIVESTRENGTHINDEX_EXP
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

/// 外接行情数据初始化，调用后开始计算指标值
int axapi::MarketQuotationIndex::initialize(axapi::MarketQuotationAPI *in_pMarketQuotationAPI, unsigned int in_n1mKBars, std::string in_strContract)
{
    if (m_blAutoRun == true)
    {
        m_blAutoRun = false;
        WaitForSingleObject(m_hCaculateRuning, INFINITE);
    }
    /// 传入参数错误则返回错误
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
    * 新线程计算指标值
    */
    /// 订阅行情
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

/// 获得指定位置的指标值 错误返回NULL
double axapi::MarketQuotationIndex::getIndexValue(int in_iCurrentOffset)
{
    /*
    * 请求索引位置超出指标序列范围，则返回NULL
    */
    if (m_arrayIndexValue.size() + in_iCurrentOffset <= 0 || in_iCurrentOffset > 0)
    {
        return NULL;
    }

    /*
    * 返回指定位置的指标值
    */
    return m_arrayIndexValue[m_arrayIndexValue.size() + in_iCurrentOffset - 1];
}

/// 计算入口 继承后需用户自己定义
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