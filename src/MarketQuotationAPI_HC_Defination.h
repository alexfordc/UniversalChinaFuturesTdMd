#ifndef _MARKETQUOTATIONAPI_HC_DEFINATION_H_
#define _MARKETQUOTATIONAPI_HC_DEFINATION_H_
#pragma once

namespace axapi
{
    /// ��������ʱ����
    struct TradingClockField
    {
        /// ��Ȼ��
        char basedate[9];
        /// ʱ��hh:mm:ss
        char minute[9];
        /// ����
        int millisecond;
        /// ���
        int sequence;
        /// �Ƿ��ױ�־
        char istradeflag;
    };
}
#endif _MARKETQUOTATIONAPI_HC_DEFINATION_H_