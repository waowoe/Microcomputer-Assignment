#include "gizwits_product.h"
#include "gizwits_protocol.h"
#include "delay.h"
#include "sys.h"
#include "bmp.h"
#include "usart.h"
#include "dht11.h"
#include "control.h"
#include "adc.h"
#include "key.h"
#include "kqm2800.h"
#include "timer.h"
#include "oled.h"

/****  菜单变量  ****/
uint8_t keu_num = 0;
uint8_t page_num = 1;
uint8_t option = 0;

/****  功能函数声明  ****/
void get_all_sensor_data();		//传感器数据采集
void Sensor_Data_Handle();		//不同模式下控制效果
void Key_event_handle(void);	//按键功能函数

void OLED_Show_FirstPage();		//第一屏显示内容
void OLED_Show_SecondPage();	//第二屏显示内容
void OLED_Show_ThirdPage();		//第三屏显示内容s
void OLED_Show_FourthPage();	//第四屏显示内容
void OLED_Show_FivePage();

int main(void)
{
	delay_init();			//系统延时函数初始化
	OLED_Init();			//屏幕初始化
	uart3_init(9600);		//串口3初始化，波特率9600
	uart1_init(115200);		//串口1初始化，波特率115200
	uart2_init(9600);		//串口2初始化，波特率9600
	DHT11_Init();			//DHT11温湿度初始化
	Control_Init();			//声光报警，风扇、水泵、照明初始化
	KEY_Init();				//按键初始化
	Adc_Init();				//土壤湿度、光照强度初始化
	TIME_Config();			//定时器初始化
	userInit();				//机智云数据初始化
	gizwitsInit();
	LED = 1;	//上电默认关灯
	while(1)
	{
		if(Time_Period[1]>1000)		//Time_Period[1] 在定时器中累加
		{
			get_all_sensor_data();	//传感器数据采集
			userHandle();			//数据上传云平台
			Time_Period[1] = 0;
		}
		Key_event_handle();			//按键监测
		Sensor_Data_Handle();		//数据处理
		gizwitsHandle((dataPoint_t *)&currentDataPoint);	//机智云下行消息解析处理
		if(page_num == 1){			//屏幕更新
			OLED_Show_FirstPage();
		}else if(page_num == 2){
			OLED_Show_SecondPage();
		}else if(page_num == 3){
			OLED_Show_ThirdPage();
		}else if(page_num == 4){
			OLED_Show_FourthPage();
		}else if(page_num == 5){
			OLED_Show_FivePage();
		}
	}
}

void Key_event_handle(void)
{
	keu_num = KEY_Scan(0);		//按键扫描赋值
	switch(keu_num)
	{
		case 1:		//按键1按下
			if(page_num==1){
				page_num=2;
				OLED_Clear();OLED_Show_SecondPage();
			}else if(page_num==2){
				option++;
				if(option>5){
					option=0;
					page_num=3;
					OLED_Clear();OLED_Show_ThirdPage();
				}else OLED_Show_SecondPage();
			}else if(page_num==3){
				option++;
				if(option>1){
					option=0;
					page_num=4;
					OLED_Clear();OLED_Show_FourthPage();
				}else OLED_Show_ThirdPage();
			}else if(page_num==4){
				option++;
				if(option>3){
					option=0;
					page_num=1;
					OLED_Clear();OLED_Show_FirstPage();
				}else OLED_Show_FourthPage();
			}else if(page_num==5){
				option++;
				if(option>2){
					option=0;
					page_num=1;
					OLED_Clear();OLED_Show_FirstPage();
				}else OLED_Show_FivePage();			
			}
			break;
		case 2:		//按键2按下
			if(page_num==1){
				currentDataPoint.valueauto_mode = 1;
			}else if(page_num==2){
				if(option==1)currentDataPoint.valuetemp_door++;
				else if(option==2)currentDataPoint.valuehumi_door++;
				else if(option==3)currentDataPoint.valuelight_door+=25;
				else if(option==4)currentDataPoint.valuesoil_door+=1;
			}else if(page_num==3){
				if(option==1)currentDataPoint.valueco2_door+=15;
			}else if(page_num==4){
				if(currentDataPoint.valueauto_mode==0){
					if(option==1){
						LED=0;
						currentDataPoint.valueled = 1;
					}else if(option==2){
						Wind = 1;
						currentDataPoint.valuewind = 1;
					}else if(option==3){
						Water_pump = 1;
						currentDataPoint.valuewater = 1;
					}
				}
			}
			break;
		case 3:		//按键3按下
			if(page_num==1){
				currentDataPoint.valueauto_mode = 0;
			}else if(page_num==2){
				if(option==1)currentDataPoint.valuetemp_door--;
				else if(option==2)currentDataPoint.valuehumi_door--;
				else if(option==3)currentDataPoint.valuelight_door-=25;
				else if(option==4)currentDataPoint.valuesoil_door-=1;
			}else if(page_num==3){
				if(option==1)currentDataPoint.valueco2_door-=15;
			}else if(page_num==4){
				if(currentDataPoint.valueauto_mode==0){
					if(option==1){
						LED=1;
						currentDataPoint.valueled = 0;
					}else if(option==2){
						Wind = 0;
						currentDataPoint.valuewind = 0;
					}else if(option==3){
						Water_pump = 0;
						currentDataPoint.valuewater = 0;
					}
				}
			}
			break;
		case 4:		//按键4按下
			if(page_num!=5){
				page_num=5;option=0;
				OLED_Clear();OLED_Show_FivePage();
			}else if(page_num==5){
				if(option==1)gizwitsSetMode(WIFI_AIRLINK_MODE);		//机智云按键配网-一键配网
				else if(option==2)gizwitsSetMode(WIFI_SOFTAP_MODE);		//机智云按键配网-热点配网
			}
			break;
	}
}

