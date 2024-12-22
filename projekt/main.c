
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#define UART LPC_UART0
void UART0_Init() {
    PIN_Configure(0, 2, 1, 0, 0);
    PIN_Configure(0, 3, 1, 0, 0);
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
    UART->DLL = 163;  // bitrate=9600
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b00000111;
}
void UART_Char_Send(char c) {
    while(!(UART->LSR & 1 << 5));  // wait for THRE flag (5th bit of LSR reg) to reset
    UART->THR = c;
}
void UART_Send(const char *str) {
    for(int i = 0; str[i]; i++) UART_Char_Send(str[i]);
}

void DEBUG(char *str) {
    UART_Send(str);
}

int main() {
	UART0_Init();
	UART_Send("dziala\r\n");
	
	// FRAM: odczyt danych, override i ponowny zapis (bo za malo blokow), FM24CL16B podobno
	// RTC: odczyt i zapis aktualnego czasu
	
	while(1){}
}
