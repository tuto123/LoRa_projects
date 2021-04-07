#include "sx1276_user.h"
#include "radio.h"
#include "sx1276.h"
#include "sx1276-board.h"
#include "sx1276_hal.h"
#include "usart.h"
#include "stimer.h"



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



static RadioEvents_t RadioEvents;

void delay_ms(uint32_t cnt)
{
    volatile uint32_t i = cnt * 3190;
    while(i--);
}


void OnTxDone( void )
{
	printf("tx done\r\n");
	spTimerStart(STIMER_LED_GREEN,200,2);//���յ���Ч������ɫLED��˸
	Radio.Rx(0);//��������
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t _rssi, int8_t _snr )
{
		bufferSize = size;
		memcpy( buffer, payload, bufferSize );
		printf("rx done\r\n");
		HAL_UART_Transmit(&huart1, buffer, bufferSize, 0xFF);
	  spTimerStart(STIMER_LED_GREEN,200,4);//���յ���Ч������ɫLED��˸
		Radio.Rx(0);//��������
    
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
}

void OnCadDone(bool channelActivityDetected )
{
    printf("cad done\r\n");
	 if(channelActivityDetected)
	  Radio.Rx(2000);//CAD��⵽���ݿ���2s���մ�
	 else
		Radio.Sleep( );//û�м�⵽��Ч����Radio����
}
//�˺������ӵ���������ʼ���У�һ��Ҫ����SPI�����ų�ʼ��֮��
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
}											 