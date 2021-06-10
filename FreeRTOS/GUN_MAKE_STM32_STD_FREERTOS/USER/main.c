#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"

/************************************************
 ALIENTEK��ӢSTM32������ʵ��4
 ���� ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define TASK1_TASK_PRIO		2
//�����ջ��С
#define TASK1_STK_SIZE 		128
//������
TaskHandle_t Task1Task_Handler;
//������
void task1_task(void *pvParameters);

//�������ȼ�
#define TASK2_TASK_PRIO		3
//�����ջ��С
#define TASK2_STK_SIZE 		128
//������
TaskHandle_t Task2Task_Handler;
//������
void task2_task(void *pvParameters);

//�������ȼ�
#define TASK3_TASK_PRIO		4
//�����ջ��С
#define TASK3_STK_SIZE 		256
//������
TaskHandle_t Task3Task_Handler;
//������
void task3_task(void *pvParameters);

int main(void)
{
	u16 t;
	u16 len;
	u16 times = 0;

	delay_init();									//��ʱ������ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);								//���ڳ�ʼ��Ϊ115200
	LED_Init();										//LED�˿ڳ�ʼ��
	KEY_Init();										//��ʼ���밴�����ӵ�Ӳ���ӿ�
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    vTaskStartScheduler();          //�����������
	while (1) {
		if (USART_RX_STA & 0x8000) {
			len = USART_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for (t = 0; t < len; t++) {
				USART_SendData(USART1, USART_RX_BUF[t]); //�򴮿�1��������
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
					; //�ȴ����ͽ���
			}
			printf("\r\n\r\n"); //���뻻��
			USART_RX_STA = 0;
		} else {
			times++;
			if (times % 5000 == 0) {
				printf("\r\n��ӢSTM32������ ����ʵ��\r\n");
				printf("����ԭ��@ALIENTEK\r\n\r\n");
			}
			if (times % 200 == 0)
				printf("����������,�Իس�������\n");
			if (times % 30 == 0)
				LED0 = !LED0; //��˸LED,��ʾϵͳ��������.
			delay_ms(10);
		}
	}
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    //����TASK3����
    xTaskCreate((TaskFunction_t )task3_task,
                (const char*    )"task3_task",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_TASK_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void task1_task(void *pvParameters)
{
	u16 t;
	u16 len;
	u16 times = 0;
	while(1)
	{
		if (USART_RX_STA & 0x8000) {
			len = USART_RX_STA & 0x3fff; //�õ��˴ν��յ������ݳ���
			printf("\r\n�����͵���ϢΪ:\r\n");
			for (t = 0; t < len; t++) {
				USART_SendData(USART1, USART_RX_BUF[t]); //�򴮿�1��������
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
					; //�ȴ����ͽ���
			}
			printf("\r\n\r\n"); //���뻻��
			USART_RX_STA = 0;
		} else {
			times++;
			if (times % 5000 == 0) {
				printf("\r\n��ӢSTM32������ ����ʵ��\r\n");
				printf("����ԭ��@ALIENTEK\r\n\r\n");
			}
			if (times % 200 == 0)
				printf("����������,�Իس�������\n");
			if (times % 30 == 0)
				LED0 = !LED0; //��˸LED,��ʾϵͳ��������.
			delay_ms(10);
		}
	}
}

//task2������
void task2_task(void *pvParameters)
{
	u8 task2_num=0;

	while(1)
	{
		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
        LED1=!LED1;
		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
		delay_ms(500);
	}
}

//task3������
void task3_task(void *pvParameters)
{
	static float float_num=0.01;

	while(1)
	{
		float_num+=0.01f;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
		taskENTER_CRITICAL();           //�����ٽ���
		printf("float_num��ֵΪ: %.4f\r\n",float_num);
		taskEXIT_CRITICAL();            //�˳��ٽ���
		delay_ms(500);
	}
}
