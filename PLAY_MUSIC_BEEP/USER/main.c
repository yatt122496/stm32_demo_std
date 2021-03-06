#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "music.h"

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
	u8 res, bkey;
	u32 dwSys_time = 0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();									//延时函数初始化
	uart_init(115200);								//串口初始化为115200
	LED_Init();										//LED端口初始化
	KEY_Init();
	TIM4_PWM_Init(900 - 1, 72 - 1);						//不分频。PWM频率=72000000/900=80Khz
	// play_music_beep(MUSIC_PLAY);

	while (1) {
		bkey = KEY_Scan(0);
		if (bkey == KEY0_PRES) {
			play_music_beep(MUSIC_PLAY);
		} else if (bkey == WKUP_PRES) {
			play_music_beep(MUSIC_STOP);
		}
		res = play_music_beep(0);
		if (!res) {
			LED1 = 1;
		} else {
			LED1 = 0;
		}
		if (Sys_time - dwSys_time > 499) {
			dwSys_time = Sys_time;
			LED0 = !LED0;
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
