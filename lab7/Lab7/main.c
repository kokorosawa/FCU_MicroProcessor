#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"
#include <stdlib.h>
#include "LCD.h"
#include <string.h>

char text[16] = "";
char line1[16] = "";
char line2[16] = "";
char line3[16] = "";
char line4[16] = "";

int num[2] = {0, 0};
int index = 0;
int last;
int trytime = 0;
int operation = 0;
volatile uint8_t sleepState = 0;
volatile uint8_t a[4] = {0, 0, 0, 0};
volatile uint8_t KEY_Flag;
volatile uint32_t index_5ms, cnt_5ms, cnt_1s, cnt_100ms, index_key_scan;
int count = 0;

void TMR1_IRQHandler(void)
{
  TIMER_ClearIntFlag(TIMER1);
}
void feeddog(void)
{
  SYS_UnlockReg();
  WDT_RESET_COUNTER();
  SYS_LockReg();
}
void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE, 10);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

int Seven_Segment_index = 0;

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
  }                    // else it is unexpected interrupts
  PA->ISRC = PA->ISRC; // clear all GPB pins
}

void Init_KEY(void)
{
  GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
  GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
  GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
  GPIO_EnableInt(PA, 2, GPIO_INT_LOW);
  NVIC_EnableIRQ(GPAB_IRQn);
  NVIC_SetPriority(GPAB_IRQn, 3);
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
  GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, 200);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

void WDT_IRQHandler(void)
{
  clear_LCD();
  sleepState = 1;
  CLK_SysTickDelay(1000);
  clear_LCD();
  WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
}

void Init_WDT(void)
{
  // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
  SYS_UnlockReg();
  WDT_Open(WDT_TIMEOUT_2POW16, 0, FALSE, FALSE);
  WDT_EnableInt();          // Enable WDT timeout interrupt
  NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
  SYS_LockReg();
}

void Init_GPIO(void)
{
  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
  PC12 = 1;
  PC13 = 1;
  PC14 = 1;
  PC15 = 1;
}

void EINT1_IRQHandler(void)
{
  CloseSevenSegment();
  clear_LCD();
  CLK_SysTickDelay(1000);
  clear_LCD();
  num[0] = 0;
  num[1] = 0;
  index = 0;
  trytime = 0;
  operation = 0;
  GPIO_CLR_INT_FLAG(PB, BIT15); // Clear GPIO interrupt flag
}

void Init_EXTINT(void)
{
  // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
  GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
  GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
  NVIC_EnableIRQ(EINT1_IRQn);

  // Enable interrupt de-bounce function and select de-bounce sampling cycle time
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
  GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

int main(void)
{
  Init_KEY();
  Init_WDT();
  SYS_Init();
  Init_GPIO();
  Init_EXTINT();
  Init_Timer0();
  Init_Timer1();
  CloseSevenSegment();
  init_LCD();
  clear_LCD();

  while (1)
  {
    if (trytime == 4)
    {
      continue;
    }
    switch (KEY_Flag)
    {
    case 1:
      num[index] *= 10;
      num[index] += 1;
      sprintf(text, "%d", num[index]);
      if (index == 0)
      {
        sprintf(line1, "%d", num[index]);
      }
      else
      {
        sprintf(line3, "%d", num[index]);
      }
      print_Line(trytime, text);
      KEY_Flag = 0;
      break;
    case 2:
      num[index] *= 10;
      num[index] += 2;
      if (index == 0)
      {
        sprintf(line1, "%d", num[index]);
      }
      else
      {
        sprintf(line3, "%d", num[index]);
      }
      sprintf(text, "%d", num[index]);
      print_Line(trytime, text);
      KEY_Flag = 0;
      break;
    case 4:
      num[index] *= 10;
      num[index] += 3;
      if (index == 0)
      {
        sprintf(line1, "%d", num[index]);
      }
      else
      {
        sprintf(line3, "%d", num[index]);
      }
      sprintf(text, "%d", num[index]);
      print_Line(trytime, text);
      KEY_Flag = 0;
      break;
    case 5:
      num[index] *= 10;
      num[index] += 4;
      if (index == 0)
      {
        sprintf(line1, "%d", num[index]);
      }
      else
      {
        sprintf(line3, "%d", num[index]);
      }
      sprintf(text, "%d", num[index]);
      print_Line(trytime, text);
      KEY_Flag = 0;
      break;
    case 3:
      trytime++;
      index++;
      operation = 1;
      sprintf(text, "+");
      sprintf(line2, "+");
      print_Line(trytime, text);
      KEY_Flag = 0;
      trytime++;
      break;
    case 6:
      trytime++;
      index++;
      operation = 2;
      sprintf(text, "-");
      sprintf(line2, "-");
      print_Line(trytime, text);
      KEY_Flag = 0;
      trytime++;
      break;
    case 8:
      trytime++;
      index++;
      operation = 3;
      sprintf(text, "/");
      sprintf(line2, "/");
      print_Line(trytime, text);
      trytime++;
      KEY_Flag = 0;
      break;
    case 9:
      trytime++;
      index++;
      operation = 4;
      sprintf(text, "*");
      sprintf(line2, "*");
      print_Line(trytime, text);
      trytime++;
      KEY_Flag = 0;
      break;
    case 7:
      trytime++;
      if (operation == 1)
      {
        sprintf(text, "%d", num[0] + num[1]);
        sprintf(line4, "%d", num[0] + num[1]);
        print_Line(3, text);
      }
      else if (operation == 2)
      {
        sprintf(text, "%d", num[0] - num[1]);
        sprintf(line4, "%d", num[0] + num[1]);
        print_Line(3, text);
      }
      else if (operation == 3)
      {
        sprintf(text, "%d", num[0] / num[1]);
        sprintf(line4, "%d", num[0] + num[1]);
        print_Line(3, text);
      }
      else if (operation == 4)
      {
        sprintf(text, "%d", num[0] * num[1]);
        sprintf(line4, "%d", num[0] + num[1]);
        print_Line(3, text);
      }
      break;
    default:
      break;
    }
  }
}