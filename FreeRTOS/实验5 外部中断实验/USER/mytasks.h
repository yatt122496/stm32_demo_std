#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "beep.h"
#include "key.h"
#include "exti.h"

extern void start_task(void *pvParameters);
extern TaskHandle_t StartTask_Handler;
