#include "mytasks.h"

extern TaskHandle_t StartTask_Handler;
extern void start_task(void *pvParameters);

#define LED0_TASK_PRIO      2
#define LED0_STK_SIZE       50
TaskHandle_t LED0Task_Handler;
void led0_task(void *pvParameters);

#define LED1_TASK_PRIO      3
#define LED1_STK_SIZE       50
TaskHandle_t LED1Task_Handler;
void led1_task(void *pvParameters);

#define RTC_TASK_PRIO       4
#define RTC_STK_SIZE       100
TaskHandle_t RTCTask_Handler;
void rtc_task(void *pvParameters);


//LED状态设置函数
void led_set(u8 sta)
{
	LED1=sta;
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
	ledset(sta);
}


void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
	//创建LED0任务
	xTaskCreate((TaskFunction_t   )led0_task,
							(const char*      )"led0_task",
							(uint16_t         )LED0_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED0_TASK_PRIO,
							(TaskHandle_t*    )&LED0Task_Handler);
	//创建LED1任务
	xTaskCreate((TaskFunction_t   )led1_task,
							(const char*      )"led1_task",
							(uint16_t         )LED1_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED1_TASK_PRIO,
							(TaskHandle_t*    )&LED1Task_Handler);
	//创建RTC任务
	xTaskCreate((TaskFunction_t   )rtc_task,
							(const char*      )"rtc_task",
							(uint16_t         )RTC_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )RTC_TASK_PRIO,
							(TaskHandle_t*    )&RTCTask_Handler);
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}

void led0_task(void *pvParameters)
{
	for(;;)
	{
		LED0=~LED0;
		vTaskDelay(500);
	}
}

void led1_task(void *pvParameters)
{
	for(;;)
	{
		LED1=~LED1;
		vTaskDelay(2000);
	}
}

void rtc_task(void *pvParameters)
{
	u8 t=0;
	RTC_Init();
	for(;;)
	{
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			printf("now time is : %2.2d:%2.2d.%2.2d\r\n",calendar.hour,calendar.min,calendar.sec);
		}
		vTaskDelay(1000);
	}
}
