#include "Input.h"
#include "..//tm4c123gh6pm.h"



void PortE_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_AMSEL_R &= ~0x03;        // 3) disable analog on PF
  GPIO_PORTE_PCTL_R &= ~0x000000FF;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTE_DIR_R &= ~0x03;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTE_AFSEL_R &= ~0x03;        // 6) disable alt funct on PF7-0
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital I/O on PF4-0
}
void Input_Init(){
	ADC0_Init();
	PortE_Init();
}

struct Input Input_Read(){
	struct Input res;
	res.ADC_Value=ADC0_In();
	res.Buttons=GPIO_PORTE_DATA_R&0x03;
	return res;
}
