#include "lcd.h"

#ifndef MOCKED_EMBEDDED
#include "../embedded_libraries/Open1768_LCD.h"
#include "../embedded_libraries/LCD_ILI9325.h"

void LCD_setup() {
    lcdConfiguration();
    // lcdReadReg(OSCIL_ON); // = 37672 = 0x9328 = ILI9328
    init_ILI9325();
    uint16_t entry_data = lcdReadReg(ENTRYM);
    // int AM = 1, ID = 0b11;
    entry_data = (entry_data & (0xFF00 | 0b11000111)) | 5 << 3;  // AM << 3 | ID << (3+1);
    lcdWriteReg(ENTRYM, entry_data);
    LCD_Background(LCDBlack);
}

void LCD_Background(uint16_t color) {
    int w = 320;
    int h = 240;
    lcdWriteIndex(DATA_RAM);
    for(int i = 0; i < w * h; i++) { lcdWriteData(color); }
}

void LCD_Pixel(int x, int y, uint16_t color) {
    lcdWriteReg(ADRX_RAM, 239 - y);
    lcdWriteReg(ADRY_RAM, x);
    lcdWriteReg(DATA_RAM, color);
}

#endif

void LCD_Char(int x, int y, char c, int color) {
    unsigned char char_buf[16];
    GetASCIICode(char_buf, c);
    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 16; ++j) {
            int cond = char_buf[j] >> i & 1;
            if(cond) LCD_Pixel(8 - i + x, j + y - 2, LCDWhite);
        }
    }
}

void LCD_Text(int x, int y, const char *s, int color) {
    for(int i = 0; s[i]; i++) LCD_Char(x + i * 8, y, s[i], color);
}

void LCD_Addr(int x, int y, const uint8_t addr[8], int color) {
    char buf[24];
    snprintf(
        buf, 24, "%02X%02X%02X%02X%02X%02X%02X%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
    for(int i = 0;; ++i) {
        LCD_Char(x, y, buf[2 * i], color);
        LCD_Char(x + 8, y, buf[2 * i + 1], color);
        if(i == 7) break;
        LCD_Char(x + 14, y, ':', color);
        x += 20;
    }
}

void LCD_Line(int x1, int y1, int x2, int y2, uint16_t color) {
    if(x1 == x2) {
        int d = y1 < y2 ? 1 : -1;
        for(; y1 != y2; y1 += d) { LCD_Pixel(x1, y1, color); }
    } else if(y1 == y2) {
        int d = x1 < x2 ? 1 : -1;
        for(; x1 != x2; x1 += d) { LCD_Pixel(x1, y1, color); }
    } else {
        while(1) DEBUG("invalid line coords ");
    }
}

#ifdef MOCKED_EMBEDDED
#include "lcd_mock.c"
#endif
