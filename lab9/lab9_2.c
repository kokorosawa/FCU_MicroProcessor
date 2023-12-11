//
// I2C_MPU6050_Tilt : using I2C to read Accelerometer to calculate tilt angles
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN (LQFP100)
// IMU : MPU6050 (3-axis accelerometer & 3-axis gyroscope)
//
// MPU6050 Connections
// SCL : to I2C1-SCL/PA11 (LQFP100 pin9 )
// SDA : to I2C1-SDA/PA10 (LQFP100 pin10)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "MPU6050.h"
#include "Seven_Segment.h"
#include "I2Cdev.h"
#include "Draw2D.h"
#define ACCEL_SENSITIVITY 16384    // 32768/2 = 16384 LSB per g
#define GYRO_SENSITIVITY 16.384    // 32768/2000 = 16.384 LSB per degree
#define RAD_TO_DEG 180 / 3.1415926 // degree = radians * 180 / PI
#define ALPHA 0.98

#define PI 3.14159265359
#define RAD_TO_DEG 180 / PI // degree = radians * 180 /  PI

float dt = 0.01;
int ballx = 32, bally = 32;
void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, 10);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

void TMR0_IRQHandler(void)
{
	clear_LCD();
draw_Circle(ballx, bally,8, 0xFF, 0x00);
  TIMER_ClearIntFlag(TIMER0); // Clear Timer1 time-out interrupt flag
}

int main(void)
{
  int16_t AccX, AccY, AccZ;
  float theta, psi, phi;
	

  SYS_Init();
  I2C_Open(I2C1, I2C1_CLOCK_FREQUENCY);
  Init_MPU6050();
	//Init_Timer0();
	init_LCD();
	 clear_LCD();

  while (1)
  {
		clear_LCD();
    AccX = Read_MPU6050_AccX();
    AccY = Read_MPU6050_AccY();
    AccZ = Read_MPU6050_AccZ();
    // printf("Accelerometer: %6d, %6d, %6d\n", AccX, AccY, AccZ);

    // calculate tilt (degree = radians *180 / PI)
    theta = atan(AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI;
    psi = atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI;
    phi = atan(sqrt(pow(AccX, 2) + pow(AccY, 2)) / AccZ) * 180 / PI;
    //printf("theta=%d, psi=%d, phi=%d\n", (int)theta, (int)psi, (int)phi);
		

	if(theta > 15){
			ballx += 3;
		}
	
		if(theta < 15){
			ballx -= 3;
		}
		
		if(psi < 15){
			bally += 3;
		}
		
		if(psi > 15){
			bally -= 3;
		}

		if(ballx + 9 > LCD_Xmax){
			ballx = 120;
		}
		
		if(ballx + 9 < 0){
			ballx = 9;
		}
		
			if(bally + 9 > LCD_Ymax){
			ballx = 55;
		}
			
		if(bally + 9 < 0){
			ballx = 9;
		}
		
			clear_LCD();
draw_Circle(ballx, bally,8, 0xFF, 0x00);
		CLK_SysTickDelay(100000);
		
  }
}
