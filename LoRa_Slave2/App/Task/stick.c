#include <stdlib.h>
#include <stdint.h>
#include "stick.h"
#include "stimer.h"

static uint32_t spTicker = 0;

/************************************************
�������� �� spTickCount
��    �� �� Ϊϵͳ�����ṩʱ��,��Ҫ���ú������뵽�������еĺ�����;
��    �� �� ��
�� �� ֵ �� ��
��    �� �� sun
*************************************************/
void spTickCount(void)
{
	spTicker++;
	spTimerTickerHandle();
}

/************************************************
�������� �� spGetTickCount
��    �� �� ��ȡ��ǰϵͳʱ��ֵ
��    �� �� ��
�� �� ֵ �� uint32_t����ǰϵͳʱ��ֵ
��    �� �� sun
*************************************************/
uint32_t spGetTickCount(void)
{
	return spTicker;
}
