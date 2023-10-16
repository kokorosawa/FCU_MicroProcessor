//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
//
#include <stdio.h>
#include <math.h>
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
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
			
	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}

void first(){
	int i;
	for (i=0; i<250; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,1);
		
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1200);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2800);	 // Delay 
	}
	for (i=0; i<300; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,1);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2300);	 // Delay 
	}
	for (i=0; i<333; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,1);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(900);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2100);	 // Delay 
	}
}

void second(){
	int i;
	for (i=0; i<333; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,2);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(900);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2100);	 // Delay 
	}
	for (i=0; i<300; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,2);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2300);	 // Delay 
	}
	for (i=0; i<250; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,2);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1200);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2800);	 // Delay 
	}
}

void third(){
	int i;
	CLK_SysTickDelay(5000);
	for (i=0; i<250; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,3);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1200);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2800);	 // Delay 
	}
	CLK_SysTickDelay(5000);
	for (i=0; i<250; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,3);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1200);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2800);	 // Delay 
	}
	CLK_SysTickDelay(5000);
	for (i=0; i<250; i++) {
		CloseSevenSegment();
		ShowSevenSegment(3,3);
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(1200);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(2800);	 // Delay 
	}
}

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(100000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(100000);	 // Delay 
	}
}



uint8_t Scan(void)
{
  PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 1;
	if (PA4==0) return 4;
	if (PA5==0) return 7;
  PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 2;
	if (PA4==0) return 5;
	if (PA5==0) return 8;
  PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 3;
	if (PA4==0) return 6;
	if (PA5==0) return 9;
	return 0;
}

int main(void)
{		
	int time = 0;
	int index = 0; 
	//            3   2  1  0
	int tmp[4] = {16,16,16,16};
	int num;
	int last;
	int shift;
	int l;
	uint16_t i;
	
    SYS_Init();

    OpenSevenSegment();
	  OpenKeyPad();
		

 	  while(1) {
			//i=ScanKey();
			num = Scan();
			if(last != num){
			if(num == 6){
					for(l = 0; l < 3; l++){
						tmp[l] = tmp[l + 1];
					}
					tmp[3] = 16;
			}else if(num == 5){
				int store;
				
				for(l = 2; l >= 0; l--){
					store = tmp[l];
					tmp[3] = tmp[l];
					tmp[l] = 16;
					switch(store){
						case 1:
							ShowSevenSegment(3,tmp[l]);
							first();
							break;
						case 2:
							ShowSevenSegment(3,tmp[l]);
							second();
							break;
						case 3:
							ShowSevenSegment(3,tmp[l]);
							third();
							break;
						default:
							break;
					}
					tmp[l] = store;
				}
				tmp[3] = 16;
				
				CloseSevenSegment();
				for(l = 0; l <= 3;l++){
					tmp[l] = 16;
				}
			}else if(num == 4){
					for(l = 3; l >= 0; l--){
						tmp[l] = 16;
					}
				}else if(num > 0 && num <= 3){
					for(l = 2; l > 0; l--){
						tmp[l] = tmp[l - 1];
					}
					tmp[0] = num;
				}
			}
			//printf("%d %d %d %d\n",tmp[0], tmp[1], tmp[2], tmp[3]);
				
			
			//printf("%d\n",time);
			CloseSevenSegment();
			ShowSevenSegment(3,tmp[3]);
			CLK_SysTickDelay(5000);
			CloseSevenSegment();
			ShowSevenSegment(2,tmp[2]);
			CLK_SysTickDelay(5000);
			CloseSevenSegment();
			ShowSevenSegment(1,tmp[1]);
			CLK_SysTickDelay(5000);
			CloseSevenSegment();
			ShowSevenSegment(0,tmp[0]);
			CLK_SysTickDelay(5000);
			
				last = num;
			
	  }
}
