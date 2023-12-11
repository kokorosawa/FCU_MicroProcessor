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
char text[16] = "";
int Seven_Segment_index = 0;

void TMR0_IRQHandler(void)
{
	cnt_5ms++;
	index_5ms = cnt_5ms % 4;
	CloseSevenSegment();
	if (index_5ms == 0)
	{
		ShowSevenSegment(3, a[0]);
	}
	if (index_5ms == 1)
	{
		ShowSevenSegment(2, a[1]);
	}
	if (index_5ms == 2)
	{
		ShowSevenSegment(1, a[2]);
	}
	if (index_5ms == 3)
	{
		ShowSevenSegment(0, a[3]);
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
	}					 // else it is unexpected interrupts
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

void Init_GPIO(void)
{
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PD, BIT14, GPIO_MODE_OUTPUT);
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

void Init_UART(void)
{
	UART_Open(UART0, 115200);					  // set UART0 baud rate
	UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
	NVIC_EnableIRQ(UART02_IRQn);				  // enable Cortex-M0 NVIC interrupt for UART02
}

int guess[4] = {0};
int guessindex = 0;
int trytime = 0;
int A = 0;
int B = 0;
int i = 0;
int j = 0;
void checker(){
	A=0;
	B=0;
	for(i = 0; i< 4; i++){			
		if(a[i] == guess[i])
				A++;
		else{
					for(j = 0; j < 4 ;j++){
				if(a[i] == guess[j])
					B++;
		}
		}

	}
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
				guess[guessindex] = c - 48;
				if (guessindex == 3)
				{
					guessindex = 0;
					checker();
					//printf("g:%d %d %d %d\n", guess[0], guess[1], guess[2], guess[3]);
					//printf("a:%d %d %d %d\n", a[0], a[1], a[2], a[3]);
					sprintf(text, "%d%d%d%d  %dA%dB", guess[0], guess[1], guess[2], guess[3], A, B);
					print_Line(trytime, text);
					trytime++;
				}
				else
				{
					guessindex++;
				}
			}
		}
	}
}

int main(void)
{

	int pwd[10] = {0};
	int pwdindex = 0;
	int randnum = 0;
	int ans[4] = {0};
	int ansindex = 0;
	uint8_t ASSCII = 0;
	Init_KEY();
	SYS_Init();
	Init_GPIO();
	Init_Timer0();
	OpenSevenSegment();
	CloseSevenSegment();
	srand(0);
	init_LCD();
	clear_LCD();
	srand(2);
	Init_UART();
	while (pwdindex != 4)
	{
		randnum = rand() % 9 + 1;
		if (pwd[randnum] == 0)
		{
			a[pwdindex] = randnum;
			pwd[randnum] = 1;
			pwdindex++;
		}
	}

	while (1)
	{
		PD14 = 0;
		switch (KEY_Flag)
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			ans[ansindex] = KEY_Flag;
			ansindex++;
			ASSCII = KEY_Flag + 48;
			UART_Write(UART0, &ASSCII, 1);
//printf("%d %d %d %d\n", ans[0], ans[1], ans[2], ans[3]);
			KEY_Flag = 0;
			break;

		default:
			break;
		}
		if (ansindex == 4)
		{
			ansindex = 0;
		}
	}
}
