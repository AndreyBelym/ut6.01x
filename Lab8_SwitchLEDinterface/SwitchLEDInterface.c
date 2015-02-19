// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****

void PortE_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;     // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTE_AMSEL_R &= ~0x03;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= ~0x000000FF;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x1;          // 5.1) PE0 input, 
  GPIO_PORTE_DIR_R |= 0x02;          // 5.2) PE1 output  
  GPIO_PORTE_AFSEL_R &= ~0x03;        // 6) no alternate function
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital pins PE1,PE0     
}
void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2          
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
}
void SysTick_Handler(void){
  if(GPIO_PORTE_DATA_R & 0x01)
		GPIO_PORTE_DATA_R^=0x02;
	else
		GPIO_PORTE_DATA_R|=0x02;
}
// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
void WaitForInterrupt(void);
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
  PortE_Init();
	SysTick_Init(8000000);
	GPIO_PORTE_DATA_R|=0x02;
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
   WaitForInterrupt();
  }
  
}
