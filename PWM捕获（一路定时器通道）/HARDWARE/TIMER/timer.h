#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM5_Cap_Init(u16 arr,u16 psc);

extern u8  TIM5CH1_CAPTURE_STA;		// ‰»Î≤∂ªÒ◊¥Ã¨
extern u32	TIM5CH1_CAPTURE_VAL,TIM5CH1_CAPTURE_HIGHVAL,TIM5CH1_CNTTIME;

#define cPWM_PSC    2
#define cTIM_PSC    1

#endif
