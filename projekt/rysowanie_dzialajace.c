#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <Board_LED.h>
#include <stdio.h>
#include <Open1768_LCD.h>
#include <LCD_ILI9325.h> // include_paths
//#include "..\..\biblioteki\lcd_lib\LCD_ILI9325.h"
#include <asciiLib.h> // include_paths
#include <TP_Open1768.h>
#define UART LPC_UART0

int msTicks = 0;
void SysTick_Handler(void) {
	msTicks++;
	if(msTicks/1000 % 2 == 0)
		LED_On(1);
	else
		LED_Off(1);
}


void UART_Init() {
	//LPC_SC->PCONP |= 1<<24;
	
	// UART0 : P0.2 P0.3
	// UART1: P0.15, P0.16 <- sprrawdzic luty na plytce
	// UART2: P0.10, P0.11 <- sprrawdzic luty na plytce
	PIN_Configure(0,2,1,0,0);
	PIN_Configure(0,3,1,0,0);

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
void DEBUG(const char* str) {
	UART_Send(str);
}

void UART_int_Send(int x) {
	char buf[100];
	sprintf(buf, "%d", x);
	UART_Send(buf);
}

void LCD_Pixel(int x, int y, uint16_t color) {
	lcdWriteReg(ADRX_RAM, 239-y);
	lcdWriteReg(ADRY_RAM, x);
	lcdWriteReg(DATA_RAM, color);
}

void LCD_Background(uint16_t color) {
	int w = 320;
	int h = 240;
	lcdWriteIndex(DATA_RAM);
	for(int i = 0; i < w*h; i++) {
		lcdWriteData(color);
	}
}

void LCD_Display_Init() {
	lcdConfiguration();
	// lcdReadReg(OSCIL_ON); // = 37672 = 0x9328 = ILI9328
	init_ILI9325();
	uint16_t entry_data = lcdReadReg(ENTRYM);
	//int AM = 1, ID = 0b11;
	entry_data = (entry_data & (0xFF00 | 0b11000111)) | 5<<3;//AM << 3 | ID << (3+1);
	lcdWriteReg(ENTRYM, entry_data);
	LCD_Background(LCDBlack);
}



void LCD_Char(int x, int y, char c, int color) {	
	unsigned char char_buf[16];
	GetASCIICode(ASCII_8X16_System, char_buf, c);
	for(int i = 0; i < 8; ++i) {
		for(int j = 0; j < 16; ++j) {
			int cond = char_buf[j] >> i & 1;
			if(cond)
				LCD_Pixel(8-i+x, j+y, LCDWhite);
		}
	}
}
void LCD_String(int x, int y, const char* s, int color) {
	for(int i = 0; s[i]; i++)
		LCD_Char(x+i*8, y, s[i], color);
}

void LCD_Line(int x1, int y1, int x2, int y2, uint16_t color) {
	if(x1 == x2) {
		int d = y1 < y2 ? 1 : -1;
		for(; y1 != y2; y1 += d) {
			LCD_Pixel(x1,y1, color);
		}		
	} else if (y1 == y2) {
		int d = x1 < x2 ? 1 : -1;
		for(; x1 != x2; x1 += d) {
			LCD_Pixel(x1,y1, color);
		}		
	}	else {
		while(1)
			DEBUG("invalid line coords ");
	}
}


int main() {
	// Setup
	SysTick_Config(SystemCoreClock / 1000);
	// 320 x 240
	

	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	UART_Init();
	LCD_Display_Init();
	LCD_Char(50,50,'Q', LCDWhite);
	LCD_String(50,66,"QWERTY", LCDWhite);
	LCD_Line(10, 10, 310, 10, LCDGinger);
	LCD_Line(10, 10, 10, 240, LCDMagenta);
	
	
	// Loop
	while(1) {
	
	}
}

