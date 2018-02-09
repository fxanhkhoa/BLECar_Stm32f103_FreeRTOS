
#include "stm32f10x_usart.h"
#include <stdio.h>

void U_Print(USART_TypeDef * USARTx, unsigned int a);
void U_Print_Char(USART_TypeDef * USARTx, char *s);
void U_Print_float(USART_TypeDef * USARTx, float a);
