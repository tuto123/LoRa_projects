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

/*************解密功能**************/
#include "tea.h"
#include "stdio.h"
#include "stdlib.h"
uint8_t  paw[]="123456789987654321";     
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
	spTimerStart(STIMER_LED_GREEN,200,2);//数据发送成功红灯闪烁一次
	Radio.Rx(0);//连续接收
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
		bufferSize = size;
	  mybufferSize = size;
		//memcpy( buffer, payload, bufferSize );
	  memcpy( mybuffer, payload, mybufferSize );
		//printf("RSSI=%d,SNR=%d\r\n",_rssi,_snr);
	  /***********************解密**************************/
	  if(mybufferSize>0&&mybufferSize<=248)	
		{
			//printf("recieve:%d\r\n",mybufferSize);
	    decrypt(mybuffer,mybufferSize,paw);
		  //HAL_UART_Transmit(&huart1, buffer, bufferSize, 0xFF);
		  HAL_UART_Transmit(&huart1, mybuffer, mybufferSize, 0xFF);
	    spTimerStart(STIMER_LED_GREEN,200,2);//接收到有效数据蓝色LED闪烁 
		}
		//Radio.Send(buffer, bufferSize);
    Radio.Rx(0);//连续接收
		/**************************我的控制程序-V1.0**************************/
	 /* memset( buffer0, 0, ( size_t )bufferSize );
	  memcpy( buffer0, payload, bufferSize );
	  if(strcmp(buffer0,buffer1)==0)
		{
	    spTimerStart(STIMER_LIGHT,200,1);//接收到有效数据开灯PA15
			printf("light on\r\n");
		}
		if(strcmp(buffer0,buffer3)==0)
		{
	    spTimerStart(STIMER_OFFLIGHT,200,1);//接收到有效数据关灯
			printf("light off\r\n");
		}
		if(strcmp(buffer0,buffer2)==0)
		{
	    spTimerStart(STIMER_BEEP,200,2);//接收到有效数据蜂鸣器响铃一次PA8
			printf("beep done\r\n");
		}
	  if(strcmp(buffer0,buffer4)==0)
		{
	    spTimerStart(STIMER_BEEP,200,2);//接收到有效数据蜂鸣器响铃一次
			printf("beep done\r\n");
		}
		if(strcmp(buffer0,buffer5)==0)
		{
			printf("reset done!\r\n");
			Radio.Send(buffer6, ( size_t )buffer6);
	    HAL_NVIC_SystemReset();//接收到有重启后重启
		}*/
	/*********************************************************************/
	
	/********************************Cjson控制程序-V1.5*********************************************/
	/**********************************Cjson_parsing**********************/
	/*	memset( buffer_cjson, 0, ( size_t )bufferSize );
	
		memcpy( buffer_cjson, payload, bufferSize );
		
		cJSON *json,*json_device,*json_state,*json_time;
		
		//char* json_string ="{\"Device\":\"led1\",\"area\":\"001\",\"state\":\"on\"}";//手动生成Cjson格式
		//Cjson格式：{"Device":"beep","state":"on","Time":"2018-10.18-19:29:30"}
		char* json_string = buffer_cjson;
	
		json = cJSON_Parse(json_string); //解释Cjson格式并判断是否为Cjson格式
		
		if(json == NULL){
		printf("Not standard cjson data...\r");
		}
		
		json_device = cJSON_GetObjectItem(json,"Device");
		json_state = cJSON_GetObjectItem(json,"State");
		json_time = cJSON_GetObjectItem(json,"Time");
		
		char led1[] = "led1";
		char beep[] = "beep";
		char on[]  =  "on";
		char off[] =  "off";
		
		if(json != NULL){
			printf("parsing: Device:%s,State:%s,Time:%s\r\n",json_device->valuestring,json_state->valuestring,json_time->valuestring);
			
			if(strcmp(json_device->valuestring , led1)==0  && strcmp(json_state->valuestring , on)==0 ){
				spTimerStart(STIMER_LIGHT,200,1);
				printf("led1 on\r\n");
			}	
			
			if(strcmp(json_device->valuestring , led1)==0  && strcmp(json_state->valuestring , off)==0 ){
				spTimerStart(STIMER_OFFLIGHT,200,1);
				printf("led1 off\r\n");
			}	
			if(strcmp(json_device->valuestring , beep)==0  && strcmp(json_state->valuestring , on)==0 ){
				spTimerStart(STIMER_BEEP,200,2);
				printf("beep on\r\n");
			}	
			
		}		
		cJSON_Delete(json);//释放内存*/
	/****************************************************************************************************/
}

void OnTxTimeout( void )
{
    printf("tx timeout\r\n");
		//Radio.Sleep( );//发送超时Radio休眠
		Radio.Rx(0);//连续接收
}

void OnRxTimeout( void )
{
    printf("rx timeout\r\n");
		//spTimerStart(STIMER_LED_RED,200,4);//接收到有效数据蓝色LED闪烁
		//Radio.Sleep( ); //接收超时Radio休眠
	  Radio.Rx(0);//连续接收
}

void OnRxError( void )
{
    printf("rx error\r\n");
		//Radio.Sleep( );//接收错误Radio休眠
		Radio.Rx(0);//连续接收
	  //Radio.Sleep( );//没有检测到有效数据Radio休眠
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
