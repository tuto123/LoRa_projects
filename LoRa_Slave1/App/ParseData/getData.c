#include "getData.h"
#include "sx1276.h"

#define ADC_CHANNEL	3

//static uint32_t fanSpeed = 0;
static uint32_t lightStatus = 0;

static char *myitoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;
 
    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }
 
    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }
 
    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';
 
        /* Make the value positive. */
        value *= -1;
    }
 
    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;
 
        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }
 
    /* Null terminate the string. */
    *ptr = 0;
 
    return string;
 
} /* NCL_Itoa */

void GenerateTask()
{
    uint8_t generateData[255];
    GenerateData(generateData);
		//Radio.Send(generateData, sizeof(generateData));
		free(generateData);
}

/*
getvalue[0] = ADC_SOIL;
getvalue[1] = ADC_AIR;
getvalue[2] = ADC_LIGHT;
*/
static void GetAllData(uint32_t* getvalue)
{
		int i;
		for(i = 0; i < ADC_CHANNEL; i++)
		{
			uint16_t ADC_Value;
			HAL_ADC_Start(&hadc1);     //启动ADC转换
			HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，等待最大时间为50ms

			if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
			{
					getvalue[i] = HAL_ADC_GetValue(&hadc1);   //获取AD值
					printf("ADC1 Reading : %d",ADC_Value);
			}
		}
		HAL_ADC_Stop(&hadc1);
		printf("\r\n");
}

static void GenerateData(uint8_t *generateData)
{
    uint32_t getValue[ADC_CHANNEL];
		uint8_t int_temp, int_humi;
		int ret = 1;
    ret = DHT11_Get_Temperature_Humidity(&int_temp, &int_humi);
    if(ret == 0)
    {
        printf("temperature = %d degree humidity: %d\r\n",int_temp ,int_humi);
    }
		
		GetAllData(getValue);
		uint32_t soilHumi, airQuaity, lightIntensity, fanSpeed;
		
		soilHumi = 100 - (getValue[0] * 100 / 4096);
		//printf("True Humi value : %d \r\n", soilHumi);
		if(soilHumi > 60 && soilHumi < 100)
		{
				//HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin);
		}
		
		airQuaity = getValue[1] * 100 / 4096;
		if(airQuaity > 60 && airQuaity < 100)
		{
				fanSpeed = (uint32_t)99;
				FAN_Set_Speed(fanSpeed);
		}
		if(airQuaity < 30 && airQuaity > 0)
		{
				fanSpeed = (uint32_t)1;
				FAN_Set_Speed(fanSpeed);
		}
		
		lightIntensity = 100 - (getValue[2] * 100 / 4096);
		if(lightIntensity < 40 && airQuaity > 0)
		{
				lightStatus = 1;
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET);
		}
		if(lightIntensity > 60 && airQuaity < 100)
		{
				lightStatus = 0;
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET);
		}
		
#if 0
		if(Body_Get_Status() == 1)
		{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET);
				FAN_Set_Speed(100);
		}
		else if (Body_Get_Status() == 0)
		{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET);	
		}
#endif
    char str_device_id[20];
    char str_terminal_id[20];
    char str_maskNum[20];
    char str_temp[20];
    char str_humidity[20];
    char str_lightIntensity[20];
    char str_airQuality[20];
    char str_fanSpeed[20];
    char str_lightStatus[20];
#if 0
		myitoa(DEVICE_ID, 			str_device_id, 			10);
		myitoa(TERMINAL_ID, 		str_terminal_id, 		10);
		myitoa(MASK_NUM, 				str_maskNum, 				10);
		myitoa(int_temp, 				str_temp, 					10);
		myitoa(int_humi, 				str_humidity, 			10);
		myitoa(lightIntensity, 	str_lightIntensity, 10);
		myitoa(airQuaity, 			str_airQuality, 		10);
		myitoa(fanSpeed, 				str_fanSpeed, 			10);
		myitoa(lightStatus, 		str_lightStatus, 		10);
		
		free(str_device_id);
		free(str_terminal_id);
		free(str_maskNum);
		free(str_temp);
		free(str_humidity);
		free(str_lightIntensity);
		free(str_airQuality);
		free(str_fanSpeed);
		free(str_lightStatus);
#endif

		sprintf(str_device_id, 			"%u", DEVICE_ID);
		sprintf(str_terminal_id, 		"%u", TERMINAL_ID);
		sprintf(str_maskNum, 				"%u", MASK_NUM);
		//sprintf(str_temp, 					"%d", int_temp);
		//sprintf(str_humidity, 			"%d", int_humi);
		sprintf(str_lightIntensity, "%u", (uint32_t)lightIntensity);
		sprintf(str_airQuality, 		"%u", (uint32_t)airQuaity);
		if(fanSpeed == 99)
		{
			sprintf(str_fanSpeed, 			"%u", (uint32_t)99);
		}
		else if(fanSpeed == 1)
		{
			sprintf(str_fanSpeed, 			"%u", (uint32_t)1);
		}
		sprintf(str_lightStatus, 		"%u", (uint32_t)lightStatus);

    strcat((char *)generateData, str_device_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_terminal_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_maskNum);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_temp);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_humidity);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_lightIntensity);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_airQuality);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_fanSpeed);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_lightStatus);
		
		printf("generateData:%s\n", generateData);
		HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin);
		
		HAL_Delay(300);
}

