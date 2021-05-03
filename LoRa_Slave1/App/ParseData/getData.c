#include "getData.h"
#include "sx1276.h"
#include "usart.h"

#define ADC_CHANNEL	1

//static uint32_t fanSpeed = 0;
//static uint32_t lightStatus = 0;

void GenerateTask(void)
{
    uint8_t *generateData;
		uint16_t generateDataSize;
	
		generateData = (uint8_t *) malloc(50);
    GenerateData(generateData);
		//printf("generateData:%s\r\n", generateData);
		generateDataSize = strlen(generateData);
		HAL_UART_Transmit(&huart1, generateData, generateDataSize, 0xFF);
		//printf("===================\r\n");
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
		HAL_ADC_Start(&hadc1);     //启动ADC转换
		for(i = 0; i < ADC_CHANNEL; i++)
		{
			HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，等待最大时间为50ms

			if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
			{
					getvalue[i] = HAL_ADC_GetValue(&hadc1);   //获取AD值
					//printf("ADC1 Reading getvalue[%d]: %d\r\n",i, getvalue[i]);
			}
		}
		HAL_ADC_Stop(&hadc1);
}

static void GenerateData(uint8_t *generateData)
{
    uint32_t getValue[ADC_CHANNEL];
		uint8_t int_temp, int_humi;
		int ret = 1;
    ret = DHT11_Get_Temperature_Humidity(&int_temp, &int_humi);
    if(ret == 0)
    {
        //printf("temperature = %d degree humidity: %d\r\n",int_temp ,int_humi);
    }
		
		GetAllData(getValue);
		uint32_t lightIntensity;
		
		lightIntensity = 100 - (getValue[0] * 100 / 4096);
		if(lightIntensity < 40 && lightIntensity > 0)
		{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET);
		}
		if(lightIntensity >= 40 && lightIntensity < 100)
		{
				HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET);
		}
		//printf("lightIntensity = %d\r\n",lightIntensity);
		
    char str_device_id[20];
    char str_terminal_id[20];
    char str_maskNum[20];
    char str_temp[20];
    char str_humidity[20];
    char str_lightIntensity[20];
		
		sprintf(str_device_id, 			"%u", DEVICE_ID);
		sprintf(str_terminal_id, 		"%u", TERMINAL_ID);
		sprintf(str_maskNum, 				"%u", MASK_NUM);
		sprintf(str_temp, 					"%u", int_temp);
		sprintf(str_humidity, 			"%u", int_humi);
		sprintf(str_lightIntensity, "%u", lightIntensity);

    strcat((char *)generateData, str_device_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_terminal_id);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_maskNum);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_temp);
    strcat((char *)generateData, SPLIT_CHAR);
    strcat((char *)generateData, str_humidity);

		
		//HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin);
		
		HAL_Delay(3000);
}

