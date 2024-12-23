#ifdef GCC_COMPILE_TEST
#include "LPC17xx.h"
LPC_UART0_TypeDef *LPC_UART0;
LPC_UART1_TypeDef *LPC_UART1;
int SysTick_Config(int) {}
void PIN_Configure(int, int, int, int, int) {}
void GPIO_PinWrite(int, int, int) {}
int GPIO_PinRead(int, int) {}
void LED_On(int led) {}
void LED_Off(int led) {}
void LED_Initialize() {}
int LED_GetCount() {}
void init_ILI9325(void) {}
void lcdConfiguration(void) {}
void lcdSend(uint16_t byte) {}
void wait_delay(int count) {}
uint16_t lcdRead(void) {}
void lcdWriteIndex(uint16_t index) {}
void lcdWriteData(uint16_t data) {}
uint16_t lcdReadData(void) {}
void lcdWriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue) {}
uint16_t lcdReadReg(uint16_t LCD_Reg) {}
void lcdSetCursor(uint16_t Xpos, uint16_t Ypos) {}
#endif
