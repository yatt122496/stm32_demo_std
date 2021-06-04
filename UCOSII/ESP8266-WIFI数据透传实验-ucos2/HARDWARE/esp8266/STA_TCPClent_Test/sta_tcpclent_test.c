#include "sta_tcpclent_test.h"
#include "delay.h"
#include "usart.h"
#include "esp8266_drive.h"
#include <string.h>
#include "led.h"
#include "beep.h"
#include "key.h"

volatile u8 TcpClosedFlag = 0;

void ESP8266_STA_TCPClient_Test(void)
{
	char cCmd [120];
	
	printf ( "\r\n正在配置ESP8266请耐心等待...\r\n" );

	ESP8266_CH_PD_Pin_SetH;
//	ESP8266_ExitUnvarnishSend();
		ESP8266_AT_Test();
	ESP8266_Net_Mode_Choose(STA_AP);
	ESP8266_Rst();
	ESP8266_Creat_Wifi(Wifi_ESP8266_SSID,Wifi_ESP8266_PWD);

	while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
	
	ESP8266_Enable_MultipleId ( DISABLE );
	while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));
//	while(!ESP8266_Creat_Server(User_ESP8266_TCPServer_PORT));
	
//	while(!ESP8266_Set_Time(2880));
	
	while(!ESP8266_UnvarnishSend());
//	while(!ESP8266_Send_Data(0));
	
	printf ( "\r\n配置ESP8266 OK！\r\n" );
	
	ESP8266_Send_AT_Cmd( cCmd, NULL, NULL, 50 );
	printf ( "\r\n开始数据透传!\r\n" );
//	while ( 1 )
//	{	
//		strcpy ( str,"\0" );
//		strcpy ( zhiling,"\0" );
//		Datalen=ESP8266_Rec_Data(str);
//		sprintf ( cCmd, "this send is %d ",a);
//		ESP8266_Send_AT_Cmd( cCmd, NULL, NULL, 50 );
//		a++;
//		delay_ms(50);
//	}	
}



