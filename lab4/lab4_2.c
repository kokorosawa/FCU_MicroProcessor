#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include "LCD.h"
#include <stdlib.h>

int main(void)
{
	char text[16] = "";
	int last;
	int scan;
	//              3  2  1   0
	int a[4] = {16, 16, 16, 16};
	int input[4] = {NULL, NULL, NULL, NULL};
	int i = 0;
	int seed = 0;
	int trytime = 0;
	int l;
	int flag = 0;
	SYS_Init();
	OpenSevenSegment();
	OpenKeyPad();
	init_LCD();
	clear_LCD();
	while (1)
	{
		scan = ScanKey();
		seed++;
		srand(seed);
		if (scan != 0 && last != scan)
		{
			if (scan >= 1 && scan <= 6)
			{
				if (i == 0)
				{
					input[i] = scan;
					sprintf(text, "%d", input[0]);
					print_Line(trytime, text);
				}
				if (i == 1)
				{
					input[i] = scan;
					sprintf(text, "%d%d", input[0], input[1]);
					print_Line(trytime, text);
				}
				if (i == 2)
				{
					input[i] = scan;
					sprintf(text, "%d%d%d", input[0], input[1], input[2]);
					print_Line(trytime, text);
				}
				if (i == 3)
				{
					input[i] = scan;
					sprintf(text, "%d%d%d%d", input[0], input[1], input[2], input[3]);
					print_Line(trytime, text);
				}
				i++;
				if (i == 4)
					i = 0;
			}

			if (scan == 7)
			{
				a[0] = rand() % 6 + 1;
				a[1] = rand() % 6 + 1;
				a[2] = rand() % 6 + 1;
				a[3] = rand() % 6 + 1;
			}

			if (scan == 8)
			{
				clear_LCD();
				input[0] = NULL;
				input[1] = NULL;
				input[2] = NULL;
				input[3] = NULL;
				trytime = 0;
			}

			if (scan == 9 && trytime < 4)
			{
				for (l = 0; l < 4; l++)
				{
					if (input[l] == NULL)
					{
						printS(80, trytime * 16, "NULL");
						flag = 1;

						break;
					}
					else if (a[l] != input[l])
					{
						printS(80, trytime * 16, "ERROR");
						for (l = 0; l < 50; l++)
						{
							PB11 = 0;				// PB11 = 0 to turn on Buzzer
							CLK_SysTickDelay(1200); // Delay
							PB11 = 1;				// PB11 = 1 to turn off Buzzer
							CLK_SysTickDelay(2800); // Delay
						}
						flag = 1;
						break;
					}
				}

				if (flag)
				{
					if (trytime == 4)
					{
						break;
					}

					input[0] = NULL;
					input[1] = NULL;
					input[2] = NULL;
					input[3] = NULL;
				}
				else
				{
					printS(80, trytime * 16, "PASS");
					PC15 = 0;				  // turn on LED
					CLK_SysTickDelay(100000); // Delay
					PC15 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC14 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC14 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC13 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC13 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC12 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC12 = 1;				  // turn off LED
					CLK_SysTickDelay(100000);
					CLK_SysTickDelay(3000000);
					CLK_SysTickDelay(100000);
					PC12 = 0;				  // turn on LED
					CLK_SysTickDelay(100000); // Delay
					PC12 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC13 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC13 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC14 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC14 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC15 = 0;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					PC15 = 1;				  // turn off LED
					CLK_SysTickDelay(100000); // Delay
					CLK_SysTickDelay(3000000);
				}
				trytime++;
				flag = 0;
			}
		}
		CloseSevenSegment();
		ShowSevenSegment(3, a[0]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(2, a[1]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(1, a[2]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(0, a[3]);
		CLK_SysTickDelay(5000);
		last = scan;
	}
}
