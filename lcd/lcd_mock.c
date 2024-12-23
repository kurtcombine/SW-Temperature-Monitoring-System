#ifdef MOCKED_EMBEDDED
#include "lcd.h"
#include <SDL2/SDL.h>

#define PIXEL_SCALE 1

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    SDL_Rect canvas;
} LcdSimulator;

LcdSimulator lcd_sim;
void ___LCD_feed();
void ___LCD_exit();

void LCD_setup() {
    lcd_sim.canvas.w = lcd_sim.canvas.h = PIXEL_SCALE;
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    lcd_sim.window = SDL_CreateWindow("SDL Canvas Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LCD_SCREEN_WIDTH * PIXEL_SCALE, LCD_SCREEN_HEIGHT * PIXEL_SCALE, SDL_WINDOW_SHOWN);
    if(!lcd_sim.window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    lcd_sim.renderer = SDL_CreateRenderer(lcd_sim.window, -1, SDL_RENDERER_ACCELERATED);

    if(!lcd_sim.renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(lcd_sim.window);
        SDL_Quit();
        exit(1);
    }
    ___LCD_feed();
}
void ___LCD_feed() {
    while(SDL_PollEvent(&lcd_sim.event))
        if(lcd_sim.event.type == SDL_QUIT) ___LCD_exit();
}

void ___LCD_exit() {
    OnMockDestroyWindow();
    SDL_DestroyRenderer(lcd_sim.renderer);
    SDL_DestroyWindow(lcd_sim.window);
    SDL_Quit();
    exit(1);
}

void ___LCD_set_color(uint16_t c) {
    int r = (c & 0xF800) >> 8;  // >> (11+5-8);
    int g = (c & 0x07E0) >> 3;  // >> (5+6-8);
    int b = (c & 0x001F) << 3;  // >> (0+5-8);
    if(r & 8) r |= 7;
    if(g & 4) g |= 3;
    if(b & 8) b |= 7;
    SDL_SetRenderDrawColor(lcd_sim.renderer, r, g, b, 255);
}

void LCD_Background(uint16_t c) {
    ___LCD_feed();
    ___LCD_set_color(c);
    SDL_RenderPresent(lcd_sim.renderer);
    SDL_RenderClear(lcd_sim.renderer);
}

void LCD_Pixel(int x, int y, uint16_t c) {
    ___LCD_feed();
    ___LCD_set_color(c);
    bool f = true;
    lcd_sim.canvas.x = x * PIXEL_SCALE;
    lcd_sim.canvas.y = y * PIXEL_SCALE;
    SDL_RenderFillRect(lcd_sim.renderer, &lcd_sim.canvas);
}
#endif
