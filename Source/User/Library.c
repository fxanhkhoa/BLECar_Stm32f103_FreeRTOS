#include "Library.h"

/* The transmit task as described at the top of the file. */
static portTASK_FUNCTION_PROTO( vComTxTask, pvParameters );

void ledInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
 
	// Configure PC13, PC14, PC15 as push-pull output
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void USART_Initial()
{
	USART_InitTypeDef USART;
	GPIO_InitTypeDef GPIO;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	/*------ TX-Pin PA9 & RX-Pin PA10 -----*/
			
			GPIO.GPIO_Pin = GPIO_Pin_9;
			GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, & GPIO);
			
			GPIO.GPIO_Pin = GPIO_Pin_10;
			GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO);

		USART_Cmd(USART1, ENABLE);
	
	USART.USART_BaudRate = 9600;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_WordLength = USART_WordLength_8b;
	USART.USART_Parity = USART_Parity_No;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	USART_Init(USART1, &USART);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void Motor_Init()
{
	Driver_RCC_Init();
	Driver_GPIO_Init();
	Driver_PWM_Init();
}

struct Motor getLeftRight(int x, int y)
{
	struct Motor temp;
	int right,left;
	if ((x==0) && (y==0))
	{
		right = 0;
		left = 0;
	}
	else if ((x > 0) && (y < 0))
	{
		if ((abs(x) < abs(y)))
		{
			right = 100 - abs(x);
			left = abs(y);
		}
		else
		{
			right = 100 - abs(y);
			left = abs(x);
		}
	}
	else if ((x < 0) && (y < 0))
	{
		if (abs(x) < abs(y))
		{
			right = abs(y);
			left = 100 - abs(x);
		}
		else
		{
			right = abs(x);
			left = 100 - abs(y);
		}
	}
	else if ((x >0) && (y > 0))
	{
		if (abs(x) < abs(y))
		{
			right = 100 - abs(x);
			left = abs(y);
		}
		else
		{
			right = 100 - abs(y);
			left = abs(x);
		}
		right = - right;
		left = - left;
	}
	else if ((y > 0) && (x < 0))
	{
		if (abs(x) < abs(y))
		{
			right = abs(y);
			left = 100 - abs(x);
		}
		else
		{
			right = abs(x);
			left = 100 - abs(y);
		}
		right = - right;
		left = - left;
	}
	temp.left = left;
	temp.right = right;
	return temp;
}
