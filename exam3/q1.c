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
#define P125ms 125000
#define P250ms 250000
#define P500ms 500000
#define P1S 1000000

uint32_t u32Flag;
uint32_t u32ADCvalue;
char text[16] = "";
volatile uint8_t u8ADF;

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

int32_t main(void)
{

	uint8_t i = 0;
	int player1 = 0;
	double convert = 0;
	int8_t x = 0, y = 0;
	int8_t keyin = 0;
	int8_t movX = 3, dirX = 0;
	int8_t movY = 3, dirY = 0;
	int player1_score = 0;
	int player2_score = 0;
	unsigned char bmp16x16[32] = {
		0x00, 0x00, 0x40, 0xE0, 0xE0, 0xE0, 0xE0, 0x70, 0x7C, 0xFE, 0xF8, 0x80, 0x80, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x7F, 0x3E, 0x1E, 0x07, 0x07, 0x07, 0x03, 0x03, 0x02, 0x00};
	SYS_Init();
	Init_ADC();
	Init_PWM();
	init_LCD();
	dirX = 1;
	dirY = 1;
	while (1)
	{
		ADC_START_CONV(ADC);
		sprintf(text, "ADC7 = %4d", u32ADCvalue);
		print_Line(1, text);
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
		convert = u32ADCvalue / 4096.0;
		player1 = convert * LCD_Xmax;
		draw_Bmp16x16(x, y, FG_COLOR, BG_COLOR, bmp16x16);
		x = x + dirX * movX; // increment/decrement X
		y = y + dirY * movY; // increment/decrement Y

		if (x < 16)
		{
			dirX = 1; // check X boundary Min
			Buzz(10);
		}

		if (x > LCD_Xmax - 16)
		{
			Buzz(10);
			dirX = -1; // check X boundary Max
		}

		if (y < 16 && x <= player1 && x + 16 > player1)
			dirY = 1; // check Y boundary Min
		if (y > LCD_Ymax - 16 && x <= player1 && x + 16 > player1)
			dirY = -1; // check Y boundary Max
	}
}