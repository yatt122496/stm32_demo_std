#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验9
 PWM输出实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	KEY_Init();
	TIM2_PWM_DMA_Init();
 	TIM3_PWM_DMA_Init();
	
	delay_ms(10);
	tim2_dma_pulse_Output(10,72,50,5);
	tim3_dma_pulse_Output(10,72,50,5);
	delay_ms(10);
	tim2_dma_pulse_Output(10,9,60,5);
	tim3_dma_pulse_Output(10,9,40,5);
	while(1) {
		ITM_SendChar('B');
		tim3_dma_pulse_Output(2000,7200,10,5);
		delay_ms(1000);
		delay_ms(1000);
	}
}

