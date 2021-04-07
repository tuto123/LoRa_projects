#ifndef _BASE_USART_H_
#define _BASE_USART_H_

#include "stm32l0xx_hal.h"
#define USART_MAX_LEN 256

struct USART_t
{
	uint8_t  RecvBuffer[USART_MAX_LEN];
  uint16_t RecvLen;
	uint16_t RecvCount;
  uint8_t  RecvFalg;
};
extern struct USART_t usart1_s ;
void USER_UART_Receive_Timeout(void);
uint8_t USER_Get_Usart_Receive_Flag(void);
void USER_Clear_Usart_Receive_Flag(void);
void USER_UART_Receive(UART_HandleTypeDef *huart);
void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USER_UART1_SendData(uint8_t *data,uint16_t len);
void Base_Usart1_Init(void);
#endif
