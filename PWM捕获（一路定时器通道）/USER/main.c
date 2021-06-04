#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"

/************************************************
 ALIENTEK精英STM32开发板实验10
 输入捕获实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
 



int main(void)
{		
 	u32 temp=0; 
	u16 arr=180;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
 
 	TIM3_PWM_Init(arr-1,cPWM_PSC-1); 		//不分频。PWM频率=72000/(899+1)=80Khz
 	TIM5_Cap_Init(0XFFFF,cTIM_PSC-1);	//以1Mhz的频率计数
	TIM3->CCR2=arr-2;
//	TIM_SetCompare2(TIM3,arr/2);
  while(1)
	{
//		TIM3->CCR2--;
// 		delay_ms(10);
//		if(TIM3->CCR2==0)
//		{
//			TIM3->CCR2=arr-1;
//		}
 		if(TIM5CH1_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			temp+=TIM5CH1_CAPTURE_VAL;//得到总的高电平时间
			printf("HIGH:%.3f us,TOTle:%.3f us,占空比%.2f%%\r\n",TIM5CH1_CAPTURE_HIGHVAL*cTIM_PSC/72.0,TIM5CH1_CAPTURE_VAL*cTIM_PSC/72.0,TIM5CH1_CAPTURE_HIGHVAL*100.0/TIM5CH1_CAPTURE_VAL);//打印总的高点平时间
			delay_ms(50);
			TIM5CH1_CAPTURE_VAL=0;
			TIM5CH1_CNTTIME=0;
			TIM5CH1_CAPTURE_STA=0;//开启下一次捕获
		}
	}
}

