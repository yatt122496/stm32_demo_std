#include "timer.h"

static u16 wDma_tim2_cr1 = 0;
void TIM2_PWM_DMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef          DMA_InitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM2
	TIM_TimeBaseStructure.TIM_Period = 0; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM2->CR1 |= (1<<3) | (1 << 2);
	//��ʼ��TIM3 Channel2 PWMģʽ
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC1

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ���
 
	TIM2->CCR1=0;
//	TIM2->EGR|=1;
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	
	DMA_DeInit(DMA1_Channel2);                                                 //��DMA��ͨ��2�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&TIM2->CR1;               //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&wDma_tim2_cr1;               //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //DMA_DIR_PeripheralSRC:�������  DMA_DIR_PeripheralDST:���ڴ��
	DMA_InitStructure.DMA_BufferSize         = 0;                            //DMAͨ����DMA����Ĵ�С��ʵ�ⲻ��1��һ����Ӧ�������һ����û������жϹرն�ʱ���ˣ�+1�ͺ���
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc          = DMA_PeripheralInc_Disable;      //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;                //����������ģʽ
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;            //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //�Ƿ����ڴ浽�ڴ洫��(�رձ�ʾֻ���¼������Ŵ���һ�����ݣ�������ʾһֱ����)DMA_M2M_Disable:�Ǵ洢�����洢��ģʽ(�ر��ڴ浽�ڴ�ģʽ)  DMA_M2M_Enable:�����洢�����洢��ģʽ(�����ڴ浽�ڴ�ģʽ)
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);     //��ʱ��2 TIM_DMA_Updateʹ��
	wDma_tim2_cr1 = TIM2->CR1 | 0x01;
//	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearFlag(DMA1_Channel2_IRQn);                                         //���DMA���б�־ 
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);                              //����������ж�
}

void tim2_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num)
{
	TIM_SetAutoreload(TIM2,arr - 1);                               //������װ�ؼĴ���
	TIM_PrescalerConfig(TIM2,psc - 1,TIM_PSCReloadMode_Immediate); //���ö�ʱ��ʱ�ӷ�Ƶ��������Ч

	TIM_SetCompare1(TIM2,arr * (100 - wDuty_cycle) / 100);//ռ�ձ�����ȽϼĴ���
	
	TIM2->EGR|=1;
	DMA_Cmd(DMA1_Channel2, DISABLE);                           //DMAʧ�ܲ�������
	DMA_SetCurrDataCounter(DMA1_Channel2,wPulse_num);             //����DMA���ȣ�����������
	DMA_Cmd(DMA1_Channel2, ENABLE);                            //����
}

static u16 wDma_tim3_cr1 = 0;
void TIM3_PWM_DMA_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef          DMA_InitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
 
   //���ø�����Ϊ�����������,���TIM3 CH2��PWM���岨��	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = 0; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM3->CR1 |= (1<<3) | (1 << 2);
	//��ʼ��TIM3 Channel2 PWMģʽ
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	TIM3->CCR1=0;
//	TIM3->EGR|=1;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	
	DMA_DeInit(DMA1_Channel3);                                                 //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&TIM3->CR1;               //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&wDma_tim3_cr1;               //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;          //DMA_DIR_PeripheralSRC:�������  DMA_DIR_PeripheralDST:���ڴ��
	DMA_InitStructure.DMA_BufferSize         = 0;                            //DMAͨ����DMA����Ĵ�С��ʵ�ⲻ��1��һ����Ӧ�������һ����û������жϹرն�ʱ���ˣ�+1�ͺ���
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;      //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc          = DMA_PeripheralInc_Disable;      //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;    //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;                //����������ģʽ
	DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;            //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                //�Ƿ����ڴ浽�ڴ洫��(�رձ�ʾֻ���¼������Ŵ���һ�����ݣ�������ʾһֱ����)DMA_M2M_Disable:�Ǵ洢�����洢��ģʽ(�ر��ڴ浽�ڴ�ģʽ)  DMA_M2M_Enable:�����洢�����洢��ģʽ(�����ڴ浽�ڴ�ģʽ)
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);     //��ʱ��2 TIM_DMA_Updateʹ��
	wDma_tim3_cr1 = TIM3->CR1 | 0x01;
//	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // ���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearFlag(DMA1_Channel3_IRQn);                                         //���DMA���б�־ 
	DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);                              //����������ж�
}

void tim3_dma_pulse_Output(u16 arr,u16 psc,u8 wDuty_cycle,u16 wPulse_num)
{
	TIM_SetAutoreload(TIM3,arr - 1);                               //������װ�ؼĴ���
	TIM_PrescalerConfig(TIM3,psc - 1,TIM_PSCReloadMode_Immediate); //���ö�ʱ��ʱ�ӷ�Ƶ��������Ч

	TIM_SetCompare1(TIM3,arr * (100 - wDuty_cycle) / 100);                          //ռ�ձ�����ȽϼĴ���
	
	TIM3->EGR|=1;
	DMA_Cmd(DMA1_Channel3, DISABLE);                           //DMAʧ�ܲ�������
	DMA_SetCurrDataCounter(DMA1_Channel3,wPulse_num);             //����DMA���ȣ�����������
	DMA_Cmd(DMA1_Channel3, ENABLE);                            //����
}

void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC3))
    {
		DMA_Cmd(DMA1_Channel3, DISABLE);                           //DMAʧ�ܲ�������
        DMA_ClearFlag(DMA1_Channel3_IRQn);			
		DMA_ClearFlag(DMA1_FLAG_TC3);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC2))
    {
		DMA_Cmd(DMA1_Channel2, DISABLE);                           //DMAʧ�ܲ�������
        DMA_ClearFlag(DMA1_Channel2_IRQn);			
		DMA_ClearFlag(DMA1_FLAG_TC2);
    }
}
