#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "myiic.h"
#include "usart.h"


//**********��IIC����������ָ����ַ��ָ�����ȵ�����IIC_read_lendata()***
unsigned char IicReadLenData (unsigned char Driver,unsigned char len,unsigned char *ptr)
{
    //----------------------
    if(len > 8)
    return  0;    
    //----------------------
	IIC_Start();				 
	IIC_Send_Byte (Driver | 0x01);			 	
	if(RxAck())				     
	{
	  IIC_Stop();
	  return  0;
	}
	
	for(;len>1;len--)	
	{
	  *ptr = IIC_Read_Byte (1);
	  ptr++;		
	}
	*ptr = IIC_Read_Byte (0);
	IIC_Stop();	
	return 1;		
}
//��IIC����������ָ����ַ��ʼдָ�����ȵ�����IIC_write_lendata()*******
/************************************************************
*                        
*                                 
***********************************************************/
unsigned char IicWriteLenData(unsigned char Driver,unsigned char len,unsigned char *ptr) 
{
    if(len>8)
    return  0;
    //----------
    IIC_Start();				 	    //����IIC START�ź�
	//----------------
	IIC_Send_Byte(Driver);				//	���������ӵ�ַ
	if(RxAck())					//	���Ӧ���ź�
	{
	  IIC_Stop();
	  return  0;
	}
	//----------------
	for(;len>0;len--)					//	��������LEN���ȵ��ֽ�����
	{
	  IIC_Send_Byte(*ptr);
	  ptr++;
	  if(RxAck())					//	���Ӧ���ź�
	  {
	    IIC_Stop();
	    return  0;
	  }
	}
	IIC_Stop();							//	����IIC STOP�ź�	
	return 1;		
}
/************************************************
 ALIENTEK��ӢSTM32������ʵ��22
 IIC ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
	
int main(void)
{	 
	u8 key, bTemp, j, bData[3];
	u16 i=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//������ʼ��		 	 	
	IIC_Init();
  
	while(1)
	{
		key = KEY_Scan(0);
		if (key == KEY1_PRES) {//KEY_UP����,д��24C02
			IIC_Start();
			IIC_Send_Byte(0xAA);
			bTemp = RxAck();
			if (bTemp) {
				printf("no ack\r\n");
				IIC_Stop();
				continue;
			}
			IIC_Send_Byte(0x0);
			bTemp = RxAck();
			if (bTemp) {
				printf("no ack\r\n");
				IIC_Stop();
				continue;
			}
			IIC_Send_Byte(i);
			bTemp = RxAck();
			if (bTemp) {
				printf("no ack\r\n");
				IIC_Stop();
				continue;
			}
			printf("i = %x\r\n", (u8)i);
			
			IIC_Stop();
		}
		if (key == KEY0_PRES) {//KEY1����,��ȡ�ַ�������ʾ
			bData[0] = 0;
			bData[1] = ~bData[0];
			bData[2] = 0x5a;
			IicWriteLenData(0xAA, 3, &bData[0]);
//			IIC_Start();
//			IIC_Send_Byte(0xAA);
//			bTemp = RxAck();
//			if (bTemp) {
//				printf("no ack\r\n");
//				IIC_Stop();
//				continue;
//			}
//			IIC_Send_Byte(0x0);
//			bTemp = RxAck();
//			if (bTemp) {
//				printf("no ack\r\n");
//				IIC_Stop();
//				continue;
//			}
//			IIC_Stop();
//			delay_us(15);
//			IIC_Start();
//			IIC_Send_Byte(0xAB);
//			bTemp = RxAck();
//			if (bTemp) {
//				printf("no ack\r\n");
//				IIC_Stop();
//				continue;
//			}
//			for (j = 0; j < 19; j++) {
//				bTemp = IIC_Read_Byte(1);
//				printf("bTemp = %x, ", bTemp);
//			}
//			bTemp = IIC_Read_Byte(0);
//			IIC_Stop();
//			printf("bTemp = %x\r\n", bTemp);
		}
		if (key == WKUP_PRES) {//KEY1����,��ȡ�ַ�������ʾ
			if (IicReadLenData(0xAb, 3, &bData[0])) {
				bTemp = bData[0];
				if ((bTemp == (u8)~bData[1]) && (bData[2] == 0xa5)) {
					printf("bTemp = %x\r\n", bTemp);
				}
			}
		}
		i++;
		delay_ms(10);
		if (i == 20) {
//			if (IicReadLenData(0xAb, 3, &bData[0])) {
//				bTemp = bData[0];
//				if ((bTemp == (u8)~bData[1]) && (bData[2] == 0xa5)) {
//					printf("bTemp = %x\r\n", bTemp);
//				}
//			}
			LED0 = !LED0;//��ʾϵͳ��������
			i = 0;
		}
	}
}
