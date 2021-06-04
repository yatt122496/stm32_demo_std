#include "mytasks.h"

//LED0��˸
//�����������ȼ�
#define LED0_TASK_PRIO       			9 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
__align(8) OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1��˸
//�����������ȼ�
#define LED1_TASK_PRIO       			8 
//���������ջ��С
#define LED1_STK_SIZE  		    		64
//�����ջ	
__align(8) OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//ѭ����ӡ
//�����������ȼ�
#define PRINT_TASK_PRIO       			6 
//���������ջ��С
#define PRINT_STK_SIZE  		    		64
//�����ջ	
__align(8) OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//������
void print_task(void *pdata);


//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)
	
	OSTaskCreate(led0_task,(void *)0,
								(OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE-1],
								LED0_TASK_PRIO );//��������
	OSTaskCreate(led1_task,(void *)0,
								(OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE-1],
								LED1_TASK_PRIO );//������ʼ����
	OSTaskCreate(print_task,(void *)0,
								(OS_STK *)&PRINT_TASK_STK[PRINT_STK_SIZE-1],
								PRINT_TASK_PRIO );//����ѭ����ӡ
								
 	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED0����
void led0_task(void *pdata)
{
	for(;;)
	{
		LED0=~LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//LED1����
void led1_task(void *pdata)
{
	for(;;)
	{
		LED1=~LED1;
		OSTimeDlyHMSM(0,0,2,0);
	}
}

//ѭ����ӡ����
void print_task(void * pdata)
{
	static INT32U num = 0;
	for(;;)
	{
		printf("%d,",num++);
		OSTimeDlyHMSM(0,0,0,500);
	}
}

