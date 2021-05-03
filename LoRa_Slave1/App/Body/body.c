#include "body.h"

int Body_Get_Status(void)
{
		int ret = -1;
		ret = HAL_GPIO_ReadPin(BODY_INPUT_GPIO_Port, BODY_INPUT_Pin);
    return ret;
}