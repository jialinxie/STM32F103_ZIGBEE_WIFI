#ifndef _BMP180_H_
#define _BMP180_H_
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "lcd.h"

#define	BMP180_SlaveAddress	0xEE	//IIC ADDRESS OF write, READ Address is (Write_add + 1)

//************************************
#define SCL_H         GPIO_SetBits(GPIOA,GPIO_Pin_11) 
#define SCL_L         GPIO_ResetBits(GPIOA,GPIO_Pin_11) 
   
#define SDA_H         GPIO_SetBits(GPIOA,GPIO_Pin_12)
#define SDA_L         GPIO_ResetBits(GPIOA,GPIO_Pin_12)

#define SCL_read      GPIOA->IDR  & GPIO_Pin_11
#define SDA_read      GPIOA->IDR  & GPIO_Pin_12

typedef enum { FALSE = 0, TRUE } BOOL;
#define bool BOOL

void I2C_GPIO_Config(void);
void InitBMP180(void);
void bmp180Convert();

int GetData(unsigned char REG_Address);
void DATA_Diplay();

void I2C_Gpio_Init(void);
void I2C_Start();
void I2C_Stop();
void I2C_SendACK(char ack);
char I2C_RecvACK();
void I2C_SendByte(u8 dat);
u8 I2C_RecvByte(void);
void Single_WriteI2C(u8 SlaveAddress, u8 REG_Address,u8 REG_data);
u8 Single_ReadI2C(u8 SlaveAddress, u8 REG_Address);
short Multiple_read(u8 SlaveAddress, u8 ST_Address);
void Acc_Gas_Test(void);


#endif