void get_all_sensor_data()
{
	DHT11_Read_Data(&temp,&hum);    //获取温湿度数据
	Get_Light_Value(&Light_Value);	//获取光照数据
	Get_Soil_Value(&Soil_Value);	//获取土壤湿度数据
	Get_CO2_Data();					//获取CO2浓度数据
}




void Sensor_Data_Handle()
{
	//自动模式
	if(currentDataPoint.valueauto_mode == 1)  //如果开启了自动模式则可以实现根据数据的自动控制
	{
		//光照强度低于阈值打开补光灯
		if(Light_Value < currentDataPoint.valuelight_door){
			LED = 0;
			currentDataPoint.valueled = 1;
		} else {
			LED = 1;
			currentDataPoint.valueled = 0;
		}
		//温度、湿度、CO2浓度大于阈值，打开风扇通风并蜂鸣器报警
		if(temp > currentDataPoint.valuetemp_door || hum > currentDataPoint.valuehumi_door || Kqzl.CO2 > currentDataPoint.valueco2_door){
			Wind = 1;
			currentDataPoint.valuewind = 1;
			Beep = 1;
		} else {
			Wind = 0;
			currentDataPoint.valuewind = 0;
			Beep = 0;
		}
		//土壤湿度低于阈值打开水泵灌溉
		if(Soil_Value < currentDataPoint.valuesoil_door){
			Water_pump = 1;
			currentDataPoint.valuewater = 1;
		} else {
			Water_pump = 0;
			currentDataPoint.valuewater = 0;
		}
	}else Beep = 0;	//手动模式下，关闭蜂鸣器
}

