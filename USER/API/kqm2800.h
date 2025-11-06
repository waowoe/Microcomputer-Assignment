#ifndef _KQM2800_H_
#define _KQM2800_H_

#include "usart.h"
#include "sys.h"
#include "delay.h"

typedef struct
{
	float   VOC;
	float   CH2O;
	int CO2;
}KQZL;

extern KQZL Kqzl;

void Clear_KQM2800_Buff(void);
void Get_KQM2800_Data(void);
void Get_CO2_Data(void);

#endif