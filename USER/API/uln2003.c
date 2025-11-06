#include "uln2003.h"

// 电机的旋转方向

uint8_t STEP;	// 用于存储电机正在走过的整步编号

void Uln2003_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = IN1|IN4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, IN1|IN4);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = IN2|IN3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, IN2|IN3);
}

/**
  * @brief  电机停转函数
  * @param  无
  * @retval 无
  */
void Stepper_stop(void)
{
	GPIO_ResetBits(GPIOA, IN2|IN3);
	GPIO_ResetBits(GPIOB, IN1|IN4);
}

/**
  * @brief  4拍单相整步驱动函数
  * @param  StepNum 	整步编号，0~3对应A~D
  * @param	Delay_Time_xms 		每步旋转后延时时间x ms，用于控制步进电机速度（一般需大于等于2）
  * @retval 无
  */
void Stepper_SingleStep(uint8_t StepNum, uint16_t Delay_Time_xms)
{
	switch(StepNum)  //A-AB-B-BC-C-CD-D-DA
	{
		case 0:		// A
			GPIO_SetBits(GPIOB, IN1);
			GPIO_ResetBits(GPIOA, IN2);
			GPIO_ResetBits(GPIOA, IN3);
			GPIO_ResetBits(GPIOB, IN4);
		break;
		case 1:		// AB
			GPIO_SetBits(GPIOB, IN1);
			GPIO_SetBits(GPIOA, IN2);
			GPIO_ResetBits(GPIOA, IN3);
			GPIO_ResetBits(GPIOB, IN4);
		break;			
		case 2:		// B
			GPIO_ResetBits(GPIOB, IN1);
			GPIO_SetBits(GPIOA, IN2);
			GPIO_ResetBits(GPIOA, IN3);
			GPIO_ResetBits(GPIOB, IN4);
		break;
		case 3:		// BC
			GPIO_ResetBits(GPIOB, IN1);
			GPIO_SetBits(GPIOA, IN2);
			GPIO_SetBits(GPIOA, IN3);
			GPIO_ResetBits(GPIOB, IN4);
		break;
		case 4:		// C
			GPIO_ResetBits(GPIOB, IN1);
			GPIO_ResetBits(GPIOA, IN2);
			GPIO_SetBits(GPIOA, IN3);
			GPIO_ResetBits(GPIOB, IN4);
		break;
		case 5:		// CD
			GPIO_ResetBits(GPIOB, IN1);
			GPIO_ResetBits(GPIOA, IN2);
			GPIO_SetBits(GPIOA, IN3);
			GPIO_SetBits(GPIOB, IN4);
		break;			
		case 6:		// D
			GPIO_ResetBits(GPIOB, IN1);
			GPIO_ResetBits(GPIOA, IN2);
			GPIO_ResetBits(GPIOA, IN3);
			GPIO_SetBits(GPIOB, IN4);
		break;
		case 7:		// DA
			GPIO_SetBits(GPIOB, IN1);
			GPIO_ResetBits(GPIOA, IN2);
			GPIO_ResetBits(GPIOA, IN3);
			GPIO_SetBits(GPIOB, IN4);
		break;
		default: break;
	}
	delay_ms(Delay_Time_xms);	// 延时，控制电机速度
	Stepper_stop();				// 断电，防止电机过热
}

/**
  * @brief  步进电机按步旋转
  * @param  direction		电机旋转方向，可以是Forward（正传）或者Reversal（反转）
  * @param	step			电机转过的步数
  * @param	Delay_Time_xms	每步旋转后延时时间x ms，用于控制步进电机速度（一般需大于等于2）
  * @retval 无
  */

void STEP_MOTOR_NUM (u8 RL,u16 num,u8 speed)  //电机按步数运行
{
	u16 i;
	for(i=0;i<num;i++)
    {	
		if(RL==1)
		{ //当RL=1右转，RL=0左转
			STEP++;
			if(STEP>7)STEP=0;
		}
		else
		{
			if(STEP==0)STEP=8;
			STEP--;
		}
		Stepper_SingleStep(STEP,speed);
	}
}
/**
  * @brief  步进电机按整数圈旋转
  * @param  direction		电机旋转方向，可以是Forward（正传）或者Reversal（反转）
  * @param  Loop			电机旋转的圈数
  * @param  Delay_Time_xms	每步旋转后延时时间x ms，用于控制步进电机速度（一般需大于等于2）
  * @retval 
  */
void Stepper_RotateByLoop(RotDirection direction, uint32_t Loop, uint16_t Delay_Time_xms)
{
	STEP_MOTOR_NUM(direction, Loop * 4096, Delay_Time_xms);
}

