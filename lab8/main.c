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
uint32_t u32Flag;
uint32_t u32ADCvalue;

void ADC_IRQHandler(void)
{

  // Get ADC conversion finish interrupt flag
  u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

  if (u32Flag & ADC_ADF_INT)
  {
    u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
    sprintf(text, "ADC7 = %d", u32ADCvalue);
    clear_LCD();
    print_Line(1, text);
    if (u32ADCvalue > 500 &&)
      if (u32ADCvalue > 4000)
        PWM_ConfigOutputChannel(PWM1, PWM_CH0, u32ADCvalue, 90);
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
  SYS_Init();
  Init_ADC();
  Init_PWM();

  while (1)
    ;
}
