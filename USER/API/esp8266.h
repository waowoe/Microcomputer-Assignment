#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "stdio.h"	
#include "sys.h" 
#include "aliyun.h"
#include "delay.h"
#include "usart.h"
#include "control.h"
#include <string.h>
#include <stdio.h>

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


extern int hum_door_value;
extern int soil_door_value;
extern int smoke_door_value;
extern int light_door_value;
extern int temp_door_value;
extern int co2_door_value;
extern int pm25_door_value;
extern int rain_door_value;


extern int led_status;
extern int bump_status;
extern int alarm_status;
extern int wind_status;
extern int auto_mode;
extern int fire_status;
extern int curten_status;
extern int body_status;
extern int door_status;

extern unsigned char esp8266_buf[128];
extern unsigned short esp8266_cnt , esp8266_cntPre;

extern uint8_t RX_BUF[500];
extern uint16_t RX_CNT;
extern uint8_t ESP_rec_Flag;
extern int curten_flag;

void ESP8266_Init(void);

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);
void ESP_U3_SendBuff(uint8_t *buff,uint16_t length);
void ESP_U3_SendString(char *str);

unsigned char *ESP8266_GetIPD(unsigned short timeOut);


#endif
