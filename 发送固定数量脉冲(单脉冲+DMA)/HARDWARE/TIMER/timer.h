#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//********************************************************************************

void TIM2_PWM_DMA_Init(void);
void tim2_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num);
void TIM3_PWM_DMA_Init(void);
void tim3_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num);

#endif
