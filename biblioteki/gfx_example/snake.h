#ifndef __Snake_H
#define __Snake_H

#include "Open1768_LCD.h"
#include "LCD_ILI9325.h"
#include "TP_Open1768.h"

#include "lcdDrawing.h"
#include "lcdTouch.h"
#include "sound.h"
#include "win.h"
#include "lose.h"

// Cell:     Apple:
// #########         /|\ /|\ Apple offset
// #########          |  \|/
// #########  #####   |
// #########  #####   |
// #########  #####   | Cell size
// #########  #####   |
// #########  #####   |
// #########          |
// #########         \|/
#define CELL_SIZE 10
#define APPLE_OFFSET 1
#define GRID_SIZE_X (LCD_MAX_X / CELL_SIZE)
#define GRID_SIZE_Y (LCD_MAX_Y / CELL_SIZE)
#define SAMPLE_SIZE_INGAME 8

enum MovementDirection {LEFT, RIGHT, UP, DOWN};
int snake_head_x = (LCD_MAX_X / 2) / CELL_SIZE;
int snake_head_y = (LCD_MAX_Y / 2) / CELL_SIZE;
int snake_size = 5;
short grid[GRID_SIZE_X][GRID_SIZE_Y];
int areYouWinningSon = -1;
int isAppleOnTheScreen = 0;
int appleX, appleY;
int touch_x, touch_y;
enum MovementDirection direction = LEFT;
uint64_t ra = 1664525;
uint64_t rx;
uint64_t rc = 1013904223;
uint64_t rm = 4294967296;

int rand(int max){
	uint64_t r = (ra * rx + rc) % rm;
	rx = r;
	return r % max;
}

// Convert LCD coords to Snake coords
int lcd_to_snake(int x)
{
	return x / CELL_SIZE;
}

// Convert Snake coords to LCD coords
int snake_to_lcd(int x)
{
	return x * CELL_SIZE;
}

// Draw a body part of Snake from Snake coords
void draw_snake_part(int x, int y, int color)
{
	draw_rectangle(snake_to_lcd(x), snake_to_lcd(y), CELL_SIZE, CELL_SIZE, color);
}

// Draw an apple from Snake coords
void draw_apple(int seed1, int seed2)
{
    if (!isAppleOnTheScreen)
    {
        appleX = rand(GRID_SIZE_X);
        appleY = rand(GRID_SIZE_Y);
        while (grid[appleX][appleY] > 0){
            seed1 += 17;
            seed2 += 3;
            appleX = rand(GRID_SIZE_X);
            appleY = rand(GRID_SIZE_Y);
        }
				
        grid[appleX][appleY] = -1;
        isAppleOnTheScreen = 1;
    }
	draw_rectangle(snake_to_lcd(appleX) + APPLE_OFFSET, snake_to_lcd(appleY) + APPLE_OFFSET, CELL_SIZE - 2 * APPLE_OFFSET, CELL_SIZE - 2 * APPLE_OFFSET, LCDRed);
}

void config_Timer()
{
	LPC_TIM0->PR = 0;
	LPC_TIM0->MR0 = SystemCoreClock / 16;
	LPC_TIM0->MCR = 3;
	LPC_TIM0->TCR = 1;
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void move_snake(enum MovementDirection direction)
{
	int target_x;
	int target_y;
	switch (direction)
	{
			case LEFT:
					target_x = snake_head_x - 1;
					target_y = snake_head_y;
					break;
			case RIGHT:
					target_x = snake_head_x + 1;
					target_y = snake_head_y;
					break;
			case UP:
					target_x = snake_head_x;
					target_y = snake_head_y + 1;
					break;
			case DOWN:
					target_x = snake_head_x;
					target_y = snake_head_y - 1;
					break;
	}
	switch (target_x)
	{
		case -1:
		{
			target_x = GRID_SIZE_X - 1;
			break;
		}
		case GRID_SIZE_X:
		{
			target_x = 0;
			break;
		}
		default:
			break;
	}

	switch (target_y)
	{
		case -1:
		{
			target_y = GRID_SIZE_Y - 1;
			break;
		}
		case GRID_SIZE_Y:
		{
			target_y = 0;
			break;
		}
		default:
			break;
	}

	snake_head_x = target_x;
	snake_head_y = target_y;

	if (grid[snake_head_x][snake_head_y] > 0){
			play_sound(lose, LOSE_LEN);
			areYouWinningSon = 0;
	}



	if (grid[snake_head_x][snake_head_y] == -1)
	{
			snake_size += 2;
			isAppleOnTheScreen = false;
			play_sound (win, WIN_LEN);
	}
	grid[target_x][target_y] = snake_size;
	{
			for (int i = 0; i < GRID_SIZE_X; i++)
					for (int j = 0; j < GRID_SIZE_Y; j++)
					{
							if (grid[i][j] == 1)
									draw_snake_part(i, j, LCDBlack);
							if (grid[i][j] > 0)
									grid[i][j] -= 1;
					}
	}

	for (int i = 0; i < GRID_SIZE_X; i++)
	{
			for (int j = 0; j < GRID_SIZE_Y; j++)
			{
					if (grid[i][j] > 0)
							draw_snake_part(i, j, LCDWhite);				
			}
	}
	
	char score_text[30];
	sprintf(score_text, "Score: %d", (snake_size - 5) / 2);
	draw_string(0, 0, score_text, 0);
}

void play()
{
		int x;
		int y;
		int sample_x, sample_y;
		x = 0;
		for (int i = 0; i < SAMPLE_SIZE_INGAME; i++)
		{
			touchpanelGetXY(&sample_x, &sample_y);
			x += sample_x;
			y += sample_y;
		}
		x /= SAMPLE_SIZE_INGAME;
		y /= SAMPLE_SIZE_INGAME;
		if (x > 20 && x < 3980)
		{
			touch_x = x;
			
			switch (direction){
				case LEFT : 
				{
					if (touch_x <= 2000)
						direction = UP;
					else
						direction = DOWN;
					break;
				}
				case RIGHT :
				{
					if (touch_x <= 2000)
						direction = DOWN;
					else
						direction = UP;
					break;
				}
				case UP :
				{
					if (touch_x <= 2000)
						direction = RIGHT;
					else
						direction = LEFT;
					break;
				}
				case DOWN :
				{
					if (touch_x <= 2000)
						direction = LEFT;
					else
						direction = RIGHT;
					break;
				}
			}
		}
		move_snake (direction);
		draw_apple (x, y);
		
		LPC_TIM0->IR = 1;
}

void loseScreen()
{
	fill_background (LCDRed);
	int x;
	int y;
	wait_for_touch(&x, &y);
	if (x > 20 && x < 3980 && y > 20 && y < 3980)
	{
		areYouWinningSon = -1;
		fill_background (LCDBlack);
	}
}

void restartGame()
{
	direction = LEFT;
	snake_size = 5;
	areYouWinningSon = 1;
	for (int i = 0; i < GRID_SIZE_X; i++)
		for (int j = 0; j < GRID_SIZE_Y; j++)
			grid[i][j] = 0;
	snake_head_x = (LCD_MAX_X / 2) / CELL_SIZE;
	snake_head_y = (LCD_MAX_Y / 2) / CELL_SIZE;
	isAppleOnTheScreen = 0;
}

void TIMER0_IRQHandler(void)
{
	if (areYouWinningSon == 1)
	{
		play ();
	}
	if (areYouWinningSon == 0)
	{
		loseScreen();
	}
	if (areYouWinningSon == -1)
	{
		restartGame();
	}
}

#endif
