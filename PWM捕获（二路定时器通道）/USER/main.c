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
	u16 arr=1000;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
 
 	TIM3_PWM_Init(arr-1,1-1); 		//不分频。PWM频率=72000/(899+1)=80Khz
 	TIM5_Cap_Init(0XFFFF,72-1);	//以1Mhz的频率计数
	TIM3->CCR2=100;
//	TIM3->CCR2=arr/2;
  while(1)
	{
 		delay_ms(10);
		TIM_SetCompare2(TIM3,TIM_GetCapture2(TIM3)+1);

		if(TIM_GetCapture2(TIM3)==300)
		{
			TIM_SetCompare2(TIM3,0);
		}
 		if(TIM5CH1_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
			temp+=TIM5CH1_CAPTURE_VAL;//得到总的高电平时间
			printf("HIGH:%d us,TOTle:%d us,占空比%d%%\r\n",TIM5CH1_CAPTURE_HIGHVAL,TIM5CH1_CAPTURE_VAL,TIM5CH1_CAPTURE_HIGHVAL*100/TIM5CH1_CAPTURE_VAL);//打印总的高点平时间
			delay_ms(50);
			TIM5CH1_CAPTURE_STA=0;//开启下一次捕获
		}
	}
}

