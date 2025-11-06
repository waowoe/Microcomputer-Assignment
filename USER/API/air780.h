#ifndef _AIR780_H
#define _AIR780_H

#include "stm32f10x.h"

#include "stdio.h"	
#include "sys.h" 
#include "string.h"
#include "delay.h"

//阿里云MQTT  连接参数

#define clientId       "ilvjlKMliiF.test.one|securemode=2,signmethod=hmacsha256,timestamp=1698210844090|"    
#define username	   "test.one&ilvjlKMliiF"
#define passwd         "274aaeb4127626a681e927e78426c6bb3ce17eee8ec12ce5aea1cc298edc1b53"
#define mqttHostUrl    "iot-06z00jbus4sj5ja.mqtt.iothub.aliyuncs.com"
#define port           "1883"
#define TOPIC_PUB_0 	"/sys/ilvjlKMliiF/test.one/thing/event/property/post"   //发布 "/sys/iccpF8rzFf1/ruoyu_demo_05/thing/event/property/post"
#define TOPIC_SUB_0 	"/sys/ilvjVzuNHlD/LEI_DA4/thing/service/property/set"		//订阅 "/sys/iccpF8rzFf1/ruoyu_demo_05/thing/service/property/set"
#define TOPIC_PUB_1      "/ilvjlKMliiF/test.one/user/update"		


extern uint8_t Air_Rx_Buf[300];
extern int Air_RX_CNT;
extern uint8_t Air_rec_Flag ;


void Air780_Init(void);
void Air780EX_SendString(char *str);
void Air780_SendBuff(uint8_t *buff,uint16_t length);
void Air780_SendByte(uint16_t date);
void Air780_SendString(unsigned char *str, unsigned short len);

void Air_mqtt_ali_connect(void);
void Air_mqtt_ali_pub(float level,int rsrq,double longitude,double laitude,char *t,int level_warning,int battery,int Q);
void Air_mqtt_ali_sub();




#endif