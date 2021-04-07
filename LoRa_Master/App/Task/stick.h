#ifndef _STICK_H_
#define _STICK_H_

#define TICK_RATE_HZ   1000 //系统运行频率需要配置

#include <stdint.h>

void spTickCount(void);
uint32_t spGetTickCount(void);

#endif
