#include "mytasks.h"

//LED0闪烁
//设置任务优先级
#define LED0_TASK_PRIO       			9 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
__align(8) OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1闪烁
//设置任务优先级
#define LED1_TASK_PRIO       			8 
//设置任务堆栈大小
#define LED1_STK_SIZE  		    		64
//任务堆栈	
__align(8) OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//循环打印
//设置任务优先级
#define PRINT_TASK_PRIO       			6 
//设置任务堆栈大小
#define PRINT_STK_SIZE  		    		64
//任务堆栈	
__align(8) OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//任务函数
void print_task(void *pdata);


//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	
	OSTaskCreate(led0_task,(void *)0,
								(OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE-1],
								LED0_TASK_PRIO );//创建任务
	OSTaskCreate(led1_task,(void *)0,
								(OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE-1],
								LED1_TASK_PRIO );//创建起始任务
	OSTaskCreate(print_task,(void *)0,
								(OS_STK *)&PRINT_TASK_STK[PRINT_STK_SIZE-1],
								PRINT_TASK_PRIO );//创建循环打印
								
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//LED0任务
void led0_task(void *pdata)
{
	for(;;)
	{
		LED0=~LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//LED1任务
void led1_task(void *pdata)
{
	for(;;)
	{
		LED1=~LED1;
		OSTimeDlyHMSM(0,0,2,0);
	}
}

//循环打印任务
void print_task(void * pdata)
{
	static INT32U num = 0;
	for(;;)
	{
		printf("%d,",num++);
		OSTimeDlyHMSM(0,0,0,500);
	}
}

