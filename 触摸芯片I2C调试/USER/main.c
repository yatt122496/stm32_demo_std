#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "myiic.h"
#include "usart.h"


//**********从IIC总线上器件指定地址读指定长度的数据IIC_read_lendata()***
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
//往IIC总线上器件指定地址开始写指定长度的数据IIC_write_lendata()*******
/************************************************************
*                        
*                                 
***********************************************************/
unsigned char IicWriteLenData(unsigned char Driver,unsigned char len,unsigned char *ptr) 
{
    if(len>8)
    return  0;
    //----------
    IIC_Start();				 	    //发送IIC START信号
	//----------------
	IIC_Send_Byte(Driver);				//	发送器件从地址
	if(RxAck())					//	检测应答信号
	{
	  IIC_Stop();
	  return  0;
	}
	//----------------
	for(;len>0;len--)					//	连续发送LEN长度的字节数据
	{
	  IIC_Send_Byte(*ptr);
	  ptr++;
	  if(RxAck())					//	检测应答信号
	  {
	    IIC_Stop();
	    return  0;
	  }
	}
	IIC_Stop();							//	发送IIC STOP信号	
	return 1;		
}
/************************************************
 ALIENTEK精英STM32开发板实验22
 IIC 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
	
int main(void)
{	 
	u8 key, bTemp, j, bData[3];
	u16 i=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化		 	 	
	IIC_Init();
  
	while(1)
	{
		key = KEY_Scan(0);
		if (key == KEY1_PRES) {//KEY_UP按下,写入24C02
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
		if (key == KEY0_PRES) {//KEY1按下,读取字符串并显示
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
		if (key == WKUP_PRES) {//KEY1按下,读取字符串并显示
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
			LED0 = !LED0;//提示系统正在运行
			i = 0;
		}
	}
}
