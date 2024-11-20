#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <Board_LED.h>
#include <stdio.h>
#include <Open1768_LCD.h>
#include <LCD_ILI9325.h> // include_paths
//#include "..\..\biblioteki\lcd_lib\LCD_ILI9325.h"
#include <asciiLib.h> // include_paths
#include <TP_Open1768.h>
#define UART LPC_UART1

void LCD_Touch_Init() {
	PIN_Configure(0,19,1,0,0);
	LPC_GPIOINT->IO0IntEnF |= 1<<19;
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void) {
	LPC_GPIOINT->IO0IntClr = 1 << 19;
}
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
void UART_int_Send(int x) {
	char buf[100];
	sprintf(buf, "%d", x);
	UART_Send(buf);
}

    //#define ADRX_RAM = RAM address set X
    //#define ADRY_RAM = RAM address set Y
    //#define DATA_RAM = RAM data
		//void lcdWriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue);
    //uint16_t lcdReadReg(uint16_t LCD_Reg);
    //void lcdSetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_Pixel(int x, int y, uint16_t color) {
	lcdWriteReg(ADRX_RAM, x);
	lcdWriteReg(ADRY_RAM, y);
	lcdWriteReg(DATA_RAM, color);
}

void LCD_Display_Init() {
	lcdConfiguration();
	// lcdReadReg(OSCIL_ON); // = 37672 = 0x9328 = ILI9328
	init_ILI9325();
	uint16_t entry_data = lcdReadReg(ENTRYM);
	int AM = 1, ID = 0b11;
	entry_data = (entry_data & (0xFF00 | 0b11000111)) | 5<<3;//AM << 3 | ID << (3+1);
	lcdWriteReg(ENTRYM, entry_data);
	lcdWriteReg(ADRX_RAM, 239);
	lcdWriteReg(ADRY_RAM, 0);
}
// x1 , y1 - wspólrzedne poczatku odcinka
 // x2 , y2 - wspólrzedne konca odcinka
 void LCD_Line_Bresenham(const int x1, const int y1, const int x2, const int y2, const uint16_t color) {
   // zmienne pomocnicze
   int d, dx, dy, ai, bi, xi, yi;
   int x = x1, y = y1;
   // ustalenie kierunku rysowania
   if (x1 < x2) {
     xi = 1;
     dx = x2 - x1;
   } else {
     xi = -1;
     dx = x1 - x2;
   }
   // ustalenie kierunku rysowania
   if (y1 < y2) {
     yi = 1;
     dy = y2 - y1;
   } else {
     yi = -1;
     dy = y1 - y2;
   }
   // pierwszy piksel
   LCD_Pixel(x, y, color);
   // os wiodaca OX
   if (dx > dy) {
     ai = (dy - dx) * 2;
     bi = dy * 2;
     d = bi - dx;
     // petla po kolejnych x
     while (x != x2) {
       // test wspólczynnika
       if (d >= 0) {
         x += xi;
         y += yi;
         d += ai;
       } else {
         d += bi;
         x += xi;
       }
       LCD_Pixel(x, y, color);
     }
   }
   // os wiodaca OY
   else {
     ai = (dx - dy) * 2;
     bi = dx * 2;
     d = bi - dy;
     // petla po kolejnych y
     while (y != y2) {
       // test wspólczynnika
       if (d >= 0) {
         x += xi;
         y += yi;
         d += ai;
       } else {
         d += bi;
         y += yi;
       }
       LCD_Pixel(x, y, color);
     }
   }
 }
 void LCD_Line_Bresenham_yx(const int y1, const int x1, const int y2, const int x2, const uint16_t color) {
	 LCD_Line_Bresenham(x1, y1, x2, y2, color);
 }
 // src: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
void __circleBres___drawCircle(int xc, int yc, int x, int y, int color){
    LCD_Pixel(xc+x, yc+y, color);
    LCD_Pixel(xc-x, yc+y, color);
    LCD_Pixel(xc+x, yc-y, color);
    LCD_Pixel(xc-x, yc-y, color);
    LCD_Pixel(xc+y, yc+x, color);
    LCD_Pixel(xc-y, yc+x, color);
    LCD_Pixel(xc+y, yc-x, color);
    LCD_Pixel(xc-y, yc-x, color);
}

void LCD_Circle_Bresenham(int xc, int yc, int r, uint16_t color){
    int x = 0, y = r;
    int d = 3 - 2 * r;
    __circleBres___drawCircle(xc, yc, x, y, color);
    while (y >= x){
      
        // check for decision parameter
        // and correspondingly 
        // update d, y
        if (d > 0) {
            y--; 
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;

        // Increment x after updating decision parameter
        x++;
        
        // Draw the circle using the new coordinates
        __circleBres___drawCircle(xc, yc, x, y, color);
    }
}

void LCD_Set_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
	lcdWriteReg(HADRPOS_RAM_START, x1);
	lcdWriteReg(HADRPOS_RAM_END, x2);
	lcdWriteReg(VADRPOS_RAM_START, y1);
	lcdWriteReg(VADRPOS_RAM_END, y2);
}

void LCD_Get_Window(uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2) {
	*x1 = lcdReadReg(HADRPOS_RAM_START);
	*x2 = lcdReadReg(HADRPOS_RAM_END);
	*y1 = lcdReadReg(VADRPOS_RAM_START);
	*y2 = lcdReadReg(VADRPOS_RAM_END);
}

