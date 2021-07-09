#include "timer.h"

static u16 wDma_tim2_cr1 = 0;
void TIM2_PWM_DMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef          DMA_InitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = 0; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM2->CR1 |= (1<<3) | (1 << 2);
	//初始化TIM3 Channel2 PWM模式
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC1

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器
 
	TIM2->CCR1=0;
//	TIM2->EGR|=1;
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
	
	DMA_DeInit(DMA1_Channel2);                                                 //将DMA的通道2寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&TIM2->CR1;               //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&wDma_tim2_cr1;               //DMA内存基地址
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //DMA_DIR_PeripheralSRC:从外设读  DMA_DIR_PeripheralDST:从内存读
	DMA_InitStructure.DMA_BufferSize         = 0;                            //DMA通道的DMA缓存的大小，实测不加1少一个，应该是最后一个还没输出就中断关闭定时器了，+1就好了
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc          = DMA_PeripheralInc_Disable;      //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //数据宽度为8位
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;                //工作在正常模式
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;            //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //是否开启内存到内存传输(关闭表示只有事件产生才传输一次数据，开启表示一直传输)DMA_M2M_Disable:非存储器到存储器模式(关闭内存到内存模式)  DMA_M2M_Enable:启动存储器到存储器模式(开启内存到内存模式)
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);     //定时器2 TIM_DMA_Update使能
	wDma_tim2_cr1 = TIM2->CR1 | 0x01;
//	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearFlag(DMA1_Channel2_IRQn);                                         //清除DMA所有标志 
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);                              //开传输完成中断
}

void tim2_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num)
{
	TIM_SetAutoreload(TIM2,arr - 1);                               //设置重装载寄存器
	TIM_PrescalerConfig(TIM2,psc - 1,TIM_PSCReloadMode_Immediate); //设置定时器时钟分频，立即生效

	TIM_SetCompare1(TIM2,arr * (100 - wDuty_cycle) / 100);//占空比填入比较寄存器
	
	TIM2->EGR|=1;
	DMA_Cmd(DMA1_Channel2, DISABLE);                           //DMA失能才能设置
	DMA_SetCurrDataCounter(DMA1_Channel2,wPulse_num);             //设置DMA长度，即脉冲数量
	DMA_Cmd(DMA1_Channel2, ENABLE);                            //开启
}

static u16 wDma_tim3_cr1 = 0;
void TIM3_PWM_DMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef          DMA_InitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
 
   //设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = 0; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM3->CR1 |= (1<<3) | (1 << 2);
	//初始化TIM3 Channel2 PWM模式
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
	TIM3->CCR1=0;
//	TIM3->EGR|=1;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
	
	DMA_DeInit(DMA1_Channel3);                                                 //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&TIM3->CR1;               //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&wDma_tim3_cr1;               //DMA内存基地址
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //DMA_DIR_PeripheralSRC:从外设读  DMA_DIR_PeripheralDST:从内存读
	DMA_InitStructure.DMA_BufferSize         = 0;                            //DMA通道的DMA缓存的大小，实测不加1少一个，应该是最后一个还没输出就中断关闭定时器了，+1就好了
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc          = DMA_PeripheralInc_Disable;      //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //数据宽度为8位
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;                //工作在正常模式
	DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;            //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //是否开启内存到内存传输(关闭表示只有事件产生才传输一次数据，开启表示一直传输)DMA_M2M_Disable:非存储器到存储器模式(关闭内存到内存模式)  DMA_M2M_Enable:启动存储器到存储器模式(开启内存到内存模式)
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);     //定时器2 TIM_DMA_Update使能
	wDma_tim3_cr1 = TIM3->CR1 | 0x01;
//	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 优先级设置
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearFlag(DMA1_Channel3_IRQn);                                         //清除DMA所有标志 
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);                              //开传输完成中断
}

void tim3_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num)
{
	TIM_SetAutoreload(TIM3,arr - 1);                               //设置重装载寄存器
	TIM_PrescalerConfig(TIM3,psc - 1,TIM_PSCReloadMode_Immediate); //设置定时器时钟分频，立即生效

	TIM_SetCompare1(TIM3,arr * (100 - wDuty_cycle) / 100);                          //占空比填入比较寄存器
	
	TIM3->EGR|=1;
	DMA_Cmd(DMA1_Channel3, DISABLE);                           //DMA失能才能设置
	DMA_SetCurrDataCounter(DMA1_Channel3,wPulse_num);             //设置DMA长度，即脉冲数量
	DMA_Cmd(DMA1_Channel3, ENABLE);                            //开启
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC3))
    {
		DMA_Cmd(DMA1_Channel3, DISABLE);                           //DMA失能才能设置
        DMA_ClearFlag(DMA1_Channel3_IRQn);			
		DMA_ClearFlag(DMA1_FLAG_TC3);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC2))
    {
		DMA_Cmd(DMA1_Channel2, DISABLE);                           //DMA失能才能设置
        DMA_ClearFlag(DMA1_Channel2_IRQn);			
		DMA_ClearFlag(DMA1_FLAG_TC2);
    }
}
