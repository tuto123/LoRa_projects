/*
   / _____)             _              | |
   ( (____  _____ ____ _| |_ _____  ____| |__
   \____ \| ___ |    (_   _) ___ |/ ___)  _ \
   _____) ) ____| | | || |_| ____( (___| | | |
   (______/|_____)_|_|_| \__)_____)\____)_| |_|
   (C)2013 Semtech

Description: Generic SX1276 driver implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Wael Guibene
*/

#include "sx1276_hal.h"
#include "spi.h"


//该文件下需要根据实际硬件修改


//引脚初始化
void SX1276HalPinInit(void)
{
    //HalSpiInit();//SPI初始化 （由于cube生成的代码已经在主函数中初始化，所以此处可以屏蔽掉）
		//HalDioInit();//IO口初始化 中断引脚DIO0  DIO1 DIO2 DIO3  普通数据 RESET  RXTX切换 
}
//控制spi nss信号
void HalSpiWriteNss(uint8_t level)
{
    if(!level)
		{
        HAL_GPIO_WritePin(NSS_IOPORT,NSS_PIN,GPIO_PIN_RESET);
    }
		else
		{
        HAL_GPIO_WritePin(NSS_IOPORT,NSS_PIN,GPIO_PIN_SET);
    }
}

uint8_t HalSpiTransmit( uint8_t outData )
{
    uint8_t txData = outData;
    uint8_t rxData = 0;
    HAL_SPI_TransmitReceive(&hspi1, &txData, &rxData, 1, 100);
    return rxData;
}

//控制复位信号
void HalWriteReset(uint8_t level)
{
	if(!level)
	{ //输出低电平
			HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_RESET);
		 
	}else
	{ //设为输入
			HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_SET);
			//设置为输入可以减少功耗
	}
}

//控制发射接收引脚 高收低发
void HalWriteRxTx(uint8_t level)
{
	if(!level)
	{  //输出低
			HAL_GPIO_WritePin( RXTX_IOPORT, RXTX_PIN, GPIO_PIN_RESET );
	}
	else
	{  //输出高
			HAL_GPIO_WritePin( RXTX_IOPORT, RXTX_PIN, GPIO_PIN_SET );
	}
}



void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

uint32_t millis(void)
{
    return HAL_GetTick();
}
