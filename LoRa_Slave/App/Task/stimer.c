#include <stdlib.h>
#include "stimer.h"
#include "stask.h"
#include "stick.h"
#include <stdint.h>
//#include "bsp.h"
#include "main.h"
#include "base_key.h"
#include "base_usart.h"

#define spTIMER_SIZE(array) (sizeof(array)/sizeof(*array))
#define creat_timer(timer_id, state, expires,          runs,arg, handler) \
                   {timer_id, state, expires,expires,  runs,arg, handler}

#define   TASK_INVALID      0xFF
									 
#define   ALWAYS_ALIVE      0xFFFF
#define   ALWAYS_UNLIVE     0x0000
enum
{
	  UNALIVE      = 0x00, 
	  ALIVE        = 0x01,
};


/************************************************
�������� �� _timer_led_red_cb
��    �� �� 500ms��ʱ���ص�����
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
static void _timer_led_red_cb(void *args)
{ 
	LED_RED_TOG();
} 


/************************************************
�������� �� _timer_led_blue_cb
��    �� �� 1000ms��ʱ���ص�����
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
static void _timer_led_blue_cb(void *args)
{ 
	LED_BLUE_TOG();
}   


/************************************************
�������� �� _timer_led_green_cb
��    �� �� 2000ms��ʱ���ص�����
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
static void _timer_led_green_cb(void *args)
{ 
	LED_GREEN_TOG();
}  

/************************************************
�������� �� _timer_key_cb
��    �� �� 10ms��ʱ���ص�����
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
static void _timer_key_cb(void *args)
{ 
	key1_process();
	//key2_process();
	//key3_process();
}
/************************************************
�������� �� _timer_key_cb
��    �� �� 10ms��ʱ���ص�����
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
static void _timer_usart_cb(void *args)
{ 
	USER_UART_Receive_Timeout();
}
 
/************************************************
�������� �� stimerlist
��    �� �� ��ʱ���б�
��    �� �� 
�� �� ֵ �� 
��    �� �� sun
*************************************************/
struct stimer_t  stimerlist[] =
{
	 creat_timer( STIMER_LED_RED,	  ALIVE,	  500,   ALWAYS_UNLIVE, NULL,	_timer_led_red_cb 	    ),
	 creat_timer( STIMER_LED_BLUE,	ALIVE,	  1000,  ALWAYS_UNLIVE, NULL,	_timer_led_blue_cb 	    ),
	 creat_timer( STIMER_LED_GREEN,	ALIVE,	  2000,  ALWAYS_UNLIVE, NULL,	_timer_led_green_cb 	  ),
	 creat_timer( STIMER_KEY,	      ALIVE,	  10,    ALWAYS_ALIVE,  NULL,	_timer_key_cb 	        ),
	 creat_timer( STIMER_USART,	    UNALIVE,	10,    ALWAYS_UNLIVE, NULL,	_timer_usart_cb 	      ),
};

/************************************************
�������� �� _getTimerTaskNum
��    �� �� ���ݶ�ʱ��id��ѯ�ö�ʱ���ڶ�ʱ�б��е����
��    �� �� ��ʱ��Id
�� �� ֵ �� uint8_t
��    �� �� sun
*************************************************/
static uint8_t _getTimerTaskNum (uint8_t timer_id)
{
   uint8_t i;
   for (i = 0; i < spTIMER_SIZE(stimerlist); ++i)
   {
     if(stimerlist[i].id == timer_id )
		 {
		   return i;
		 }
   }
	 return TASK_INVALID;
}
/************************************************
�������� �� spTimerStart
��    �� �� ��ʼ��ʱ����
��    �� �� timer_id:��ʱ��id��time��ʱʱ�䣬 num�����д���
�� �� ֵ �� void
��    �� �� sun
*************************************************/
void spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num)
{
  uint8_t t_id;
	double timeTmp = 0;
	t_id = _getTimerTaskNum(timer_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	timeTmp = 1000.00/TICK_RATE_HZ;
	timeTmp = time/timeTmp;
	time = (uint32_t )timeTmp;
	stimerlist[t_id].expires = time;
	stimerlist[t_id].expires_t = time;
	stimerlist[t_id].runs = num;
}

/************************************************
�������� �� spTimerStop
��    �� �� �رն�ʱ��
��    �� �� timer_id:��ʱ��id
�� �� ֵ �� void
��    �� �� sun
*************************************************/
void spTimerStop(uint8_t timer_id)
{
	uint8_t t_id;
	t_id = _getTimerTaskNum(timer_id);
	if(t_id == TASK_INVALID )
	{
		return;
	}
	stimerlist[t_id].flags = UNALIVE;
  stimerlist[t_id].runs = ALWAYS_UNLIVE;
}

/************************************************
�������� �� spTimerTickerHandle
��    �� �� ��ʱ��ʱ�������
��    �� �� void
�� �� ֵ �� void
��    �� �� sun
*************************************************/
void spTimerTickerHandle(void)
{ 
	 uint8_t i;
	 static uint32_t temp = 0;
	 if(spGetTickCount() == temp)
	 {return ;}
	 for (i = 0; i < spTIMER_SIZE(stimerlist); ++i)
    {
      if (stimerlist[i].runs != ALWAYS_UNLIVE)
      {
        if(stimerlist[i].expires > 0)
				{
				  --stimerlist[i].expires;
				}
				else
				{
					stimerlist[i].expires = 0;
				  stimerlist[i].flags = ALIVE;
				}
      }
    }
		temp = spGetTickCount();
}

/************************************************
�������� �� spTimerTaskScheduler
��    �� �� ��ѯ��ʱ�б�����״ִ̬������
��    �� �� void *args
�� �� ֵ �� void
��    �� �� sun
*************************************************/
void spTimerTaskScheduler(void *args)
{ 
	 uint8_t i;
	 for (i = 0; i < spTIMER_SIZE(stimerlist); ++i)
    {
      if (stimerlist[i].flags == ALIVE)
      {
			    if (stimerlist[i].runs == ALWAYS_UNLIVE)
					{
							stimerlist[i].flags = UNALIVE;
							stimerlist[i].expires = 0;
							continue;
					}
					else if (stimerlist[i].runs == ALWAYS_ALIVE)
					{
						  stimerlist[i].flags = UNALIVE;
							stimerlist[i].timer_cb(stimerlist[i].args);
					    stimerlist[i].expires = stimerlist[i].expires_t;
					}
					else
					{
					  stimerlist[i].runs --;
						stimerlist[i].flags = UNALIVE;
						stimerlist[i].timer_cb(stimerlist[i].args);
					  stimerlist[i].expires = stimerlist[i].expires_t;
					}
				  
			}
					
    }
}