void _LCD_Char2(int x, int y, char c, int color) {
	// okno na jeden znak
	
	unsigned char char_buf[16];
	GetASCIICode(ASCII_8X16_System, char_buf, c);
	for(int i = 0; i < 8; ++i) {
		for(int j = 0; j < 16; ++j) {
			int cond = char_buf[j] >> i & 1;
			if(cond)
				LCD_Pixel(239-j, i, LCDWhite);
		}
	}
}

void _LCD_Char(int x, int y, char c, int color) {
	//TODO: fix
	// okno na jeden znak
	LCD_Set_Window(x, y, x+7, y+15);
	lcdSetCursor(x, y);
	unsigned char char_buf[16];
	GetASCIICode(ASCII_8X16_System, char_buf, c);
	lcdWriteIndex(DATA_RAM);
	for(int i = 0; i < 16; i++) {
		for(int j = 0; j < 8; j++) {
			if(char_buf[i] & 1)
				lcdWriteData(color) ;
			else{
				// TODO: skip pixel
			}
			char_buf[i] >>= 1;
		}		
	} 
}
void LCD_Char(int x, int y, char c, int color) {
	//FIXME
	uint16_t h_start, h_end, v_start, v_end;
	LCD_Get_Window(&h_start, &v_start, &h_end, &v_end);
	
	_LCD_Char(x, y, c, color);
	
	LCD_Set_Window(h_start, v_start, h_end, v_end);
}
void LCD_String(int x, int y, const char* s, int color) {
	//FIXME
	// zapis obecnej autoinkrementacji i okna
	uint16_t h_start, h_end, v_start, v_end;
	LCD_Get_Window(&h_start, &v_start, &h_end, &v_end);
	
	// autoinkrementacja
	// jakas logika liczaca wspólrzedne kolejnych znaków
		// _LCD_Char();
	// odtworzenie starej autoinkrementacji i okna
	LCD_Set_Window(h_start, v_start, h_end, v_end);
}

void LCD_Touch_Panel_init() {
	touchpanelInit();
}

void LCD_Uncal_Touch(int * x, int * y) {
	do{
		touchpanelGetXY(x, y);
	} while(*x > 4000);
}

void LCD_Calibrate() {
	// TODO
//black screen
	// krzyzyki
	// while()
	// ocdczytujemy, sprawdzamy, liczyny, odajemy, avg
	
	lcdWriteIndex(DATA_RAM);
	for(int i = 0; i < 320*240; i++) {
		lcdWriteData(LCDBlack);
	}
	
	LCD_Line_Bresenham(20, 20, 40, 40, LCDWhite);
	LCD_Line_Bresenham(20, 40, 40, 20, LCDWhite);
	
	int x, y;
	int sx = 0, sy = 0, n = 20;
	LCD_Uncal_Touch(&x, &y);
	
	for(int i = 0; i < n; i++) {
		LCD_Uncal_Touch(&x, &y);
		sx += x;
		sy += y;
	}
	
	int srx = sx/n, sry = sy/n;
	UART_Send("\r\nx = ");
		UART_int_Send(srx);
		UART_Send(" y = ");
		UART_int_Send(sry);
	
	
	LCD_Line_Bresenham(240 - 20, 320 - 20, 240 - 40, 320 - 40, LCDWhite);
	LCD_Line_Bresenham(240 - 40, 320 - 20, 240 - 20, 320 - 40, LCDWhite);
	
	
	
}

int main() {
	// Setup
	SysTick_Config(SystemCoreClock / 1000);
	
	
	
	LCD_Touch_Init();
	LCD_Touch_Panel_init();
	LED_Initialize();
	for(int i = 0; i < LED_GetCount(); ++i)
		LED_Off(i);
	UART_Init();
	LCD_Display_Init();
	int w = 320/3;
	int h = 240;
	lcdWriteIndex(DATA_RAM);
	for(int j = 0; j < h; ++j) {
		for(int i = 0; i < w+1; ++i) lcdWriteData(LCDRed);
		for(int i = 0; i < w; ++i) lcdWriteData(LCDGreen);
		for(int i = 0; i < w+1; ++i) lcdWriteData(LCDBlue);
	}
	LCD_Line_Bresenham_yx(30, 119, 64, 188, LCDWhite);
	LCD_Line_Bresenham_yx(64, 188, 134, 188, LCDWhite);
	LCD_Line_Bresenham_yx(134, 188, 169, 119, LCDWhite);
	LCD_Line_Bresenham_yx(169, 119, 134, 39, LCDWhite);
	LCD_Line_Bresenham_yx(169, 119, 134, 39, LCDWhite);
	LCD_Line_Bresenham_yx(134, 39, 64, 39, LCDWhite);
	LCD_Line_Bresenham_yx(64, 39, 30, 119, LCDWhite);
	LCD_Circle_Bresenham(115, 100, 80, LCDBlueSea);
	LCD_Char(0,0,'B', LCDWhite);

	//lcdWriteReg(HADRPOS_RAM_START, 0);
	//lcdWriteReg(HADRPOS_RAM_END, 1);
	//lcdWriteReg(VADRPOS_RAM_START, 0);
	//lcdWriteReg(VADRPOS_RAM_END, 1);
	//lcdWriteIndex(DATA_RAM);
	
	int x, y;
	LCD_Calibrate();
	
	// Loop
	while(1) {
		touchpanelGetXY(&x, &y);		
	}
}

