#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "sys.h"


#define User_ESP8266_SSID	  "ausa"	      //Ҫ���ӵ��ȵ������
#define User_ESP8266_PWD	  "123456789"  //Ҫ���ӵ��ȵ������

#define Wifi_ESP8266_SSID	  "WifiSTM32"	      //Ҫ���ӵ��ȵ������
#define Wifi_ESP8266_PWD	  "123456123456"	  //Ҫ���ӵ��ȵ������

#define User_ESP8266_TCPServer_IP	  "134.175.58.30"	  //Ҫ���ӵķ�������IP
#define User_ESP8266_TCPServer_PORT	  "8080"	  //Ҫ���ӵķ������Ķ˿�


extern volatile uint8_t TcpClosedFlag;  //����һ��ȫ�ֱ���


void ESP8266_STA_TCPClient_Test(void);



#endif
