#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "stm32f10x.h"
#include <stdio.h>

int ESP8266_init(void);
int ESP8266_Send(uint8_t *p, uint8_t length);

#endif