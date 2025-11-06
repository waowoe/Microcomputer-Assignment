#include "esp8266.h"
#include "timer.h"
#include "dht11.h"
#include "uln2003.h"
#include "adc.h"

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"yhblsqt\",\"lc667788\"\r\n"

#define ESP8266_ALIYUN_INFO		"AT+CIPSTART=\"TCP\",\"a1IYaEMjgjV.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n"

//ESP_Send_Cmd_Check("AT+CIPSTART=\"TCP\",\"iot-06z00jbus4sj5ja.mqtt.iothub.aliyuncs.com\",1883\r\n","OK",1800)


unsigned char esp8266_buf[128];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;

uint8_t RX_BUF[500];
uint8_t rx_buff[500];
uint16_t RX_CNT = 0;
uint8_t ESP_rec_Flag = 0;
char buff[20];

int curten_flag = 0;

int hum_door_value = 20;   		//湿度阈值解析
int soil_door_value = 20;			//土壤湿度阈值解析
int smoke_door_value = 500;			//烟雾阈值解析
int temp_door_value = 30;    	//温度阈值解析
int light_door_value = 0;			//光照阈值解析
int co2_door_value = 0;				//co2阈值解析
int pm25_door_value = 0;				//pm2.5阈值解析
int rain_door_value = 0;			//降雨量阈值解析


int led_status = 0;				//LED灯状态
int bump_status = 0;			//水泵状态
int alarm_status = 0;			//警告（蜂鸣器）状态
int wind_status = 0;			//风扇状态
int curten_status = 0;		//窗帘状态
int fire_status = 0;			//火焰状态
int body_status = 0;			//人体状态
int door_status = 0;			//门磁状态
int auto_mode = 0;  //auto_mode = 0;表示为手动模式
						//auto_mode = 1;表示为自动模式

uint8_t esp_init_finish_flag = 0;

