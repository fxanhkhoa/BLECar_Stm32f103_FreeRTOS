#ifndef _MOTOR_DRIVER_H_
#define _MOTOR_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif 
	
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
	
#include "stdint.h"
#include "math.h"
	
#define MAX_PULSE 1000
	
void Driver_RCC_Init();
void Driver_GPIO_Init();
void Driver_PWM_Init();
void Control(int left, int right);

#ifdef __cplusplus
}
#endif

#endif /* _LIBRARY_H_ */
