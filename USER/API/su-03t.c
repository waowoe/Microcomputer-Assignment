#include "su-03t.h"
#include "string.h"
#include "stdio.h"
#include "dht11.h"

#define SU_03_Lengh 50
uint8_t SU_03_Buffer[SU_03_Lengh];
uint8_t SU_03_Send_Buffer[SU_03_Lengh];
uint8_t SU_03_Len=0;
uint8_t SU_03_Flag=0;
extern u8 * humidity_door_value;

extern uint8_t Tempture;
extern uint8_t Hum;
extern uint8_t relay_flag;
extern void relay_set(uint8_t flag);
extern uint8_t mode_flag;

void USART2_IRQHandler(void)
{

	uint8_t date;

	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)  
	{

		SU_03_Buffer[SU_03_Len++]=USART_ReceiveData(USART2);
		if(SU_03_Len>=SU_03_Lengh)
			SU_03_Len=0;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET) 
	{
	
		SU_03_Flag=1;
	
		date=USART_GetFlagStatus(USART2,USART_FLAG_IDLE);

		date=USART_ReceiveData(USART2);
	}
}




void SU_03T_Config(void)
{

 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;    
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);    

	USART_InitTypeDef USART_InitStruct;

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2,&USART_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;   
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;

	NVIC_Init(&NVIC_InitStruct);
	

	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);	
		

	
	USART_Cmd(USART2,ENABLE);
}



void SU_03T_SendByte(uint16_t date)
{

	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET){
	}
	USART_SendData(USART2,date);
}

void SU_03T_SendBuff(uint8_t *buff,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		SU_03T_SendByte(*(buff++));

	}
}


uint8_t SU_03T_Date_Analyze(void)
{

	if(SU_03_Flag==1)
	{

		SU_03_Flag=0;
	
		if(SU_03_Buffer[0]==0xAA&&SU_03_Buffer[1]==0x55&&SU_03_Buffer[3]==0x55&&SU_03_Buffer[4]==0xAA)   //帧头是对的
		{
			switch(SU_03_Buffer[2])
			{
				case 1:
//					
						SU_03_Send_Buffer[0]=0xAA;
						SU_03_Send_Buffer[1]=0x55;	
						SU_03_Send_Buffer[2]=0x01;	
						SU_03_Send_Buffer[3]=Hum;
						SU_03_Send_Buffer[4]=0x55;
						SU_03_Send_Buffer[5]=0xAA;
						SU_03T_SendBuff(SU_03_Send_Buffer,6);
					break;
				case 2:
						SU_03_Send_Buffer[0]=0xAA;
						SU_03_Send_Buffer[1]=0x55;	
						SU_03_Send_Buffer[2]=0x02;	
						SU_03_Send_Buffer[3]=Tempture;
						SU_03_Send_Buffer[4]=0x55;
						SU_03_Send_Buffer[5]=0xAA;
						SU_03T_SendBuff(SU_03_Send_Buffer,6);					
					break;	
				case 3:
//				
						SU_03_Send_Buffer[0]=0xAA;
						SU_03_Send_Buffer[1]=0x55;	
						SU_03_Send_Buffer[2]=0x03;	
						SU_03_Send_Buffer[3]=(*humidity_door_value);
						SU_03_Send_Buffer[4]=0x55;
						SU_03_Send_Buffer[5]=0xAA;
						SU_03T_SendBuff(SU_03_Send_Buffer,6);					
					break;	
				case 4:
						relay_flag = 1;	relay_set(1);	
					break;
				case 5:
						relay_flag = 0;	relay_set(0);					
					break;	
				case 6:
						mode_flag = 1; 
					break;	
				case 7:
						mode_flag = 0; 
					break;	
				case 8:
					break;
				default:
					break;
			}
			memset(SU_03_Buffer,0,sizeof(SU_03_Buffer));
			SU_03_Len=0;			
		}
		else  
		{
			memset(SU_03_Buffer,0,sizeof(SU_03_Buffer));
			SU_03_Len=0;
			return 0;
		}
	}
}

