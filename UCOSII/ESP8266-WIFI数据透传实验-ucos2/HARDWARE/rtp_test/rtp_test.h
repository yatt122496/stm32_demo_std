#ifndef __REP_TEST_H
#define __REP_TEST_H
#include "sys.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "flash.h"
#include "touch.h"
#include "beep.h"
#include "timer.h"
#include "selectcolor.h"
#include "resetdisplay.h"
#include "includes.h"

void rtp_test(void);
void Load_Drow_Dialog(void);
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);
u16 my_abs(u16 x1,u16 x2);
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);
void ctp_test(void);

extern u16 arr3,psc3,arr4,psc4;
extern OS_EVENT * msg_key;			//按键邮箱事件块指针
extern OS_EVENT * sem_beep;		//蜂鸣器信号量指针	 	
#endif
/*----------------------德飞莱 技术论坛：www.doflye.net--------------------------*/
