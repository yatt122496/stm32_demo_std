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
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103开发板 
//ATK-LORA-01模块UI和底层处理驱动	  
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/4/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//******************************************************************************** 
//无
 	
//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void lora_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("%s",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	} 
}
//lora发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* lora_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}
//lora发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{
	   while(--waittime)	//等待倒计时
	   { 
		  delay_ms(10);
		  if(USART3_RX_STA&0X8000)//接收到期待的应答结果
		  {
			  if(lora_check_cmd(ack))break;//得到有效数据 
			  USART3_RX_STA=0;
		  } 
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}

//字符转成16进制数据
//test[] 存储位置
//buffer[] 需转换的数组
//len： 长度
void char2hex_buff(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
 		test[i] = chr2hex(buff[i]);
	}
}

//将16进制数据转换成数字
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

//计算数字位数
//如:100,3位数、10,1位数
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


//键盘码表
const u8* kbd_tbl1[11]={"1","2","3","4","5","6","7","8","9","0","DEL"};
u8** kbd_tbl;
u8* kbd_fn_tbl[2]={"确定","返回"};


u8* cnttbl_mode[3]={"一般模式","唤醒模式","省电模式"};
u8* cnttbl_sta[2]={"透明传输","定向传输"};
u8* cnttbl_power[4]={"11dBm","14dBm","17dBm","20dBm"};
u8* cnttbl_rate[6]={"0.3kbps","1.2kbps","2.4kbps","4.8kbps","9.6kbps","19.2kbps"};


//主界面参数配置
void Menu_cfg(u8 num)
{
    u8 key=0;
	u8 t=0;
	
    while(1)
    {
	  key = KEY_Scan(0);
	  
	  if(key==KEY1_PRES)//按键+ 
	  {
		  switch(num)
		  {
			  case 1: LoRa_CFG.addr++; break;//地址
	          case 2: LoRa_CFG.chn++;//信道
			          if(LoRa_CFG.chn>31) LoRa_CFG.chn=0;
          			  break;
			  case 3: LoRa_CFG.wlrate++;//无线速率
			          if(LoRa_CFG.wlrate>5) LoRa_CFG.wlrate=0;
			          break;
			  case 4: LoRa_CFG.power++;//发射功率
			          if(LoRa_CFG.power>3)  LoRa_CFG.power=0;
			          break;	   
			  case 5: LoRa_CFG.mode++;//工作模式
			          if(LoRa_CFG.mode>2) LoRa_CFG.mode=0;
         			  break;
              case 6: LoRa_CFG.mode_sta++;//工作状态
			          if(LoRa_CFG.mode_sta>1) LoRa_CFG.mode_sta=0;
			          break;		   
		  }
	  }else if(key==KEY0_PRES)//按键-
	  {
		  switch(num)
		  {
			  case 1: LoRa_CFG.addr--; break;//地址
	          case 2: LoRa_CFG.chn--;//信道
			          if(LoRa_CFG.chn==255) LoRa_CFG.chn=31;
			          break;
			  case 3: LoRa_CFG.wlrate--;//无线速率 
			         if(LoRa_CFG.wlrate==255) LoRa_CFG.wlrate=5;
			          break;
			  case 4: LoRa_CFG.power--;//发射功率
			          if(LoRa_CFG.power==255)  LoRa_CFG.power=3;
        			  break;	   
			  case 5: LoRa_CFG.mode--;//工作模式
                      if(LoRa_CFG.mode==255) LoRa_CFG.mode=2; 			  
			          break;
              case 6: LoRa_CFG.mode_sta--;//工作状态
			          if(LoRa_CFG.mode_sta>1) LoRa_CFG.mode_sta=1;
        			  break;			   
		  }
	  }else if(key==WKUP_PRES)  break;//退出
	  t++;
	  if(t==30)
	  {
		  t=0;
		  LED1=~LED1;
	  }
	  delay_ms(10);
	    
  }
	
		
}

u32 obj_addr=0;//记录目标地址（用户通过键盘数据输入时最大出现99999,所以需要定义u32类型）
u8 obj_chn=0;//记录目标信道
const u8* set_mode[2]={"设置目标地址:","设置目标信道"};
u8 callbuf[5]={0};//存储用户触摸输入值

//设置目标地址和目标信道(定向传输)
void Dire_Set(void)
{
	u8 t=0;
	u8 chn=0;
	u32 addr=0;
	u8 key=0;
	u8 test[5]={0};
	u8 addrnumlen; //地址长度,最长5个数(65535) 
	u8 chnnumlen;  //信道长度,最大2个数(31)
	u8 mode=1;//0:主菜单页面 1:设置目标地址 2:设置目标信道
	
 RESTART:
	addrnumlen=0;
	chnnumlen=0;
	while(1)
	{
		
		if(key)
		{
			if(key<11)
			{
				if(mode==1)//设置目标地址下
				{   
					if(addrnumlen<5)
					{
						if(addrnumlen==1&&callbuf[0]=='0') callbuf[0]=kbd_tbl[key-1][0];
						else callbuf[addrnumlen++]=kbd_tbl[key-1][0];
					}
				}else if(mode==2)//设置目标信道
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
				if(key==11)//删除按键 
				{
					if(mode==1)//目标地址设置
					{
						if(addrnumlen) addrnumlen--;
					}else if(mode==2)//目标信道设置
					{
						if(chnnumlen) chnnumlen--;
					}
				}
				else if(key==12)//确定按键 
				{
					if(mode==1)//目标地址设置
					{
						if(addrnumlen)
						{
							addr = hexbuff_num(test,callbuf,addrnumlen);//字符转数字
							if(addr>65535)//设定地址越界
							{
								delay_ms(500);
								delay_ms(500);
								addrnumlen=0;
							}else //数据正常
							{
								obj_addr = addr;
								mode=2;//进入目标信道设置界面
							}
						}else //目标地址没设定
						{
								delay_ms(500);
								delay_ms(500);
						}
						
					}else if(mode==2)//目标信道设置
					{
						if(chnnumlen)
						{
							chn = hexbuff_num(test,callbuf,chnnumlen);//字符转数字
							if(chn>31)
							{
								delay_ms(500);
								delay_ms(200);
								chnnumlen=0;
							}else
							{
								//目标信道设置成功,则返回测试界面
								obj_chn = chn;//保存设置的目标信道
								break;
							}
						}else//目标信道没设定
						{
								delay_ms(500);
								delay_ms(200);
						}
					}
				}else //返回按键
				{
					mode--;//返回上级
					if(mode==1) goto RESTART;//返回目标地址设置页面
					if(mode==0) break; //返回主菜单
				}
				
			 }
			    //数据更新显示
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
