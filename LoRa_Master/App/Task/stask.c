#include <stdlib.h>
#include "stask.h"
#include "stimer.h"
#include "base_usart.h"
#include "sx1276.h"
#define spTASK_SIZE(array) (sizeof(array)/sizeof(*array))
#define  TASK_INVALID  0xFF



enum
{
	  UNALIVE      = 0x00, 
	  ALIVE        = 0x01,
    ALWAYS_ALIVE = 0xFF,
};



/************************************************
�������� �� _task_soft_timer_cb
��    �� �� ��ʱ����Ļص�����
��    �� �� void
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
static void _task_soft_timer_cb(void *args)
{
	  spTimerTaskScheduler(NULL);
}

/************************************************
�������� �� _task_scan_key_cb
��    �� �� ��ʱ����Ļص�����
��    �� �� void
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
static void _task_scan_key_cb(void *args)
{
	  /*��Ӱ���ɨ��ѭ������*/
}

/************************************************
�������� �� _task_scan_key_cb
��    �� �� ��ʱ����Ļص�����
��    �� �� void
�� �� ֵ �� ��
��    �� �� sun
*************************************************/

/*************���ܹ���*****************/
#include "tea.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cJSON.h"
uint8_t  key[]="123456789987654321";
/**************************************/
static void _task_scan_usart_cb(void *args)
{
		if(USER_Get_Usart_Receive_Flag())
		{
			USER_Clear_Usart_Receive_Flag();
			//USER_UART1_SendData(usart1_s.RecvBuffer,usart1_s.RecvLen);
			Radio.Send(usart1_s.RecvBuffer,usart1_s.RecvLen);			
		}
}
/************************************************
�������� �� _task_scan_radio_cb
��    �� �� ��ʱ����Ļص�����
��    �� �� void
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
static void _task_scan_radio_cb(void *args)
{
		SX1276Task();
}


/************************************************
�������� �� tasklist
��    �� �� �����б�
��    �� �� 
�� �� ֵ �� 
��    �� �� sun
*************************************************/
struct task_t tasklist[] =
{
	 { TASK_SOFT_TIMER,		ALWAYS_ALIVE,	 NULL,	_task_soft_timer_cb 	  }, //ʱ�������������ɾ��
	 { TASK_SCAN_KEY,		  ALWAYS_ALIVE,	 NULL,	_task_scan_key_cb     	}, 
	 { TASK_SCAN_USART,		ALWAYS_ALIVE,	 NULL,	_task_scan_usart_cb   	}, 
	 { TASK_SCAN_RADIO,		ALWAYS_ALIVE,	 NULL,	_task_scan_radio_cb   	}, //����Sx1278�������
};


/************************************************
�������� �� _getTaskNum
��    �� �� ��ȡָ�������������б��е�λ��
��    �� �� �����
�� �� ֵ �� ����Id��
��    �� �� sun
*************************************************/
static uint8_t _getTaskNum (uint8_t task_id)
{
   uint8_t i;
   for (i = 0; i < spTASK_SIZE(tasklist); ++i)
   {
     if(tasklist[i].id == task_id )
		 {
		   return i;
		 }
   }
	 return TASK_INVALID;
}

/************************************************
�������� �� spTaskStart
��    �� �� ��ʼָ������
��    �� �� �����
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void spTaskStart(uint8_t task_id)
{
  uint8_t t_id;
	t_id = _getTaskNum(task_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	tasklist[t_id].flags = ALIVE;
}
/************************************************
�������� �� spTaskStop
��    �� �� ָֹͣ������
��    �� �� �����
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void spTaskStop(uint8_t task_id)
{
  uint8_t t_id;
	t_id = _getTaskNum(task_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	tasklist[t_id].flags = UNALIVE;
}

/************************************************
�������� �� spTaskScheduler
��    �� �� ��ѵ�����б��е�����,���������
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
void spTaskScheduler(void)
{
	uint8_t i;
	for (i = 0; i < spTASK_SIZE(tasklist); ++i)
	{
		if (tasklist[i].flags != UNALIVE ) 
		{
			if(tasklist[i].flags != ALWAYS_ALIVE ) 
			{
				tasklist[i].flags = UNALIVE;
				tasklist[i].handle(tasklist[i].args);
			}
			else
			{
				tasklist[i].handle(tasklist[i].args);
			}
		}
	}
}


