//
// ADC_PWM : read VR1 (ADC7) and set PWM1 to dim blue-LED (GPA12)
//
// Connections of Nu-LB-NUC140 (learning board)
// PA7 /ADC7 is connected to VR1
// PA12/PWM1 is connected to RGB-LED (blue)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "adc.h"
#include "pwm.h"
#include "LCD.h"
#include "Note_Freq.h"
#include "Draw2D.h"
#define P125ms 125000
#define P250ms 250000
#define P500ms 500000
#define P1S 1000000

uint32_t u32Flag;
uint32_t u32ADCvalue;
char text[16] = "";
volatile uint8_t u8ADF;
volatile uint8_t KEY_Flag;
volatile uint32_t index_5ms, cnt_5ms, cnt_1s, cnt_100ms, index_key_scan;

void GPAB_IRQHandler(void)
{
	feeddog();
	if (sleepState == 1)
	{

		print_Line(0, line1);

		print_Line(1, line2);

		print_Line(2, line3);

		print_Line(3, line4);

		sleepState = 0;
	}

	NVIC_DisableIRQ(GPAB_IRQn);
	if (PA->ISRC & BIT0)
	{ // check if PA0 interrupt occurred
		PA0 = 1;
		PA->ISRC |= BIT0; // clear PA0 interrupt status

		if (PA3 == 0)
		{
			KEY_Flag = 3;
			PA3 = 1;
		}
		if (PA4 == 0)
		{
			KEY_Flag = 6;
			PA4 = 1;
		}
		if (PA5 == 0)
		{
			KEY_Flag = 9;
			PA5 = 1;
		}
		return;
	}
	if (PA->ISRC & BIT1)
	{ // check if PA1 interrupt occurred
		PA1 = 1;
		PA->ISRC |= BIT1; // clear PA1 interrupt status
		if (PA3 == 0)
		{
			KEY_Flag = 2;
			PA3 = 1;
		}
		if (PA4 == 0)
		{
			KEY_Flag = 5;
			PA4 = 1;
		}
		if (PA5 == 0)
		{
			KEY_Flag = 8;
			PA5 = 1;
		}
		return;
	}
	if (PA->ISRC & BIT2)
	{ // check if PB14 interrupt occurred
		PA2 = 1;
		PA->ISRC |= BIT2; // clear PA interrupt status
		if (PA3 == 0)
		{
			KEY_Flag = 1;
			PA3 = 1;
		}
		if (PA4 == 0)
		{
			KEY_Flag = 4;
			PA4 = 1;
		}
		if (PA5 == 0)
		{
			KEY_Flag = 7;
			PA5 = 1;
		}
		return;
	}					 // else it is unexpected interrupts
	PA->ISRC = PA->ISRC; // clear all GPB pins
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, 200);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE, 10);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

void ADC_IRQHandler(void)
{

	// Get ADC conversion finish interrupt flag
	u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
	ADC_EnableInt(ADC, ADC_ADF_INT);
	if (u32Flag & ADC_ADF_INT)
	{
		u8ADF = 1;
		u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
	}
	ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
	ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
	ADC_POWER_ON(ADC);
	ADC_EnableInt(ADC, ADC_ADF_INT);
	NVIC_EnableIRQ(ADC_IRQn);
	ADC_START_CONV(ADC);
}

void Init_PWM(void)
{ // PWMA_Ch0 = 50Hz, 50% duty cycle
	PWM_ConfigOutputChannel(PWM1, PWM_CH0, 50, 50);
	PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
	PWM_Start(PWM1, PWM_CH_0_MASK);
}

void Buzz(int number)
{
	int i;
	for (i = 0; i < number; i++)
	{
		PB11 = 0;				// PB11 = 0 to turn on Buzzer
		CLK_SysTickDelay(1000); // Delay
		PB11 = 1;				// PB11 = 1 to turn off Buzzer
		CLK_SysTickDelay(1000); // Delay
	}
}

void TMR0_IRQHandler(void)
{
	cnt_5ms++;
	index_5ms = cnt_5ms % 4;
	CloseSevenSegment();
	if (cnt_5ms % 20 == 0)
	{
		cnt_100ms++;
		index_key_scan = cnt_100ms++ % 3;
		if (index_key_scan == 0)
		{
			PA0 = 1;
			PA1 = 1;
			PA2 = 1;
			PA3 = 1;
			PA4 = 1;
			PA5 = 0;
		}
		if (index_key_scan == 1)
		{
			PA0 = 1;
			PA1 = 1;
			PA2 = 1;
			PA3 = 1;
			PA4 = 0;
			PA5 = 1;
		}
		if (index_key_scan == 2)
		{
			PA0 = 1;
			PA1 = 1;
			PA2 = 1;
			PA3 = 0;
			PA4 = 1;
			PA5 = 1;
		}
		NVIC_EnableIRQ(GPAB_IRQn);
	}

	if (cnt_5ms % 200 == 0)
		cnt_1s++;

	TIMER_ClearIntFlag(TIMER0); // Clear Timer1 time-out interrupt flag
}

int32_t main(void)
{

	uint8_t i = 0;
	int player1x = 0;
	double convert = 0;
	int8_t x = 0, y = 0;
	int8_t keyin = 0;
	int8_t movX = 3, dirX = 0;
	int8_t movY = 3, dirY = 0;
	int player1_score = 0;
	int player2_score = 0;
	unsigned char bmp16x16[16] = {
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	SYS_Init();
	Init_ADC();
	Init_PWM();
	init_LCD();
	dirX = 1;
	dirY = 1;
	PB11 = 1;
	while (1)
	{
		clear_LCD();
		ADC_START_CONV(ADC);
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
		PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
		convert = u32ADCvalue / 4096.0;
		player1x = convert * LCD_Xmax;
		draw_Circle(x, y, 1, FG_COLOR, BG_COLOR);
		draw_Bmp16x16(player1x, 56, FG_COLOR, BG_COLOR, bmp16x16);
		x = x + dirX * movX; // increment/decrement X
		y = y + dirY * movY; // increment/decrement Y

		if (x < 16)
		{
			dirX = 1; // check X boundary Min
					  // Buzz(10);
		}

		if (x > LCD_Xmax - 16)
		{
			// Buzz(10);
			dirX = -1; // check X boundary Max
		}

		if (y < 16 && x <= player1x && x + 16 > player1x)
			dirY = 1; // check Y boundary Min
		if (y > LCD_Ymax - 16 && x <= player1x && x + 16 > player1x)
			dirY = -1; // check Y boundary Max
	}
}