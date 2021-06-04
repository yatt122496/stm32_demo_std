#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "rtc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usmart.h"


#define START_TASK_PRIO      1
#define START_STK_SIZE       128

