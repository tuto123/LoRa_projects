#include <stdlib.h>
#include <stdint.h>
#include "stick.h"
#include "stimer.h"

static uint32_t spTicker = 0;

/************************************************
函数名称 ： spTickCount
功    能 ： 为系统运行提供时钟,需要将该函数放入到周期运行的函数中;
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
void spTickCount(void)
{
	spTicker++;
	spTimerTickerHandle();
}

/************************************************
函数名称 ： spGetTickCount
功    能 ： 获取当前系统时钟值
参    数 ： 无
返 回 值 ： uint32_t，当前系统时钟值
作    者 ： sun
*************************************************/
uint32_t spGetTickCount(void)
{
	return spTicker;
}
