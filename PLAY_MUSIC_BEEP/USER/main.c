#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "music.h"

/************************************************
 ALIENTEK��ӢSTM32������ʵ��9
 PWM���ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 �������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	u16 led0pwmval = 0;
	u8 dir = 1;
	u8 res = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();									//��ʱ������ʼ��
	uart_init(115200);								//���ڳ�ʼ��Ϊ115200
	LED_Init();										//LED�˿ڳ�ʼ��
	TIM3_PWM_Init(900 - 1, 72 - 1);						//����Ƶ��PWMƵ��=72000000/900=80Khz

	play_music_beep(1);
	while (1) {
		res = play_music_beep(0);
		if (!res) {
			delay_ms(1000);
			play_music_beep(1);
		}
		// delay_ms(1);
//		if (dir)
//			led0pwmval++;
//		else
//			led0pwmval--;
//		if (led0pwmval > 100) {
//			dir = 0;
//			LED1 = !LED1;
//		}
//		if (led0pwmval == 0) {
//			dir = 1;
//			LED1 = !LED1;
//		}
//		TIM_SetCompare2(TIM3, led0pwmval);
		//		TIM_SetAutoreload(TIM3,0);
	}
}