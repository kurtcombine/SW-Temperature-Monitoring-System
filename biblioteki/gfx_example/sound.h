#ifndef __Sound_H
#define __Sound_H

#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"                // Keil::Device:PIN

void config_sound(void);
void play_sound(const uint16_t sound[], int sound_len);
void play_long_sound(const uint16_t sound[], int sound_len);
#endif
