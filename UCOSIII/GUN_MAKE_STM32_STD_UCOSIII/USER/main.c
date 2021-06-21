#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
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
#define START_TASK_PRIO		3
//任务堆栈大小
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define LED0_TASK_PRIO		4
//任务堆栈大小
#define LED0_STK_SIZE 		128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//任务优先级
#define LED1_TASK_PRIO		5
//任务堆栈大小
#define LED1_STK_SIZE 		256
//任务控制块
OS_TCB Led1TaskTCB;
//任务堆栈
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *p_arg);

//任务优先级
#define FLOAT_TASK_PRIO		6
//任务堆栈大小
#define FLOAT_STK_SIZE		256
//任务控制块
OS_TCB	FloatTaskTCB;
//任务堆栈
CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE] __attribute__((aligned(8)));
//任务函数
void float_task(void *p_arg);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();

	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //中断分组配置
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	KEY_Init();          //初始化与按键连接的硬件接口

	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif

	OS_CRITICAL_ENTER();	//进入临界区
	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,
				 (CPU_CHAR	* )"led0 task",
                 (OS_TASK_PTR )led0_task,
                 (void		* )0,
                 (OS_PRIO	  )LED0_TASK_PRIO,
                 (CPU_STK   * )&LED0_TASK_STK[0],
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,
                 (CPU_STK_SIZE)LED0_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	//创建LED1任务
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,
				 (CPU_CHAR	* )"led1 task",
                 (OS_TASK_PTR )led1_task,
                 (void		* )0,
                 (OS_PRIO	  )LED1_TASK_PRIO,
                 (CPU_STK   * )&LED1_TASK_STK[0],
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,
                 (CPU_STK_SIZE)LED1_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);

	//创建浮点测试任务
	OSTaskCreate((OS_TCB 	* )&FloatTaskTCB,
				 (CPU_CHAR	* )"float test task",
                 (OS_TASK_PTR )float_task,
                 (void		* )0,
                 (OS_PRIO	  )FLOAT_TASK_PRIO,
                 (CPU_STK   * )&FLOAT_TASK_STK[0],
                 (CPU_STK_SIZE)FLOAT_STK_SIZE/10,
                 (CPU_STK_SIZE)FLOAT_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK	  )0,
                 (void   	* )0,
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0=0;
		ITM_SendChar(0xaa);
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED0=1;
		ITM_SendChar(0xbb);
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//led1任务函数
void led1_task(void *p_arg)
{
	u16 t;
	u16 len;
	u16 times=0;
	// OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		if(USART_RX_STA&0x8000) {
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
			}
			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
		} else {
			times++;
			if(times%5000==0) {
				printf("\r\n精英STM32开发板 串口实验\r\n");
				printf("正点原子@ALIENTEK\r\n\r\n");
			}
			if(times%200==0){
				printf("请输入数据,以回车键结束\n");
			}
			if(times%30==0)LED1=!LED1;//闪烁LED,提示系统正在运行.
			delay_ms(10);
		}
		// LED1=~LED1;
		// OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

//浮点测试任务
void float_task(void *p_arg)
{
	CPU_SR_ALLOC();
	static float float_num=0.01;
	while(1)
	{
		float_num+=0.01f;
		OS_CRITICAL_ENTER();	//进入临界区
		printf("float_num的值为: %.4f\r\n",float_num);
		OS_CRITICAL_EXIT();		//退出临界区
		delay_ms(500);			//延时500ms
	}
}

//硬件错误处理
void HardFault_Handler(void)
{
	u32 i;
	u8 t=0;
	u32 temp;
	temp=SCB->CFSR;					//fault状态寄存器(@0XE000ED28)包括:MMSR,BFSR,UFSR
 	printf("CFSR:%8lX\r\n",temp);	//显示错误值
	temp=SCB->HFSR;					//硬件fault状态寄存器
 	printf("HFSR:%8lX\r\n",temp);	//显示错误值
 	temp=SCB->DFSR;					//调试fault状态寄存器
 	printf("DFSR:%8lX\r\n",temp);	//显示错误值
   	temp=SCB->AFSR;					//辅助fault状态寄存器
 	printf("AFSR:%8lX\r\n",temp);	//显示错误值
 	LED1=!LED1;
 	while(t<5) {
		t++;
		LED0=!LED0;
		//BEEP=!BEEP;
		for(i=0;i<0X1FFFFF;i++);
 	}
}

