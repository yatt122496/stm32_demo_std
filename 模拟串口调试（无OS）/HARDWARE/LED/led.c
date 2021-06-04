#include "led.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ��ӢSTM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_5);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 ����� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //LED1-->PE.5 �˿�����, �������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
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
	VTX=0;		//!<��ʼλ
	delay_us(time);
	for(i = 0; i < 8; i++){
		if(data & 0x01)
			VTX=1;
		else
			VTX=0;
		delay_us(time);
		data >>= 1;
	}
	VTX=1;		//!<ֹͣλ
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
	i=strlen((const char*)VTX_Tx_Buf);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
		UART_SendData(VTX_Tx_Buf[j]);
	}
}
