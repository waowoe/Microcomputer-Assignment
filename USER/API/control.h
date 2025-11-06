#ifndef _CONTROL_H
#define _CONTROL_H

#include "sys.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rcc.h"


#define Beep 		PAout(8)
#define Wind 		PBout(8)
#define Water_pump  PBout(9)
#define LED  		PCout(14)


void Control_Init(void);//≥ı ºªØ

//void Wind_ON(void);

		 				    
#endif
