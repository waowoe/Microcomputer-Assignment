#ifndef __TIME_H_
#define __TIME_H_

#include "stm32f10x.h"
#include "sys.h"

extern uint16_t Time_Period[3];

void TIME_Config(void);

#endif
