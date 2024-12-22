#ifndef __LcdTouch_H
#define __LcdTouch_H

#include "Open1768_LCD.h"
#include "TP_Open1768.h"
#include "lcdDrawing.h"
#include "stdbool.h"

#define SAMPLE_SIZE 5

volatile bool touched = false;
volatile int last_touch_x = 0;
volatile int last_touch_y = 0;
int x1, x2, y1, y2;

void wait_for_touch(int *x, int *y)
{
	while(1)
	{
		if (touched)
		{
			*x = last_touch_x;
			*y = last_touch_y;
			touched = false;
			break;
		}
	}
}

void read_touch(int *x, int *y)
{
	int sample_x, sample_y;
	*x = 0;
	*y = 0;
	for (int i = 0; i < SAMPLE_SIZE; i++)
	{
		touchpanelGetXY(&sample_x, &sample_y);
		*x += sample_x;
		*y += sample_y;
	}
	*x /= SAMPLE_SIZE;
	*y /= SAMPLE_SIZE;
}

void config_GPIOINT()
{
	LPC_GPIOINT->IO0IntEnF |= (1 << 19);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	int x, y;
	read_touch(&x, &y);
	if (!(x > 4000 || y < 100))
	{
		last_touch_x = x;
		last_touch_y = y;
		touched = true;
	}
	LPC_GPIOINT->IO0IntClr |= (1 << 19);
}

uint64_t calibrate()
{
	int discard_x, discard_y;
	
	fill_background(LCDBlack);
	draw_cross(40, LCD_MAX_Y - 40, LCDWhite);
	draw_string(104, 16, "Please touch the crosses", 1);
	draw_string(120, 16, "to calibrate the screen", 1);
	wait_for_touch(&discard_x, &x1);
	
	fill_background(LCDBlack);
	draw_cross(LCD_MAX_X - 40, LCD_MAX_Y - 40, LCDWhite);
	wait_for_touch(&y1, &x2);
	
	fill_background(LCDBlack);
	draw_cross(LCD_MAX_X - 40, 40, LCDWhite);
	wait_for_touch(&y2, &discard_y);
	
	fill_background(LCDBlack);
	
	return discard_x * x1 + y1 * x2 + y2 * discard_y;
}

void touch_to_lcd(int *x, int *y)
{
	*x = (80 - LCD_MAX_X) * *x / (x1 - x2) + 40 - x1 * (80 - LCD_MAX_X) / (x1 - x2);
	*y = (80 - LCD_MAX_Y) * *y / (y1 - y2) + 40 - y1 * (80 - LCD_MAX_Y) / (y1 - y2);
}

#endif
