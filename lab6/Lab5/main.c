//
// LCD_Bmp_Keypad: use 3x3 keypad to move bitmap on LCD
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"



void TMR1_IRQHandler(void)
{
	Timer_ClearIntFlag(TIMER1);
}

void Init_Timer(void)
{
	TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
	TIMER_EnableInt(TIMER1);
	NVIC_EnableIRQ(TMR1_IRQn);
	TIMER_Start(TIMER1);
}

int main(void)
{
	SYS_Init();
	init_LCD();
	clear_LCD();
	OpenKeyPad();
	Init_Timer();
}
