#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"

/************************************************
 ALIENTEK��ӢSTM32������ʵ��10
 ���벶��ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
 



int main(void)
{		
 	u32 temp=0; 
	u16 arr=1000;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
 
 	TIM3_PWM_Init(arr-1,1-1); 		//����Ƶ��PWMƵ��=72000/(899+1)=80Khz
 	TIM5_Cap_Init(0XFFFF,72-1);	//��1Mhz��Ƶ�ʼ���
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
 		if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ��������
		{
			temp+=TIM5CH1_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%d us,TOTle:%d us,ռ�ձ�%d%%\r\n",TIM5CH1_CAPTURE_HIGHVAL,TIM5CH1_CAPTURE_VAL,TIM5CH1_CAPTURE_HIGHVAL*100/TIM5CH1_CAPTURE_VAL);//��ӡ�ܵĸߵ�ƽʱ��
			delay_ms(50);
			TIM5CH1_CAPTURE_STA=0;//������һ�β���
		}
	}
}