/*	
 已(0) 正(1) 在(2) 连(3) 接(4) 温(5) 湿(6) 浓(7) 度(8) 光(9) 照(10) 强(11) 人(12) 体(13) 火(14) 焰(15)
 报(16) 警(17) 土(18) 壤(19) 降(20) 雨(21) 量(22) 风(23) 速(24) 有(25) 害(26) 气(27) 体(28) 阈(29) 值(30) 烟(31)
 雾(32) 有(33) 无(34) 自(35) 手(36) 动(37) 照(38) 明(39) 风(40) 扇(41) 水(42) 泵(43) 开(44) 关(45)
 一(46) 键(47) 热(48) 点(49) 配(50) 网(51) 
*/
void OLED_Show_FirstPage(){
	
	OLED_ShowChinese(16*0,0,5,16,1);		//	“温”
	OLED_ShowChinese(16*1,0,8,16,1);		//	“度”
	OLED_ShowChar(16*2,0,':',16,1);			//	“：”
	OLED_ShowNum(16*2+8,0,temp,2,16,1);		//	temp
	
	OLED_ShowChinese(16*4,0,6,16,1);		//	“湿”
	OLED_ShowChinese(16*5,0,8,16,1);		//	“度”
	OLED_ShowChar(16*6,0,':',16,1);		//	“：”
	OLED_ShowNum(16*6+8,0,hum,2,16,1);		//	hum	
	
	OLED_ShowString(8*0,16,"CO2",16,1);		//	“CO2”
	OLED_ShowChinese(8*3,16,7,16,1);		//	“浓”
	OLED_ShowChinese(8*3+16,16,8,16,1);		//	“度”
	OLED_ShowChar(8*3+16*2,16,':',16,1);	//	“：”
	OLED_ShowNum(8*4+16*2,16,Kqzl.CO2,4,16,1);	//	CO2_Value	
	
	OLED_ShowChinese(16*0,32,9,16,1);		//	“光”
	OLED_ShowChinese(16*1,32,10,16,1);		//	“照”
	OLED_ShowChinese(16*2,32,11,16,1);		//	“强”
	OLED_ShowChinese(16*3,32,8,16,1);		//	“度”
	OLED_ShowChar(16*4,32,':',16,1);		//	“：”
	OLED_ShowNum(16*4+8,32,Light_Value,4,16,1);	//	Light_Value	
	
	OLED_ShowChinese(16*0,48,18,16,1);		//	“土”
	OLED_ShowChinese(16*1,48,19,16,1);		//	“壤”
	OLED_ShowChinese(16*2,48,6,16,1);		//	“湿”
	OLED_ShowChinese(16*3,48,8,16,1);		//	“度”
	OLED_ShowChar(16*4,48,':',16,1);			//	“：”
	OLED_ShowNum(16*4+8,48,Soil_Value,3,16,1);				//	Soil_Value	
	
	if(currentDataPoint.valueauto_mode == 1){
		OLED_ShowChinese(16*7,32,35,16,1);			//	“自”
		OLED_ShowChinese(16*7,48,37,16,1);			//	“动”
	}else if(currentDataPoint.valueauto_mode == 0){
		OLED_ShowChinese(16*7,32,36,16,1);			//	“手”
		OLED_ShowChinese(16*7,48,37,16,1);			//	“动”
	}

	OLED_Refresh();
	
}
void OLED_Show_SecondPage(){
	
	uint8_t a,b,c,d;
	if(option==1){a=0;b=1;c=1;d=1;}
	else if(option==2){a=1;b=0;c=1;d=1;}
	else if(option==3){a=1;b=1;c=0;d=1;}
	else if(option==4){a=1;b=1;c=1;d=0;}
	else{a=1;b=1;c=1;d=1;}	
	
	OLED_ShowChinese(16*0,0,5,16,1);		//	“温”
	OLED_ShowChinese(16*1,0,8,16,1);		//	“度”
	OLED_ShowChinese(16*2,0,29,16,1);		//	“阈”
	OLED_ShowChinese(16*3,0,30,16,1);		//	“值”
	OLED_ShowChar(16*4,0,':',16,1);			//	“：”
	OLED_ShowNum(16*4+8,0,currentDataPoint.valuetemp_door,3,16,a);		//	temp_door
	
	OLED_ShowChinese(16*0,16,6,16,1);		//	“湿”
	OLED_ShowChinese(16*1,16,8,16,1);		//	“度”
	OLED_ShowChinese(16*2,16,29,16,1);		//	“阈”
	OLED_ShowChinese(16*3,16,30,16,1);		//	“值”
	OLED_ShowChar(16*4,16,':',16,1);			//	“：”
	OLED_ShowNum(16*4+8,16,currentDataPoint.valuehumi_door,3,16,b);		//	hum_door_value
	
	OLED_ShowChinese(16*0,32,9,16,1);		//	“光”
	OLED_ShowChinese(16*1,32,10,16,1);		//	“照”
	OLED_ShowChinese(16*2,32,29,16,1);		//	“阈”
	OLED_ShowChinese(16*3,32,30,16,1);		//	“值”
	OLED_ShowChar(16*4,32,':',16,1);			//	“：”
	OLED_ShowNum(16*4+8,32,currentDataPoint.valuelight_door,4,16,c);	//	light_door_value
	
	OLED_ShowChinese(16*0,48,18,16,1);		//	“土”
	OLED_ShowChinese(16*1,48,19,16,1);		//	“壤”
	OLED_ShowChinese(16*2,48,29,16,1);		//	“阈”
	OLED_ShowChinese(16*3,48,30,16,1);		//	“值”
	OLED_ShowChar(16*4,48,':',16,1);			//	“：”
	OLED_ShowNum(16*4+8,48,currentDataPoint.valuesoil_door,3,16,d);		//	hum_door_value
	
	OLED_Refresh();
}

