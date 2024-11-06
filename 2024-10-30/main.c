#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <Board_LED.h>
#include <stdio.h>
#define UART LPC_UART1

void Ping_Timer_Init() {
	LPC_TIM0->MCR = 3;
	LPC_TIM0->MR0 = 25e6/2;
	LPC_TIM0->TCR |= 1;
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void RTC_Timer_Init() {
	LPC_RTC->CCR = 1;
	LPC_RTC->ILR = 1;
	LPC_RTC->CIIR = 1;
	NVIC_EnableIRQ(RTC_IRQn);
}

void KEY2_Interrupt_Init() {
	LPC_SC->EXTMODE = 1;
	LPC_SC->EXTPOLAR = 0;
	LPC_SC->EXTINT = 1;
	PIN_Configure(2,10,1,0,0);
	NVIC_EnableIRQ(EINT0_IRQn);
}

void LCD_Touch_Init() {
	//LPC_SC->EXTMODE = 1;
	//LPC_SC->EXTPOLAR = 0;
	//LPC_SC->EXTINT = 1;
	PIN_Configure(0,19,1,0,0);
	LPC_GPIOINT->IO0IntEnF |= 1<<19;
	NVIC_EnableIRQ(EINT3_IRQn);
}

void UART_Init() {
	//LPC_SC->PCONP |= 1<<24;
	
	// UART0 : P0.2 P0.3
	// UART1: P0.15, P0.16 <- sprrawdzic luty na plytce
	// UART2: P0.10, P0.11 <- sprrawdzic luty na plytce
	PIN_Configure(0,15,1,0,0);
	PIN_Configure(0,16,1,0,0);

	UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
	//DLAB = 1
	//Multiplier values for 115,5k baud rade
	if(0){
		UART->DLL = 10;
		UART->DLM = 0;
	
		UART->FDR = 5 | 14<<4; //DivAdd = 5, Mul = 14
	} else {
		UART->DLL = 163;
	}
	
	UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b00000111;	
}

void UART_Char_Send(char c) {
		while(!(UART->LSR & 1<<5)); // wait for THRE flag (5th bit of LSR reg) to reset
		UART->THR = c;
}
void UART_Send(const char* str) {
	for(int i = 0; str[i]; i++) {
		UART_Char_Send(str[i]);
	}
}

void TIMER0_IRQHandler(void) {
	LPC_TIM0->IR |= 1;
	UART_Send("Ping ");
}

void RTC_IRQHandler(void) {
	static bool tik = false;
	LPC_RTC->ILR = 1;
	if(tik) {
		UART_Send("Tik ");
	} else {
		UART_Send("Tak ");
	}
	tik = !tik;
}

int led_blink_start_time = 0;
volatile uint32_t msTicks = 0;

void EINT0_IRQHandler(void) {
	LPC_SC->EXTINT = 1;
	UART_Send("Click ");
	led_blink_start_time = msTicks;
}

void EINT3_IRQHandler(void) {
	//LPC_SC->EXTINT |= 1<<3;
	LPC_GPIOINT->IO0IntClr = 1 << 19;
	UART_Send("Bang ");
}

void SysTick_Handler(void) {
	msTicks++;
	int t1 = msTicks - led_blink_start_time;
	// 0..4 5..9 .. 39
	if(t1 < 39) {
		int state = t1 / 5 % 2;
		if(state) LED_Off(0);
		else LED_On(0);
	}
}

int main() {
	// Setup
	SysTick_Config(SystemCoreClock / 10);
	
	//mialo byc na priorytetach a jest na systicku :p
	NVIC_SetPriority(TIMER0_IRQn, 7);
	NVIC_SetPriority(RTC_IRQn, 10);
	NVIC_SetPriority(EINT0_IRQn, 9); // button
	NVIC_SetPriority(EINT3_IRQn, 8);
	NVIC_SetPriority(SysTick_IRQn, 11);
	
	UART_Init();
	Ping_Timer_Init();
	RTC_Timer_Init();
	KEY2_Interrupt_Init();
	LCD_Touch_Init();
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	
	// Loop
	while(1) {
		//UART_Send("Ala ma kota, a kot ma Ale. ");
		char buf[20];
		sprintf(buf, "%d", SystemCoreClock);
		UART_Send(buf);
		UART_Char_Send(' ');
		while(!(UART->LSR&1));
		UART_Char_Send(UART->RBR);
		UART_Char_Send(' ');

	}
}

// UART0 : P0.2 P0.3
