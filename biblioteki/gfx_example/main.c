#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include "TP_Open1768.h"

#include "lcdDrawing.h"
#include "lcdTouch.h"
#include "sound.h"
#include "win.h"
#include "snake.h"

int main()
{
	config_GPIOINT();
	config_sound();
	lcdConfiguration();
	init_ILI9325();
	touchpanelInit();
	rx = calibrate();	
	config_Timer();
	while(1);
}
