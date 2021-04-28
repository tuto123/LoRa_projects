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

/*************���ܹ���**************/
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
	spTimerStart(STIMER_LED_GREEN,200,2);//���ݷ��ͳɹ������˸һ��
	Radio.Rx(0);//��������
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
		bufferSize = size;
	  mybufferSize = size;
		//memcpy( buffer, payload, bufferSize );
	  memcpy( mybuffer, payload, mybufferSize );
		//printf("RSSI=%d,SNR=%d\r\n",_rssi,_snr);
	  /***********************����**************************/
	  if(mybufferSize>0&&mybufferSize<=248)	
		{
			//printf("recieve:%d\r\n",mybufferSize);
	    decrypt(mybuffer,mybufferSize,paw);
		  //HAL_UART_Transmit(&huart1, buffer, bufferSize, 0xFF);
		  HAL_UART_Transmit(&huart1, mybuffer, mybufferSize, 0xFF);
	    spTimerStart(STIMER_LED_GREEN,200,2);//���յ���Ч������ɫLED��˸ 
		}
		//Radio.Send(buffer, bufferSize);
    Radio.Rx(0);//��������
		/**************************�ҵĿ��Ƴ���-V1.0**************************/
	 /* memset( buffer0, 0, ( size_t )bufferSize );
	  memcpy( buffer0, payload, bufferSize );
	  if(strcmp(buffer0,buffer1)==0)
		{
	    spTimerStart(STIMER_LIGHT,200,1);//���յ���Ч���ݿ���PA15
			printf("light on\r\n");
		}
		if(strcmp(buffer0,buffer3)==0)
		{
	    spTimerStart(STIMER_OFFLIGHT,200,1);//���յ���Ч���ݹص�
			printf("light off\r\n");
		}
		if(strcmp(buffer0,buffer2)==0)
		{
	    spTimerStart(STIMER_BEEP,200,2);//���յ���Ч���ݷ���������һ��PA8
			printf("beep done\r\n");
		}
	  if(strcmp(buffer0,buffer4)==0)
		{
	    spTimerStart(STIMER_BEEP,200,2);//���յ���Ч���ݷ���������һ��
			printf("beep done\r\n");
		}
		if(strcmp(buffer0,buffer5)==0)
		{
			printf("reset done!\r\n");
			Radio.Send(buffer6, ( size_t )buffer6);
	    HAL_NVIC_SystemReset();//���յ�������������
		}*/
	/*********************************************************************/
	
	/********************************Cjson���Ƴ���-V1.5*********************************************/
	/**********************************Cjson_parsing**********************/
	/*	memset( buffer_cjson, 0, ( size_t )bufferSize );
	
		memcpy( buffer_cjson, payload, bufferSize );
		
		cJSON *json,*json_device,*json_state,*json_time;
		
		//char* json_string ="{\"Device\":\"led1\",\"area\":\"001\",\"state\":\"on\"}";//�ֶ�����Cjson��ʽ
		//Cjson��ʽ��{"Device":"beep","state":"on","Time":"2018-10.18-19:29:30"}
		char* json_string = buffer_cjson;
	
		json = cJSON_Parse(json_string); //����Cjson��ʽ���ж��Ƿ�ΪCjson��ʽ
		
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
		cJSON_Delete(json);//�ͷ��ڴ�*/
	/****************************************************************************************************/
}

void OnTxTimeout( void )
{
    printf("tx timeout\r\n");
		//Radio.Sleep( );//���ͳ�ʱRadio����
		Radio.Rx(0);//��������
}

void OnRxTimeout( void )
{
    printf("rx timeout\r\n");
		//spTimerStart(STIMER_LED_RED,200,4);//���յ���Ч������ɫLED��˸
		//Radio.Sleep( ); //���ճ�ʱRadio����
	  Radio.Rx(0);//��������
}

void OnRxError( void )
{
    printf("rx error\r\n");
		//Radio.Sleep( );//���մ���Radio����
		Radio.Rx(0);//��������
	  //Radio.Sleep( );//û�м�⵽��Ч����Radio����
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
