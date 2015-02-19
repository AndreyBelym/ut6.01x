// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****
#define CARS_LIGHT GPIO_PORTB_DATA_R
#define PEDS_LIGHT GPIO_PORTF_DATA_R
#define SENSOR GPIO_PORTE_DATA_R
#define SENS_MASK 0x07
#define PEDS_MASK 0x0A
#define CARS_MASK 0x3F
struct state {
  unsigned long CarsOut;
  unsigned long PedsOut;	
  unsigned long Time;
  unsigned long Next[8];
};
enum {
	TotalStop,
  GoWest,
  WarnWest,
	GoSouth,
	WarnSouth,
	GoWalk,
	FlashWalk_1_0,
	FlashWalk_1_1,
	FlashWalk_2_0,
	FlashWalk_2_1,
	FlashWalk_3_0,
	FlashWalk_3_1,
}; 
typedef struct state state_t;

state_t FSM[]={
	//Total stop
	{0x24,0x02,500,{TotalStop,GoWest,GoSouth,GoWest,GoWalk,GoWest,GoSouth,GoWest}},
	//GoWest
	{0x0C,0x02,1500,{WarnWest,GoWest,WarnWest,WarnWest,WarnWest,WarnWest,WarnWest,WarnWest}},
	//WarnWest
	{0x14,0x02,500,{TotalStop,TotalStop,GoSouth,GoSouth,GoWalk,GoWalk,GoSouth,GoSouth}},
	//GoSouth
	{0x21,0x02,1500,{WarnSouth,WarnSouth,GoSouth,WarnSouth,WarnSouth,WarnSouth,WarnSouth,WarnSouth}},
	//WarnSouth
	{0x22,0x02,500,{TotalStop,GoWest,TotalStop,GoWest,GoWalk,GoWalk,GoWalk,GoWalk}},
	//GoWalk
	{0x24,0x08,1500,{TotalStop,FlashWalk_1_0,FlashWalk_1_0,FlashWalk_1_0,GoWalk,FlashWalk_1_0,FlashWalk_1_0,FlashWalk_1_0}},
	//FlashWalk_1_0
	{0x24,0x00,200,{TotalStop,GoWest,GoSouth,GoWest,GoWalk,GoWest,GoSouth,TotalStop}},
	//FlashWalk_1_1
	{0x24,0x08,200,{FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0,FlashWalk_2_0}},	
	//FlashWalk_2_0
	{0x24,0x00,200,{FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1,FlashWalk_2_1}},	
	//FlashWalk_2_1
	{0x24,0x08,200,{FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0,FlashWalk_3_0}},	
	//FlashWalk_3_0
	{0x24,0x00,200,{FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1,FlashWalk_3_1}},	
	//FlashWalk_3_1
	{0x24,0x08,200,{TotalStop,GoWest,GoSouth,GoWest,TotalStop,GoWest,GoSouth,GoWest}}
};
void PortF_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x20;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_AMSEL_R &= ~PEDS_MASK;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R |= PEDS_MASK;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R &= ~PEDS_MASK;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_DEN_R |= PEDS_MASK;         // 7) enable digital I/O on PF4-0
}
void PortB_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x02;     // 1) activate clock for Port B
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~CARS_MASK;        // 3) disable analog on PF
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTB_DIR_R |= CARS_MASK;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTB_AFSEL_R &= ~CARS_MASK;        // 6) disable alt funct on PF7-0
  GPIO_PORTB_DEN_R |= CARS_MASK;          // 7) enable digital I/O on PF4-0
}
void PortE_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;     // 1) activate clock for Port E
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTE_AMSEL_R &= ~SENS_MASK;        // 3) disable analog on PF
  GPIO_PORTE_PCTL_R &= ~0x00000FFF;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTE_DIR_R &= ~SENS_MASK;          // 5) PF4,PF0 in, PF3-1 out
	GPIO_PORTE_AFSEL_R &= ~SENS_MASK;        // 6) disable alt funct on PF7-0
  GPIO_PORTE_DEN_R |= SENS_MASK;          // 7) enable digital I/O on PF4-0
}
void FSM_Loop(){
	unsigned long S=0;
	for(;;){
		CARS_LIGHT = FSM[S].CarsOut;  
		PEDS_LIGHT = FSM[S].PedsOut; // set lights
    SysTick_Wait10ms(FSM[S].Time/10);
    S = FSM[S].Next[(SENSOR)&SENS_MASK];
	}
}
int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	SysTick_Init();
  PortF_Init();
	PortB_Init();
	PortE_Init();
  EnableInterrupts();
  FSM_Loop();
}

