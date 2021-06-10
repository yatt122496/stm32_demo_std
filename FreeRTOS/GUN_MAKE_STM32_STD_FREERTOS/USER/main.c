#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

/************************************************
 ALIENTEK精英STM32开发板实验4
 串口 实验
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小
#define TASK1_STK_SIZE 		128
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		3
//任务堆栈大小
#define TASK2_STK_SIZE 		128
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);

//任务优先级
#define TASK3_TASK_PRIO		4
//任务堆栈大小
#define TASK3_STK_SIZE 		256
//任务句柄
TaskHandle_t Task3Task_Handler;
//任务函数
void task3_task(void *pvParameters);

int main(void)
{
	u16 t;
	u16 len;
	u16 times = 0;

	delay_init();									//延时函数初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);								//串口初始化为115200
	LED_Init();										//LED端口初始化
	KEY_Init();										//初始化与按键连接的硬件接口
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    vTaskStartScheduler();          //开启任务调度
	while (1) {
		if (USART_RX_STA & 0x8000) {
			len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for (t = 0; t < len; t++) {
				USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
					; //等待发送结束
			}
			printf("\r\n\r\n"); //插入换行
			USART_RX_STA = 0;
		} else {
			times++;
			if (times % 5000 == 0) {
				printf("\r\n精英STM32开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n");
			}
			if (times % 200 == 0)
				printf("请输入数据,以回车键结束\n");
			if (times % 30 == 0)
				LED0 = !LED0; //闪烁LED,提示系统正在运行.
			delay_ms(10);
		}
	}
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    //创建TASK3任务
    xTaskCreate((TaskFunction_t )task3_task,
                (const char*    )"task3_task",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_TASK_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	u16 t;
	u16 len;
	u16 times = 0;
	while(1)
	{
		if (USART_RX_STA & 0x8000) {
			len = USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for (t = 0; t < len; t++) {
				USART_SendData(USART1, USART_RX_BUF[t]); //向串口1发送数据
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
					; //等待发送结束
			}
			printf("\r\n\r\n"); //插入换行
			USART_RX_STA = 0;
		} else {
			times++;
			if (times % 5000 == 0) {
				printf("\r\n精英STM32开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n");
			}
			if (times % 200 == 0)
				printf("请输入数据,以回车键结束\n");
			if (times % 30 == 0)
				LED0 = !LED0; //闪烁LED,提示系统正在运行.
			delay_ms(10);
		}
	}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	u8 task2_num=0;

	while(1)
	{
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
        LED1=!LED1;
		printf("任务2已经执行：%d次\r\n",task2_num);
		delay_ms(500);
	}
}

//task3任务函数
void task3_task(void *pvParameters)
{
	static float float_num=0.01;

	while(1)
	{
		float_num+=0.01f;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
		taskENTER_CRITICAL();           //进入临界区
		printf("float_num的值为: %.4f\r\n",float_num);
		taskEXIT_CRITICAL();            //退出临界区
		delay_ms(500);
	}
}
