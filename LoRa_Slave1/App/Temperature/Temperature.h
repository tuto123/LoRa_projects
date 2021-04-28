#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include "stm32f1xx_hal.h"
#include "gpio.h"

#define  DHT11_DQ_OUT_HIGH       HAL_GPIO_WritePin(TEMP_DATA_GPIO_Port, TEMP_DATA_Pin, GPIO_PIN_SET)
#define  DHT11_DQ_OUT_LOW        HAL_GPIO_WritePin(TEMP_DATA_GPIO_Port, TEMP_DATA_Pin, GPIO_PIN_RESET)
#define  DHT11_DQ_IN             HAL_GPIO_ReadPin(TEMP_DATA_GPIO_Port, TEMP_DATA_Pin)

void delay_us(uint32_t time);
void DHT11_IO_IN(void);
void DHT11_IO_OUT(void);
void DHT11_Rst(void);
uint8_t DHT11_Check(void);
void DHT11_Write_Byte(uint8_t data);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Init(void);
uint8_t DHT11_Get_Temperature_Humidity(uint8_t *temp, uint8_t *humi);
uint8_t DHT11_Get_Float_Temperature_Humidity(float *temp, float *humi);

#endif //__TEMPERATURE_H__