void OLED_Show_ThirdPage(){

	uint8_t a,b,c,d;
	if(option==1){a=0;b=1;c=1;d=1;}
	else if(option==2){a=1;b=0;c=1;d=1;}
	else if(option==3){a=1;b=1;c=0;d=1;}
	else if(option==4){a=1;b=1;c=1;d=0;}
	else{a=1;b=1;c=1;d=1;}

	OLED_ShowString(16*0,0,"CO2",16,1);		//CO2
	OLED_ShowChinese(16*1+8,0,7,16,1);		//浓
	OLED_ShowChinese(16*2+8,0,8,16,1);		//度
	OLED_ShowChinese(16*3+8,0,29,16,1);		//阈
	OLED_ShowChinese(16*4+8,0,30,16,1);		//值		
	OLED_ShowChar(16*5+8,0,':',16,1);	
	OLED_ShowNum(16*6,0,currentDataPoint.valueco2_door,4,16,a);
	
	OLED_Refresh();
}

void OLED_Show_FourthPage(){

	uint8_t a,b,c,d;
	if(option==1){a=0;b=1;c=1;d=1;}
	else if(option==2){a=1;b=0;c=1;d=1;}
	else if(option==3){a=1;b=1;c=0;d=1;}
	else if(option==4){a=1;b=1;c=1;d=0;}
	else{a=1;b=1;c=1;d=1;}

	OLED_ShowChinese(16*0,0,38,16,1);	//照
	OLED_ShowChinese(16*1,0,39,16,1);	//明
	OLED_ShowChar(16*2,0,':',16,1);		
	if(currentDataPoint.valueled==1)OLED_ShowChinese(16*2+8,0,44,16,a);	//开
	else OLED_ShowChinese(16*2+8,0,45,16,a);	//关

	OLED_ShowChinese(16*0,16,40,16,1);	//风
	OLED_ShowChinese(16*1,16,41,16,1);	//扇
	OLED_ShowChar(16*2,16,':',16,1);		
	if(currentDataPoint.valuewind==1)OLED_ShowChinese(16*2+8,16,44,16,b);	//开
	else OLED_ShowChinese(16*2+8,16,45,16,b);	//关

	OLED_ShowChinese(16*0,32,42,16,1);	//水
	OLED_ShowChinese(16*1,32,43,16,1);	//泵
	OLED_ShowChar(16*2,32,':',16,1);		
	if(currentDataPoint.valuewater==1)OLED_ShowChinese(16*2+8,32,44,16,c);	//开
	else OLED_ShowChinese(16*2+8,32,45,16,c);	//关	
	
	OLED_Refresh();
}

void OLED_Show_FivePage()
{
	uint8_t a,b,c,d;
	if(option==1){a=0;b=1;c=1;d=1;}
	else if(option==2){a=1;b=0;c=1;d=1;}
	else if(option==3){a=1;b=1;c=0;d=1;}
	else if(option==4){a=1;b=1;c=1;d=0;}
	else{a=1;b=1;c=1;d=1;}
	
	OLED_ShowChinese(16*0,0,46,16,a);	//一
	OLED_ShowChinese(16*1,0,47,16,a);	//键
	OLED_ShowChinese(16*2,0,50,16,a);	//配
	OLED_ShowChinese(16*3,0,51,16,a);	//网	
	
	OLED_ShowChinese(16*0,16,48,16,b);	//热
	OLED_ShowChinese(16*1,16,49,16,b);	//点
	OLED_ShowChinese(16*2,16,50,16,b);	//配
	OLED_ShowChinese(16*3,16,51,16,b);	//网	
	
	OLED_Refresh();
}

