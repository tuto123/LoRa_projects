#include "getData.h"

static int lightSpeed = 0;
static int fanSpeedFlag = 0;

void GenerateTask()
{
    char generateData[255];
    GenerateData(generateData);
}

static void GenerateData(char *generateData)
{
    int ret = 1;
		uint8_t int_temp, int_humi;
    ret = DHT11_Get_Temperature_Humidity(&int_temp, &int_humi);
    if(ret == 0)
    {
        printf("temperature = %d degree humidity: %d\r\n",int_temp ,int_humi);
    }
#if 0
    char device_id[10];
    char terminal_id[10];
    char maskNum[10];
    char temp[10];
    char humidity[10];
    char lightIntensity[10];
    char airQuality[10];
    char fanSpeed[10];
    char lightStatus[10];
	
		sprintf(device_id, 			"%s", DEVICE_ID);
		sprintf(terminal_id, 		"%s", TERMINAL_ID);
		sprintf(maskNum, 				"%s", MASK_NUM);
		sprintf(temp, 					"%s", int_temp);
		sprintf(humidity, 			"%s", int_humi);
		sprintf(lightIntensity, "%s", 95);
		sprintf(airQuality, 		"%s", 66);
		sprintf(fanSpeed, 			"%s", 56);
		sprintf(lightStatus, 		"%s", 1);
		
    strcat(generateData, device_id);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, terminal_id);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, maskNum);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, temp);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, humidity);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, lightIntensity);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, airQuality);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, fanSpeed);
    strcat(generateData, SPLIT_CHAR);
    strcat(generateData, lightStatus);
#endif
		
    if(fanSpeedFlag == 0)
    {
        if(lightSpeed > 98)
        {
            fanSpeedFlag = 1;
        }
        lightSpeed++;
        FAN_Set_Speed(lightSpeed);
				HAL_Delay(10);
    }
    if (fanSpeedFlag == 1)
    {
        if(lightSpeed < 2)
        {
            fanSpeedFlag = 0;
        }
        lightSpeed--;
        FAN_Set_Speed(lightSpeed);
				//HAL_GPIO_TogglePin(MCU_LED_GPIO_Port, MCU_LED_Pin);
				HAL_Delay(10);
    }
}

