#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "usart3.h"
#include "timer.h"
#include "string.h"
#include "math.h"
#include "laser_def.h"
#include "vl53l1x.h"
#include "vl53l1x_i2c.h"
 
/************************************************
 ALIENTEK精英STM32开发板实验9
 PWM输出实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
uint16_t Distance_data[2] = {0,0};
extern VL53L1_Dev_t VL53L1_dev[];	//2, device param, include I2C
extern uint8_t Ajusted[];//2, adjusted sign, 0-not, 1-had
extern VL53L1_RangingMeasurementData_t VL53L1_data[];//2, ranging result struct, distance, max distance,etc.
extern uint16_t Distance_data[];//2, the catched distance. VL53L1_data->RangeMilliMeter; 

u16 j=1999;

void HMISends(char *buf1);
void HMISendb(u8 k);
void HMISendstart(void);
void HMISendDate(u8 id,char* data);

void shoot(void)
{
	TIM_SetCompare3(TIM3,1999);
	delay_ms(500);
	LED2=1;
	delay_ms(50);
	LED2=0;
	j=1999;
	delay_ms(500);
	TIM_SetCompare1(TIM3,1778);
}

int main(void)
{
	u8 mode = 1,Startdu=12;
	u8 Status = 0,m=0,dir=1,k=0;
	u16 i=0,rlen=0;
	u8 key=0;
	int dis=0,du=0;
	char str[20],dis0[10];
	char* strstr;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	USART2_Init(115200);
	usart3_init(9600);
	HMISendstart();
 	LED_Init();			     //LED端口初始化
	KEY_Init();
	TIM3_PWM_Init(1999,719);	 //不分频。PWM频率=72000000/900=80Khz
	TIM_SetCompare1(TIM3,1797);
	TIM_SetCompare2(TIM3,1827);
	TIM_SetCompare3(TIM3,1999);
//	TIM_SetCompare4(TIM3,0);
	Status = VL53L1_init(&VL53L1_dev[0]);
	if(0 != Status)
	{
		printf("VL53L1 Init error!\r\n");
	}
	else
	{
		printf("VL53L1 Init OK\r\n");
	}
	
	if(VL53L1_set_mode(&VL53L1_dev[0],mode))
	{
		printf("mode set error\r\n");
	}
	else
	{
		printf("set mode OK\r\n");
	}
	
  while(1)
	{
		TIM_SetCompare3(TIM3,j);
		if(i%3==0)
		{
			if(j>100)
				j-=100;
			else
				j=0;
		}
		key=KEY_Scan(0);
		if(key)
		{
			if(key==WKUP_PRES)
			{
				shoot();
			}
			else if(key==KEY0_PRES)
			{
				Startdu++;
				LED1=~LED1;
				printf("%d\r\n",Startdu);
			}
			else if(key==KEY1_PRES)
			{
				Startdu--;
				LED1=~LED1;
				printf("%d\r\n",Startdu);
			}
		}
		strcpy ( str,"\0");
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符
			strstr=strchr((const char*)USART3_RX_BUF,'e');
			strcpy(str,strstr+7);
			printf("%s\r\n",str);	//发送到串口 
			USART3_RX_STA=0;
		}
		if(strcmp(str,"\0")!=0)
		{
			if(strcmp(str,"start0")==0)
			{
				k=0;
				m=0;
				TIM_SetCompare2(TIM3,1807);
				TIM_SetCompare1(TIM3,1797);
				delay_ms(50);
				while(1)
				{
					k++;
					if(k>=200)
						k=0;
					if(k%6==0)
					{
						if(dir)
							m++;
						else
							m--;
						TIM_SetCompare2(TIM3,1807+m);
					}
					if(m==40)
						dir=0;
					if(m==0)
						dir=1;
					Status = VL53L1_single_test(&VL53L1_dev[0],&VL53L1_data[0]);
					if(Status==VL53L1_Error_NONE)
					{
						printf("Distance = %4d\r\n",Distance);
						Distance-=160;
						if(Distance>=2700&&Distance<=3300)
							break;
					}
					else
					{
						printf("measure error\r\n");
						VL53L1_init(&VL53L1_dev[0]);
						VL53L1_set_mode(&VL53L1_dev[0],mode);
					}
				}
				dis=Distance/10-30;
				if(dis>=250&&dis<260)
				{
					dis=Startdu+0;
				}
				else if(dis>=260&&dis<280)
				{
					dis=Startdu+1;
				}
				else if(dis>=280&&dis<290)
				{
					dis=Startdu+2;
				}
				else if(dis>=290&&dis<300)
				{
					dis=Startdu+3;
				}
				else
					dis=Startdu+0;
				TIM_SetCompare1(TIM3,1797-dis);
				delay_ms(1000);
				delay_ms(1000);
				shoot();
			}
			else if(strcmp(str,"start1")==0)
			{
				k=0;
				m=0;
				TIM_SetCompare2(TIM3,1807);
				TIM_SetCompare1(TIM3,1797);
				delay_ms(50);
				while(1)
				{
					k++;
					if(k>=200)
						k=0;
					if(k%6==0)
					{
						if(dir)
							m++;
						else
							m--;
						TIM_SetCompare2(TIM3,1807+m);
					}
					if(m==40)
						dir=0;
					if(m==0)
						dir=1;
					Status = VL53L1_single_test(&VL53L1_dev[0],&VL53L1_data[0]);
					if(Status==VL53L1_Error_NONE)
					{
						printf("Distance = %4d\r\n",Distance);
						Distance-=160;
						if(Distance>=2700&&Distance<=2900)
							break;
					}
					else
					{
						printf("measure error\r\n");
						VL53L1_init(&VL53L1_dev[0]);
						VL53L1_set_mode(&VL53L1_dev[0],mode);
					}
				}
				TIM_SetCompare1(TIM3,1797-12);
				delay_ms(1000);
				delay_ms(1000);
				shoot();
			}
			else if((strchr((const char*)str,'d'))!=0)
			{
				u8 len=0,i=0,a=0;
				len=strlen(str);
				for(i=0;i<len;i++)
					if(str[i]=='a')
					{
						a=i;
						break;
					}
				dis=0;
				for(i=0;i<a-2;i++)
					dis+=(str[a-1-i]-'0')*pow(10,i);
				du=0;
				for(i=0;i<len-a-2;i++)
					du+=(str[len-1-i]-'0')*pow(10,i);
				printf("%d,%d\r\n",dis,du);	//发送到串口 
				if(dis>=250&&dis<260)
				{
					dis=Startdu+0;
				}
				else if(dis>=260&&dis<280)
				{
					dis=Startdu+1;
				}
				else if(dis>=280&&dis<=290)
				{
					dis=Startdu+2;
				}
				else if(dis>=290&&dis<300)
				{
					dis=Startdu+3;
				}
				else
					dis=Startdu+0;
				TIM_SetCompare1(TIM3,1797-dis);
				du-=30;
				TIM_SetCompare2(TIM3,1827+du*0.67);
				delay_ms(1000);
				delay_ms(1000);
				shoot();
			}
		}
		Status = VL53L1_single_test(&VL53L1_dev[0],&VL53L1_data[0]);
		if(Status==VL53L1_Error_NONE)
		{
			Distance-=160;
			printf("Distance = %4d\r\n",Distance);
			sprintf(dis0,"%3.3d",Distance/10);
			HMISendDate(3,dis0);
		}
		else
		{
			printf("measure error\r\n");
			VL53L1_init(&VL53L1_dev[0]);
			VL53L1_set_mode(&VL53L1_dev[0],mode);
		}
//		while(1)
//		{
//			LED0=~LED0;
//			delay_ms(100);
//		}
		i++;
		if(i%50==0)
		{
			LED0=~LED0;
		}
//		delay_ms(5);
//		TIM_SetAutoreload(TIM3,0);		
	}
}

void HMISends(char *buf1)
{
	u8 i=0;
	while(1)
	{
	 if(buf1[i]!=0)
	 	{
			USART_SendData(USART3,buf1[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
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
			USART_SendData(USART3,k);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};
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
	sprintf(stringHMI,"main.t%d.txt=\"",id);
	HMISends(stringHMI);
	u3_printf("%s ",data);
	HMISends("\"");
	HMISendb(0xff);
}
