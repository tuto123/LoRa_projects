#include "sx1276_user.h"
#include "radio.h"
#include "sx1276.h"
#include "sx1276-board.h"
#include "sx1276_hal.h"
#include "usart.h"
#include "stimer.h"
#include "string.h"
#include "cJson.h"
#include <stdio.h>
#include "fan.h"

#define RF_FREQUENCY                               435575000 // ���幤��Ƶ��
#define TX_OUTPUT_POWER                            20        // ���巢�͹���

#define LORA_BANDWIDTH                             2         // [0: 125 kHz, �������
                                                             //  1: 250 kHz,
                                                             //  2: 500 kHz,

#define LORA_SPREADING_FACTOR                      7         // [SF7..SF12] ������Ƶ����
#define LORA_CODINGRATE                            1         // [1: 4/5,    ���������
                                                             //  2: 4/6,
                                                             //  3: 4/7,
                                                             //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                       8         // Same for Tx and Rx  ����ǩ���볤��
#define LORA_SYMBOL_TIMEOUT                        1023      // Symbols   ��ʱ���Ÿ���

#define LORA_FIX_LENGTH_PAYLOAD_ON                 false
#define LORA_IQ_INVERSION_ON                       false



#define BUFFER_SIZE   256 // Define the payload size here

static uint16_t bufferSize = BUFFER_SIZE;// RF buffer size
static uint8_t buffer[BUFFER_SIZE] = {1,2,3,4,5,6,7,8};// RF buffer
char buffer1[] = "����";
char buffer2[] = "����";
char buffer3[] = "�ص�";
char buffer4[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,};
char buffer5[] = "����";
char buffer6[] = "reset done!";
char buffer0[BUFFER_SIZE];
char buffer_cjson[BUFFER_SIZE];

bool userControlFan = false;
uint32_t userControlFanSpeed;

/*************���ܹ���**************/
#include "tea.h"
#include "stdio.h"
#include "stdlib.h"
uint8_t  paw[]="123456789987654321";   
uint8_t encryptKey[] = "lifu123@outlook.com";
static uint16_t mybufferSize = BUFFER_SIZE;
static uint8_t  mybuffer[BUFFER_SIZE];
/***********************************/

static RadioEvents_t RadioEvents;

void delay_ms(uint32_t cnt)
{
    volatile uint32_t i = cnt * 3190;
    while(i--);
}

void OnTxDone( void )
{
	printf("tx done\r\n");
	//spTimerStart(STIMER_LED_GREEN,200,2);//���ݷ��ͳɹ������˸һ��
	Radio.Rx(0);//��������
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
		bufferSize = size;
		memcpy( buffer, payload, bufferSize );
		//printf("rx done\r\n");
		//bufferSize = size;
		//memcpy( buffer, payload, bufferSize );
		//printf("RSSI=%d,SNR=%d\r\n",_rssi,_snr);
	  /***********************����**************************/
	  if(bufferSize > 0 && bufferSize <= 248)	
		{
			uint8_t MyMes[256];
			memset(MyMes, 0, bufferSize);
			memcpy(MyMes, buffer, bufferSize);
	    decrypt(MyMes, bufferSize, encryptKey);
		  printf("decrypt:%s size:%d\r\n", MyMes, bufferSize);
			cJSON *json_ret,*json_fanSpeed,*json_lightStatus;
			json_ret = cJSON_Parse((char *)MyMes); //parse cJSOn
			if(json_ret == NULL)
			{
					printf("Not standard cjson data, reseting...\r");
					HAL_NVIC_SystemReset();
			}

			json_fanSpeed = cJSON_GetObjectItem(json_ret,"FanSpeed");
			json_lightStatus = cJSON_GetObjectItem(json_ret,"LightStat");
			
			if(json_fanSpeed != NULL)
			{
				userControlFanSpeed = json_fanSpeed->valueint;
				if(userControlFanSpeed > 0 && userControlFanSpeed < 100)
				{
					if(userControlFanSpeed != 1)
					{
							if(userControlFan == false)
							{
								userControlFan = true;
							}
					}
					if(userControlFanSpeed == 1)
					{
							if(userControlFan == true)
							{
								userControlFan = false;
							}
					}
					FAN_Set_Speed((uint32_t)userControlFanSpeed);
				}
			}
			
			if(json_lightStatus != NULL)
			{
				if(json_lightStatus->valueint == 1)
				{
					HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_RESET);
				}
				else if(json_lightStatus->valueint == 0)
				{
					HAL_GPIO_WritePin(MCU_LED_GPIO_Port, MCU_LED_Pin, GPIO_PIN_SET);
				}
			}

			free(json_ret);
			free(json_fanSpeed);
			free(json_lightStatus);
		}
}

void OnTxTimeout( void )
{
    printf("tx timeout\r\n");
		//Radio.Sleep( );//���ͳ�ʱRadio����
		Radio.Rx(0);//��������
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnRxTimeout( void )
{
    printf("rx timeout\r\n");
		//spTimerStart(STIMER_LED_RED,200,4);//���յ���Ч������ɫLED��˸
		//Radio.Sleep( ); //���ճ�ʱRadio����
	  Radio.Rx(0);//��������
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnRxError( void )
{
    printf("rx error\r\n");
		//Radio.Sleep( );//���մ���Radio����
		Radio.Rx(0);//��������
	  //Radio.Sleep( );//û�м�⵽��Ч����Radio����
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnCadDone(bool channelActivityDetected )
{
    printf("cad done\r\n");
	 if(channelActivityDetected)
	  Radio.Rx(2000);//CAD��⵽���ݿ���2s���մ�
	 else
	 {
	   Radio.Rx(0);	
		}
}
//�˺�����ӵ���������ʼ���У�һ��Ҫ����SPI�����ų�ʼ��֮��
void SX1276AppInit(void)
{
	  SX1276BoardInit();
    RadioEvents.TxDone    = OnTxDone;
    RadioEvents.RxDone    = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError   = OnRxError;
    RadioEvents.CadDone   = OnCadDone;
    Radio.Init( &RadioEvents );

    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetModem( MODEM_LORA );

	                    /*LoRaģʽ*/  /*���͹���20*/ /*Ƶƫ0(FSK)*/ /*����500*/
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
											/*LoRa��Ƶ����*/						/*LoRa����*/
                       LORA_SPREADING_FACTOR, LORA_CODINGRATE,
											/*LoRaǰ����*/							/*LoRa�Ƿ�̶����ͳ���*/	
                       LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
											/*CRCУ��*/	/*LoRa��Ƶ����*/	 /*LoRa��Ƶ������*/	/*LoRa�ж��źŷ�ת*/ /*���ͳ�ʱʱ��*/
                       true,             0,               0,         LORA_IQ_INVERSION_ON, 9000 );
	
											 /*LoRaģʽ*/ /*����500*/			/*LoRa��Ƶ����*/	
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
												/*LoRa����*/	/*(FSK)*/  /*LoRaǰ����*/
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
											 /*LoRa���շ��ų�ʱ*/	/*LoRa�Ƿ�̶����ճ���*/
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
											 /*���ø��س���*/  /*CRCУ��*//*LoRa��Ƶ����*/ /*LoRa�ж��źŷ�ת*/    /*��������*/
                       0,               true,      0,      0,       LORA_IQ_INVERSION_ON, 1 );
   Radio.Rx(0);									 
}											 
