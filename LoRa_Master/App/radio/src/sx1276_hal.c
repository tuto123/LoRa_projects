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


//���ļ�����Ҫ����ʵ��Ӳ���޸�


//���ų�ʼ��
void SX1276HalPinInit(void)
{
    //HalSpiInit();//SPI��ʼ�� ������cube���ɵĴ����Ѿ����������г�ʼ�������Դ˴��������ε���
		//HalDioInit();//IO�ڳ�ʼ�� �ж�����DIO0  DIO1 DIO2 DIO3  ��ͨ���� RESET  RXTX�л� 
}
//����spi nss�ź�
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

//���Ƹ�λ�ź�
void HalWriteReset(uint8_t level)
{
	if(!level)
	{ //����͵�ƽ
			HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_RESET);
		 
	}else
	{ //��Ϊ����
			HAL_GPIO_WritePin(RESET_IOPORT,RESET_PIN,GPIO_PIN_SET);
			//����Ϊ������Լ��ٹ���
	}
}

//���Ʒ���������� ���յͷ�
void HalWriteRxTx(uint8_t level)
{
	if(!level)
	{  //�����
			HAL_GPIO_WritePin( RXTX_IOPORT, RXTX_PIN, GPIO_PIN_RESET );
	}
	else
	{  //�����
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
