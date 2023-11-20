#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"
#include <stdlib.h>
#include "LCD.h"

volatile uint8_t ledState = 0;
volatile uint8_t a[4] = {0, 0, 0, 0};
volatile uint8_t KEY_Flag;
volatile uint32_t index_5ms, cnt_5ms, cnt_1s, cnt_100ms, index_key_scan;
int count = 0;

void TMR1_IRQHandler(void)
{
  if (ledState == 0)
  {
    if (a[0] == 9)
    {
      a[1]++;
      a[0] = 0;
    }
    if (a[1] == 6)
    {
      a[1] = 0;
      a[2]++;
    }
    if (a[2] == 9)
    {
      a[3]++;
      a[2] = 0;
    }
    a[0]++;
  }

  TIMER_ClearIntFlag(TIMER1);
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
  if (index_5ms == 0)
  {
    ShowSevenSegment(0, a[0]);
  }
  if (index_5ms == 1)
  {
    ShowSevenSegment(1, a[1]);
  }
  if (index_5ms == 2)
  {
    ShowSevenSegment(2, a[2]);
  }
  if (index_5ms == 3)
  {
    ShowSevenSegment(3, a[3]);
  }
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
  WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
}

void feeddog(void)
{
  SYS_UnlockReg();
  WDT_RESET_COUNTER();
  SYS_LockReg();
}

void Init_WDT(void)
{
  // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
  SYS_UnlockReg();
  WDT_Open(WDT_TIMEOUT_2POW16, 0, TRUE, TRUE);
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
  while (TRUE)
  {
    PC12 = 1;
    CLK_SysTickDelay(100000);
    PC12 = 0;
    CLK_SysTickDelay(100000);
  };
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
  int num;
  int last;
  char text[16] = "";
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
    feeddog();
    switch (KEY_Flag)
    {
    case 2:
      sprintf(text, "%d: %d%d:%d%d", count + 1, a[3], a[2], a[1], a[0]);
      print_Line(count, text);
      KEY_Flag = 0;
      count++;
      break;
    case 1:
      ledState = ~ledState;
      KEY_Flag = 0;
      break;
    case 3:
      clear_LCD();
      a[0] = 0;
      a[1] = 0;
      a[2] = 0;
      a[3] = 0;
      KEY_Flag = 0;
      count = 0;
      break;
    default:
      break;
    }
  }
}
