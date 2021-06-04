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

 
/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//初始化任务
//设置任务优先级
#define ESP8266_TASK_PRIO       			9 
//设置任务堆栈大小
#define ESP8266_STK_SIZE  		    		128
//任务堆栈	
OS_STK ESP8266_TASK_STK[ESP8266_STK_SIZE];
//任务函数
void esp8266_task(void *pdata);

//接收数据
//设置任务优先级
#define RECIVEDATA_TASK_PRIO       			1 
//设置任务堆栈大小
#define RECIVEDATA_STK_SIZE  		    		64
//任务堆栈	
OS_STK RECIVEDATA_TASK_STK[RECIVEDATA_STK_SIZE];
//任务函数
void recivedata_task(void *pdata);

//发送数据
//设置任务优先级
#define SENDDATA_TASK_PRIO       			2 
//设置任务堆栈大小
#define SENDDATA_STK_SIZE  		    		64
//任务堆栈	
OS_STK SENDDATA_TASK_STK[SENDDATA_STK_SIZE];
//任务函数
void senddata_task(void *pdata);

//RGB闪烁
//设置任务优先级
#define RGB_TASK_PRIO       			8 
//设置任务堆栈大小
#define RGB_STK_SIZE  		    		64
//任务堆栈	
OS_STK RGB_TASK_STK[RGB_STK_SIZE];
//任务函数
void rgb_task(void *pdata);

//蜂鸣器闪烁
//设置任务优先级
#define BEEP_TASK_PRIO       			7 
//设置任务堆栈大小
#define BEEP_STK_SIZE  		    		64
//任务堆栈	
OS_STK BEEP_TASK_STK[BEEP_STK_SIZE];
//任务函数
void beep_task(void *pdata);


 int main(void)
 {
	 delay_init();	    	 //延时函数初始化	  
	 NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	 uart_init(115200);	 	//串口初始化为9600
 	 LED_Init();			     //LED端口初始化
	 KEY_Init();
	 BEEP_Init();
	 TIM3_PWM_Init(900-1,0);
	 ESP8266_Init(115200);					//esp8226初始化
	 delay_ms(500);
	 OSInit();  	 			//初始化UCOSII		 			  
 	 OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	 OSStart();
 }

//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 			  
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
	OSTaskCreate(esp8266_task,(void *)0,(OS_STK *)&ESP8266_TASK_STK[ESP8266_STK_SIZE-1],ESP8266_TASK_PRIO ); 				   
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}


//ESP8266初始化任务
void esp8266_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	ESP8266_STA_TCPClient_Test();
	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
	OSTaskCreate(recivedata_task,(void *)0,(OS_STK *)&RECIVEDATA_TASK_STK[RECIVEDATA_STK_SIZE-1],RECIVEDATA_TASK_PRIO ); 	
	OSTaskCreate(senddata_task,(void *)0,(OS_STK *)&SENDDATA_TASK_STK[SENDDATA_STK_SIZE-1],SENDDATA_TASK_PRIO );	
	LED0 = 0;
 	OSTaskSuspend(ESP8266_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//ESP8266接收数据任务
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
				OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
				OSTaskCreate(rgb_task,(void *)0,(OS_STK *)&RGB_TASK_STK[RGB_STK_SIZE-1],RGB_TASK_PRIO ); 	
				OSTaskSuspend(ESP8266_TASK_PRIO);	//挂起起始任务.
				OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
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

//ESP8266发送数据任务
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

//RGB闪烁任务
void rgb_task(void *pdata)
{
	u16 led0pwmval=0;
	u8 dir=1,i=0;
	for(;;)
	{
		if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //判断是否有删除请求
		{
		 TIM_SetCompare1(TIM3,0);
		 TIM_SetCompare2(TIM3,0);
		 TIM_SetCompare3(TIM3,0);
		 OSTaskDel(OS_PRIO_SELF);						   //删除任务本身TaskLed
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

//蜂鸣器任务
void beep_task(void *pdata)
{
	for(;;)
	{
		if(OSTaskDelReq(OS_PRIO_SELF)==OS_ERR_TASK_DEL_REQ) //判断是否有删除请求
		{
		 OSTaskDel(OS_PRIO_SELF);						   //删除任务本身TaskLed
		}
		BEEP = 1;
		delay_ms(10);
		BEEP = 0;
		delay_ms(2000);
	}
}
