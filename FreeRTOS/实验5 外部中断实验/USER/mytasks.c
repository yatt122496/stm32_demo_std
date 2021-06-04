#include "mytasks.h"

#define LED0_TASK_PRIO      2
#define LED0_STK_SIZE       50
TaskHandle_t LED0Task_Handler;
void led0_task(void *pvParameters);

#define LED1_TASK_PRIO      3
#define LED1_STK_SIZE       50
TaskHandle_t LED1Task_Handler;
void led1_task(void *pvParameters);

#define KEY_TASK_PRIO      4
#define KEY_STK_SIZE       50
TaskHandle_t KEYTask_Handler;
void key_task(void *pvParameters);

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
	//����LED0����
	printf("kaishi");
	xTaskCreate((TaskFunction_t   )led0_task,
							(const char*      )"led0_task",
							(uint16_t         )LED0_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED0_TASK_PRIO,
							(TaskHandle_t*    )&LED0Task_Handler);
	//����LED1����
	xTaskCreate((TaskFunction_t   )led1_task,
							(const char*      )"led1_task",
							(uint16_t         )LED1_STK_SIZE,
							(void*            )NULL,
							(UBaseType_t      )LED1_TASK_PRIO,
							(TaskHandle_t*    )&LED1Task_Handler);
	//����KEY����
//	xTaskCreate((TaskFunction_t   )key_task,
//							(const char*      )"key_task",
//							(uint16_t         )KEY_STK_SIZE,
//							(void*            )NULL,
//							(UBaseType_t      )KEY_TASK_PRIO,
//							(TaskHandle_t*    )&KEYTask_Handler);
	vTaskDelete(StartTask_Handler);
	printf("jieshu");
	taskEXIT_CRITICAL();
}

void led0_task(void *pvParameters)
{
	for(;;)
	{
		LED0=~LED0;
		delay_ms(500);
	}
}

void led1_task(void *pvParameters)
{
	for(;;)
	{
		LED1=~LED1;
		delay_ms(1000);
	}
}

void key_task(void *pvParameters)
{
	vu8 key=0;
	KEY_Init();
	for(;;)
	{
		key=KEY_Scan(0);	//�õ���ֵ
	  if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//���Ʒ�����
					BEEP=!BEEP;
					printf("S4");
					break; 
				case KEY2_PRES:	//ͬʱ����LED0,LED1��ת 
					LED0=~LED0;
					LED1=~LED1;
					printf("S3");
					break;
				case KEY1_PRES:	//����LED1��ת
					LED1=~LED1;					
					printf("S2");
					break;
				case KEY0_PRES:	//ͬʱ����LED0,LED1��ת 
					LED0=~LED0;
					printf("S1");
					break;
			}
		}
		else
			delay_ms(10);
	}
}
