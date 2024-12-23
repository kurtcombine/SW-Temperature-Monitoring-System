#include "lcd/lcd.h"

void OnMockDestroyWindow() {}

int main(int argc, char *argv[]) {
    global_setup();
    LCD_setup();
    LCD_Background(LCDBlue);
    int j = 0;
    for(int j = 0; j < LCD_SCREEN_HEIGHT; j++) {
        for(int i = 0; i < LCD_SCREEN_WIDTH; i++) { LCD_Pixel(i, j, LCDWhite); }
        if(j % 10 == 9) LCD_Background(LCDBlue);
    }

    while(true) {
        LCD_Background(LCDBlue);

        LCD_Char(50, 50, 'Q', LCDWhite);
        LCD_Text(50, 66, "QWERTY", LCDWhite);
        LCD_Line(10, 10, 310, 10, LCDGinger);
        LCD_Line(10, 10, 10, 230, LCDMagenta);
    }
    return 0;
}
