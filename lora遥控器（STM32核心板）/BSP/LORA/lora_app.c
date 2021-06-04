#include "lora_app.h"
#include "lora_ui.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "stdio.h"
#include "usart3.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板 
//ATK-LORA-01模块功能驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/4/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//******************************************************************************** 
//无

//设备参数初始化(具体设备参数见lora_cfg.h定义)
_LoRa_CFG LoRa_CFG=
{
	LORA_ADDR,       //设备地址
	LORA_CHN,         //信道
	LORA_POWER,     //发射功率
	LORA_RATE,     //空中速率
	LORA_WLTIME,   //睡眠时间
	LORA_MODE,       //工作模式
	LORA_STA,    //发送状态
	LORA_TTLBPS,     //波特率设置
	LORA_TTLPAR,    //校验位设置
};

//全局参数
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
char* zhongliang;
char* dianliang;
u8 dianliang_baojing=0;

//设备工作模式(用于记录设备状态)
u8 Lora_mode=0;//0:配置模式 1:接收模式 2:发送模式
//记录中断状态
static u8 Int_mode=0;//0:关闭 1:上升沿 2:下降沿

//AUX中断设置
//mode:配置的模式 0:关闭 1:上升沿 2:下降沿
void Aux_Int(u8 mode)
{
    if(!mode)
	{
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;//关闭中断
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;	
	}else
	{
		if(mode==1)
			 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
	    else if(mode==2)
			 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
		
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	}
	Int_mode = mode;//记录中断模式
	EXTI_Init(&EXTI_InitStructure);
	NVIC_Init(&NVIC_InitStructure);  
    
}

//LORA_AUX中断服务函数
void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line6))
	{  
	   if(Int_mode==1)//上升沿(发送:开始发送数据 接收:数据开始输出)     
	   {
		  if(Lora_mode==1)//接收模式
		  {
			 USART3_RX_STA=0;//数据计数清0
		  }
		  Int_mode=2;//设置下降沿触发
//		  LED0=0;//DS0亮
	   }
     else if(Int_mode==2)//下降沿(发送:数据已发送完 接收:数据输出结束)	
	   {
		  if(Lora_mode==1)//接收模式
		  {
			 USART3_RX_STA|=1<<15;//数据计数标记完成
		  }else if(Lora_mode==2)//发送模式(串口数据发送完毕)
		  {
			 Lora_mode=1;//进入接收模式
		  }
		  Int_mode=1;//设置上升沿触发
//          LED0=1;//DS0灭		   
	   }
     Aux_Int(Int_mode);//重新设置中断边沿
	   EXTI_ClearITPendingBit(EXTI_Line6); //清除LINE4上的中断标志位  
	}	
}

//LoRa模块初始化
//返回值: 0,检测成功
//        1,检测失败
u8 LoRa_Init(void)
{
	 u8 retry=0;
	 u8 temp=1;
	
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PA端口时钟
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	    		 //LORA_MD0
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				 //LORA_AUX
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		     //下拉输入
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.4
	
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource6);
	
	 EXTI_InitStructure.EXTI_Line=EXTI_Line6;
  	 EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	 EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
  	 EXTI_InitStructure.EXTI_LineCmd = DISABLE;              //中断线关闭(先关闭后面再打开)
  	 EXTI_Init(&EXTI_InitStructure);//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	 NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//LORA_AUX
  	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级2， 
  	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//子优先级3
  	 NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; //关闭外部中断通道（后面再打开）
   	 NVIC_Init(&NVIC_InitStructure); 
	 
	 LORA_MD0=0;
	 LORA_AUX=0;
	
	 while(LORA_AUX)//确保LORA模块在空闲状态下(LORA_AUX=0)
	 {
		 delay_ms(500);
         delay_ms(100);		 
	 }
	 usart3_init(115200);//初始化串口3
	 
	 LORA_MD0=1;//进入AT模式
	 delay_ms(40);
	 retry=3;
	 while(retry--)
	 {
		 if(!lora_send_cmd("AT","OK",70))
		 {
			 temp=0;//检测成功
			 break;
		 }	
	 }
	 if(retry==0) temp=1;//检测失败
	 return temp;
}

