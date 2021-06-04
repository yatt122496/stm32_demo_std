#include "sys.h"
#include "delay.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "ili93xx.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"

#include "touch.h"
#include "GUI.h"

 
 
/************************************************
 ALIENTEK精英STM32开发板实验37
 汉字显示 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
//WEWIN文本显示例程
void emwin_texttest(void)
{
	int i;
	char acText[]	= "This example demostrates text wrapping";
	GUI_RECT Rect	={200,240,259,299};	//定义矩形显示区域
	GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE,
						  GUI_WRAPMODE_CHAR,
						  GUI_WRAPMODE_WORD};
				  
	GUI_SetBkColor(GUI_BLUE);		//设置背景颜色
	GUI_Clear();					//清屏
	GUI_SetFont(&GUI_Font24_ASCII); //设置字体
	GUI_SetColor(GUI_YELLOW);       //设置前景色(如文本，画线等颜色)
	GUI_DispString("HELLO WORD!");
	
	GUI_SetFont(&GUI_Font8x16);		//设置字体
	GUI_SetPenSize(10);				//设置笔大小
	GUI_SetColor(GUI_RED);			//红色字体
	GUI_DrawLine(300,50,500,130);	//绘线
	GUI_DrawLine(300,130,500,50);	//绘线
	GUI_SetBkColor(GUI_BLACK);		//设置黑色背景
	GUI_SetColor(GUI_WHITE);		//设置字体颜色为白色
	GUI_SetTextMode(GUI_TM_NORMAL);	//正常模式
	GUI_DispStringHCenterAt("GUI_TM_NORMAL",400,50);
	GUI_SetTextMode(GUI_TM_REV);	//反转文本
	GUI_DispStringHCenterAt("GUI_TM_REV"   ,400,66);
	GUI_SetTextMode(GUI_TM_TRANS);	//透明文本
	GUI_DispStringHCenterAt("GUI_TM_TRANS" ,400,82);
	GUI_SetTextMode(GUI_TM_XOR);   	//异或文本
	GUI_DispStringHCenterAt("GUI_TM_XOR"   ,400,98);
	GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_REV);//透明反转文本
	GUI_DispStringHCenterAt("GUI_EM_TRANS|GUI_TM_REV",400,114);
	
	GUI_SetTextMode(GUI_TM_TRANS);  //透明文本
	for(i=0;i<3;i++)
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringInRectWrap(acText,&Rect,GUI_TA_LEFT,aWm[i]);//在当前窗口指定的矩形区域内显示字符串(并可自动换行)
		Rect.x0 += 70;
		Rect.x1 += 70;
	}	
}

int main(void)
{	 
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk码
	u8 key,t;   	    

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	TFTLCD_Init();			   		//初始化TFTLCD
	TP_Init();				//触摸屏初始化
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//使能CRC时钟，否则STemWin不能使用 
	 
	GUI_Init();
	exfuns_init();				//为fatfs相关变量申请内存 
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.

	emwin_texttest();
	while(1)
	{
		GUI_TOUCH_Exec();
		GUI_CURSOR_Show(); //显示鼠标
	}
}





