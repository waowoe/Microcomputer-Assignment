#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


/*
1-2智慧农业  
uart1 用作printf 和 4G模块串口通信
uart2 用作KQM2800通信使用
uart3 用作esp8266通信使用 
*/
void U1_SendString(uint8_t *str);
void U2_SendString(uint8_t *str);
void uart3_init(unsigned int baud);
void uart2_init(unsigned int baud);
void uart1_init(unsigned int baud);

void UART1_SendDate(uint8_t Date);
void UART2_SendDate(uint8_t Date);
void UART3_SendDate(uint8_t Date);

#endif


