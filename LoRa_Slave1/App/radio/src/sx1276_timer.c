#include "sx1276_timer.h"

//定义定时器时间链表
static timer_t *timerListHead = NULL;
//定义定时器时间链表初始化标志
static bool timerListInitFlag = false;

/************************************************
函数名称 ： TimerListInit
功    能 ： 定时任务链表初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListInit(void)
{
	timerListHead = (timer_t*)malloc(sizeof(timer_t));
	if(timerListHead == NULL)
	{
		return;
	}
	timerListHead->next = NULL;
}

/************************************************
函数名称 ： TimerExists
功    能 ： 遍历定时链表，查看指定定时任务是否存在
参    数 ： 定时任务
返 回 值 ： true ：存在  false:不存在
作    者 ： sun
*************************************************/
static bool TimerExists( timer_t *obj )
{
	timer_t* cur = timerListHead->next;
	while( cur != NULL )
	{
			if( cur->timerNum == obj->timerNum )
			{
					return true;
			}
			cur = cur->next;
	}
	return false;
}
/************************************************
函数名称 ： TimerListInsert
功    能 ： 在定时链表中插入指定定时任务
参    数 ： timer_t ，定时任务
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListInsert(timer_t *obj)
{
    timer_t *head = timerListHead;
    timer_t *cur = timerListHead->next;

    if( ( obj == NULL ) || ( TimerExists( obj ) == true ) )
		{
       // printf("timer is exists\r\n");
        return;
    }

    if(cur == NULL) 
		{
        head->next = obj;
        obj->next = NULL;
    }
		else
		{
			while((cur->next != NULL))
			{
				 cur = cur->next;
			}
			cur->next = obj;
			obj->next = NULL;
		}
}
/************************************************
函数名称 ： TimerListChangPeriod
功    能 ： 改变指定定时任务的运行周期
参    数 ： num，任务号 period:周期值
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListChangPeriod(uint8_t num, uint32_t period)
{
    timer_t* cur = timerListHead->next;

    while( cur != NULL )
    {
        if( cur->timerNum == num )
        {
            cur->period = period;
        }
        cur = cur->next;
    }
}
/************************************************
函数名称 ： TimerListTimerStart
功    能 ： 开始指定定时任务
参    数 ： num，任务号 
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListTimerStart(uint8_t num)
{
    timer_t* cur = timerListHead->next;

    while( cur != NULL )
    {
        if( cur->timerNum == num )
        {
            cur->start = true;
        }
        cur = cur->next;
    }
}
/************************************************
函数名称 ： TimerListTimerStart
功    能 ： 停止指定定时任务
参    数 ： num，任务号 
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListTimerStop(uint8_t num)
{
    timer_t* cur = timerListHead->next;

    while( cur != NULL )
    {
        if( cur->timerNum == num )
        {
            cur->start = false;
        }
        cur = cur->next;
    }
}
/************************************************
函数名称 ： TimerListTimerReset
功    能 ： 重启指定定时任务
参    数 ： num，任务号 
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListTimerReset(uint8_t num)
{
    timer_t* cur = timerListHead->next;

    while( cur != NULL )
    {
        if( cur->timerNum == num )
        {
            cur->timerTick = 0;
        }
        cur = cur->next;
    }
}
/************************************************
函数名称 ： TimerListLoop
功    能 ： 定时器主循环
参    数 ： 无
返 回 值 ： 无
作    者 ： sun
*************************************************/
static void TimerListLoop(void)
{
    timer_t* head = timerListHead;
    timer_t* cur = timerListHead->next;

    while( cur != NULL && head != NULL)
    {
        if(cur->start)
        {
            if(++cur->timerTick >= cur->period)
            {
                cur->timerTick = 0;
                if(cur->timerCbFunc != NULL)
                {
                    cur->timerCbFunc();
                }
                if(cur->oneShot) //只执行一次
                {
                    cur->start = false;
                }
            }
        }
        cur = cur->next;
    }
}

void TimerRegister(uint8_t num, uint32_t period, bool oneShot, cbTimerFunc cbFunc)
{
    if(!timerListInitFlag){
        timerListInitFlag = true;
        TimerListInit();
    }

    timer_t *p = (timer_t*)malloc(sizeof(timer_t));
    if(p == NULL){
        //printf("error malloc\r\n");
        return;
    }
    p->timerNum= num;
    p->period= period;
    p->oneShot = oneShot;
    p->start = false;
    p->timerTick = 0;
    p->timerCbFunc = cbFunc;
    p->next = NULL;

    TimerListInsert(p);
}

void TimerChangePeriod(uint8_t num, uint32_t period)
{
    TimerListChangPeriod(num, period);
}

void TimerStart(uint8_t num)
{
    TimerListTimerReset(num);
    TimerListTimerStart(num);
}

void TimerStop(uint8_t num)
{
    TimerListTimerStop(num);
}

void TimerReset(uint8_t num)
{
    TimerListTimerReset(num);
}

void TimerLoop(void)
{
    TimerListLoop();
}

const timers_t Timer = 
{
    TimerRegister,
    TimerChangePeriod,
    TimerStart,
    TimerStop,
    TimerReset,
    TimerLoop,
};
