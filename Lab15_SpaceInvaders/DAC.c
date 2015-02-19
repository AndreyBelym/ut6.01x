// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"
void PortB_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x02;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0x3F;        // 3) disable analog on PF
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;   // 4) PCTL GPIO on PF4-0
	GPIO_PORTB_DIR_R |= 0x3F;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTB_DR8R_R |= 0x3F;  
	GPIO_PORTB_AFSEL_R &= ~0x3F;        // 6) disable alt funct on PF7-0
  GPIO_PORTB_DEN_R |= 0x3F;          // 7) enable digital I/O on PF4-0
}
// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	PortB_Init();
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R=(GPIO_PORTB_DATA_R&0xF0)|data;
}
