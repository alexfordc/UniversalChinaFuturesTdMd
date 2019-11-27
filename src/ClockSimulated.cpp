#include "ClockSimulated.h"
#ifdef _MQREAD_MODE_
#define _MIN_SPEEDMULTIPLIER 100000 //�ڲ�����ģ��������£�ģ��ʱ��֧�����ģ��Ϊ10000���٣�����ʵ1��=ģ��ʱ��10000��
#else
#ifdef _FASTEST_MODE_
#define _MIN_SPEEDMULTIPLIER 50000 //�ڲ�����ģ��������£�ģ��ʱ��֧�����ģ��Ϊ20000���٣�����ʵ1��=ģ��ʱ��20000��
#else
#ifdef _LOWEST_MODE_
#define _MIN_SPEEDMULTIPLIER 5000000 //������־���������£�ģ��ʱ��֧�����ģ��Ϊ200���٣�����ʵ1��=ģ��ʱ��200��
#else
#define _MIN_SPEEDMULTIPLIER 5000000 //������־���������£�ģ��ʱ��֧�����ģ��Ϊ200���٣�����ʵ1��=ģ��ʱ��200��
#endif _LOWEST_MODE_
#endif _FASTEST_MODE_
#endif

axapi::ClockSimulated::ClockSimulated()
{
    m_nSpeedMultiplier = _MIN_SPEEDMULTIPLIER;
}


axapi::ClockSimulated::~ClockSimulated()
{
    m_vClockSimulatedList.clear();
}

long axapi::ClockSimulated::setSpeedMultiplier(long in_nSpeedMultiplier)
{
    m_nSpeedMultiplier = in_nSpeedMultiplier < _MIN_SPEEDMULTIPLIER ? _MIN_SPEEDMULTIPLIER : in_nSpeedMultiplier;
    return m_nSpeedMultiplier;
}

void axapi::ClockSimulated::fillupClockLine(std::vector<TradingClockField>* in_vTradingClockLine)
{
    ClockSimulatedField t_ClockSimulatedField;
    memset(&t_ClockSimulatedField, '\0', sizeof(t_ClockSimulatedField));
    m_vClockSimulatedList.clear();
    for (auto &t_objTradingClock : (*in_vTradingClockLine))
    {
        strcpy_s(t_ClockSimulatedField.basedate, t_objTradingClock.basedate);
        strcpy_s(t_ClockSimulatedField.minute, t_objTradingClock.minute);
        t_ClockSimulatedField.millisecond = t_objTradingClock.millisecond;
        t_ClockSimulatedField.sequence = t_objTradingClock.sequence;
        m_vClockSimulatedList.push_back(t_ClockSimulatedField);
    }
    std::cout << "ģ��ʱ�ӹ�����" << m_vClockSimulatedList.size() << "��ʱ�ӵ�" << std::endl;
}

void axapi::ClockSimulated::startClocking()
{
    m_starttime = std::chrono::high_resolution_clock::now();
}

axapi::ClockSimulatedField * axapi::ClockSimulated::getCurrentTime()
{
    auto t_currenttime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::nano> t_duration = t_currenttime - m_starttime;
    long t_sequence = floor(t_duration.count() / m_nSpeedMultiplier);
#ifdef _LOWEST_MODE_
    std::cout << " " << t_sequence << ":" /*<< t_tmp*/ << t_duration.count() << std::endl;
#endif
    return t_sequence >= m_vClockSimulatedList.size() ? &(m_vClockSimulatedList[m_vClockSimulatedList.size() - 1]) : &(m_vClockSimulatedList[t_sequence]);
}
