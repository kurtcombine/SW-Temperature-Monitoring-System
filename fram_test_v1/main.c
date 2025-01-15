#include <LPC17xx.h>
#include <stdlib.h>
#include "eeprom_i2c.h"

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
void UART_Char_Send(uint8_t c) {
    while(!(UART->LSR & 1 << 5));  // wait for THRE flag (5th bit of LSR reg) to reset
    UART->THR = c;
}
void UART_Send(const uint8_t *str) {
    for(int i = 0; str[i]; i++) UART_Char_Send(str[i]);
}void DEBUG(uint8_t *str) {
    UART_Send(str);
}
void DEBUG_int(int x) {
    uint8_t buf[14];
    snprintf(buf, 14, "%d\r\n", x);
    DEBUG(buf);
}
void DEBUG_float(double x) {
    uint8_t buf[24];
    snprintf(buf, 24, "%f", x);
    DEBUG(buf);
}
void DEBUG_addr(const uint8_t addr[8]) {
    uint8_t buf[26];
    snprintf(buf, 26, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6],
        addr[7]);
    UART_Send(buf);
}
char buf[0x7ff];
int main() {
	UART0_Init();
	UART_Send("xd\r\n");
	EEPROM_Initialize(false);
	UART_Send("xd2\r\n");
	if(1){
	for(int i = 0; i < 0x7ff; i++) buf[i] = i % 44;
	//char xd[] = "abcd";
	//memcpy(buf, xd, sizeof(xd));
	UART_Send("xd3\r\n");
	DEBUG_int(EEPROM_Write_Event(0b0, buf, 0x7ff));
	for(int i = 0; i < 0x7ff; i++) buf[i] = 0;
	UART_Send("xd4\r\n");
	}
	
	for(int i =0; true; i++){
	DEBUG_int		(EEPROM_Read_Event(i, buf, 8));		DEBUG_addr(buf);
	}
	UART_Send("xd5\r\n");
	while(1){}
	
	while(0) {
		DEBUG_int
		(EEPROM_Read_Event(0b0, buf, 128));
		DEBUG_addr(buf);
		buf[0] = 0xde;
		buf[1] = 0xad;
		DEBUG_int
		(EEPROM_Write_Event(0b0, buf, 64));
		DEBUG_addr(buf);DEBUG_int
		(EEPROM_Read_Event(0b0, buf, 128));
		DEBUG_addr(buf);
		for(int i = 0; i < 3e4; ++i);
	}
}