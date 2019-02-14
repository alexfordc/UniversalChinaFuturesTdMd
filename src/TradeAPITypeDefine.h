#ifndef _TradeAPITypeDefine_H_
#define _TradeAPITypeDefine_H_
#pragma once
namespace axapi
{
	//-----------------------------����ִ��״̬--------------------------
	//�ӿ�����״̬
	#define _TradeAPI_STATUS_Uninitial      '0'  //δ��ʼ��
	#define _TradeAPI_STATUS_Initiating     '1'  //��ʼ����
	#define _TradeAPI_STATUS_Initiated      '2'  //�ѳ�ʼ��
	#define _TradeAPI_STATUS_UndefinedError '3'  //δ�������
	#define _TradeAPI_STATUS_Terminate      '9'  //�ж�
	#define _TradeAPI_STATUS_Ready          'a'  //���µ�
	#define _TradeAPI_STATUS_LinkError      'A'  //���Ӵ���

	//------------------------------��������--------------------------
	//������־
	#define ORDER_DIRECTION_BUY  0
	#define ORDER_DIRECTION_SELL 1
	//��ƽ��־
	#define ORDER_OFFSETFLAG_OPEN   0
	#define ORDER_OFFSETFLAG_OFFSET 1
	#define ORDER_OFFSETFLAG_OFFSET_TODAY 3
	//��������
	//�޼�
	#define ORDER_LIMITPRICE 1
	//�м�
	#define ORDER_ANYPRICE 2
	//�Լ�
	#define ORDER_AGAINSTPRICE 3
	//���޼���ֵ,����һЩ���ú�Ĵ��󷵻�
	#define Unlimite_Big 9999999999 
	//���޼�Сֵ,����һЩ���ú�Ĵ��󷵻�
	#define Unlimite_Small -99999 

	#define WAIT_RSP_SECONDS 5000

	//------------------------------��־����--------------------------
    #define LOG_TRACE 0
    #define LOG_DEBUG 1
    #define LOG_INFO  2
    #define LOG_WARN  3
    #define LOG_ERROR 4
    #define LOG_FATAL 5
}
#endif _TradeAPITypeDefine_H_