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
#include "NewScankey.h"

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

    uint8_t i;

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
    init_keypad_INT();
    // for (i = 0; i < 72; i++)
    // {
    //   PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[i], 90); // 0=Buzzer ON
    //   if (music[i] != 0)
    //     PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    //   else
    //     PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
    //   CLK_SysTickDelay(pitch[i]);
    // }
    clear_LCD();
    while (1)
    {
        PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
        ADC_START_CONV(ADC);
        if (KEY_FLAG != 0)
        {
            PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
            switch (KEY_FLAG)
            {
            case 1:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, C4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "C4");
                break;
            case 2:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, D4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "D4");
                break;
            case 3:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, E4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "E4");
                break;
            case 4:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, F4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "F4");
                break;
            case 5:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, G4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "G4");
                break;
            case 6:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, A4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "A4");
                break;
            case 7:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, B4, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "B4");
                break;
            case 8:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, C5, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "C5");
                break;
            case 9:
                PWM_ConfigOutputChannel(PWM1, PWM_CH0, D5, 90); // 0=Buzzer ON
                sprintf(text, "Music = %s", "D5");
                break;

            default:
                break;
            }
						PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
						CLK_SysTickDelay(P250ms);
						KEY_FLAG = 0;
            print_Line(0, text);
        }
    }
}
