#include "lora_ui.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	 	 	 	 
#include "string.h"    
#include "usart3.h" 
#include "timer.h"
#include "lora_cfg.h"
#include "lora_app.h"

//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������ 
//ATK-LORA-01ģ��UI�͵ײ㴦������	  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/4/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//******************************************************************************** 
//��
 	
//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void lora_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s",USART3_RX_BUF);	//���͵�����
		if(mode)USART3_RX_STA=0;
	} 
}
//lora���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* lora_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//lora��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//��������
	
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
	   while(--waittime)	//�ȴ�����ʱ
	   { 
		  delay_ms(10);
		  if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		  {
			  if(lora_check_cmd(ack))break;//�õ���Ч���� 
			  USART3_RX_STA=0;
		  } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}

//�ַ�ת��16��������
//test[] �洢λ��
//buffer[] ��ת��������
//len�� ����
void char2hex_buff(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
 		test[i] = chr2hex(buff[i]);
	}
}

//��16��������ת��������
u32 hexbuff_num(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	u32 num=0; 
	
	for(i=0;i<len;i++) test[i] = chr2hex(buff[i]);
    if(len==5)  num = test[0]*10000+test[1]*1000+test[2]*100+test[3]*10+test[4];
	else if(len==4) num = test[0]*1000+test[1]*100+test[2]*10+test[3];		
	else if(len==3) num = test[0]*100+test[1]*10+test[2];		
	else if(len==2) num = test[0]*10+test[1];
	else if(len==1) num = test[0];
	
	return num;
}

//��������λ��
//��:100,3λ����10,1λ��
u8 num_count(u32 num)
{
	u32 date=0;
	u8 count=0;
	
	date = num;
	do
	{
		count++;
		date/=10;
	}while(date>0);
	
	return count;
}


//�������
const u8* kbd_tbl1[11]={"1","2","3","4","5","6","7","8","9","0","DEL"};
u8** kbd_tbl;
u8* kbd_fn_tbl[2]={"ȷ��","����"};


u8* cnttbl_mode[3]={"һ��ģʽ","����ģʽ","ʡ��ģʽ"};
u8* cnttbl_sta[2]={"͸������","������"};
u8* cnttbl_power[4]={"11dBm","14dBm","17dBm","20dBm"};
u8* cnttbl_rate[6]={"0.3kbps","1.2kbps","2.4kbps","4.8kbps","9.6kbps","19.2kbps"};


//�������������
void Menu_cfg(u8 num)
{
    u8 key=0;
	u8 t=0;
	
    while(1)
    {
	  key = KEY_Scan(0);
	  
	  if(key==KEY1_PRES)//����+ 
	  {
		  switch(num)
		  {
			  case 1: LoRa_CFG.addr++; break;//��ַ
	          case 2: LoRa_CFG.chn++;//�ŵ�
			          if(LoRa_CFG.chn>31) LoRa_CFG.chn=0;
          			  break;
			  case 3: LoRa_CFG.wlrate++;//��������
			          if(LoRa_CFG.wlrate>5) LoRa_CFG.wlrate=0;
			          break;
			  case 4: LoRa_CFG.power++;//���书��
			          if(LoRa_CFG.power>3)  LoRa_CFG.power=0;
			          break;	   
			  case 5: LoRa_CFG.mode++;//����ģʽ
			          if(LoRa_CFG.mode>2) LoRa_CFG.mode=0;
         			  break;
              case 6: LoRa_CFG.mode_sta++;//����״̬
			          if(LoRa_CFG.mode_sta>1) LoRa_CFG.mode_sta=0;
			          break;		   
		  }
	  }else if(key==KEY0_PRES)//����-
	  {
		  switch(num)
		  {
			  case 1: LoRa_CFG.addr--; break;//��ַ
	          case 2: LoRa_CFG.chn--;//�ŵ�
			          if(LoRa_CFG.chn==255) LoRa_CFG.chn=31;
			          break;
			  case 3: LoRa_CFG.wlrate--;//�������� 
			         if(LoRa_CFG.wlrate==255) LoRa_CFG.wlrate=5;
			          break;
			  case 4: LoRa_CFG.power--;//���书��
			          if(LoRa_CFG.power==255)  LoRa_CFG.power=3;
        			  break;	   
			  case 5: LoRa_CFG.mode--;//����ģʽ
                      if(LoRa_CFG.mode==255) LoRa_CFG.mode=2; 			  
			          break;
              case 6: LoRa_CFG.mode_sta--;//����״̬
			          if(LoRa_CFG.mode_sta>1) LoRa_CFG.mode_sta=1;
        			  break;			   
		  }
	  }else if(key==WKUP_PRES)  break;//�˳�
	  t++;
	  if(t==30)
	  {
		  t=0;
		  LED1=~LED1;
	  }
	  delay_ms(10);
	    
  }
	
		
}

