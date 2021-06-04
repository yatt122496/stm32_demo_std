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
 ALIENTEK��ӢSTM32������ʵ��37
 ������ʾ ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
//WEWIN�ı���ʾ����
void emwin_texttest(void)
{
	int i;
	char acText[]	= "This example demostrates text wrapping";
	GUI_RECT Rect	={200,240,259,299};	//���������ʾ����
	GUI_WRAPMODE aWm[] = {GUI_WRAPMODE_NONE,
						  GUI_WRAPMODE_CHAR,
						  GUI_WRAPMODE_WORD};
				  
	GUI_SetBkColor(GUI_BLUE);		//���ñ�����ɫ
	GUI_Clear();					//����
	GUI_SetFont(&GUI_Font24_ASCII); //��������
	GUI_SetColor(GUI_YELLOW);       //����ǰ��ɫ(���ı������ߵ���ɫ)
	GUI_DispString("HELLO WORD!");
	
	GUI_SetFont(&GUI_Font8x16);		//��������
	GUI_SetPenSize(10);				//���ñʴ�С
	GUI_SetColor(GUI_RED);			//��ɫ����
	GUI_DrawLine(300,50,500,130);	//����
	GUI_DrawLine(300,130,500,50);	//����
	GUI_SetBkColor(GUI_BLACK);		//���ú�ɫ����
	GUI_SetColor(GUI_WHITE);		//����������ɫΪ��ɫ
	GUI_SetTextMode(GUI_TM_NORMAL);	//����ģʽ
	GUI_DispStringHCenterAt("GUI_TM_NORMAL",400,50);
	GUI_SetTextMode(GUI_TM_REV);	//��ת�ı�
	GUI_DispStringHCenterAt("GUI_TM_REV"   ,400,66);
	GUI_SetTextMode(GUI_TM_TRANS);	//͸���ı�
	GUI_DispStringHCenterAt("GUI_TM_TRANS" ,400,82);
	GUI_SetTextMode(GUI_TM_XOR);   	//����ı�
	GUI_DispStringHCenterAt("GUI_TM_XOR"   ,400,98);
	GUI_SetTextMode(GUI_TM_TRANS|GUI_TM_REV);//͸����ת�ı�
	GUI_DispStringHCenterAt("GUI_EM_TRANS|GUI_TM_REV",400,114);
	
	GUI_SetTextMode(GUI_TM_TRANS);  //͸���ı�
	for(i=0;i<3;i++)
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRectEx(&Rect);
		GUI_SetColor(GUI_BLACK);
		GUI_DispStringInRectWrap(acText,&Rect,GUI_TA_LEFT,aWm[i]);//�ڵ�ǰ����ָ���ľ�����������ʾ�ַ���(�����Զ�����)
		Rect.x0 += 70;
		Rect.x1 += 70;
	}	
}

int main(void)
{	 
	u32 fontcnt;		  
	u8 i,j;
	u8 fontx[2];//gbk��
	u8 key,t;   	    

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	usmart_dev.init(72);		//��ʼ��USMART		
 	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	TFTLCD_Init();			   		//��ʼ��TFTLCD
	TP_Init();				//��������ʼ��
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//ʹ��CRCʱ�ӣ�����STemWin����ʹ�� 
	 
	GUI_Init();
	exfuns_init();				//Ϊfatfs��ر��������ڴ� 
 	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.

	emwin_texttest();
	while(1)
	{
		GUI_TOUCH_Exec();
		GUI_CURSOR_Show(); //��ʾ���
	}
}





