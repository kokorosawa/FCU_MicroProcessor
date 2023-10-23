#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include "LCD.h"

int main(void)
{
    char text[16] = "";
		int last;
		int scan;
		int a[2] = {16, 16};
		int i = 0;
		SYS_Init();
	  OpenSevenSegment();
	  OpenKeyPad();
		init_LCD();
		clear_LCD();
		while(1){
			scan = ScanKey();
			if(last != scan && scan != 0){
				a[i] = scan;
				i++;
				if(i == 2){
					i = 0;
					clear_LCD();
					sprintf(text ,"%d + %d = %d",a[0], a[1], a[0] + a[1]);
					print_Line(0, text);
					sprintf(text ,"%d - %d = %d",a[0], a[1], a[0] - a[1]);
					print_Line(1, text);
					sprintf(text ,"%d * %d = %d",a[0], a[1], a[0] * a[1]);
					print_Line(2, text);
					sprintf(text ,"%d / %d = %d",a[0], a[1], a[0] / a[1]);
					print_Line(3, text);
				}
					
			}
			CloseSevenSegment();
			ShowSevenSegment(3,a[0]);
			CLK_SysTickDelay(5000);
			CloseSevenSegment();
			ShowSevenSegment(0,a[1]);
			CLK_SysTickDelay(5000);
			last = scan;
		}
}
