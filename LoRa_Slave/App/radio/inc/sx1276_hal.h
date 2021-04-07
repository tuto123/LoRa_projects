/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Generic SX1276 driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __SX1276_HAL_H__
#define __SX1276_HAL_H__


#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


//#define EVENT_IN_IRQ     //该功能定义sx1276_hal.c中定义的函数实在中断中处理还是在主函数中处理，开启:在中断中处理，注销:在主函数中处理。建议使用在主函数中处理。



#define RESET_IOPORT                                SX1276_RESET_GPIO_Port
#define RESET_PIN                                   SX1276_RESET_Pin

/*!
 * SX1278 SPI NSS I/O definitions
 */
#define NSS_IOPORT                                 SX1276_SPI_NSS_GPIO_Port
#define NSS_PIN                                    SX1276_SPI_NSS_Pin

/*!
 * SX1278 DIO pins  I/O definitions  TxDone  RxDone  CadDone都是用DIOO引脚，暂时使用跳频
 */
#define DIO0_IOPORT                                 SX1276_DIO0_GPIO_Port
#define DIO0_PIN                                    SX1276_DIO0_Pin

//#define DIO1_IOPORT                                 SX1276_DIO1_GPIO_Port
//#define DIO1_PIN                                    SX1276_DIO1_Pin

//#define DIO2_IOPORT                                 SX1276_DIO2_GPIO_Port
//#define DIO2_PIN                                    SX1276_DIO2_Pin

//#define DIO3_IOPORT                                 SX1276_DIO3_GPIO_Port
//#define DIO3_PIN                                    SX1276_DIO3_Pin

//#define DIO4_IOPORT                                 GPIOB
//#define DIO4_PIN                                    GPIO_PIN_7

//#define DIO5_IOPORT                                 GPIOB
//#define DIO5_PIN                                    GPIO_PIN_8

//接收发送使能控制 高收低发
#define RXTX_IOPORT                                SX1276_TXRX_GPIO_Port
#define RXTX_PIN                                   SX1276_TXRX_Pin




void SX1276HalPinInit(void);
void HalSpiWriteNss(uint8_t level);
uint8_t HalSpiTransmit( uint8_t outData );
void HalWriteReset(uint8_t level);

void delay(uint32_t ms);
uint32_t millis(void);
#ifdef __cplusplus
}
#endif

#endif // __SX1276_H__
