#include "getData.h"
#include "sx1276.h"
#include "usart.h"

#define ADC_CHANNEL	2

static uint32_t fanSpeed = 0;
static uint32_t lightStatus = 0;

extern bool userControlFan;
extern uint32_t userControlFanSpeed;

void GenerateTask()
{
    uint8_t *generateData;
		uint16_t generateDataSize;
	
		generateData = malloc(50);
    GenerateData(generateData);
		generateDataSize = strlen(generateData);
		HAL_UART_Transmit(&huart1, generateData, generateDataSize, 0xFF);
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
					//printf("ADC1 Reading : %d",ADC_Value);
			}
		}
		HAL_ADC_Stop(&hadc1);
}

static void GenerateData(uint8_t *generateData)
{
    uint32_t getValue[ADC_CHANNEL];
		
		GetAllData(getValue);
		uint32_t soilHumi, airQuaity, fanSpeed, lightStatus;
		
		soilHumi = 100 - (getValue[0] * 100 / 4096);
		
		airQuaity = getValue[1] * 100 / 4096;
	
		if(userControlFan)
		{
			fanSpeed = userControlFanSpeed;
		}
		if(airQuaity >= 60 && airQuaity < 100 && !userControlFan)
		{
				fanSpeed = (uint32_t)99;
				FAN_Set_Speed(fanSpeed);
		}
		if(airQuaity < 60 && airQuaity > 0 && !userControlFan)
		{
				fanSpeed = (uint32_t)1;
				FAN_Set_Speed(fanSpeed);
		}
		
		lightStatus = (uint32_t)!(HAL_GPIO_ReadPin(MCU_LED_GPIO_Port, MCU_LED_Pin));
		
    char str_device_id[20];
    char str_terminal_id[20];
    char str_maskNum[20];
    char str_airQuality[20];
    char str_fanSpeed[20];
		char str_soilHumi[20];
		char str_lightStatus[20];
		
		sprintf(str_device_id, 			"%u", DEVICE_ID);
		sprintf(str_terminal_id, 		"%u", TERMINAL_ID);
		sprintf(str_maskNum, 				"%u", MASK_NUM);
		sprintf(str_soilHumi, 			"%u", soilHumi);
		sprintf(str_airQuality, 		"%u", airQuaity);
		sprintf(str_fanSpeed, 			"%u", fanSpeed);
		sprintf(str_lightStatus, 		"%u", lightStatus);

    strcat((char *)generateData, str_device_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_terminal_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_maskNum);
    strcat((char *)generateData, SPLIT_CHAR);
		strcat((char *)generateData, str_soilHumi);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_airQuality);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_fanSpeed);
		strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_lightStatus);
		
		//HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin);
		
		HAL_Delay(3000);
}

