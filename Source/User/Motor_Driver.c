#include "Motor_Driver.h"

void Driver_RCC_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}

void Driver_GPIO_Init()
{
	GPIO_InitTypeDef GPIO;
	/********* PWM PIN ***********/
	GPIO.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
	GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO);
	
	/********* DIR PIN ***********/
	GPIO.GPIO_Pin = GPIO_Pin_4  | GPIO_Pin_5 ;
	GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO);
}

void Driver_PWM_Init()
{
	TIM_TimeBaseInitTypeDef TIM_BASE;
	TIM_OCInitTypeDef TIM_OC;
	
	TIM_BASE.TIM_Prescaler = 0;
	TIM_BASE.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BASE.TIM_Period = 1000;
	TIM_BASE.TIM_ClockDivision = 0;
	
	TIM_TimeBaseInit(TIM2, &TIM_BASE);
	
	TIM_OC.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC.TIM_Channel = TIM_Channel_3;
	TIM_OC.TIM_Pulse = 500;
	
	TIM_OCInit(TIM2, &TIM_OC);

	TIM_OC.TIM_Channel = TIM_Channel_2;
	TIM_OC.TIM_Pulse = 10;
	
	TIM_OCInit(TIM2, &TIM_OC);
	TIM_Cmd(TIM2, ENABLE);
}

void Control(int left, int right)
{	
	// Voltage tham chieu vao cau H la = |in1 - in2| nen left > 0 se lay (max vol - xung ra pwm) de hoat dong dc tu 10%->100%
	// con lay min vol thi chi xai dc tam logic input
	long temp_left, temp_right;
	temp_left = left * MAX_PULSE / 100;
	temp_right = right * MAX_PULSE / 100;
	if (left < 0)
	{
		TIM2->CCR2 = -temp_left;
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
	}
	else
	{
		TIM2->CCR2 = MAX_PULSE - temp_left;
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
	}
	
	if (right < 0)
	{
		TIM2->CCR3 = -temp_right;
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
	}
	else
	{
		TIM2->CCR3 = MAX_PULSE - temp_right;
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
	}
	//TIM2->CCR2 = left;
	//TIM2->CCR3 = right;
	/* Buoc phai dung CCR vi neu dung init lai tu dau thi se lau ngoai ra con bi loi nhung config ban dau*/
}
