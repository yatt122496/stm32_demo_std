#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��9
 PWM���ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
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

