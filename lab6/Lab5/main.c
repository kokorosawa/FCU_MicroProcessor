//
// LCD_Bmp_Keypad: use 3x3 keypad to move bitmap on LCD
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "gpio.h"

volatile uint8_t ledState = 0;

void TMR1_IRQHandler(void)
{
	ledState = ~ledState;
	if (ledState)
		PC12 = 0;
	else
		PC12 = 1;

	TIMER_ClearIntFlag(TIMER1);
}

void Init_Timer(void)
{
	TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
	TIMER_EnableInt(TIMER1);
	NVIC_EnableIRQ(TMR1_IRQn);
	TIMER_Start(TIMER1);
}

void EINT1_IRQHandler(void)
{
	GPIO_CLR_INT_FLAG(PB, BIT15);
}

void Init_EXTINT(void)
{
	GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
	GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING);
	NVIC_EnableIRQ(EINT0_IRQn);

	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
	GPIO_ENABLE_DEBOUNCE(PC, BIT15);
}

int main(void)
{
	SYS_Init();
	init_LCD();
	clear_LCD();
	Init_Timer();
	Init_EXTINT();
}