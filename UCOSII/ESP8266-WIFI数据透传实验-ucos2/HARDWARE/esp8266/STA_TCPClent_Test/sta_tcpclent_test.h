#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "sys.h"


#define User_ESP8266_SSID	  "ausa"	      //要连接的热点的名称
#define User_ESP8266_PWD	  "123456789"  //要连接的热点的密码

#define Wifi_ESP8266_SSID	  "WifiSTM32"	      //要连接的热点的名称
#define Wifi_ESP8266_PWD	  "123456123456"	  //要连接的热点的密码

#define User_ESP8266_TCPServer_IP	  "134.175.58.30"	  //要连接的服务器的IP
#define User_ESP8266_TCPServer_PORT	  "8080"	  //要连接的服务器的端口


extern volatile uint8_t TcpClosedFlag;  //定义一个全局变量


void ESP8266_STA_TCPClient_Test(void);



#endif
