#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "timer.h"
#include "w25qxx.h"
#include "adc.h"
#include "key.h"
#include "wdg.h"
#include "malloc.h"

#include "d1155.h"
#include "voice.h"
#include "L610.h"
#include "handpiece.h"

extern void start_task(void *pdata);
#define START_TASK_PRIO      		10


typedef enum{
	START_UP                =1,
	STANDBY                 =2,
	LOTTERY_OUT             =3,
	LOTTERY_DONE            =4,
	UPDATE                  =5,
}MAIN_STATE;
