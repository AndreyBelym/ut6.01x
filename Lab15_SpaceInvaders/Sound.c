// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// March 13, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"
const unsigned short Sine[32] = {
	8 ,9 ,11,12,13,14,14,15,15,15,14,
	14,13,12,11,9 ,8 ,7 ,5 ,4 ,3 ,2 ,
	2 , 1, 1,1 ,2 ,2 ,3 ,4 ,5 ,7
};

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
static volatile int mode;
static volatile unsigned char *start,*current,*end;
void Timer0_Init(){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x00000000; // 8) priority 0
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
}
void Sound_Init(void){
	DAC_Init();
	Timer0_Init();
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	Sound_Off();
	if(period){
		mode=0;
		TIMER0_TAILR_R = period-1;
		TIMER0_CTL_R = 0x00000001;  // enable with core clock and interrupts
	}
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	TIMER0_CTL_R = 0x00000000; 
	DAC_Out(0);
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void Timer0A_Handler(void){
	TIMER0_ICR_R = 0x00000001;
	if(mode){
		if(current<end)
			DAC_Out((*(current++))>>4);
		else
			current=start;
	} else {
   static unsigned char i=0;
	 DAC_Out(Sine[i]);
	 i=(i+1)%32;
	}
}

void Sound_WAV(unsigned char* p,int len,int bitrate){
	Sound_Off();
	mode=1;
	start=p;current=p;end=start+len;
	TIMER0_TAILR_R = 80000000/bitrate-1;
	TIMER0_CTL_R = 0x00000001;  // enable with core clock and interrupts
}
