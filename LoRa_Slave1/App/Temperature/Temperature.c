#include "Temperature.h"


/****************************************************************************
函数名：delay_us
功能：微秒级延时
输入：延时数据
输出：无
返回值：无
备注：
****************************************************************************/
void delay_us(uint32_t us)
{
  	uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
	while (delay--)
	{
		;
	}
}

/****************************************************************************
函数名：DHT11_IO_IN
功能：使DHT11_DQ引脚变为输入模式
输入：无
输出：无
返回值：无
备注：DQ引脚为PA5
****************************************************************************/
void DHT11_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = TEMP_DATA_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(TEMP_DATA_GPIO_Port,&GPIO_InitStructure);
}


/****************************************************************************
函数名：DHT11_IO_OUT
功能：使DHT11_DQ引脚变为推挽输出模式
输入：无
输出：无
返回值：无
备注：DQ引脚为PA5
****************************************************************************/
void DHT11_IO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = TEMP_DATA_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TEMP_DATA_GPIO_Port,&GPIO_InitStructure);
}


/***************************************************************************
函数名：DHT11_Rst
功  能：发送复位信号
输  入: 无
输  出：无
返回值：无
备  注：
***************************************************************************/
void DHT11_Rst(void)
{
	DHT11_IO_OUT();//引脚输出模式
	
	//拉低总线并延时750us
	DHT11_DQ_OUT_LOW;
	HAL_Delay(20); 
	
	//释放总线为高电平并延时等待15~60us
	DHT11_DQ_OUT_HIGH;
	delay_us(30);
}
	

/***************************************************************************
函数名：DHT11_Check
功  能：检测DHT11返回的存在脉冲
输  入: 无
输  出：无
返回值：0:成功  1：失败   2:释放总线失败
备  注：
***************************************************************************/
uint8_t DHT11_Check(void)
{
	//定义一个脉冲持续时间
	uint8_t retry = 0;
	//引脚设为输入模式
	DHT11_IO_IN();
	while(DHT11_DQ_IN && retry < 100){
		retry++;
		delay_us(1);
	}
	
	if(retry >= 100)
		return 1;
	else
		retry = 0;
	
	//判断DHT11是否释放总线
	while(!DHT11_DQ_IN && retry < 100){
		retry++;
		delay_us(1);
	}
	
	if(retry >= 100)
		return 1;
	
	return 0;
}

/***************************************************************************
函数名：DHT11_Write_Byte
功  能：向DHT11写一个字节
输  入: 要写入的字节
输  出：无
返回值：无
备  注：
***************************************************************************/
void DHT11_Write_Byte(uint8_t data)
{
	uint8_t j;
	uint8_t databit;
	for(j=1;j<=8;j++){
		databit=data&0x01;//取数据最低位
		data=data>>1;     //右移一位
		if(databit){      //当前位写1
			DHT11_DQ_OUT_LOW;
			delay_us(2);
			DHT11_DQ_OUT_HIGH;
			delay_us(60);
		}else{          //当前位写0
			DHT11_DQ_OUT_LOW;
			delay_us(60);
			DHT11_DQ_OUT_HIGH;
			delay_us(2);
		}
	}
}

/***************************************************************************
函数名：DHT11_Read_Bit
功  能：向DHT11读一个位
输  入: 无
输  出：无
返回值：读入数据
备  注：
***************************************************************************/
uint8_t DHT11_Read_Bit(void)
{
	//定义一个脉冲持续时间
	uint8_t retry = 0;
	uint8_t data;
	while(DHT11_DQ_IN && retry < 100)//等待变为低电平
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN && retry < 100)//等待变高电平
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_DQ_IN)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


/***************************************************************************
函数名：DHT11_Read_Byte
功  能：向DHT11读一个字节
输  入: 无
输  出：无
返回值：读入数据
备  注：
***************************************************************************/
uint8_t DHT11_Read_Byte(void)
{
	uint8_t i,data;
	data = 0;
	for (i=0; i<8; i++)
  {
    data<<=1;
    data|=DHT11_Read_Bit();
  }
	return data;
}

/***************************************************************************
函数名：DHT11_Init
功  能：DHT11初始化
输  入: 无
输  出：无
返回值：无
备  注：
***************************************************************************/
uint8_t DHT11_Init(void)
{
	//引脚初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = TEMP_DATA_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TEMP_DATA_GPIO_Port,&GPIO_InitStructure);
	
	DHT11_Rst();
	return DHT11_Check();
}

/***************************************************************************
DHT11_Get_Temperature_Humidity
功  能：读取温度湿度
输  入: 无
输  出：无
返回值：读取到的温度数据
备  注：适用于总线上只有一个DHT11的情况
***************************************************************************/
uint8_t DHT11_Get_Temperature_Humidity(uint8_t *temp, uint8_t *humi)
{
	uint8_t buf[5];
	uint8_t i;
	
	DHT11_Rst();
	if(DHT11_Check() == 0)
	{
			for(i=0; i<5; i++) //读取40位数据
      {
         buf[i]=DHT11_Read_Byte();
      }
      if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
      {
				*humi=buf[0];
        *temp=buf[2];
      }
	}
	else
	{
		return 1;
	}
	return 0;
}

/***************************************************************************
DHT11_Get_Float_Temperature_Humidity
功  能：读取小数温度湿度
输  入: 无
输  出：无
返回值：读取到的温度数据
备  注：适用于总线上只有一个DHT11的情况
***************************************************************************/
uint8_t DHT11_Get_Float_Temperature_Humidity(float *temp, float *humi)
{
	uint8_t buf[5];
	uint8_t i;
	
	DHT11_Rst();
	if(DHT11_Check() == 0)
	{
			for(i=0; i<5; i++) //读取40位数据
      {
         buf[i]=DHT11_Read_Byte();
      }
      if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
      {
				*humi=((buf[0] << 8) + buf[1]) / 10.0;
				*temp=((buf[2] << 8) + buf[3]) / 10.0;
      }
	}
	else
	{
		return 1;
	}
	return 0;
}