u32 obj_addr=0;//��¼Ŀ���ַ���û�ͨ��������������ʱ������99999,������Ҫ����u32���ͣ�
u8 obj_chn=0;//��¼Ŀ���ŵ�
const u8* set_mode[2]={"����Ŀ���ַ:","����Ŀ���ŵ�"};
u8 callbuf[5]={0};//�洢�û���������ֵ

//����Ŀ���ַ��Ŀ���ŵ�(������)
void Dire_Set(void)
{
	u8 t=0;
	u8 chn=0;
	u32 addr=0;
	u8 key=0;
	u8 test[5]={0};
	u8 addrnumlen; //��ַ����,�5����(65535) 
	u8 chnnumlen;  //�ŵ�����,���2����(31)
	u8 mode=1;//0:���˵�ҳ�� 1:����Ŀ���ַ 2:����Ŀ���ŵ�
	
 RESTART:
	addrnumlen=0;
	chnnumlen=0;
	while(1)
	{
		
		if(key)
		{
			if(key<11)
			{
				if(mode==1)//����Ŀ���ַ��
				{   
					if(addrnumlen<5)
					{
						if(addrnumlen==1&&callbuf[0]=='0') callbuf[0]=kbd_tbl[key-1][0];
						else callbuf[addrnumlen++]=kbd_tbl[key-1][0];
					}
				}else if(mode==2)//����Ŀ���ŵ�
				{
					if(chnnumlen<2)
					{
						if(chnnumlen==1&&callbuf[0]=='0') callbuf[0]=kbd_tbl[key-1][0];
						else callbuf[chnnumlen++]=kbd_tbl[key-1][0];
					}
				}
			}
			else
			{
				if(key==11)//ɾ������ 
				{
					if(mode==1)//Ŀ���ַ����
					{
						if(addrnumlen) addrnumlen--;
					}else if(mode==2)//Ŀ���ŵ�����
					{
						if(chnnumlen) chnnumlen--;
					}
				}
				else if(key==12)//ȷ������ 
				{
					if(mode==1)//Ŀ���ַ����
					{
						if(addrnumlen)
						{
							addr = hexbuff_num(test,callbuf,addrnumlen);//�ַ�ת����
							if(addr>65535)//�趨��ַԽ��
							{
								delay_ms(500);
								delay_ms(500);
								addrnumlen=0;
							}else //��������
							{
								obj_addr = addr;
								mode=2;//����Ŀ���ŵ����ý���
							}
						}else //Ŀ���ַû�趨
						{
								delay_ms(500);
								delay_ms(500);
						}
						
					}else if(mode==2)//Ŀ���ŵ�����
					{
						if(chnnumlen)
						{
							chn = hexbuff_num(test,callbuf,chnnumlen);//�ַ�ת����
							if(chn>31)
							{
								delay_ms(500);
								delay_ms(200);
								chnnumlen=0;
							}else
							{
								//Ŀ���ŵ����óɹ�,�򷵻ز��Խ���
								obj_chn = chn;//�������õ�Ŀ���ŵ�
								break;
							}
						}else//Ŀ���ŵ�û�趨
						{
								delay_ms(500);
								delay_ms(200);
						}
					}
				}else //���ذ���
				{
					mode--;//�����ϼ�
					if(mode==1) goto RESTART;//����Ŀ���ַ����ҳ��
					if(mode==0) break; //�������˵�
				}
				
			 }
			    //���ݸ�����ʾ
	            if(mode==1) callbuf[addrnumlen]=0; 
			    else if(mode==2) callbuf[chnnumlen]=0; 
		}
		t++;
		if(t==30)
		{
			t=0;
			LED1=~LED1;
		}
		delay_ms(10);
	}
		
}
