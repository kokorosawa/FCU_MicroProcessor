//
// GPIO_LED : GPIO output to control an on-board red LED
// 
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// low-active output control by GPC12

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
    SYS_Init(); 
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
		GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
    while(1) {
	  PC15 =0; // turn on LED
	  CLK_SysTickDelay(100000);	 // Delay 
	  PC15 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC14 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC14 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC13 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC13 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC12 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC12 =1; // turn off LED
	  CLK_SysTickDelay(100000);
		CLK_SysTickDelay(3000000);
		CLK_SysTickDelay(100000);
		PC12 =0; // turn on LED
	  CLK_SysTickDelay(100000);	 // Delay 
	  PC12 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC13 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC13 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC14 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC14 =1; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC15 =0; // turn off LED
	  CLK_SysTickDelay(100000);	 // Delay 			
		PC15 =1; // turn off LED
	  CLK_SysTickDelay(100000);			// Delay 	
		CLK_SysTickDelay(3000000);
		}
}
