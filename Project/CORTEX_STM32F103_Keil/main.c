/* Library includes. */
#include "stm32f10x_it.h"
#include "Library.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_nvic.h"
#include "usart_print.h"
#include "semphr.h"

#include "stdint.h"
#include "stdlib.h"
#include "string.h"

/* Demo app includes. */
#include "lcd.h"
#include "LCD_Message.h"
#include "BlockQ.h"
#include "death.h"
#include "integer.h"
#include "blocktim.h"
#include "partest.h"
#include "semtest.h"
#include "PollQ.h"
#include "flash.h"
#include "comtest2.h"

char s[30];
char pos = 0;
char ok = 0;
int x, y;
motor_controller _motor;

xSemaphoreHandle left_right_gotsignal;

#define MAX_TICK 10

#define mainECHO_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainCOM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Constants related to the LCD. */
#define mainMAX_LINE						( 240 )
#define mainROW_INCREMENT					( 24 )
#define mainMAX_COLUMN						( 20 )
#define mainCOLUMN_START					( 319 )
#define mainCOLUMN_INCREMENT 				( 16 )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainLCD_QUEUE_SIZE					( 3 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* The number of nano seconds between each processor clock. */
#define mainNS_PER_CLOCK ( ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Baud rate used by the comtest tasks. */
#define mainCOM_TEST_BAUD_RATE		( 9600 )

/* The LED used by the comtest tasks. See the comtest.c file for more
information. */
#define mainCOM_TEST_LED			( 3 )

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed portCHAR *pcTaskName);


void vApplicationStackOverflowHook(xTaskHandle *pxTask,signed portCHAR *pcTaskName) 
{
	(void)pxTask;
	(void)pcTaskName;
	for(;;);
}

static void prvSetupHardware( void );
void ledInit(void);
// RTOS task
void vTaskLedRed(void *p);
void vTaskLedYellow(void *p);
void vTaskLedGreen(void *p);
void vTaskDataRead(void *p);

char data;

int main(void)
{
	prvSetupHardware();
    // Configure GPIO for LED
    ledInit();
	Motor_Init();
	
	vSemaphoreCreateBinary(left_right_gotsignal); 
	//binary_sem = xSemaphoreCreateBinary(); // hoac khai bao = cách nay
		//USART_Initial();
		
	//uart_txq = xQueueCreate(256, sizeof(char));	

    // Create LED blink task
    //xTaskCreate(vTaskLedRed, (const char*) "Red LED Blink",128, NULL, 1, NULL);
    //xTaskCreate(vTaskLedYellow, (const char*) "Yellow LED Blink",128, NULL, 1, NULL);
		vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );
    xTaskCreate(vTaskLedGreen, (const char*) "Green LED Blink", 128, NULL, 1, NULL);
		xTaskCreate(vTaskDataRead, (const char*) "Data Read", 128, NULL, 1, NULL);
    // Start RTOS scheduler
    vTaskStartScheduler();

    return 0;
}

void vTaskLedRed(void *p)
{
    for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_14;
        vTaskDelay(100/portTICK_RATE_MS);
    }
}

void vTaskLedYellow(void *p)
{
    for (;;)
    {
        GPIOC->ODR ^= GPIO_Pin_15;
        vTaskDelay(500/portTICK_RATE_MS);
    }
}

void vTaskLedGreen(void *p)
{
    for (;;)
    {
			if (xSemaphoreTake(left_right_gotsignal, MAX_TICK))
			{
				Control(_motor.left, _motor.right);
			}
    }
}

void vTaskDataRead(void *p)
{
	char temp[3];
	while (1)
	{
		if (pos == 8)
		{
			U_Print_Char(USART1, s);
			pos = 0;
			
			memcpy(temp, &s[0], 3); // lay 3 ki tu dau (copy tu memory nen phai lay dia chi)
			//U_Print_Char(USART1, temp);
			x = atoi(temp);
			
			strncpy(temp, s + 4, 3); // lay 3 ki tu sau khoang trang (copy gia tri nen lay thang mang chuoi)
			//U_Print_Char(USART1, temp);
			y = atoi(temp);
			
			//U_Print(USART1, x);
			//U_Print(USART1, y);
			
			_motor = getLeftRight(x,y);
			
			xSemaphoreGive(left_right_gotsignal);
		}
	}
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );


	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	vParTestInitialise();
}
