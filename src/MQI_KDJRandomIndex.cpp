#define MQI_KDJRandomIndex_EXE
#include "MQI_KDJRandomIndex.h"
#include <iostream>
#include <thread>

axapi::MQI_KDJRandomIndex::MQI_KDJRandomIndex()
{
    char* t_strLogFuncName = "MQI_KDJRandomIndex::MQI_KDJRandomIndex";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

axapi::MQI_KDJRandomIndex::~MQI_KDJRandomIndex()
{
    char* t_strLogFuncName = "MQI_KDJRandomIndex::MQI_KDJRandomIndex";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

/*
* KDJ�ļ���Ƚϸ��ӣ�����Ҫ�������ڣ�n�ա�n�ܵȣ���RSVֵ����δ�������ָ��ֵ��Ȼ���ټ���Kֵ��Dֵ��Jֵ�ȡ���n��KDJ��ֵ�ļ���Ϊ��������㹫ʽΪ
n��RSV=��Cn��Ln��/��Hn��Ln����100
��ʽ�У�CnΪ��n�����̼ۣ�LnΪn���ڵ���ͼۣ�HnΪn���ڵ���߼ۡ�
��Σ�����Kֵ��Dֵ��
����Kֵ=2/3��ǰһ��Kֵ+1/3������RSV
����Dֵ=2/3��ǰһ��Dֵ+1/3������Kֵ
����ǰһ��K ֵ��Dֵ����ɷֱ���50�����档
Jֵ=3*����Kֵ-2*����Dֵ
��9��Ϊ���ڵ�KD��Ϊ������δ�������ֵ�����㹫ʽΪ
9��RSV=��C��L9���£�H9��L9����100
��ʽ�У�CΪ��9�յ����̼ۣ�L9Ϊ9���ڵ���ͼۣ�H9Ϊ9���ڵ���߼ۡ�
Kֵ=2/3����8��Kֵ+1/3����9��RSV
Dֵ=2/3����8��Dֵ+1/3����9��Kֵ
Jֵ=3*��9��Kֵ-2*��9��Dֵ
����ǰһ��K
ֵ��Dֵ������Էֱ���50���档
*/
void axapi::MQI_KDJRandomIndex::caculate()
{
    char* t_strLogFuncName = "MQI_KDJRandomIndex::caculate";
    char t_strLog[500];
    sprintf_s(t_strLog, 500, "%s", t_strLogFuncName);
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

    std::string t_strContract = m_strContract;

#pragma region
    /*
    * �Զ���ָ����㲿��
    */
    // �������̼�Cn, ������ͼ�Ln, ������߼�Hn
    double t_ClosePrice, t_LowestPrice, t_HighestPrice;
    MQI_KDJRandomIndexField t_objLatestIndexValue;
    t_objLatestIndexValue.BarSerials = NULL;
    t_objLatestIndexValue.indexValue = NULL;

    axapi::KMarketDataField *t_pMarketDataField = NULL;
    axapi::KMarketDataField *t_pMarketDataFieldLastest = NULL;

    while (m_blAutoRun)
    {
        sprintf_s(t_strLog, 500, "%s:KDJ caculating%s...", t_strLogFuncName, t_strContract.c_str());
        //LOG4CPLUS_TRACE(m_objLogger, t_strLog);

        // �������̼�Cn
        t_ClosePrice = 0;
        // ������ͼ�Ln
        t_LowestPrice = 9999999;
        // ������߼�Hn
        t_HighestPrice = 0;

        t_pMarketDataFieldLastest = m_pMarketQuotation->getKLine(t_strContract.c_str(), 1, 0);
        if (t_pMarketDataFieldLastest == NULL)
        {
            continue;
        }

        /*
        * Ѱ��ָ�����
        */
        for (int i = 0; i < m_n1mKBars; i++)
        {
            t_pMarketDataField = m_pMarketQuotation->getKLineBySerials(t_strContract.c_str(), 1, t_pMarketDataFieldLastest->BarSerials - i);
            if (t_pMarketDataField != NULL)
            {
                if (i == 0)
                {
                    t_ClosePrice = t_pMarketDataField->ClosePrice;
                }
                t_LowestPrice = t_LowestPrice > t_pMarketDataField->LowestPrice ? t_pMarketDataField->LowestPrice : t_LowestPrice;
                t_HighestPrice = t_HighestPrice < t_pMarketDataField->HighestPrice ? t_pMarketDataField->HighestPrice : t_HighestPrice;
            }
            else
            {
                break;
            }
        }

        /*
        * ����ָ��
        */
        if (t_pMarketDataField == NULL)
        {
            continue;
        }
        else
        {
            /*
            * ��¼INDEXֵ
            */
            /// �����ǰ�����INDEXֵ�����δ�������
            if (t_objLatestIndexValue.BarSerials == t_pMarketDataFieldLastest->BarSerials)
            {
                m_arrayIndexValue.pop_back();
            }

            t_objLatestIndexValue.BarSerials = t_pMarketDataFieldLastest->BarSerials;
            t_objLatestIndexValue.RSVValue = (t_HighestPrice - t_LowestPrice) == 0 ? 100 : (t_ClosePrice - t_LowestPrice) / (t_HighestPrice - t_LowestPrice) * 100;
            if (m_arrayIndexValue.size() > 0 && m_arrayIndexValue[m_arrayIndexValue.size() - 1].BarSerials == t_pMarketDataFieldLastest->BarSerials - 1)
            {
                t_objLatestIndexValue.KValue = m_arrayIndexValue[m_arrayIndexValue.size() - 1].KValue * 2 / 3 + t_objLatestIndexValue.RSVValue * 1 / 3;
                t_objLatestIndexValue.DValue = m_arrayIndexValue[m_arrayIndexValue.size() - 1].DValue * 2 / 3 + t_objLatestIndexValue.KValue * 1 / 3;
            }
            else
            {
                t_objLatestIndexValue.KValue = 50 + t_objLatestIndexValue.RSVValue * 1 / 3;
                t_objLatestIndexValue.DValue = 50 + t_objLatestIndexValue.KValue * 1 / 3;
            }
            t_objLatestIndexValue.JValue = t_objLatestIndexValue.KValue * 3 + t_objLatestIndexValue.DValue * 2;
            t_objLatestIndexValue.indexValue = t_objLatestIndexValue.JValue;
            m_arrayIndexValue.push_back(t_objLatestIndexValue);
        }
#pragma endregion

        Sleep(100);
    }

    SetEvent(m_hCaculateRuning);
    sprintf_s(t_strLog, 500, "%s:KDJ exit caculate%s", t_strLogFuncName, t_strContract.c_str());
    //LOG4CPLUS_TRACE(m_objLogger, t_strLog);
}

double axapi::MQI_KDJRandomIndex::getIndexValue(int in_iCurrentOffset)
{
    char* t_strLogFuncName = "MQI_KDJRandomIndex::getIndexValue";
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