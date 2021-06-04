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
	u16 arr=180;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
 
 	TIM3_PWM_Init(arr-1,cPWM_PSC-1); 		//����Ƶ��PWMƵ��=72000/(899+1)=80Khz
 	TIM5_Cap_Init(0XFFFF,cTIM_PSC-1);	//��1Mhz��Ƶ�ʼ���
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
 		if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ��������
		{
			temp+=TIM5CH1_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%.3f us,TOTle:%.3f us,ռ�ձ�%.2f%%\r\n",TIM5CH1_CAPTURE_HIGHVAL*cTIM_PSC/72.0,TIM5CH1_CAPTURE_VAL*cTIM_PSC/72.0,TIM5CH1_CAPTURE_HIGHVAL*100.0/TIM5CH1_CAPTURE_VAL);//��ӡ�ܵĸߵ�ƽʱ��
			delay_ms(50);
			TIM5CH1_CAPTURE_VAL=0;
			TIM5CH1_CNTTIME=0;
			TIM5CH1_CAPTURE_STA=0;//������һ�β���
		}
	}
}