//Lora模块参数配置
void LoRa_Set(void)
{
	u8 sendbuf[20];
	u8 lora_addrh,lora_addrl=0;
	
	usart3_set(LORA_TTLBPS_115200,LORA_TTLPAR_8N1);//进入配置模式前设置通信波特率和校验位(115200 8位数据 1位停止 无数据校验）
	usart3_rx(1);//开启串口3接收
	
	while(LORA_AUX);//等待模块空闲
	LORA_MD0=1; //进入配置模式
	delay_ms(40);
	Lora_mode=0;//标记"配置模式"
	
	lora_addrh =  (LoRa_CFG.addr>>8)&0xff;
	lora_addrl = LoRa_CFG.addr&0xff;
	sprintf((char*)sendbuf,"AT+ADDR=%02x,%02x",lora_addrh,lora_addrl);//设置设备地址
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+WLRATE=%d,%d",LoRa_CFG.chn,LoRa_CFG.wlrate);//设置信道和空中速率
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+TPOWER=%d",LoRa_CFG.power);//设置发射功率
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+CWMODE=%d",LoRa_CFG.mode);//设置工作模式
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+TMODE=%d",LoRa_CFG.mode_sta);//设置发送状态
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+WLTIME=%d",LoRa_CFG.wltime);//设置睡眠时间
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+UART=%d,%d",LoRa_CFG.bps,LoRa_CFG.parity);//设置串口波特率、数据校验位
	lora_send_cmd(sendbuf,"OK",50);
	sprintf((char*)sendbuf,"AT+FLASH=1");
	lora_send_cmd(sendbuf,"OK",50);

	LORA_MD0=0;//退出配置,进入通信
	delay_ms(40);
	while(LORA_AUX);//判断是否空闲(模块会重新配置参数)
	USART3_RX_STA=0;
	Lora_mode=1;//标记"接收模式"
	usart3_set(LoRa_CFG.bps,LoRa_CFG.parity);//返回通信,更新通信串口配置(波特率、数据校验位)
	Aux_Int(1);//设置LORA_AUX上升沿中断	
	
}

u8 Tran_Data[30]={0};//透传数组

#define Dire_DateLen sizeof(Dire_Date)/sizeof(Dire_Date[0])


//Lora模块发送数据
void LoRa_SendData(void)
{
	sprintf((char*)Tran_Data,"ATK");
	u3_printf("%s",Tran_Data);

}

u8 rlcd_buff[10]={0}; //LCD显示字符串缓冲区
//Lora模块接收数据
void LoRa_ReceData(void)
{
    u16 len=0;
   
	//有数据来了
	if(USART3_RX_STA&0x8000)
	{
		len = USART3_RX_STA&0X7FFF;
		USART3_RX_BUF[len]=0;//添加结束符
		USART3_RX_STA=0;

//		for(i=0;i<len;i++)
//		{
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//			USART_SendData(USART1,USART3_RX_BUF[i]); 
//		}
		if(strcmp(USART3_RX_BUF,"KEY5_1")==0)
		{
			LED0=~LED0;
			HMISendDate(ZhuZhou,"正");
		}
		else if(strcmp(USART3_RX_BUF,"KEY5_2")==0)
		{
			LED0=~LED0;
			HMISendDate(ZhuZhou,"反");
		}
		else if(strcmp(USART3_RX_BUF,"KEY6_1")==0)
		{
			LED0=~LED0;
			HMISendDate(LaJiXiang,"正");
		}
		else if(strcmp(USART3_RX_BUF,"KEY6_2")==0)
		{
			LED0=~LED0;
			HMISendDate(LaJiXiang,"反");
		}
		else if((strchr(USART3_RX_BUF,'='))!=0)
		{
			LED0=~LED0;
			zhongliang=strchr(USART3_RX_BUF,'=');
			HMISendDate(LaJiZhongLiang,zhongliang+1);
		}
		else if((strchr(USART3_RX_BUF,'+'))!=0)
		{
			LED0=~LED0;
			dianliang=strchr(USART3_RX_BUF,'+');
			HMISendDate(DianLiang,dianliang+1);
			dianliang_baojing=1;
		}

		memset((char*)USART3_RX_BUF,0x00,len);//串口接收缓冲区清0
//		LED0=1;
	}
}

//发送和接收处理
void LoRa_Process(void)
{
	u8 t=0;
		
	LoRa_Set();//LoRa配置(进入配置需设置串口波特率为115200) 
	while(1)
	{
		LoRa_SendData();
		delay_ms(10);
		//数据接收
		LoRa_ReceData();
		
		t++;
		if(t==20)
		{
			t=0;
		}			
		delay_ms(10);		
   }
	
}

//主测试函数
void Lora_Test(void)
{
	while(LoRa_Init());  //初始化ATK-LORA-01模块
    delay_ms(500); 
	LoRa_Process();
	while(1)
	{
		
	}		
	
}

void HMISends(char *buf1)
{
	u8 i=0;
	while(1)
	{
	 if(buf1[i]!=0)
	 	{
			USART_SendData(USART1,buf1[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};
		 	i++;
		}
	 else 
	 return ;

		}
	}
void HMISendb(u8 k)
{		 
	u8 i;
	 for(i=0;i<3;i++)
	 {
	 if(k!=0)
	 	{
			USART_SendData(USART1,k);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};
		}
	 else 
	 return ;

	 } 
}

void HMISendstart(void)
{
	delay_ms(200);
	HMISendb(0xff);
	delay_ms(200);
}

void HMISendDate(u8 id,char* data)
{
	char stringHMI[20]={0};
	HMISendb(0xff);
	sprintf(stringHMI,"t%d.txt=\"",id);
	HMISends(stringHMI);
	printf("%s ",data);
	HMISends("\"");
	HMISendb(0xff);
}
