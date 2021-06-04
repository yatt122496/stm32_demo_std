#ifndef __VL53L1_I2C_H
#define __VL53L1_I2C_H

#include "stdint.h"
#include "sys.h"

//const
#define I2C_TX_TIMEOUT  0x02
#define I2C_RX_TIMEOUT  0x02

//Status
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01
#define VL_SDA_IN()  {GPIOF->CRL&=0XFFFFF0FF;GPIOF->CRL|=8<<8;}
#define VL_SDA_OUT() {GPIOF->CRL&=0XFFFFF0FF;GPIOF->CRL|=3<<8;}
#define VL_IIC_SCL    PFout(3)      //SCL
#define VL_IIC_SDA    PFout(2) 		//SDA	 
#define VL_READ_SDA   PFin(2) 		//����SDA 
//I2C operation
//void VL53L1_i2c_init(void);

uint8_t VL53L1_write_byte(uint8_t address,uint16_t index,uint8_t data);              //IICдһ��8λ����
uint8_t VL53L1_write_word(uint8_t address,uint16_t index,uint16_t data);             //IICдһ��16λ����
uint8_t VL53L1_write_dword(uint8_t address,uint16_t index,uint32_t data);            //IICдһ��32λ����
uint8_t VL53L1_write_multi(uint8_t address, uint16_t index,uint8_t *pdata,uint16_t count);//IIC����д

uint8_t VL53L1_read_byte(uint8_t address,uint16_t index,uint8_t *pdata);             //IIC��һ��8λ����
uint8_t VL53L1_read_word(uint8_t address,uint16_t index,uint16_t *pdata);            //IIC��һ��16λ����
uint8_t VL53L1_read_dword(uint8_t address,uint16_t index,uint32_t *pdata);           //IIC��һ��32λ����
uint8_t VL53L1_read_multi(uint8_t address,uint16_t index,uint8_t *pdata,uint16_t count);  //IIC������

void DelayUs(uint16_t n);		//need tested arrcoding to system clock, for 30Mhz a slight slow 
void VL53L1_I2C_Init(void);
#endif 


