#include "soilHumi.h"

/***************************************************************************
SoilHumi_Get_Data
功  能：读取土壤湿度
输  入: 无
输  出：无
返回值：读取到的湿度数据
备  注：
***************************************************************************/
uint16_t SoilHumi_Get_Data(void)
{
    uint16_t ADC_Value, Humi_Value;
    HAL_ADC_Start(&hadc1);     //启动ADC转换
    HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，50为最大等待时间，单位为ms

    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    {
        ADC_Value = HAL_ADC_GetValue(&hadc1);   //获取AD值

        printf("ADC1 Reading : %d \r\n",ADC_Value);
				Humi_Value = (4096 - ADC_Value) / 4096 * 100;
        printf("True Humi value : %d \r\n", Humi_Value);
    }
		return Humi_Value;
}
