#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart3.h"
#include "usart.h"
#include "lora_app.h"
#include "key.h"
#include "beep.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验4
 串口 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

extern char* dianliang;
extern u8 dianliang_baojing;
int main(void)
{
	int k=0;
	extern u8 Tran_Data[30];//透传数组
	u8 key=0,baojing=0,baojing_flag=0,baojing_num=3,KEY9_Flag=0,KEY10_Flag=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	LED_GPIO_Config();			     //LED端口初始化          //初始化与按键连接的硬件接口
	KEY_Init();
	BEEP_Init();
	uart_init(9600);
	HMISendstart();
	while(LoRa_Init());

	delay_ms(500);
	LoRa_Set();//LoRa配置(进入配置需设置串口波特率为115200) 
//	while(1)
//	{
//		LED0=~LED0;
//		delay_ms(50);
//	}
 	while(1)
	{
		//有数据来了
		LoRa_ReceData();
		delay_ms(80);
		key=KEY_Scan(0);
		switch	(key)
		{
			case 1:
				sprintf((char*)Tran_Data,"KEY1");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"前");
				break;
			case 2:
				sprintf((char*)Tran_Data,"KEY2");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"左");
				break;
			case 3:
				sprintf((char*)Tran_Data,"KEY3");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"右");
				break;
			case 4:
				sprintf((char*)Tran_Data,"KEY4");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"停");
				break;
			case 5:
				sprintf((char*)Tran_Data,"KEY5");
				u3_printf("%s",Tran_Data);
				break;
			case 6:
				sprintf((char*)Tran_Data,"KEY6");
				u3_printf("%s",Tran_Data);
				break;
			case 7:
				sprintf((char*)Tran_Data,"KEY7");
				u3_printf("%s",Tran_Data);
				HMISendDate(ZhuZhou,"停");
				HMISendDate(LaJiXiang,"停");
				break;
			case 8:
				sprintf((char*)Tran_Data,"KEY8");
				u3_printf("%s",Tran_Data);
				break;
			case 9:
				KEY9_Flag++;
				if(KEY9_Flag>=5)
					KEY9_Flag=0;
				if(KEY9_Flag)
				{
					if(KEY9_Flag==1)
						HMISendDate(FaMen,"1");
					else if(KEY9_Flag==2)
						HMISendDate(FaMen,"2");
					else if(KEY9_Flag==3)
						HMISendDate(FaMen,"3");
					else
						HMISendDate(FaMen,"4");
				}
				else
					HMISendDate(FaMen,"关");
				sprintf((char*)Tran_Data,"KEY9");
				u3_printf("%s",Tran_Data);
				break;
			case 10:
				KEY10_Flag++;
				if(KEY10_Flag>=2)
					KEY10_Flag=0;
				if(KEY10_Flag)
					HMISendDate(ShuiBeng,"开");
				else
					HMISendDate(ShuiBeng,"停");
				sprintf((char*)Tran_Data,"KEY10");
				u3_printf("%s",Tran_Data);
				break;
		}
//		if(k%20==0)
//		{
//			dianliang--;
//			sprintf((char*)Tran_Data,"%d",dianliang);
//			HMISendDate(DianLiang,(char*)Tran_Data);
//		}
		if(dianliang_baojing)
		{
			if(atoi(dianliang+1)<=95)
			{
				if(baojing_num)
				{
					baojing=1;
				}
			}
			if(baojing==1)
			{
				baojing_flag++;
				BEEP=~BEEP;
				if(baojing_flag>5)
				{
					baojing=0;
					baojing_num--;
					baojing_flag=0;
				}
			}
		}
		k++;
	}	 
}

