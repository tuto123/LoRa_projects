#include "fan.h"

void FAN_Set_Speed(uint8_t fanSpeed)
{
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, fanSpeed * 5);
}