void USART3_IRQHandler(void)
{
	int i,j=0;
	uint8_t date;
	Time_Period[1] = 0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)==SET)    
	{	
		date = USART_ReceiveData(USART3);
		RX_BUF[RX_CNT++]=date;   //2.中断接收数据
		if(RX_CNT>=500) RX_CNT=0;
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);   //3.清楚中断标志位
	}
	if(USART_GetITStatus(USART3,USART_IT_IDLE)==SET)  //空闲中断
	{
		ESP_rec_Flag = 1;
		date=USART_GetFlagStatus(USART3,USART_FLAG_IDLE);
		date=USART_ReceiveData(USART3);
		
		for(j=0;j<RX_CNT;j++)   //数据缓冲过滤
		{
			rx_buff[j] = RX_BUF[j+10];
		}
		U1_SendString(rx_buff);
		
		if(strstr((char *)rx_buff,"set")!=NULL)   //判断是否接收到云平台的消息
		{	
			if(strstr((char *)rx_buff,"hum_door")!=NULL) //解析设置的湿度阈值数据
			{
				char *last_comma = strstr((char *)rx_buff,"hum_door");
				char value[] = {0};
				for(i=0;i<3;i++)
				{
					value[i] = *(last_comma+10+i);
				}
				sscanf(value,"%d",&hum_door_value);
			}
			else if(strstr((char *)rx_buff,"temp_door")!=NULL)  //解析设置的温度阈值数据
			{
				char *last_comma = strstr((char *)rx_buff,"temp_door");
				char value[] = {0};
				for(i=0;i<3;i++)
				{
					value[i] = *(last_comma+11+i);
				}
				sscanf(value,"%d",&temp_door_value);
			}
			else if(strstr((char *)rx_buff,"light_door")!=NULL)  //解析设置的光照阈值数据
			{
				char *last_comma = strstr((char *)rx_buff,"light_door");
				char value[6] = {0};
				for(i=0;i<4;i++)
				{
					value[i] = *(last_comma+12+i);
				}
				sscanf(value,"%d",&light_door_value);
			}
//			else if(strstr((char *)rx_buff,"rain_door")!=NULL)  //解析设置的降雨量阈值数据
//			{
//				
//				int *raindoor = &rain_door_value;
//				char *last_comma = strstr((char *)rx_buff,"rain_door");
//				char value[6] = {0};
//				for(i=0;i<4;i++)
//				{
//					value[i] = *(last_comma+11+i);
//				}
//				sscanf(value,"%d",raindoor);
//			}
			else if(strstr((char *)rx_buff,"co2_door")!=NULL)  //解析设置的co2阈值数据
			{
				char *last_comma = strstr((char *)rx_buff,"co2_door");
				char value[6] = {0};
				for(i=0;i<4;i++)
				{
					value[i] = *(last_comma+10+i);
				}
				sscanf(value,"%d",&co2_door_value);
			}
			else if(strstr((char *)rx_buff,"soil_door")!=NULL)  //解析设置的土壤湿度阈值数据
			{
				char *last_comma = strstr((char *)rx_buff,"soil_door");
				char value[6] = {0};
				for(i=0;i<4;i++)
				{
					value[i] = *(last_comma+11+i);
				}
				sscanf(value,"%d",&soil_door_value);
			}
//			else if(strstr((char *)rx_buff,"pm25_door")!=NULL)  //解析设置的PM2.5阈值数据
//			{
//				
//				int *pm25door = &pm25_door_value;
//				char *last_comma = strstr((char *)rx_buff,"pm25_door");
//				char value[6] = {0};
//				for(i=0;i<4;i++)
//				{
//					value[i] = *(last_comma+11+i);
//				}
//				sscanf(value,"%d",pm25door);
//			}
			else if(strstr((char *)rx_buff,"mode")!=NULL)  //解析设置的模式数据
			{
				/* auto_mode = 0 手动模式  auto_mode = 1 自动模式   */
				char *last_comma = strstr((char *)rx_buff,"mode");  
				sscanf((last_comma+6),"%d",&auto_mode);
			}
			if(auto_mode == 0)
			{
				if(strstr((char *)rx_buff,"wind")!=NULL)
				{
					Wind = !Wind; 		
					wind_status = Wind;
				}
				else if(strstr((char *)rx_buff,"water")!=NULL)
				{
					bump_status = !bump_status;
					Water_pump = bump_status;
				}
				else if(strstr((char *)rx_buff,"alarm")!=NULL)
				{
					Beep  = !Beep;
					alarm_status = Beep;
				}
				else if(strstr((char *)rx_buff,"led")!=NULL)
				{
					LED = !LED;
					led_status = LED;
				}
			}
			Mqtt_Publiash();
			
			
		}
        if(esp_init_finish_flag ==1)
        {
            ESP8266_Clear();
					
        }
	}
}
//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{
	memset(rx_buff,0,300);
	memset(RX_BUF,0,300);
	RX_CNT = 0;
}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(RX_CNT == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(RX_CNT == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		RX_CNT = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = RX_CNT;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}
void Usart3_SendString(unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	for(; count < len; count++)
	{
		USART_SendData(USART3, *str++);									//发送数据
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		//等待发送完成
	}
}
//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	int timeOut = 1000;

	Usart3_SendString((unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)RX_BUF, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存			
				return 0;
			}
		}
			delay_ms(10);
	}
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart3_SendString(data, len);		//发送设备连接请求数据
	}

}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;
	do
	{
		if(ESP8266_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)RX_BUF, "IPD,");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		
	delay_ms(5);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(void)
{
	ESP8266_Clear();
	while(ESP8266_SendCmd("AT\r\n", "OK"))
			delay_ms(500);
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
			delay_ms(500);
	while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
			delay_ms(500);
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
			delay_ms(500);
	//UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ALIYUN_INFO, "CONNECT"))
			delay_ms(500);
	while(ESP8266_SendCmd("AT+CIPMODE=1\r\n","OK"))
			delay_ms(500);
	while(ESP8266_SendCmd("AT+CIPSEND\r\n",">"))
			delay_ms(500);
	Mqtt_Connet_Aliyun();
	delay_ms(500);
	Mqtt_Subscribe_Aliyun();
	delay_ms(500);
	Mqtt_Publiash_Aliyun();
	esp_init_finish_flag = 1;
	//UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");
}
//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP_U3_SendByte(uint16_t date)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET){
	}
	USART_SendData(USART3,date);
}
void ESP_U3_SendBuff(uint8_t *buff,uint16_t length)
{
	uint16_t i;
	for(i=0;i<length;i++)
	{
		ESP_U3_SendByte(buff[i]);
	}
}
void ESP_U3_SendString(char *str)
{
	while((*str)!='\0')
		 
	{
		ESP_U3_SendByte(*str++);
	}
}