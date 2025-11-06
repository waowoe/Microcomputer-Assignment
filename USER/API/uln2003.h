#ifndef _ULN2003_H

#define _ULN2003_H

#include "stm32f10x.h"
#include "Delay.h"

#define IN1  GPIO_Pin_15
#define IN2  GPIO_Pin_11
#define IN3  GPIO_Pin_12
#define IN4  GPIO_Pin_5

typedef enum 
{
	Forward = 0,
	Reversal = 1
} RotDirection;

extern RotDirection direction;

void Stepper_stop(void);

void Uln2003_Init(void);
extern void Stepper_RotateByLoop(RotDirection direction, uint32_t Loop, uint16_t Delay_Time_xms);


#endif