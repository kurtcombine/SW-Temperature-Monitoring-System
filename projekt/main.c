#include <LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <Board_LED.h>
#include <stdio.h>
#define UART LPC_UART1

void UART1_Init() {
	PIN_Configure(0,15,1,0,0);
	PIN_Configure(0,16,1,0,0);
	UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
	UART->DLL = 163; // bitrate=9600
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
void UART_Send_Hex_Char(char x) {
	char buf[10];
	sprintf(buf, "%02hhx", x);
	UART_Send(buf);
}



int led_blink_start_time = 0;
volatile uint32_t us10Ticks = 0; // t [10us = 1e-5 * s]

void SysTick_Handler(void) {
	us10Ticks++;
	int t1 = us10Ticks - led_blink_start_time;
	// stan wysoki = 10us
	if(t1 % 2 == 0) {
		LED_On(0);
	}else{
		LED_Off(0);
	}
}
#define MAGIC_CONSTANT 15 // zawsze daje wiecej niz powinno
void delayUs(int us) {
	if(us<10) {
		for(int i = 0; i < us * MAGIC_CONSTANT; i++);
		return;
	}
	int t1 = us10Ticks + us / 10;
	while(t1 >= us10Ticks);
}

void onewire_init_ports() {
	// onewire na plytce
	// p1.26
	PIN_Configure(1,26,0,0,1); // pull-up and open drain
	// PIN_Configure(1,26,0,2,1); // no internal pull-up and open drain
}
bool onewire_start_transmission() {
	GPIO_PinWrite(1, 26, 0);
	delayUs(480); // t_RSTL
	GPIO_PinWrite(1, 26, 1);
	delayUs(60+30); // t_PDHIGH + t_PDLOW/2
	int presence = GPIO_PinRead(1, 26);
	if(presence != 0) return 0;
	delayUs(30); // t_PDLOW/2
	while(GPIO_PinRead(1, 26) == 0); // nadwyzka t_PDLOW
	
	delayUs(27); // t_REC
	return true;
}

void onewire_send_zero() {
	// assert default 1
	GPIO_PinWrite(1, 26, 0);
	delayUs(60); // t_LOW0
	GPIO_PinWrite(1, 26, 1);
	delayUs(27); // t_REC

}
void onewire_send_one() {
	// assert default 1
	GPIO_PinWrite(1, 26, 0);
	delayUs(5); // t_LOW1
	GPIO_PinWrite(1, 26, 1);
	delayUs(55+1); // reszta t_SLOT + t_REC
}
inline bool onewire_read_bit() {
	// assert default 1
	GPIO_PinWrite(1, 26, 0);
	delayUs(10); // t_RDV
	GPIO_PinWrite(1, 26, 1);
	delayUs(30); // (reszta t_SLOT) / 2
	bool bit = GPIO_PinRead(1, 26);
	delayUs(20); // (reszta t_SLOT) / 2
	return bit;
}
char onewire_read_byte() {
	// assert default 1
	char byte = 0;
	for(int i = 0; i < 8; i++) {
		byte >>= 1;
		byte |= onewire_read_bit() << 7;
	}
	return byte;
}
void onewire_send_byte(char x) { // wysylamy 8 bitow
	// assert default 1
	for(int i = 0; i < 8; i++) {
		if(x & (1 << 7))
			onewire_send_one();
		else onewire_send_zero();
		x <<= 1;
	}
}




void onewire_read_rom() { // musi byc podlaczony tylko jeden uklad by zadzialalo
	// assert default 1
	onewire_send_byte(0x33);
	char buf[8];
	for(int i = 0; i < 1; i++) {
		buf[i] = onewire_read_byte();
	}
	/*for(int i = 0; i < 8; i++) {
		UART_Send_Hex_Char(buf[i]);
	}
	UART_Send("\r\n");*/
	
}

void onewire_test() {
	onewire_init_ports();
	while(1){
		delayUs(1000);
		bool ok = onewire_start_transmission();
		if(!ok) {
			UART_Send("ERR: impuls obecnosci nie wystapil\r\n");
			continue;
		}
		onewire_read_rom();
	}
}



int main() {
	// Setup
	SysTick_Config(SystemCoreClock / 100000);
	
	UART1_Init();
	UART_Send(" UART(init)\r\n");
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	
	
	UART_Send("init completed.\r\n");
	onewire_test();
	UART_Send("onewire test completed.\r\n");
	while(1) {
	}
}
