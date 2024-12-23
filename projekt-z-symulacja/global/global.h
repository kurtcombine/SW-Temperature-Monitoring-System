#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef GLOBAL_H
#define GLOBAL_H

void global_setup();

void delay_us(int us);
void delay_ms(int ms);

void DEBUG(uint8_t *str);
void DEBUG_addr(const uint8_t addr[8]);
void DEBUG_int(int x);
void DEBUG_float(double x);
void DEBUG_halt();

#endif
