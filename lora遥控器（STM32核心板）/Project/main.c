#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart3.h"
#include "usart.h"
#include "lora_app.h"
#include "key.h"
#include "beep.h"
 
/************************************************
 ALIENTEK��ӢSTM32������ʵ��4
 ���� ʵ��   
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

extern char* dianliang;
extern u8 dianliang_baojing;
int main(void)
{
	int k=0;
	extern u8 Tran_Data[30];//͸������
	u8 key=0,baojing=0,baojing_flag=0,baojing_num=3,KEY9_Flag=0,KEY10_Flag=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	LED_GPIO_Config();			     //LED�˿ڳ�ʼ��          //��ʼ���밴�����ӵ�Ӳ���ӿ�
	KEY_Init();
	BEEP_Init();
	uart_init(9600);
	HMISendstart();
	while(LoRa_Init());

	delay_ms(500);
	LoRa_Set();//LoRa����(�������������ô��ڲ�����Ϊ115200) 
//	while(1)
//	{
//		LED0=~LED0;
//		delay_ms(50);
//	}
 	while(1)
	{
		//����������
		LoRa_ReceData();
		delay_ms(80);
		key=KEY_Scan(0);
		switch	(key)
		{
			case 1:
				sprintf((char*)Tran_Data,"KEY1");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"ǰ");
				break;
			case 2:
				sprintf((char*)Tran_Data,"KEY2");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"��");
				break;
			case 3:
				sprintf((char*)Tran_Data,"KEY3");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"��");
				break;
			case 4:
				sprintf((char*)Tran_Data,"KEY4");
				u3_printf("%s",Tran_Data);
				HMISendDate(XiaoCheFangXiang,"ͣ");
				break;
			case 5:
				sprintf((char*)Tran_Data,"KEY5");
				u3_printf("%s",Tran_Data);
				break;
			case 6:
				sprintf((char*)Tran_Data,"KEY6");
				u3_printf("%s",Tran_Data);
				break;
			case 7:
				sprintf((char*)Tran_Data,"KEY7");
				u3_printf("%s",Tran_Data);
				HMISendDate(ZhuZhou,"ͣ");
				HMISendDate(LaJiXiang,"ͣ");
				break;
			case 8:
				sprintf((char*)Tran_Data,"KEY8");
				u3_printf("%s",Tran_Data);
				break;
			case 9:
				KEY9_Flag++;
				if(KEY9_Flag>=5)
					KEY9_Flag=0;
				if(KEY9_Flag)
				{
					if(KEY9_Flag==1)
						HMISendDate(FaMen,"1");
					else if(KEY9_Flag==2)
						HMISendDate(FaMen,"2");
					else if(KEY9_Flag==3)
						HMISendDate(FaMen,"3");
					else
						HMISendDate(FaMen,"4");
				}
				else
					HMISendDate(FaMen,"��");
				sprintf((char*)Tran_Data,"KEY9");
				u3_printf("%s",Tran_Data);
				break;
			case 10:
				KEY10_Flag++;
				if(KEY10_Flag>=2)
					KEY10_Flag=0;
				if(KEY10_Flag)
					HMISendDate(ShuiBeng,"��");
				else
					HMISendDate(ShuiBeng,"ͣ");
				sprintf((char*)Tran_Data,"KEY10");
				u3_printf("%s",Tran_Data);
				break;
		}
//		if(k%20==0)
//		{
//			dianliang--;
//			sprintf((char*)Tran_Data,"%d",dianliang);
//			HMISendDate(DianLiang,(char*)Tran_Data);
//		}
		if(dianliang_baojing)
		{
			if(atoi(dianliang+1)<=95)
			{
				if(baojing_num)
				{
					baojing=1;
				}
			}
			if(baojing==1)
			{
				baojing_flag++;
				BEEP=~BEEP;
				if(baojing_flag>5)
				{
					baojing=0;
					baojing_num--;
					baojing_flag=0;
				}
			}
		}
		k++;
	}	 
}

