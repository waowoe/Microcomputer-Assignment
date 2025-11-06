#ifndef __SU_03T_H_
#define __SU_03T_H_

#include "stm32f10x.h"


void SU_03T_Config(void);
void SU_03T_SendByte(uint16_t date);
void SU_03T_SendBuff(uint8_t *buff,uint16_t length);
uint8_t SU_03T_Date_Analyze(void);

#endif