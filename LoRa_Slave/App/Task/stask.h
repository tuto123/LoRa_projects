#ifndef _STASK_H_
#define _STASK_H_

/********ϵͳ���õ���ͷ�ļ�***********/
#include <stdint.h>

struct task_t
{
    uint8_t id;     //�����־��ţ����һ������TASK_SOFT_TIMER
    uint8_t flags;  //�Ƿ����б�־�� ALWAYS_ALIVE:ʼ��ѭ����UNALIVE:�����п�ͨ����������
    void *args;     //�������в���
    void (*handle)(void *args);//�������лص�����
};

enum
{
	  TASK_SOFT_TIMER = 0x00,  //ticker������
	  TASK_SCAN_KEY   = 0x01,
		TASK_SCAN_USART = 0x02 ,
	  TASK_SCAN_RADIO = 0x03 ,
};
void spTaskStart(uint8_t task_id);
void spTaskStop(uint8_t task_id);
void spTaskScheduler(void);

#endif
