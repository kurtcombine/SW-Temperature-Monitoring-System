#ifdef GCC_COMPILE_TEST
#ifndef __LPC17xx_H
#define __LPC17xx_H
#include "__typedefs.h"
#include <stdbool.h>
#define SystemCoreClock 1000000
extern LPC_UART0_TypeDef *LPC_UART0;
extern LPC_UART1_TypeDef *LPC_UART1;
int SysTick_Config(int);
void PIN_Configure(int, int, int, int, int);
void GPIO_PinWrite(int, int, int);
int GPIO_PinRead(int, int);
void LED_On(int led);
void LED_Off(int led);
void LED_Initialize();
int LED_GetCount();
#endif
#endif
