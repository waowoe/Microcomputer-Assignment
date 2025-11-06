#include "IoData.h"

uint8_t hcsr,fire;

void IoData_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		
	
	GPIO_InitStructure.GPIO_Pin = HC_SR_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(HC_SR_Port, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	
}

void Get_Fire_Value(uint8_t *Value)
{
//	*Value = GPIO_ReadInputDataBit(HC_SR_Port,HC_SR_Pin);
}

void Get_HCSR_Value(uint8_t *Value)
{
	*Value = GPIO_ReadInputDataBit(HC_SR_Port,HC_SR_Pin);
}