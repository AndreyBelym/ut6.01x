#include "ADC.h"

#define BUTTON_A(input)(input.Buttons&0x01)
#define BUTTON_B(input)(input.Buttons&0x02)

struct Input{
	unsigned long ADC_Value;
	unsigned Buttons;
};

void Input_Init(void);
struct Input Input_Read(void);
