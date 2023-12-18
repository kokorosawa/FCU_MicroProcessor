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
volatile uint8_t buffer[10];


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

void UART02_IRQHandler(void)
{
	uint8_t c;
	uint32_t u32IntSts = UART0->ISR;

	if (u32IntSts & UART_IS_RX_READY(UART0)) // check ISR on & RX is ready
	{
		while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk))
		{ // check RX is not empty

			c = UART_READ(UART0); // read UART RX data
			if (c != '0')
			{
				
			}
		}
	}
}

int32_t main(void)
{

	uint8_t i = 0;

	uint16_t music[72] = {
		E6, D6u, E6, D6u, E6, B5, D6, C6, A5, A5, 0, 0,
		C5, E5, A5, B5, B5, 0, C5, A5, B5, C6, C6, 0,
		E6, D6u, E6, D6u, E6, B5, D6, C6, A5, A5, 0, 0,
		C5, E5, A5, B5, B5, 0, E5, C6, B5, A5, A5, 0,
		B5, C6, D6, E6, E6, 0, G5, F6, E6, D6, D6, 0,
		F5, E6, D6, C6, C6, 0, E5, D6, C6, B5, B5, 0};

	uint32_t pitch[72] = {
		P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms,
		P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms, P250ms,
		P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
		P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
		P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms};
	SYS_Init();
	Init_ADC();
	Init_PWM();
	init_LCD();
	// for (i = 0; i < 72; i++)
	// {
	//   PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[i], 90); // 0=Buzzer ON
	//   if (music[i] != 0)
	//     PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
	//   else
	//     PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
	//   CLK_SysTickDelay(pitch[i]);
	// }
	while (1)
	{
		ADC_START_CONV(ADC);
		sprintf(text, "ADC7 = %4d", u32ADCvalue );
		print_Line(1, text);
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
		if (u32ADCvalue < 500)
			PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
		else if (u32ADCvalue > 500 && u32ADCvalue < 4000)
			PWM_ConfigOutputChannel(PWM1, PWM_CH0, u32ADCvalue, 90);
		else
		{
				i++;
				PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[i], 90); // 0=Buzzer ON
				if (music[i] != 0)
					PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
				else
					PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
				CLK_SysTickDelay(pitch[i]);
				if(i == 71) i = 0;
			
		}
	}
}
