#ifndef __USART_H
#define __USART_H
#include "stdio.h"


extern u8 USART_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符 
extern u8 USART_RX_STA;         //接收状态标记	

void uart_init(u32 bound);
void UART1_SendString(u8 *p, u16 length);
#endif
