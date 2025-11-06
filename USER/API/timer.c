#include "timer.h"
#include "stm32f10x_tim.h"
#include "gizwits_product.h"

uint16_t Time_Period[3]={0,0,6000};  //0为计数值，10000为周期值

void TIME_Config(void)
{
	//1.开TIME6的时钟
	NVIC_InitTypeDef  NVIC_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//2.定义定时器相关的结构体变量
	
	//3.给结构体赋值
	TIM_TimeBaseInitStruct.TIM_Period = 1000-1;  //分频后的时钟频率为1M,计1000次数对应的周期为1ms
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;  //将72M的时钟频率分频为1M
	
	//4.调用INIT函数，将参数写入到寄存器中
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	
	//5.配置中断触发方式
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	//6.定义中断管理需要的变量
	
	//7.给该结构体赋值
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;  //配置中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;   //使能中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 10;       //抢占优先级 因为我们用2分组，抢占和次级各占2位，能分配的数字为0--3
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;  //次级优先级
	
	//8.调用init 将参数写入到寄存器中
	NVIC_Init(&NVIC_InitStruct);
	
	//9.使能定时器

	TIM_Cmd(TIM3,ENABLE);
}

//编写TIME6中断服务函数
//1.函数的名字不能随便定义，必须从启动文件复制
//2.不能返回值，不能传参数,不需要声明
//3.中断服务函数不能有过多的延时操作，检测标志位，置标志位
//uint16_t TIME3_Period=0;
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)  //检测更新中断是否发生
	{
//		TIME3_Period++;
		gizTimerMs();
		Time_Period[0]++;
        Time_Period[1]++;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清楚更新中断标志位，如果不清除，会一直进中断
	}
}