#include "air780.h"
#include <string.h>
#include <stdio.h>
#include "Lcd_Driver.h"
#include "LCD_Config.h"

#define	Air_config   "AT+MCONFIG=\"test.one&ilvjlKMliiF\",\"ilvjlKMliiF.test.one|securemode=2,signmethod=hmacsha256,timestamp=1698210844090|\",\"274aaeb4127626a681e927e78426c6bb3ce17eee8ec12ce5aea1cc298edc1b53\"\r\n"
#define Air_start	 "AT+MIPSTART=\"iot-06z00jbus4sj5ja.mqtt.iothub.aliyuncs.com\",1883\r\n"
#define Air_connect	 "AT+MCONNECT=1,60\r\n"

uint8_t Air_Rx_Buf[300];
int Air_RX_CNT = 0;
uint8_t Air_rec_Flag = 0;

/*
连接为USART1
连接流程
1）退出休眠  
2）AT回复
3）判断网络注册状态和附着状态
4）判断信号质量

5）MQTT相关内容
	1）连接订阅和发布
	
UART相关：

字符串发送函数
中断接收函数
中断解析函数

*/
void USART1_IRQHandler(void)
{
	uint8_t date;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)    
	{	
		date = USART_ReceiveData(USART1);
		Air_Rx_Buf[Air_RX_CNT++]=date;   //2.中断接收数据
		if(Air_RX_CNT>=350)
				Air_RX_CNT=0;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);   //3.清楚中断标志位
	}
	if(USART_GetITStatus(USART1,USART_IT_IDLE)==SET)  //空闲中断
	{
		Air_rec_Flag = 1;
		date=USART_GetFlagStatus(USART1,USART_FLAG_IDLE);
		date=USART_ReceiveData(USART1);
	}
}

void Air_Clear(void)
{
	memset(Air_Rx_Buf, 0, sizeof(Air_Rx_Buf));
	Air_RX_CNT = 0;
}



_Bool Air780_SendCmd(char *cmd, char *res)
{
	unsigned char timeOut = 200;
	Air780_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	while(timeOut--)
	{
		if(Air_rec_Flag == 1)	//如果收到数据
		{
//			Gui_DrawFont_GBK16(0,16,BLACK,WHITE,"空气湿度:    %");
//			printf();
			Air_rec_Flag = 0;
			if(strstr((const char *)Air_Rx_Buf, res) != NULL)		//如果检索到关键词
			{
				Air_Clear();									//清空缓存			
				return 0;
			}
		}
		delay_ms(10);
	}
	return 1;
}

void Air780_Init(void)
{
	Air_Clear();
	
	while(Air780_SendCmd("AT+CSCLK=0\r\n", "OK"))
			delay_ms(500);
//	Gui_DrawFont_GBK16(0,16,BLACK,WHITE,"空气湿度:    %");
	while(Air780_SendCmd("AT\r\n", "OK"))
			delay_ms(500);
	while(Air780_SendCmd("AT+CGATT?\r\n", "1"))
			delay_ms(500);
	while(Air780_SendCmd("AT+CGREG?\r\n", "1"))
			delay_ms(500);
	while(Air780_SendCmd(Air_config, "OK"))
			delay_ms(500);
	while(Air780_SendCmd(Air_start, "CONNECT"))
			delay_ms(500);
	Gui_DrawFont_GBK16(0,0,BLACK,WHITE,"空气温度:    ℃");
	while(Air780_SendCmd(Air_connect,"CONNACK"))
			delay_ms(500);
	Gui_DrawFont_GBK16(0,16,BLACK,WHITE,"空气湿度:    %");
//	while(Air780_SendCmd("AT+CIPSEND\r\n",">"))
//			delay_ms(500);
//	Mqtt_Connet_Aliyun();
//	delay_ms(1000);
//	Mqtt_Publiash_Aliyun();
}

