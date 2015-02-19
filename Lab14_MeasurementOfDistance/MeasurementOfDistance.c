// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
// April 8, 2014

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE1 and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10];
unsigned long Distance; // units 0.001 cm
unsigned long ADCdata;  // 12-bit 0 to 4095 sample
unsigned long Flag;     // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
	return sample*2000/4095;
}

// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){
  GPIO_PORTF_DATA_R^=0x02;	
	ADCdata = ADC0_In();
	Distance = Convert(ADCdata);
	Flag=1;
	GPIO_PORTF_DATA_R^=0x02;
}

void PortF_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x20;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_AMSEL_R &= ~0x02;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R &= ~0x000000F0;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= 0x02;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R &= ~0x02;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_DEN_R |= 0x02;         // 7) enable digital I/O on PF4-0
}
void SysTick_Init(){
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = 1999999;     // reload value for 500us (assuming 80MHz)
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}
//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
#define MAX_LEN 4
#define MAX_10POW 10000
#define MAX_LEN2 (MAX_LEN+1)
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 implement this function
    int i=0;
	if(n/MAX_10POW)
		for(i=0;i<MAX_LEN2;i++)
			String[i]='*';
	else{ 
		i=MAX_LEN2-1;
		do{
			String[i]=n%10+0x30;
			n/=10; i--;
		}while(n);
		while(i>=0){
			String[i]='0';
			i--;
		}
	}
	String[0]=String[1];
	String[1]='.';
	String[MAX_LEN2]=' ';
	String[MAX_LEN2+1]='c';
	String[MAX_LEN2+2]='m';
	String[MAX_LEN2+3]='\0';
}
int main(void){ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
// initialize ADC0, channel 1, sequencer 3
// initialize Nokia5110 LCD (optional)
// initialize SysTick for 40 Hz interrupts
// initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
	SysTick_Init();
	PortF_Init();
  EnableInterrupts();
// print a welcome message  (optional)
  while(1){
		Flag=0;
		while(!Flag);
		UART_ConvertDistance(Distance);
		Nokia5110_SetCursor(0, 0); Nokia5110_OutString(String); 
// read mailbox
// output to Nokia5110 LCD (optional)
  }
}

int main1(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    
  }
}

