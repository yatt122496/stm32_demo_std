#include "mytasks.h"

OS_EVENT *MutexSemp;
INT8U err,Flag_Start = 0,Flag_End = 1;
INT8U SendCommand(const char *str);
INT8U SwitchMode(const char *str);
INT8U PerformTask(INT8U num);

//LED0��˸
//�����������ȼ�
#define LED0_TASK_PRIO       			8 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1��˸
//�����������ȼ�
#define LED1_TASK_PRIO       			7 
//���������ջ��С
#define LED1_STK_SIZE  		    		64
//�����ջ	
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//ѭ����ӡ
//�����������ȼ�
#define PRINT_TASK_PRIO       			6 
//���������ջ��С
#define PRINT_STK_SIZE  		    		64
//�����ջ	
OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//������
void print_task(void *pdata);

//ADC��ӡ
//�����������ȼ�
#define ADC_TASK_PRIO       			9 
//���������ջ��С
#define ADC_STK_SIZE  		    		128
//�����ջ	
__align(8) OS_STK ADC_TASK_STK[ADC_STK_SIZE];
//������
void adc_task(void *pdata);

//�������ݴ�ӡ
//�����������ȼ�
#define RECEIVE_TASK_PRIO       			5 
//���������ջ��С
#define RECEIVE_STK_SIZE  		    		64
//�����ջ	
OS_STK RECEIVE_TASK_STK[RECEIVE_STK_SIZE];
//������
void receive_task(void *pdata);


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
	OSTaskCreate(adc_task,(void *)0,
								(OS_STK *)&ADC_TASK_STK[ADC_STK_SIZE-1],
								ADC_TASK_PRIO );//����ѭ����ӡ
	OSTaskCreate(receive_task,(void *)0,
								(OS_STK *)&RECEIVE_TASK_STK[RECEIVE_STK_SIZE-1],
								RECEIVE_TASK_PRIO );//����ѭ����ӡ
	
	MutexSemp = OSMutexCreate(OSPrioHighRdy - 1,&err);
								
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

//ADC��ӡ����
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
		num0=(1.43-num0)/0.0043+25;		//�������ǰ�¶�ֵ
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

//��������
void receive_task(void * pdata)
{
	INT16U len = 0,t;
	char* temp;
	for(;;)
	{
		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			OSMutexPend(MutexSemp,0,&err);			
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
//			printf("\r\n\r\n");//���뻻��
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
