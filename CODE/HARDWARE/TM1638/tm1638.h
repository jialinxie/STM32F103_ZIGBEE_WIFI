#include "sys.h"
#ifndef __TM1638_H
#define __TM1638_H 			   
#include "stm32f10x.h"
//extern unsigned char const keytab[]={0x01,0x02,0x10,0x0f,0x11,0x12,0x00,0x00,0x03,0x04,0x0e,0x0d,0x13,0x14,0x00,0x00,0x05,0x06,0x0c,0x0b,0x15,0x16,0x00,0x00,0x07,0x08,0x0a,0x09,0x17,0x18,00};	
//extern unsigned char const tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};//数码管0到F的显示






////TM1638 STB接PA4  CLK接PA5    DIO接PA6
/*
#define STB_RCC           RCC_APB2Periph_GPIOA
#define STB_GPIO          GPIOA
#define STB_GPIO_PIN      GPIO_Pin_4
#define STB_OUT(x)        GPIO_WriteBit(GPIOA,GPIO_Pin_4,x)

#define CLK_RCC           RCC_APB2Periph_GPIOA
#define CLK_GPIO         	GPIOA
#define CLK_GPIO_PIN      GPIO_Pin_5
#define CLK_OUT(x)        GPIO_WriteBit(GPIOA,GPIO_Pin_5,x)

#define DIO_RCC       		RCC_APB2Periph_GPIOA
#define DIO_GPIO 					GPIOA
#define DIO_GPIO_PIN      GPIO_Pin_6
#define DIO_OUT(x)        GPIO_WriteBit(GPIOA,GPIO_Pin_6,x)
#define DIO_IN() 					GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
*/
//new
//PA8-->DIO
//PB1-->CLK
//PB2-->STB

#define STB_RCC           RCC_APB2Periph_GPIOA
#define STB_GPIO          GPIOA
#define STB_GPIO_PIN      GPIO_Pin_8
#define STB_OUT(x)        GPIO_WriteBit(GPIOA,GPIO_Pin_8,x)

#define CLK_RCC           RCC_APB2Periph_GPIOB
#define CLK_GPIO         	GPIOB
#define CLK_GPIO_PIN      GPIO_Pin_1
#define CLK_OUT(x)        GPIO_WriteBit(GPIOB,GPIO_Pin_1,x)

#define DIO_RCC       		RCC_APB2Periph_GPIOB
#define DIO_GPIO 					GPIOB
#define DIO_GPIO_PIN      GPIO_Pin_2
#define DIO_OUT(x)        GPIO_WriteBit(GPIOB,GPIO_Pin_2,x)
#define DIO_IN() 					GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)



void TM1638_Write(unsigned char	DATA);
unsigned char TM1638_Read(void); 
void Write_COM(unsigned char cmd);
unsigned char Read_key(void);
void Write_DATA(unsigned char add,unsigned char DATA);
void Write_oneLED(unsigned char num,unsigned char flag);
void init_TM1638(void);	
void TM1638_GPIO_Configuration(void);
void Tm1638_Test(void);
void LED8_Display(unsigned char add,unsigned char DATA);
#endif									   





























