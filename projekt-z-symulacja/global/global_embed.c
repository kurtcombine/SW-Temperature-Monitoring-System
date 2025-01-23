#include "global.h"
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
}

void global_setup() {
    SysTick_Config(SystemCoreClock / 1000000);  // SystemCoreClock changes accordingly to clock divisor CCLKCFG
    UART0_Init();
}

volatile int usTicks = 0;
void SysTick_Handler(void) {
    usTicks++;
}

void delay_us(int us) {
    int start = usTicks;
    while(start + us < usTicks);  // wait for int overflow
    while(usTicks < start + us);
}
void delay_ms(int ms) {
    delay_us(ms * 1000);
}

void DEBUG(uint8_t *str) {
    UART_Send(str);
}

uint8_t buf[24];
void DEBUG_addr(const uint8_t addr[8]) {
    if(addr == NULL) {
        DEBUG("NULL");
        return;
    }
    snprintf(buf, 24, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6],
        addr[7]);
    DEBUG(buf);
}
void DEBUG_int(int x) {
    snprintf(buf, 12, "%d", x);
    DEBUG(buf);
}
void DEBUG_float(double x) {
    snprintf(buf, 24, "%f", x);
    DEBUG(buf);
}
void DEBUG_halt() {
    while(1);
}
