#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <stdio.h>
#define UART LPC_UART0

void UART_Init() {
	PIN_Configure(0,2,1,0,0);
	PIN_Configure(0,3,1,0,0);

	UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
	//DLAB = 1
	//Multiplier values for 115,5k baud rade
	if(0){
	UART->DLL = 10;
	UART->DLM = 0;
	
	UART->FDR = 5 | 14<<4; //DivAdd = 5, Mul = 14
	}else{
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

int main() {
	// Setup
	UART_Init();
	
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
