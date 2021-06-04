#ifndef _LORA_APP_H_
#define _LORA_APP_H_

#include "sys.h"
#include "lora_cfg.h"

//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������ 
//ATK-LORA-01ģ�鹦������	  
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/4/1
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//******************************************************************************** 
//��

#define LORA_AUX  PBin(6)    //LORAģ��״̬����
#define LORA_MD0  PBout(7)  //LORAģ���������

#define XiaoCheFangXiang	6
#define	DianLiang					7
#define	LaJiZhongLiang		8
#define	FaMen							9
#define	ZhuZhou						10
#define	LaJiXiang					11
#define	ShuiBeng					15

extern _LoRa_CFG LoRa_CFG;
extern u8 Lora_mode;

u8 LoRa_Init(void);
void Aux_Int(u8 mode);
void LoRa_Set(void);
void LoRa_SendData(void);
void LoRa_ReceData(void);
void LoRa_Process(void);
void Lora_Test(void);
void HMISends(char *buf1);
void HMISendb(u8 k);
void HMISendstart(void);
void HMISendDate(u8 id,char* data);

#endif

