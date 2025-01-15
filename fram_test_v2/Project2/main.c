//#include "lpc17xx_i2c.h"
//#include "lpc17xx_clkpwr.h"
//#include "lpc17xx_pinsel.h"
//#include "at24xx.h"

#include <stdlib.h>
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"

#define UART LPC_UART0
void UART0_Init() {
    PIN_Configure(0, 2, 1, 0, 0);
    PIN_Configure(0, 3, 1, 0, 0);
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
    UART->DLL = 163;  // bitrate=9600
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b00000111;
}
void UART_Char_Send(uint8_t c) {
    while(!(UART->LSR & 1 << 5));  // wait for THRE flag (5th bit of LSR reg) to reset
    UART->THR = c;
}
void UART_Send(const uint8_t *str) {
    for(int i = 0; str[i]; i++) UART_Char_Send(str[i]);
}

void global_setup() {
    SysTick_Config(SystemCoreClock / 1000000);  // SystemCoreClock changes accordingly to clock divisor CCLKCFG
    UART0_Init();
} 

volatile int usTicks = 0;
void SysTick_Handler(void) {
    usTicks++;
}

void delay_us(int us) {
    int start = usTicks;
    while(start + us < usTicks);  // wait for int overflow
    while(usTicks < start + us);
}
void delay_ms(int ms) {
    delay_us(ms * 1000);
}

void DEBUGa(uint8_t *str) {
    UART_Send(str);
}
void DEBUGa_addr(const uint8_t addr[8]) {
    if(addr == NULL) {
        DEBUGa("NULL");
        return;
    }
    uint8_t buf[24];
    snprintf(buf, 24, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6],
        addr[7]);
    DEBUGa(buf);
}
void DEBUGa_int(int x) {
    uint8_t buf[12];
    snprintf(buf, 12, "%d", x);
    DEBUGa(buf);
}
void DEBUGa_float(double x) {
    uint8_t buf[24];
    snprintf(buf, 24, "%f", x);
    DEBUGa(buf);
}
void DEBUGa_halt() {
    while(1);
}

/*int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t tmp;
	uint8_t *dp, *sp;

	//debug_frmwrk_init();
	DEBUGa("AT24Cxx test procedures:\r\n");
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
#if (USEDI2CDEV == 0)
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;	//P0.27 SDA0
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;	//P0.28 SCL0
	PINSEL_ConfigPin(&PinCfg);	

#elif (USEDI2CDEV == 1)
	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;	//P0.0 SDA1
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;	//P0.1 SCL1
	PINSEL_ConfigPin(&PinCfg);	   
#elif (USEDI2CDEV == 2)
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;	//P0.10 SDA2
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;	//P0.11 SCL2
	PINSEL_ConfigPin(&PinCfg);
#endif

	 I2C block ------------------------------------------------------------------- 
	// Initialize I2C peripheral
	I2C_Init(I2CDEV, 200000);

	/* Enable I2C1 operation 
	I2C_Cmd(I2CDEV, ENABLE);

	/* Transmit data ---------------------------------------------------------- 
	DEBUGa("wirte data:\r\n");
	if (AT24xx_Write() == (-1))
		DEBUGa("Error while sending data\r\n");
	else
	DEBUGa("Data write OK \r\n");

	// wait for a while
	for (tmp = 0x100000; tmp; tmp--);

	/* Receive data ----------------------------------------------------------
	DEBUGa("Read data:\r\n");
	if (AT24xx_Read() == (-1))
		DEBUGa("Error while reading data\r\n");
	else
		DEBUGa("read data OK\r\n");

	// Verify data
	dp = pca8581_rddat;
	sp = pca8581_wrdat;
	sp += 1;  // Offset +1

	for (tmp = sizeof(pca8581_rddat); tmp; tmp--)
	{
		if (*dp++ != *sp++)
		{
			DEBUGa("Verify Data error!\r\n");
			break;
		}
		else
			DEBUGa("Verify Data OK!\r\n");
	}

	return(1);
} */

extern int check_failed;

int main() {
	global_setup();
	DEBUGa("Test");
	//c_entry();

	while(1) {
	
	DEBUGa("Test");
	}
}