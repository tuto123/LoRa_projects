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

#define RF_FREQUENCY                               435575000 // 定义工作频率
#define TX_OUTPUT_POWER                            20        // 定义发送功率

#define LORA_BANDWIDTH                             2         // [0: 125 kHz, 定义带宽
                                                             //  1: 250 kHz,
                                                             //  2: 500 kHz,

#define LORA_SPREADING_FACTOR                      7         // [SF7..SF12] 定义扩频因子
#define LORA_CODINGRATE                            1         // [1: 4/5,    定义编码率
                                                             //  2: 4/6,
                                                             //  3: 4/7,
                                                             //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                       8         // Same for Tx and Rx  定义签到码长度
#define LORA_SYMBOL_TIMEOUT                        1023      // Symbols   超时符号个数

#define LORA_FIX_LENGTH_PAYLOAD_ON                 false
#define LORA_IQ_INVERSION_ON                       false



#define BUFFER_SIZE   256 // Define the payload size here

static uint16_t bufferSize = BUFFER_SIZE;// RF buffer size
static uint8_t buffer[BUFFER_SIZE] = {1,2,3,4,5,6,7,8};// RF buffer
char buffer1[] = "开灯";
char buffer2[] = "响铃";
char buffer3[] = "关灯";
char buffer4[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,};
char buffer5[] = "重启";
char buffer6[] = "reset done!";
char buffer0[BUFFER_SIZE];
char buffer_cjson[BUFFER_SIZE];

bool userControlFan = false;
uint32_t userControlFanSpeed;

/*************解密功能**************/
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
	//spTimerStart(STIMER_LED_GREEN,200,2);//数据发送成功红灯闪烁一次
	Radio.Rx(0);//连续接收
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
		bufferSize = size;
		memcpy( buffer, payload, bufferSize );
		//printf("rx done\r\n");
		//bufferSize = size;
		//memcpy( buffer, payload, bufferSize );
		//printf("RSSI=%d,SNR=%d\r\n",_rssi,_snr);
	  /***********************解密**************************/
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
		//Radio.Sleep( );//发送超时Radio休眠
		Radio.Rx(0);//连续接收
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnRxTimeout( void )
{
    printf("rx timeout\r\n");
		//spTimerStart(STIMER_LED_RED,200,4);//接收到有效数据蓝色LED闪烁
		//Radio.Sleep( ); //接收超时Radio休眠
	  Radio.Rx(0);//连续接收
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnRxError( void )
{
    printf("rx error\r\n");
		//Radio.Sleep( );//接收错误Radio休眠
		Radio.Rx(0);//连续接收
	  //Radio.Sleep( );//没有检测到有效数据Radio休眠
		printf("reseting...\r");
		HAL_NVIC_SystemReset();
}

void OnCadDone(bool channelActivityDetected )
{
    printf("cad done\r\n");
	 if(channelActivityDetected)
	  Radio.Rx(2000);//CAD检测到数据开启2s接收窗
	 else
	 {
	   Radio.Rx(0);	
		}
}
//此函数添加到主函数初始化中，一定要加在SPI和引脚初始化之后
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

	                    /*LoRa模式*/  /*发送功率20*/ /*频偏0(FSK)*/ /*带宽500*/
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
											/*LoRa扩频因子*/						/*LoRa编码*/
                       LORA_SPREADING_FACTOR, LORA_CODINGRATE,
											/*LoRa前导码*/							/*LoRa是否固定发送长度*/	
                       LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
											/*CRC校验*/	/*LoRa跳频开关*/	 /*LoRa跳频符号数*/	/*LoRa中断信号翻转*/ /*发送超时时间*/
                       true,             0,               0,         LORA_IQ_INVERSION_ON, 9000 );
	
											 /*LoRa模式*/ /*带宽500*/			/*LoRa扩频因子*/	
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
												/*LoRa编码*/	/*(FSK)*/  /*LoRa前导码*/
                       LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
											 /*LoRa接收符号超时*/	/*LoRa是否固定接收长度*/
                       LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
											 /*设置负载长度*/  /*CRC校验*//*LoRa跳频开关*/ /*LoRa中断信号翻转*/    /*连续接收*/
                       0,               true,      0,      0,       LORA_IQ_INVERSION_ON, 1 );
   Radio.Rx(0);									 
}											 
