#include "kqm2800.h"


uint8_t U2_RX_BUF[50] = {0};  //串口接收存储
uint8_t U2_rx_buff[50] = {0}; //数据缓冲存储
uint8_t U2_RX_CNT = 0;			    //串口接收数据长度
uint8_t U2_rx_over_flag = 0;		//串口数据发送完成标志位

KQZL Kqzl; //空气质量参数

/*USART2  rx 负责接收数据*/
void USART2_IRQHandler(void)
{
	int i,j=0;
	uint8_t date;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)    
	{	
		date = USART_ReceiveData(USART2);
		U2_RX_BUF[U2_RX_CNT++]=date;   //2.中断接收数据
		if(U2_RX_CNT >= 50) U2_RX_CNT = 0;
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);   //3.清楚中断标志位
	}
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET)  //空闲中断
	{
		U2_rx_over_flag = 1;
		date=USART_GetFlagStatus(USART2,USART_FLAG_IDLE);
		date=USART_ReceiveData(USART2);
	}
}

void Get_CO2_Data(void)
{
	uint8_t date_cs;
	if(U2_rx_over_flag == 1)  //空闲中断中变成1,
	{
		U2_rx_over_flag = 0; //清除接收完成标志位
		date_cs = U2_RX_BUF[0]+U2_RX_BUF[1]+U2_RX_BUF[2]+U2_RX_BUF[3]+U2_RX_BUF[4];
		if(date_cs==U2_RX_BUF[5])  //校验通过
		{
			Kqzl.CO2 = U2_RX_BUF[1]*256 + U2_RX_BUF[2];	   //取CO2,分辨率为1
			Clear_KQM2800_Buff();   //清空接收缓冲区,记着清空接收到的长度
			printf("CO2=%d",Kqzl.CO2);
		}
		else
		{
			printf("kqm2800校验失败\r\n");	
			Clear_KQM2800_Buff();			  //清空接收缓冲区,记着清空接收到的长度
		}
	}
}

void Clear_KQM2800_Buff(void)
{
	memset(U2_RX_BUF,0,U2_RX_CNT);
	U2_RX_CNT = 0;
}