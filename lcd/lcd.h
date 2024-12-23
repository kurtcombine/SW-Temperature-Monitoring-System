#include "../global/global.h"
#include "asciiLib.h"

#define LCD_SCREEN_WIDTH 320
#define LCD_SCREEN_HEIGHT 240

#define LCDWhite 0xFFFF
#define LCDBlack 0x0000
#define LCDGrey 0xa534
#define LCDBlue 0x001F
#define LCDBlueSea 0x05BF
#define LCDPastelBlue 0x051F
#define LCDViolet 0xB81F
#define LCDMagenta 0xF81F
#define LCDRed 0xF800
#define LCDGinger 0xFAE0
#define LCDGreen 0x07E0
#define LCDCyan 0x7FFF
#define LCDYellow 0xFFE0

void LCD_setup();

void LCD_Background(uint16_t c);
void LCD_Pixel(int x, int y, uint16_t c);
void LCD_Line(int x1, int y1, int x2, int y2, uint16_t color);

void LCD_Char(int x, int y, char c, int color);
void LCD_Text(int x, int y, const char *s, int color);
void LCD_Addr(int x, int y, const uint8_t addr[8], int color);

void OnMockDestroyWindow();
