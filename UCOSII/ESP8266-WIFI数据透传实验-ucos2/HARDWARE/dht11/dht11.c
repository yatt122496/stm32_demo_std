#include "dht11.h"

//	
//u8 DHT11_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOE,GPIO_Pin_3);
//	DHT11_Rst();
//	return 
//		DHT11_Check();
//}

//void DHT11_Rst(void)
//{
//	DHT11_IO_OUT();
//	DHT11_DQ_OUT=0;
//	delay_ms(20);
//	DHT11_DQ_OUT=1;
//	delay_us(30);
//}

//u8 DHT11_Check(void)
//{
//	u8 retry=0;
//	DHT11_IO_IN();	//????
//	while(DHT11_DQ_IN&&retry<100)	//DHT11 ?? 40~80us ???????
//	{
//		retry++;
//		delay_us(1);
//	}
//	if(retry>=100)
//		return 1;
//	else
//		retry=0;
//	while (!DHT11_DQ_IN&&retry<100)	//DHT11 ?? 40~80us???????
//	{
//		retry++;
//		delay_us(1);
//	}
//	if(retry>=100)
//		return 0;
//	else
//		retry=0;
//}

//u8 DHT11_Read_Data(void)
//{
//	u8 i;
//	u32 timeout;
//	DHT11_Rst();
//  timeout = 5000;
//  while( (! GPIO_ReadInputDataBit  (GPIOE, GPIO_Pin_3)) && (timeout > 0) ) timeout--;	 //wait HIGH
//  timeout = 5000;
//  while( GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_3) && (timeout > 0) ) timeout-- ;
//	for(i=0; i<5; i++)
//	{
//		buf[i]=DHT11_Read_Byte();
//	}
//	if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
//		return 1;
//	else
//		return 0;
//}

//u8 DHT11_Read_Byte(void)
//{
//	u8 i,dat;
//	dat=0;
//	for (i=0; i<8; i++)
//	{
//		dat<<=1;
//		dat|=DHT11_Read_Bit();
//	}
//	return 
//		dat;
//}

//u8 DHT11_Read_Bit(void)
//{
//	u8 retry=0;
//	while(DHT11_DQ_IN&&retry<100)	//???????
//	{
//		retry++;
//		delay_us(1);
//	}
//	retry=0;
//	while(!DHT11_DQ_IN&&retry<100)	//???????
//	{
//		retry++;
//		delay_us(1);
//	}
//	delay_us(40);
//	if(DHT11_DQ_IN)
//		return 1;
//	else
//		return 0;
//}

//void DHT11_IO_IN()
//{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOE, &GPIO_InitStruct);
//}

//void DHT11_IO_OUT()
//{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
//	GPIO_Init(GPIOE, &GPIO_InitStruct);
//}
void dht11_init (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable  clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE  , ENABLE);
  
  /* Configure Ports */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
 
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
}
 
void mode_input(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}
 
void mode_output(void )
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

u32 dht11_read(void)
{
  int i;
  long long val;
  int timeout;
 
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  delay_us(18000);  //pulldown  for 18ms
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
  delay_us( 20 );	//pullup for 30us
 
  mode_input();
 
  //??dht11??80us
  timeout = 5000;
  while( (! GPIO_ReadInputDataBit  (GPIOE, GPIO_Pin_3)) && (timeout > 0) ) timeout--;	 //wait HIGH
 
  //??dht11??80us
  timeout = 5000;
  while( GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_3) && (timeout > 0) ) timeout-- ;	 //wait LOW
 
#define CHECK_TIME 28
 
  for(i=0;i<40;i++)
  {
	timeout = 5000;
	while( (! GPIO_ReadInputDataBit  (GPIOE, GPIO_Pin_3)) && (timeout > 0) ) timeout--;	 //wait HIGH
 
	delay_us(CHECK_TIME);
	if ( GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_3) )
	{
	  val=(val<<1)+1;
	} else {
	  val<<=1;
	}
 
	timeout = 5000;
	while( GPIO_ReadInputDataBit (GPIOE, GPIO_Pin_3) && (timeout > 0) ) timeout-- ;	 //wait LOW
  }
 
  mode_output();
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
 
  if (((val>>32)+(val>>24)+(val>>16)+(val>>8) -val ) & 0xff  ) return 0; //??
    else return val>>8; 
 
}
