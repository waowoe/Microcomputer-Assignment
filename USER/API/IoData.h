#ifndef _IODATA_H
#define _IODATA_H

#include "sys.h"
#include "stm32f10x.h"

#define HC_SR_Port 	GPIOA
#define HC_SR_Pin 	GPIO_Pin_6
#define Fire_Port	
#define Fire_Pin

extern uint8_t hcsr,fire;

void IoData_Init();

void Get_HCSR_Value(uint8_t *Value);
void Get_Fire_Value(uint8_t *Value);


#endif