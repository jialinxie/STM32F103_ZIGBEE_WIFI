#include "sys.h"
#ifndef __TM1638_H
#define __TM1638_H 			   
#include "stm32f10x.h"

extern unsigned char const keytab[];
extern unsigned char const tab[];





////TM1638 STB½ÓPA4  CLK½ÓPA5    DIO½ÓPA6
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





