//uint8_t NB_net_attach(void)				
//{
//		int a;
//		int *value=&a;
//		uint8_t res[50];
//		memset(res,0,50);
//		printf("AT+CGATT?\r\n");
//		HAL_UART_Receive(&hlpuart1,res,sizeof(res), 5000);
//		char *last_comma = strrchr((char *)res, ':');
//		if (last_comma != NULL ) {
//		sscanf(last_comma + 1, "%d", value);
//			if(a == 1)
//				return 1;
//			else 
//				return 0;
//	}
//}

//uint8_t NB_net_state(void)
//{
//		int a;
//		int *value=&a;
//		uint8_t res[100];
//		memset(res,0,100);
//		printf("AT+CEREG?\r\n");
//		HAL_UART_Receive(&hlpuart1,res,sizeof(res), 5000);
//		char *last_comma = strrchr((char *)res, ',');
//		if (last_comma != NULL ) {
//		sscanf(last_comma + 1, "%d", value);
//			if((a == 1)||(a==5))
//				return 1;
//			else 
//				return 0;
//	}
//}

//uint8_t NB_net_get_ip(void)
//{
//		uint8_t res[100];
//		memset(res,0,100);
//		printf("AT+CGPADDR?\r\n");
//		HAL_UART_Receive(&hlpuart1,res,sizeof(res),2000);
//		if(strlen((char *)res) != 0)
//			return 1;
//		else
//			return 0;
//}
//uint8_t Ali_MQtt_connect(void)
//{
//		nb_mqtt_ali_open();
//		HAL_Delay(5000);
//		uint8_t res[200];
//		memset(res,0,200);
//		nb_mqtt_ali_connect();
//		HAL_UART_Receive(&hlpuart1,res,sizeof(res),5000);
//		if(strlen((char *)res) != 0)
//		{
//			return 1;
//		}
//		return 0;
//}

//uint8_t NB_connect_net(void)
//{
//	if(NB_exit_sleep("OK")==1)
//	{
//		if(NB_net_attach() == 1)
//		{
//			if(NB_net_state()== 1)
//			{
//				if(NB_net_get_ip()==1)
//				{
//					return 1;
//				}
//			}
//		}
//	}
//	return 0;
//}

void Air_mqtt_ali_connect(void)
{
		while(Air780_SendCmd("AT+CSCLK=0\r\n", "OK"))
			delay_ms(500);
	Gui_DrawFont_GBK16(0,16,BLACK,WHITE,"空气湿度:    %");
	while(Air780_SendCmd("AT\r\n", "OK"))
			delay_ms(500);
	printf("AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",clientId,username,passwd);
	delay_ms(1000);
	printf("AT+MIPSTART=\"%s\",1883\r\n",mqttHostUrl);
	delay_ms(1000);
	printf("AT+MCONNECT=1,60\r\n");  //1 表示清理会话，360 为保活周期，单位为秒
	delay_ms(1000);
}

void Air_mqtt_ali_pub(float level,int rsrq,double longitude,double laitude,char *t,int level_warning,int battery,int Q)
{
	printf("AT+MPUBEX=\"%s\",0,0\r\n",TOPIC_PUB_0);
	delay_ms(2000);
	printf("{\"id\":1698251347165,\"params\":{\"level\":%f,\
\"rsrq\":%d,\
\"GPS\":{\"longitude\":%f,\"laitude\":%f},\
\"t\":\"%s\",\
\"level_warning\":%d,\
\"battery\":%d,\
\"Q\":%d},\
\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}\r\n",level,rsrq,longitude,laitude,t,level_warning,battery,Q);
	delay_ms(2000);
	printf("\x1a");
}

void Air_mqtt_ali_sub()
{
	printf("AT+MSUB=\"%s\",0\r\n",TOPIC_SUB_0);

}

void Air780_SendString(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		USART_SendData(USART1, *str++);									//发送数据
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		//等待发送完成
	}
}

void Air780_SendByte(uint16_t date)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET){
	}
	USART_SendData(USART1,date);
}
void Air780_SendBuff(uint8_t *buff,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		Air780_SendByte(buff[i]);
	}
}
void Air780EX_SendString(char *str)
{
	while((*str)!='\0')
	{
		Air780_SendByte(*str++);
	}
}
