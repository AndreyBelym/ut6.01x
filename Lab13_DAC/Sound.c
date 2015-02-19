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
const unsigned short wave[32] = {
	8 ,9 ,11,12,13,14,14,15,15,15,14,
	14,13,12,11,9 ,8 ,7 ,5 ,4 ,3 ,2 ,
	2 , 1, 1,1 ,2 ,2 ,3 ,4 ,5 ,7
};
const unsigned char Trumpet[32] = {  
  10,11,11,12,10,8,3,1,8,15,15,
  11,10,10,11,10,10,10,10,10,10,10,
  10,11,11,11,11,11,11,10,10,10
};  
const unsigned char Horn[64] = {  
  7,8,8,8,8,9,10,12,15,15,15,
  13,10,7,4,3,3,3,3,3,3,3,
  4,4,4,4,5,6,7,8,8,9,
  9,10,11,11,12,13,13,12,12,13,
  14,12,11,9,8,6,3,2,1,1,
  0,1,1,1,2,2,3,4,4,6,7,7
};  
const unsigned char Guitar[32] = {  
  5,5,4,1,1,3,8,11,11,9,4,
  2,5,11,15,13,9,7,5,5,6,8,
  8,7,4,3,3,3,3,4,5,5
};  
const unsigned char Flute[64] = {  
  6,7,9,9,10,11,12,13,13,14,15,
  15,15,15,15,14,13,13,12,11,10,9,
  8,7,7,6,6,5,5,5,5,4,
  4,4,4,4,4,3,3,3,3,3,
  2,2,1,1,1,1,0,0,0,0,
  0,0,1,1,1,2,2,3,3,4,4,5
};  
const unsigned char Bassoon[32] = {  
  8,8,8,8,7,7,9,15,10,1,1,
  4,8,11,10,6,3,2,6,10,8,5,
  5,5,6,7,7,9,8,8,8,7
};  

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
static volatile int mode;
static volatile unsigned char *start,*current,*end;
void Sound_Init(void){
	DAC_Init();
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
	NVIC_ST_RELOAD_R = 0;
	NVIC_ST_CURRENT_R = 0;
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0 
  NVIC_ST_CTRL_R = 0x00000007;	
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
		NVIC_ST_RELOAD_R = period;
		NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
	}
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	DAC_Out(0);
	NVIC_ST_CTRL_R = 0; 
	NVIC_ST_CURRENT_R = 0;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	if(mode){
		if(current<end)
			DAC_Out((*(current++))>>4);
		else
			current=start;
	} else {
   static unsigned char i=0;
	 DAC_Out(Bassoon[i]);
	 i=(i+1)%32;
	}
}

void Sound_WAV(unsigned char* p,int len,int bitrate){
	Sound_Off();
	mode=1;
	start=p;current=p;end=start+len;
	NVIC_ST_RELOAD_R = 80000000/bitrate;
	NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}
