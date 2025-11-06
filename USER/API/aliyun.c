#include "aliyun.h"
#include "dht11.h"
#include "MQTTPacket.h"
#include "esp8266.h"
#include "delay.h"
#include "adc.h"
#include "kqm2800.h"

#define KEEPALIVE_INTERVAL     540  	//保活周期

#define ClientID   	"a1IYaEMjgjV.Device_014|securemode=2,signmethod=hmacsha256,timestamp=1746607077775|"    	//设备名称
#define Username    "Device_014&a1IYaEMjgjV"			//产品ID
#define Password    "0ee010faef567e972930965a624f897bdf9fcc0416aaf0ef01a0bb933f79ff59"	

#define Publish_Topic    "/sys/a1IYaEMjgjV/Device_014/thing/event/property/post"
#define Subscribe_Topic  "/sys/a1IYaEMjgjV/Device_014/thing/service/property/set"


extern int temp1;
extern int humi1;
extern int lux1;
extern int ledDutyRatio1;
extern int distance1;

char buf[500] = {0};
uint8_t ESP_Send_Buff[300];  //串口3的发送缓存区
uint8_t Mqtt_Connet_Flag=0; //0 表示mqtt未接通   1 表示mqtt已经接通

int ESP_Send_Len = 0;
int payloadlen = 0;

int i=0;//周期连接标志位

void Mqtt_Connet_Aliyun(void)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;	  //定义结构体
	
	data.clientID.cstring = ClientID;     //设备名称
	data.keepAliveInterval = KEEPALIVE_INTERVAL;  //保活周期
	data.cleansession = 1;     //清理会话
	data.username.cstring = Username;  //产品ID
	data.password.cstring = Password;	  //密码
	 
	memset(RX_BUF,0,300);   //清空接收缓冲区
	RX_CNT = 0;
	ESP_Send_Len = MQTTSerialize_connect(ESP_Send_Buff, 300, &data);  //调用库提供的函数，按照mqtt的格式打包，返回的是打包之后的数组有效数据长度
	ESP_U3_SendBuff(ESP_Send_Buff,ESP_Send_Len);    //调用串口3,将打包后的数据通过ESP8266透传到平台
	
	/*if(rx_buff[0]==0x20&&rx_buff[1]==0x02)  //检测回复，检测云平台的回复的CONnack连接确认报头
		Aliyun_Connet_Flag = 1; 
	else 
		Aliyun_Connet_Flag = 0;
	memset(rx_buff,0,500);
	RX_CNT = 0;*/
}


void Mqtt_Publiash_Aliyun(void)
{
	int payloadlen = 0;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = Publish_Topic;

	sprintf(buf,"{\"id\": 1729255817284,\
\"params\": {\
\"temperature\":%d,\
\"humidity\":%d,\
\"soil_hum\":%d,\
\"lightvalue\":%d,\
\"co2\":%d,\
\"wind\":%d,\
\"led\":%d,\
\"water\":%d,\
\"co2_door\":%d,\
\"rain_door\":%d,\
\"temp_door\":%d,\
\"hum_door\":%d,\
\"light_door\":%d,\
\"soil_hum_door\":%d,\
\"mode\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"
,temp,hum,Soil_Value,Light_Value,Kqzl.CO2,wind_status,led_status,bump_status,co2_door_value,rain_door_value,temp_door_value,hum_door_value,light_door_value,soil_door_value,auto_mode);                                     

	payloadlen = strlen(buf);								//,(int)Fire,(int)body ,PM
	memset(RX_BUF,0,500);   //清空接收缓冲区
	RX_CNT=0;
	
	ESP_Send_Len= MQTTSerialize_publish(ESP_Send_Buff, 500, 0, 1, 0, 0,topicString, (unsigned char*)buf, payloadlen);
	ESP_U3_SendBuff(ESP_Send_Buff,ESP_Send_Len); 
}


void Mqtt_Publiash(void)
{
	int payloadlen = 0;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = Publish_Topic;

	sprintf(buf,"{\"id\": 1728826218677,\
\"params\": {\
\"wind\":%d,\
\"led\":%d,\
\"water\":%d,\
\"co2_door\":%d,\
\"rain_door\":%d,\
\"pm25_door\":%d,\
\"temp_door\":%d,\
\"hum_door\":%d,\
\"light_door\":%d,\
\"soil_hum_door\":%d,\
\"mode\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"
,wind_status,led_status,bump_status,co2_door_value,rain_door_value,pm25_door_value,temp_door_value,hum_door_value,light_door_value,soil_door_value,auto_mode);                                     

	payloadlen = strlen(buf);								//,(int)Fire,(int)body ,PM
	memset(RX_BUF,0,500);   //清空接收缓冲区
	RX_CNT=0;
	
	ESP_Send_Len= MQTTSerialize_publish(ESP_Send_Buff, 500, 0, 1, 0, 0,topicString, (unsigned char*)buf, payloadlen);
	ESP_U3_SendBuff(ESP_Send_Buff,ESP_Send_Len); 
}

void Mqtt_Subscribe_Aliyun(void)
{
	int req_qos = 0;
	int msgid = 1;
	MQTTString topicString = MQTTString_initializer;
	topicString.cstring = Subscribe_Topic;
	
	memset(RX_BUF,0,500);   //清空接收缓冲区
	
	RX_CNT = 0;
	
	ESP_Send_Len = MQTTSerialize_subscribe(ESP_Send_Buff, 500, 0, msgid, 1, &topicString, &req_qos);
	
	ESP_U3_SendBuff(ESP_Send_Buff,ESP_Send_Len); 

}