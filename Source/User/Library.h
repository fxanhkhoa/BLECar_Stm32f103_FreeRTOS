#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif 
	
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_tim.h"
#include "Motor_Driver.h"
#include "math.h"
	
/* Scheduler include files. */
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
	
#include "serial.h"
#include "comtest.h"
	
/* The sequence transmitted is from comFIRST_BYTE to and including comLAST_BYTE. */
#define comFIRST_BYTE				( 'A' )
#define comLAST_BYTE				( 'X' )
	
#define BAUD_RATE 9600
#define comBUFFER_LEN				( ( UBaseType_t ) ( comLAST_BYTE - comFIRST_BYTE ) + ( UBaseType_t ) 1 )
	
#define TASK_COM_PRIORITY  ( tskIDLE_PRIORITY + 1 )
	
//#define GPIOC_ON(pin) {GPIO_WriteBit(GPIOC, pin, Bit_SET);}
//#define GPIOC_OFF(pin) {GPIO_WriteBit(GPIOC, pin, Bit_RESET);}

typedef struct Motor
{
	int left;
	int right;
}motor_controller;

void ledInit();
void USART_Initial();
void Motor_Init();
struct Motor getLeftRight(int x, int y);
	

#ifdef __cplusplus
}
#endif

#endif /* _LIBRARY_H_ */
