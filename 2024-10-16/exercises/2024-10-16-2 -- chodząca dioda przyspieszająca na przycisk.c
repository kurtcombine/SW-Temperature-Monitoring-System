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

volatile uint32_t msTicks = 0;
void SysTick_Handler(void)  {msTicks++;}

void Wait(int millis) {
	int start = msTicks;
	while(start + millis < msTicks); // wait for int overflow
	while(msTicks < start + millis);
}

int LED_iterator;

void Setup(void) {
	uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000);
	if (returnCode != 0) TrapError();
	Buttons_Initialize();
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	LED_iterator = LED_GetCount();
}

// Buttons_GetState 0b01 - KEY2; 0b10 - KEY1

void Loop(void) {
	int i1 = (LED_iterator-1) % LED_GetCount();
	int i2 = LED_iterator % LED_GetCount();
	LED_On(i2);
	LED_Off(i1);
	int b = Buttons_GetState() & 0b10;
	Wait((b ? 333 : 1000) / LED_GetCount());
	LED_iterator++;
}
int main() { Setup(); while(1) Loop(); /* implicit shut down */ }
