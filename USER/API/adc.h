#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
#include "stm32f10x.h"
#include "stm32f10x_adc.h"

#define ADC1_PORT 	GPIOA

#define Soil_Pin	GPIO_Pin_5
#define Light_Pin  	GPIO_Pin_1

extern u16 Light_Value,Soil_Value;

void Adc_Init(void);

void Get_Light_Value(u16 *Value);

void Get_Soil_Value(u16 *Value);

#endif 