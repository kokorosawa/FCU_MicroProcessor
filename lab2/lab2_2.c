#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

// display an integer on four 7-segment LEDs
void Display_7seg(uint16_t value)
{
	uint8_t digit;
	digit = value / 1000;
	CloseSevenSegment();
	ShowSevenSegment(3, digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2, digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1, digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0, digit);
	CLK_SysTickDelay(5000);
}

uint8_t Scan(void)
{
	PA0 = 1;
	PA1 = 1;
	PA2 = 0;
	PA3 = 1;
	PA4 = 1;
	PA5 = 1;
	if (PA3 == 0)
		return 7;
	if (PA4 == 0)
		return 4;
	if (PA5 == 0)
		return 1;
	PA0 = 1;
	PA1 = 0;
	PA2 = 1;
	PA3 = 1;
	PA4 = 1;
	PA5 = 1;
	if (PA3 == 0)
		return 8;
	if (PA4 == 0)
		return 5;
	if (PA5 == 0)
		return 2;
	PA0 = 0;
	PA1 = 1;
	PA2 = 1;
	PA3 = 1;
	PA4 = 1;
	PA5 = 1;
	if (PA3 == 0)
		return 9;
	if (PA4 == 0)
		return 6;
	if (PA5 == 0)
		return 3;
	return 0;
}

int main(void)
{
	int time = 0;
	int index = 0;
	int tmp[4] = {16, 16, 16, 16};
	int num;
	int last;
	int shift;
	int l;
	uint16_t i;

	SYS_Init();

	OpenSevenSegment();
	OpenKeyPad();

	while (1)
	{
		// i=ScanKey();
		num = Scan();
		if (last != num)
		{
			if (num == 7)
			{
				shift = tmp[3];
				for (l = 3; l > 0; l--)
				{
					tmp[l] = tmp[l - 1];
				}
				tmp[0] = shift;
			}
			else if (num == 8)
			{
				for (l = 3; l >= 0; l--)
				{
					tmp[l] = 16;
				}
			}
			else if (num == 9)
			{
				shift = tmp[0];
				for (l = 0; l < 3; l++)
				{
					tmp[l] = tmp[l + 1];
				}
				tmp[3] = shift;
			}
			else if (num > 0 && num < 7)
			{
				for (l = 3; l > 0; l--)
				{
					tmp[l] = tmp[l - 1];
				}
				tmp[0] = num;
			}
		}
		// printf("%d %d %d %d\n",tmp[0], tmp[1], tmp[2], tmp[3]);

		// printf("%d\n",time);
		CloseSevenSegment();
		ShowSevenSegment(3, tmp[3]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(2, tmp[2]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(1, tmp[1]);
		CLK_SysTickDelay(5000);
		CloseSevenSegment();
		ShowSevenSegment(0, tmp[0]);
		CLK_SysTickDelay(5000);

		last = num;
	}
}
