#include "dht11.h"

uint8_t Time = 0;
uint8_t temp,hum;


void DHT11_Init(void)
{	
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

void DHT11_Mode(uint8_t mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(mode)
	{
		GPIO_InitStructure.GPIO_Pin = DHT11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin =  DHT11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	}
	GPIO_Init(DHT11_PROT, &GPIO_InitStructure);
}

void DHT11_Sends_Start(void)
{
	DHT11_Mode(OUT);
	
	DHT11_Low;
	delay_ms(18);	//18ms
	DHT11_High;
	delay_us(40);	//40us
}

void DHT11_Sends_Response(void)
{
	DHT11_Mode(INT);
	
	while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11) == 0 && (Time < 100))	//Low 80us
	{
		Time++;
		delay_us(1);	//100us DHT11=0 80us
	}
	Time = 0;
	
	while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11) == 1 && (Time < 100))	//High 80us
	{
		Time++;
		delay_us(1);	//100us DHT11=0 80us
	}	
	Time = 0;
}

uint8_t DHT11_Read_Byte(void)
{
	int i = 0;
	uint8_t data = 0;
	
	for( i = 0; i < 8; i++)
	{
		DHT11_Mode(INT);
		
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11) == 0 && (Time < 100))	
		{
			Time++;
			delay_us(1);	//100us DHT11=0 80us
		}	
		Time = 0;
		
		data <<= 1;
	
		delay_us(40);	//get hight 70us
		
		if(GPIO_ReadInputDataBit(DHT11_PROT, DHT11) == 1)
		{
			data |= 0x01;
		
		}
		
		while(GPIO_ReadInputDataBit(DHT11_PROT,DHT11) == 1 && (Time < 100))	
		{
			Time++;
			delay_us(1);	//100us DHT11=0 80us
		}	
		Time = 0;
	}
	return data;
}

void DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
	uint8_t DATA[5] = {0,0,0,0,0,};
	int i = 0;
	DHT11_Sends_Start();
	DHT11_Sends_Response();
	
	for( i = 0; i < 5; i++)
	{
		DATA[i] = DHT11_Read_Byte();
	}
	delay_ms(1);	//1ms 50us
	
	if(DATA[0] + DATA[1] + DATA[2] + DATA[3] == DATA[4])
	{
		*temp = DATA[2];
		*humi = DATA[0];
		printf("温度=%d,湿度=%d\r\n",*temp,*humi);
	}
	else
	{
		for( i = 0; i < 5; i++)
		{
			DATA[i] = 0;
		}
	
	}
}

