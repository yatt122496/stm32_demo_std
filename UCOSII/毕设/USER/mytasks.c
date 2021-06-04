#include "mytasks.h"

OS_EVENT *MutexSemp;
INT8U err,Flag_Start = 0,Flag_End = 1;
INT8U SendCommand(const char *str);
INT8U SwitchMode(const char *str);
INT8U PerformTask(INT8U num);

//LED0闪烁
//设置任务优先级
#define LED0_TASK_PRIO       			8 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1闪烁
//设置任务优先级
#define LED1_TASK_PRIO       			7 
//设置任务堆栈大小
#define LED1_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//循环打印
//设置任务优先级
#define PRINT_TASK_PRIO       			6 
//设置任务堆栈大小
#define PRINT_STK_SIZE  		    		64
//任务堆栈	
OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//任务函数
void print_task(void *pdata);

//ADC打印
//设置任务优先级
#define ADC_TASK_PRIO       			9 
//设置任务堆栈大小
#define ADC_STK_SIZE  		    		128
//任务堆栈	
__align(8) OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void adc_task(void *pdata);

//接收数据打印
//设置任务优先级
#define RECEIVE_TASK_PRIO       			5 
//设置任务堆栈大小
#define RECEIVE_STK_SIZE  		    		64
//任务堆栈	
OS_STK RECEIVE_TASK_STK[RECEIVE_STK_SIZE];
//任务函数
void receive_task(void *pdata);


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
	OSTaskCreate(adc_task,(void *)0,
								(OS_STK *)&ADC_TASK_STK[ADC_STK_SIZE-1],
								ADC_TASK_PRIO );//创建循环打印
	OSTaskCreate(receive_task,(void *)0,
								(OS_STK *)&RECEIVE_TASK_STK[RECEIVE_STK_SIZE-1],
								RECEIVE_TASK_PRIO );//创建循环打印
	
	MutexSemp = OSMutexCreate(OSPrioHighRdy - 1,&err);
								
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
	INT8U num = 0;
	for(;;)
	{
		while(Flag_Start)
		{
			num++;
			PerformTask(num);
			if(Flag_End)
			{
				num = 0;
				break;
			}
			OSTimeDlyHMSM(0,0,0,500);
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}

//ADC打印任务
void adc_task(void * pdata)
{
	char tem[20];
	INT8U id =4;
	float num0;
	T_Adc_Init();
	for(;;)
	{
		id++;
		num0=T_Get_Adc_Average(ADC_CH_TEMP,10);
		num0=(float)num0*(3.3/4096);
		num0=(1.43-num0)/0.0043+25;		//计算出当前温度值
		if(id > 5)
		{
			id = 4;
			num0=24.0;
		}
		sprintf(tem,"%d0%.4f,",id,num0);
		OSMutexPend(MutexSemp,0,&err);
		SendCommand(tem);
		OSMutexPost(MutexSemp);
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//接收任务
void receive_task(void * pdata)
{
	INT16U len = 0,t;
	char* temp;
	for(;;)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			OSMutexPend(MutexSemp,0,&err);			
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
//			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
			temp = strstr((const char *)USART_RX_BUF,"command");
			if(SwitchMode(temp))
				printf("Switch Mode error!");
			OSMutexPost(MutexSemp);
		}
		else
		{
			OSTimeDlyHMSM(0,0,0,10);
		}
	}
}

INT8U SendCommand(const char *str)
{
	if(!str)
	{
		return (INT8U)1;
	}
	printf("Command:%s?",str);
	return (INT8U)0;
}

INT8U SwitchMode(const char *str)
{
	INT8U id =0;
	if(str != NULL)
	{
		str += 8;
		while(*str != '?')
		{
			if(*str >= '0' && *str <= '9')
				id = *str++ - 48;
		}
		switch(id)
		{
			case 0:
				Flag_Start = 1;
				Flag_End = 0;
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 9:
				Flag_Start = 0;
				Flag_End = 1;
				break;
			default:
				return 1;
				//break;
		}
	}
	else
		return 1;
	return 0;
}

INT8U PerformTask(INT8U num)
{
	char tem1[20];
	OSMutexPend(MutexSemp,0,&err);
	switch(num)
	{
		case 1:
			sprintf(tem1,"110.0000,");
			SendCommand(tem1);
			OSTimeDlyHMSM(0,0,1,0);
			sprintf(tem1,"111.5000,");
			SendCommand(tem1);
			break;
		case 2:
			sprintf(tem1,"210.0000,");
			SendCommand(tem1);
			OSTimeDlyHMSM(0,0,1,0);
			sprintf(tem1,"210.0100,");
			SendCommand(tem1);
			break;
		case 3:
			sprintf(tem1,"310.0000,");
			SendCommand(tem1);
			OSTimeDlyHMSM(0,0,1,0);
			sprintf(tem1,"310.5000,");
			SendCommand(tem1);
			break;
		default:
			Flag_Start = 0;
			Flag_End = 1;
			break;
	}
	OSMutexPost(MutexSemp);
	return 0;
}

INT8U TestInternalResistance()
{
	
	return 0;
}
