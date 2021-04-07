#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include <stdint.h>

enum
{
	 STIMER_LED_RED   = 0x00,
   STIMER_LED_BLUE  = 0x01, 
   STIMER_LED_GREEN = 0x02, 
	 STIMER_KEY       = 0x03,	
	 STIMER_USART     = 0x04,	 
};

//��ʱ������
struct stimer_t
{
    uint8_t  id;             //��ʱ����ID
    uint8_t  flags;          //���б�־
	  uint32_t expires;        //��Чʱ��
		uint32_t expires_t;      //��Чʱ�䣬������װ��
		uint16_t runs;
	  void *args;
    void (*timer_cb)(void *args);
};

void spTimerStart(uint8_t timer_id,uint32_t time,uint16_t num);
void spTimerStop(uint8_t timer_id);
void spTimerTickerHandle(void);
void spTimerTaskScheduler(void *args);
#endif
