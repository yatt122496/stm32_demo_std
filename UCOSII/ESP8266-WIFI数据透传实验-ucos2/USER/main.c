#include "led.h"
#include "timer.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "beep.h"
#include "includes.h"
#include "sta_tcpclent_test.h"
#include "esp8266_public.h"
#include "esp8266_drive.h"

 
/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//��ʼ������
//�����������ȼ�
#define ESP8266_TASK_PRIO       			9 
//���������ջ��С
#define ESP8266_STK_SIZE  		    		128
//�����ջ	
OS_STK ESP8266_TASK_STK[ESP8266_STK_SIZE];
//������
void esp8266_task(void *pdata);

//��������
//�����������ȼ�
#define RECIVEDATA_TASK_PRIO       			1 
//���������ջ��С
#define RECIVEDATA_STK_SIZE  		    		64
//�����ջ	
OS_STK RECIVEDATA_TASK_STK[RECIVEDATA_STK_SIZE];
//������
void recivedata_task(void *pdata);

//��������
//�����������ȼ�
#define SENDDATA_TASK_PRIO       			2 
//���������ջ��С
#define SENDDATA_STK_SIZE  		    		64
//�����ջ	
OS_STK SENDDATA_TASK_STK[SENDDATA_STK_SIZE];
//������
void senddata_task(void *pdata);

//RGB��˸
//�����������ȼ�
#define RGB_TASK_PRIO       			8 
//���������ջ��С
#define RGB_STK_SIZE  		    		64
//�����ջ	
OS_STK RGB_TASK_STK[RGB_STK_SIZE];
//������
void rgb_task(void *pdata);

//��������˸
//�����������ȼ�
#define BEEP_TASK_PRIO       			7 
//���������ջ��С
#define BEEP_STK_SIZE  		    		64
//�����ջ	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//������
void beep_task(void *pdata);


 int main(void)
 {
	 delay_init();	    	 //��ʱ������ʼ��	  
	 NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	 uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
 	 LED_Init();			     //LED�˿ڳ�ʼ��
	 KEY_Init();
	 BEEP_Init();
	 TIM3_PWM_Init(900-1,0);
	 ESP8266_Init(115200);					//esp8226��ʼ��
	 delay_ms(500);
	 OSInit();  	 			//��ʼ��UCOSII		 			  
 	 OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	 OSStart();
 }

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
	OSTaskCreate(esp8266_task,(void *)0,(OS_STK *)&ESP8266_TASK_STK[ESP8266_STK_SIZE-1],ESP8266_TASK_PRIO ); 				   
 	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}


//ESP8266��ʼ������
void esp8266_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	ESP8266_STA_TCPClient_Test();
	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
	OSTaskCreate(recivedata_task,(void *)0,(OS_STK *)&RECIVEDATA_TASK_STK[RECIVEDATA_STK_SIZE-1],RECIVEDATA_TASK_PRIO ); 	
	OSTaskCreate(senddata_task,(void *)0,(OS_STK *)&SENDDATA_TASK_STK[SENDDATA_STK_SIZE-1],SENDDATA_TASK_PRIO );	
	LED0 = 0;
 	OSTaskSuspend(ESP8266_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//ESP8266������������
void recivedata_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	char str[20]={0};
	for(;;)
	{
		strcpy ( str,"\0" );
		ESP8266_Rec_Data(str);
		if(strcmp(str,"LED0_OFF")==0)
		{
			LED0=1;
		}
		else if(strcmp(str,"LED0_ON")==0)
		{
			LED0=0;
		}
		else if(strcmp(str,"LED1_OFF")==0)
		{
			LED1=1;
		}
		else if(strcmp(str,"LED1_ON")==0)
		{
			LED1=0;
		}
		else if(strcmp(str,"RGB_ON")==0)
		{
				OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
				OSTaskCreate(rgb_task,(void *)0,(OS_STK *)&RGB_TASK_STK[RGB_STK_SIZE-1],RGB_TASK_PRIO ); 	
				OSTaskSuspend(ESP8266_TASK_PRIO);	//������ʼ����.
				OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
		}
		else if(strcmp(str,"RGB_OFF")==0)
		{
			OSTaskDelReq(RGB_TASK_PRIO);
		}
		else if(strcmp(str,"BEEP_ON")==0)
		{
			OSTaskCreate(beep_task,(void *)0,(OS_STK *)&BEEP_TASK_STK[BEEP_STK_SIZE-1],BEEP_TASK_PRIO );
		}
		else if(strcmp(str,"BEEP_OFF")==0)
		{
			OSTaskDelReq(BEEP_TASK_PRIO);
		}
		
	}
}

//ESP8266������������
void senddata_task(void *pdata)
{
	char cCmd [120];
	u8 a=10;
	for(;;)
	{
		sprintf ( cCmd, "this send is %d ",OSCPUUsage);
		ESP8266_Send_AT_Cmd( cCmd, NULL, NULL, 50 );
		a++;
		delay_ms(10000);
	}
}

//RGB��˸����
void rgb_task(void *pdata)
{
	u16 led0pwmval=0;
	u8 dir=1,i=0;
	for(;;)
	{
		if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //�ж��Ƿ���ɾ������
		{
		 TIM_SetCompare1(TIM3,0);
		 TIM_SetCompare2(TIM3,0);
		 TIM_SetCompare3(TIM3,0);
		 OSTaskDel(OS_PRIO_SELF);						   //ɾ��������TaskLed
		}
		if(dir)led0pwmval+=2;
			else led0pwmval-=2;
		if(led0pwmval>100)
		{
			dir=0;
		}
		if(led0pwmval<=0)
		{
			dir=1;
			i++;
		}
		if(i==1)
		{
			TIM_SetCompare1(TIM3,led0pwmval);
			TIM_SetCompare2(TIM3,led0pwmval);
		}
		else if(i==2)
		{
			TIM_SetCompare1(TIM3,led0pwmval);
			TIM_SetCompare3(TIM3,led0pwmval);
		}
		else if(i==3)
		{
			TIM_SetCompare2(TIM3,led0pwmval);
			TIM_SetCompare3(TIM3,led0pwmval);
		}
		else if(i==4)
		{
			TIM_SetCompare1(TIM3,led0pwmval);
			TIM_SetCompare2(TIM3,led0pwmval);
			TIM_SetCompare3(TIM3,led0pwmval);
		}
		else
		{
			i=1;
		}
		delay_ms(10);
	}
}

//����������
void beep_task(void *pdata)
{
	for(;;)
	{
		if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //�ж��Ƿ���ɾ������
		{
		 OSTaskDel(OS_PRIO_SELF);						   //ɾ��������TaskLed
		}
		BEEP = 1;
		delay_ms(10);
		BEEP = 0;
		delay_ms(2000);
	}
}
