#include "led.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 精英STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 输出高 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //LED1-->PE.5 端口配置, 推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_SetBits(GPIOA,GPIO_Pin_12);
}

static u8 time=52;
void SetTime(u8 bTime)
{
	time=bTime;
}

void UART_SendData(u8 data)
{
	u8 i = 0;
//	rt_enter_critical();
	VTX=0;		//!<起始位
	delay_us(time);
	for(i = 0; i < 8; i++){
		if(data & 0x01)
			VTX=1;
		else
			VTX=0;
		delay_us(time);
		data >>= 1;
	}
	VTX=1;		//!<停止位
	delay_us(time);
//	 rt_exit_critical();
}

void TS_printStr(char *str)
{
    int i = 0;

    while (str[i] != '\0')
    {
			UART_SendData(str[i++]);
	//		delay_ms(1);
    }
}

u8 VTX_Tx_Buf[256];
void VTXprintf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)VTX_Tx_Buf,fmt,ap);
	va_end(ap);
	i=strlen((const char*)VTX_Tx_Buf);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		UART_SendData(VTX_Tx_Buf[j]);
	}
}
