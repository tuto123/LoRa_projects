#include <stdlib.h>
#include "stask.h"
#include "stimer.h"
#include "base_usart.h"
#include "sx1276.h"

#define spTASK_SIZE(array) (sizeof(array)/sizeof(*array))
#define  TASK_INVALID  0xFF

#include "gpio.h"
#include "getData.h"

//bool userControlFan = false;
//uint32_t userControlFanSpeed;

enum
{
	  UNALIVE      = 0x00, 
	  ALIVE        = 0x01,
    ALWAYS_ALIVE = 0xFF,
};



/************************************************
函数名称 ： _task_soft_timer_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_soft_timer_cb(void *args)
{
	  spTimerTaskScheduler(NULL);
}

/************************************************
函数名称 ： _task_scan_key_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_scan_key_cb(void *args)
{
	  /*添加按键扫秒循环函数*/
}

/************************************************
函数名称 ： _task_scan_key_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/

/*************加密功能*****************/
#include "tea.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cJSON.h"
uint8_t key[]="123456789987654321";
//uint8_t encryptKey[] = "lifu123@outlook.com";
/**************************************/
static void _task_scan_usart_cb(void *args)
{
		if(USER_Get_Usart_Receive_Flag())
		{
			USER_Clear_Usart_Receive_Flag();
			//USER_UART1_SendData(usart1_s.RecvBuffer,usart1_s.RecvLen);
			//Radio.Send(usart1_s.RxMes,usart1_s.RecvLen);	
#if 0		
			if(usart1_s.RecvLen > 0 && usart1_s.RecvLen <= 248)	
			{
				printf("\r\nusart1_s.RxMes:%s RecvLen:%d\r\n", usart1_s.RxMes, usart1_s.RecvLen);
				uint8_t MyMes[usart1_s.RecvLen];
				memset(MyMes, 0, usart1_s.RecvLen);
				memcpy(MyMes, usart1_s.RxMes, usart1_s.RecvLen);
				for(int i = 0; i < usart1_s.RecvLen; i++)
				{
					//MyMes[i] = usart1_s.RxMes[i]; 
				}
				uint16_t encryptSize = encrypt(MyMes, usart1_s.RecvLen, encryptKey);
				printf("encrypt:%s encryptSize:%d\r\n", MyMes, encryptSize);
				decrypt(MyMes, encryptSize, encryptKey);
				printf("decrypt:%s\r\n", MyMes);
				cJSON *json_ret,*json_fanSpeed,*json_lightStatus;
				json_ret = cJSON_Parse(MyMes); //判断是否为JSON格式
				if(json_ret == NULL)
				{
					printf("Not standard cjson data: %s\r\n", MyMes);
				}
				
				json_fanSpeed = cJSON_GetObjectItem(json_ret,"FanSpeed");
				json_lightStatus = cJSON_GetObjectItem(json_ret,"LightStat");
			
				if(json_fanSpeed != NULL)
				{
					userControlFanSpeed = json_fanSpeed->valueint;
					if(userControlFanSpeed > 0 && userControlFanSpeed < 100)
					{
						if(userControlFanSpeed != 1)
						{
								if(userControlFan == false)
								{
									userControlFan = true;
								}
						}
						if(userControlFanSpeed == 1)
						{
								if(userControlFan == true)
								{
									userControlFan = false;
								}
						}
						FAN_Set_Speed((uint32_t)userControlFanSpeed);
					}
				}
			
				if(json_lightStatus != NULL)
				{
					if(json_lightStatus->valueint == 1)
					{
						HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET);
					}
					else if(json_lightStatus->valueint == 0)
					{	
						HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET);
					}
				}
			
				

				free(json_ret);
				free(json_fanSpeed);
				free(json_lightStatus);
			}
#endif
		}
}
/************************************************
函数名称 ： _task_scan_radio_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void _task_scan_radio_cb(void *args)
{
		SX1276Task();
}

/************************************************
函数名称 ： _task_scan_value_cb
功    能 ： 定时任务的回调函数
参    数 ： void
返 回 值 ： 无
作    者 ： sun
*************************************************/
static int ms;
static void _task_scan_value_cb(void *args)
{
		ms++;
		if(ms == 300000)
		{
			GenerateTask();
			ms = 0;
		}
}

/************************************************
函数名称 ： tasklist
功    能 ： 任务列表
参    数 ： 
返 回 值 ： 
作    者 ： sun
*************************************************/
struct task_t tasklist[] =
{
	 { TASK_SOFT_TIMER,		ALWAYS_ALIVE,	 NULL,	_task_soft_timer_cb 	  }, //时间管理任务，请勿删除
	 { TASK_SCAN_KEY,			ALWAYS_ALIVE,	 NULL,	_task_scan_key_cb     	}, 
	 { TASK_SCAN_USART,		ALWAYS_ALIVE,	 NULL,	_task_scan_usart_cb   	}, 
	 { TASK_SCAN_RADIO,		ALWAYS_ALIVE,	 NULL,	_task_scan_radio_cb   	}, //无线Sx1278任务管理
	 { TASK_SCAN_VALUE,		ALWAYS_ALIVE,	 NULL,	_task_scan_value_cb   	}, //获取数据任务
};


/************************************************
函数名称 ： _getTaskNum
功    能 ： 获取指定任务在任务列表中的位置
参    数 ： 任务号
返 回 值 ： 任务Id号
作    者 ： sun
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
函数名称 ： spTaskStart
功    能 ： 开始指定任务
参    数 ： 任务号
返 回 值 ： 无
作    者 ： sun
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
函数名称 ： spTaskStop
功    能 ： 停止指定任务
参    数 ： 任务号
返 回 值 ： 无
作    者 ： sun
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
函数名称 ： spTaskScheduler
功    能 ： 轮训任务列表中的任务,任务分配器
参    数 ： void
返 回 值 ： void
作    者 ： sun
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


