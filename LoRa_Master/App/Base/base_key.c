#include "base_key.h"
#include "main.h"
#include "stimer.h"
#include "sx1276.h"

uint8_t SendBuf[64] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,};
struct KEY_t
{
 uint32_t PressCount;
 uint8_t  PressFlag;
};

struct KEY_t key1_s = {0,0};
struct KEY_t key2_s = {0,0};
struct KEY_t key3_s = {0,0};
void key1_process(void)
{
	if(KEY1_READ())
	{
		key1_s.PressCount++;
		if(key1_s.PressCount > 1)
		{
//			spTimerStart(STIMER_LED_GREEN,200,4);
//			key1_s.PressCount = 0;
//			key1_s.PressFlag = 1;
			
			LED_GREEN_ON();
	 }
	}
	else if(KEY1_READ()==0)
	{
		if((key1_s.PressCount > 8)&&(key1_s.PressFlag == 0))
		{
			//LED_GREEN_TOG();
			LED_GREEN_OFF();
			Radio.Send(SendBuf, 10);
		}
		key1_s.PressFlag  = 0;
		key1_s.PressCount = 0;
		
	}
	

}
