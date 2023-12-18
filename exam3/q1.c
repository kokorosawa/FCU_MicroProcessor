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

int32_t main(void)
{

	uint8_t i = 0;
    int player = 0;
    double convert = 0;
	SYS_Init();
	Init_ADC();
	Init_PWM();
	init_LCD();
	while (1)
	{
		ADC_START_CONV(ADC);
		sprintf(text, "ADC7 = %4d", u32ADCvalue );
		print_Line(1, text);
		PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
        convert = u32ADCvalue / 4096.0;
        player = convert * LCD_Xmax;
	}
}