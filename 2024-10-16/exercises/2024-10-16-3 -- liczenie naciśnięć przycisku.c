#include <LPC17xx.h>
#include <Board_LED.h>
#include <Board_Buttons.h>

void TrapError(void) {
	// light up only the last LED and halt
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	LED_On(LED_GetCount() - 1);
	while(1);
}

int IsKey1Pressed() {
	return Buttons_GetState() & 0b10;
}

volatile uint32_t msTicks = 0;
void SysTick_Handler(void)  {msTicks++;}

void Wait(int millis) {
	int start = msTicks;
	while(start + millis < msTicks); // wait for int overflow
	while(msTicks < start + millis);
}

void Setup(void) {
	uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000);
	if (returnCode != 0) TrapError();
	Buttons_Initialize();
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
}

// Buttons_GetState 0b01 - KEY2; 0b10 - KEY1

int buttons_iterator = 0;

void Loop(void) {
	if(IsKey1Pressed()){
		++buttons_iterator;
		for(int i = 0, t = buttons_iterator; i < LED_GetCount(); ++i, t>>=1) {
			if(t&1)
				LED_On(i);
			else
				LED_Off(i);
		}
		Wait(100);
		while(IsKey1Pressed());
		Wait(100);
	}
}
int main() { Setup(); while(1) Loop(); /* implicit shut down */ }
