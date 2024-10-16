#include <LPC17xx.h>
#include <Board_LED.h>
#include <Board_Buttons.h>
#define LED_BLINKING_PERIOD 1000

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

void LED_Switch(int i, int state) {
	if(state)
		LED_On(i);
	else
		LED_Off(i);
}

volatile int msTicks = 0;
volatile int usTicks = 0;
void SysTick_Handler(void)  {
	usTicks++;
	usTicks %= 1000;
	if(usTicks == 0) msTicks++;
}

void WaitBlocking(int millis) {
	int start = msTicks;
	while(start + millis < msTicks); // wait for int overflow
	while(msTicks < start + millis);
}

int ST_countdown;
// int ST_blinking; - implicit 0

/*
The last LED always blinks. The programm wait for the button.
When the button is pressed the countdown begins
and after that the animation of shot is played.
When the button is pressed during a countdown
the countdown is dismissed.
*/

void Setup(void) {
	uint32_t returnCode = SysTick_Config(SystemCoreClock / 1000000); // SystemCoreClock changes accordingly to clock divisor CCLKCFG
	if (returnCode != 0) TrapError();
	Buttons_Initialize();
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
}

// Buttons_GetState 0b01 - KEY2; 0b10 - KEY1

int countdown_state = -1;
// -1 -> disabled; 0 -> shot, 1..(N-1) -> countdown
int old_button_state = 0;
int button_dead_starttime = -1000;
int button_dead_time = 200;

void Loop(void) {
	// LED_Switch(LED_GetCount()-1, button_dead_starttime + button_dead_time < msTicks);
	if(button_dead_starttime + button_dead_time < msTicks) {
		int button_state = IsKey1Pressed();
		if(old_button_state == 0 && button_state) { // rising slope
			button_dead_starttime = msTicks;
			if(countdown_state == -1) {
				countdown_state = LED_GetCount() - 1;
				ST_countdown = msTicks;
			} else {
				countdown_state = -1;
			}
		}
		if(old_button_state == 1 && !button_state) {
			button_dead_starttime = msTicks;
		}
		old_button_state = button_state;
	}
	
	for(int i = 0; i < LED_GetCount() - 1; ++i)
		LED_Off(i);
	// countdown_state == -1 -> noop
	if(countdown_state > 0) { // countdown animation
		LED_On(countdown_state-1);
	}
	if(countdown_state == 0) { // shot animation
		for(int i = 0; i < LED_GetCount() - 1; ++i) {
			if(i != 0) LED_Off(i-1);
			LED_On(i);
			WaitBlocking(50);
		}
		countdown_state = -1;
	}
	if(countdown_state > 0) {
		if(msTicks > ST_countdown + (LED_GetCount() - countdown_state) * 1000) {
			countdown_state--;
		}
	}
	float x = msTicks % LED_BLINKING_PERIOD;
	x /= LED_BLINKING_PERIOD;
	// x 0..1
	int xx = x * 2000;
	if(xx > 1000) xx = 2000 - xx;
	LED_Switch(LED_GetCount()-1, usTicks < xx);
	
}
int main() { Setup(); while(1) Loop(); /* implicit shut down */ }
