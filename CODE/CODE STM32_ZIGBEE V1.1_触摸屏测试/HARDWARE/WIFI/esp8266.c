#include "esp8266.h"
#include "stdbool.h"

extern  bool OK_flag;

uint8_t cmd_AT[]={'A','T',0x0D,0x0A};																								//"AT"		"OK"
uint8_t cmd_AT_CWMODE[] = {'A','T','+','C','W','M','O','D','E','=','3',0x0D,0x0A};	//AT+CWMODE=3
uint8_t cmd_CIPMUX[]    = {'A','T','+','C','I','P','M','U','X','=','1',0x0D,0x0A};								//AT+CIPMUX=1
uint8_t cmd_CIPSERVER[] = {'A','T','+','C','I','P','S','E','R','V','E','R','=','1',',','8','0','8','0',0x0D,0x0A};	//AT+CIPSERVER=1,8080
uint8_t cmd_CIPSEND[]   = {'A','T','+','C','I','P','S','E','N','D','=','0',',','6',0x0D,0x0A}; //AT+CIPSEND=0,6  ?? why the id is 1
	
void Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
}

int ESP8266_init(void)
{
	uint8_t i;
	
	USART_SendData(USART1, 0);
	
	Delay(8000000);
	//USART1_printf(USART1, cmd);	
	OK_flag = false;
	for(i = 0; i < 4; i++)
	{
		USART_SendData(USART1, cmd_AT[i]);
		Delay(5000);				
	}
	while(!OK_flag);		//wait ok
	OK_flag = false;
	//Delay(8000000);
	
	for(i = 0; i < 13; i++)
	{
		USART_SendData(USART1, cmd_AT_CWMODE[i]);
		Delay(7000);				
	}
	
	while(!OK_flag);		//wait ok
	OK_flag = false;
	//Delay(8000000);
	
	for(i = 0; i < 13; i++)
	{
		USART_SendData(USART1, cmd_CIPMUX[i]);
		Delay(7000);				
	}
	
	Delay(8000000);
	
	for(i = 0; i < 21; i++)
	{
		USART_SendData(USART1, cmd_CIPSERVER[i]);
		Delay(7000);				
	}
	
	while(!OK_flag);		//wait ok
	OK_flag = false;
	//Delay(8000000);
}

int ESP8266_Send(uint8_t *p, uint8_t length)
{	
	uint8_t i;
	for(i = 0; i < 16; i++)
	{
		USART_SendData(USART1, cmd_CIPSEND[i]);
		Delay(7000);				
	}
	
	Delay(8000000);
	
	for(i = 0; i < length; i++)
	{
		USART_SendData(USART1, p[i]);
		Delay(5000);				
	}
	
	Delay(8000000);
}