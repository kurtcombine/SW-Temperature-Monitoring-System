#pragma once
#ifdef GCC_COMPILE_TEST
#include <stdint.h>
#define __I volatile const /*! Defines 'read only' structure member permissions */
#define __O volatile       /*! Defines 'write only' structure member permissions */
#define __IO volatile      /*! Defines 'read / write' structure member permissions */
/*------------- Pin Connect Block (PINCON) -----------------------------------*/
/** @brief Pin Connect Block (PINCON) register structure definition */
typedef struct {
    __IO uint32_t PINSEL0;
    __IO uint32_t PINSEL1;
    __IO uint32_t PINSEL2;
    __IO uint32_t PINSEL3;
    __IO uint32_t PINSEL4;
    __IO uint32_t PINSEL5;
    __IO uint32_t PINSEL6;
    __IO uint32_t PINSEL7;
    __IO uint32_t PINSEL8;
    __IO uint32_t PINSEL9;
    __IO uint32_t PINSEL10;
    uint32_t RESERVED0[5];
    __IO uint32_t PINMODE0;
    __IO uint32_t PINMODE1;
    __IO uint32_t PINMODE2;
    __IO uint32_t PINMODE3;
    __IO uint32_t PINMODE4;
    __IO uint32_t PINMODE5;
    __IO uint32_t PINMODE6;
    __IO uint32_t PINMODE7;
    __IO uint32_t PINMODE8;
    __IO uint32_t PINMODE9;
    __IO uint32_t PINMODE_OD0;
    __IO uint32_t PINMODE_OD1;
    __IO uint32_t PINMODE_OD2;
    __IO uint32_t PINMODE_OD3;
    __IO uint32_t PINMODE_OD4;
    __IO uint32_t I2CPADCFG;
} LPC_PINCON_TypeDef;

/*------------- General Purpose Input/Output (GPIO) --------------------------*/
/** @brief General Purpose Input/Output (GPIO) register structure definition */
typedef struct {
    union {
        __IO uint32_t FIODIR;
        struct {
            __IO uint16_t FIODIRL;
            __IO uint16_t FIODIRH;
        };
        struct {
            __IO uint8_t FIODIR0;
            __IO uint8_t FIODIR1;
            __IO uint8_t FIODIR2;
            __IO uint8_t FIODIR3;
        };
    };
    uint32_t RESERVED0[3];
    union {
        __IO uint32_t FIOMASK;
        struct {
            __IO uint16_t FIOMASKL;
            __IO uint16_t FIOMASKH;
        };
        struct {
            __IO uint8_t FIOMASK0;
            __IO uint8_t FIOMASK1;
            __IO uint8_t FIOMASK2;
            __IO uint8_t FIOMASK3;
        };
    };
    union {
        __IO uint32_t FIOPIN;
        struct {
            __IO uint16_t FIOPINL;
            __IO uint16_t FIOPINH;
        };
        struct {
            __IO uint8_t FIOPIN0;
            __IO uint8_t FIOPIN1;
            __IO uint8_t FIOPIN2;
            __IO uint8_t FIOPIN3;
        };
    };
    union {
        __IO uint32_t FIOSET;
        struct {
            __IO uint16_t FIOSETL;
            __IO uint16_t FIOSETH;
        };
        struct {
            __IO uint8_t FIOSET0;
            __IO uint8_t FIOSET1;
            __IO uint8_t FIOSET2;
            __IO uint8_t FIOSET3;
        };
    };
    union {
        __O uint32_t FIOCLR;
        struct {
            __O uint16_t FIOCLRL;
            __O uint16_t FIOCLRH;
        };
        struct {
            __O uint8_t FIOCLR0;
            __O uint8_t FIOCLR1;
            __O uint8_t FIOCLR2;
            __O uint8_t FIOCLR3;
        };
    };
} LPC_GPIO_TypeDef;

/** @brief General Purpose Input/Output interrupt (GPIOINT) register structure definition */
typedef struct {
    __I uint32_t IntStatus;
    __I uint32_t IO0IntStatR;
    __I uint32_t IO0IntStatF;
    __O uint32_t IO0IntClr;
    __IO uint32_t IO0IntEnR;
    __IO uint32_t IO0IntEnF;
    uint32_t RESERVED0[3];
    __I uint32_t IO2IntStatR;
    __I uint32_t IO2IntStatF;
    __O uint32_t IO2IntClr;
    __IO uint32_t IO2IntEnR;
    __IO uint32_t IO2IntEnF;
} LPC_GPIOINT_TypeDef;

/*------------- Timer (TIM) --------------------------------------------------*/
/** @brief Timer (TIM) register structure definition */
typedef struct {
    __IO uint32_t IR;
    __IO uint32_t TCR;
    __IO uint32_t TC;
    __IO uint32_t PR;
    __IO uint32_t PC;
    __IO uint32_t MCR;
    __IO uint32_t MR0;
    __IO uint32_t MR1;
    __IO uint32_t MR2;
    __IO uint32_t MR3;
    __IO uint32_t CCR;
    __I uint32_t CR0;
    __I uint32_t CR1;
    uint32_t RESERVED0[2];
    __IO uint32_t EMR;
    uint32_t RESERVED1[12];
    __IO uint32_t CTCR;
} LPC_TIM_TypeDef;

/*------------- System Control (SC) ------------------------------------------*/
/** @brief System Control (SC) register structure definition */
typedef struct {
    __IO uint32_t FLASHCFG; /* Flash Accelerator Module           */
    uint32_t RESERVED0[31];
    __IO uint32_t PLL0CON; /* Clocking and Power Control         */
    __IO uint32_t PLL0CFG;
    __I uint32_t PLL0STAT;
    __O uint32_t PLL0FEED;
    uint32_t RESERVED1[4];
    __IO uint32_t PLL1CON;
    __IO uint32_t PLL1CFG;
    __I uint32_t PLL1STAT;
    __O uint32_t PLL1FEED;
    uint32_t RESERVED2[4];
    __IO uint32_t PCON;
    __IO uint32_t PCONP;
    uint32_t RESERVED3[15];
    __IO uint32_t CCLKCFG;
    __IO uint32_t USBCLKCFG;
    __IO uint32_t CLKSRCSEL;
    __IO uint32_t CANSLEEPCLR;
    __IO uint32_t CANWAKEFLAGS;
    uint32_t RESERVED4[10];
    __IO uint32_t EXTINT; /* External Interrupts                */
    uint32_t RESERVED5;
    __IO uint32_t EXTMODE;
    __IO uint32_t EXTPOLAR;
    uint32_t RESERVED6[12];
    __IO uint32_t RSID; /* Reset                              */
    uint32_t RESERVED7[7];
    __IO uint32_t SCS;     /* Syscon Miscellaneous Registers     */
    __IO uint32_t IRCTRIM; /* Clock Dividers                     */
    __IO uint32_t PCLKSEL0;
    __IO uint32_t PCLKSEL1;
    uint32_t RESERVED8[4];
    __IO uint32_t USBIntSt; /* USB Device/OTG Interrupt Register  */
    __IO uint32_t DMAREQSEL;
    __IO uint32_t CLKOUTCFG; /* Clock Output Configuration         */
} LPC_SC_TypeDef;
/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
/** @brief  Universal Asynchronous Receiver Transmitter (UART) register structure definition */
typedef struct {
    union {
        __I uint8_t RBR;
        __O uint8_t THR;
        __IO uint8_t DLL;
        uint32_t RESERVED0;
    };
    union {
        __IO uint8_t DLM;
        __IO uint32_t IER;
    };
    union {
        __I uint32_t IIR;
        __O uint8_t FCR;
    };
    __IO uint8_t LCR;
    uint8_t RESERVED1[7];
    __I uint8_t LSR;
    uint8_t RESERVED2[7];
    __IO uint8_t SCR;
    uint8_t RESERVED3[3];
    __IO uint32_t ACR;
    __IO uint8_t ICR;
    uint8_t RESERVED4[3];
    __IO uint8_t FDR;
    uint8_t RESERVED5[7];
    __IO uint8_t TER;
    uint8_t RESERVED6[39];
    __I uint8_t FIFOLVL;
} LPC_UART_TypeDef;

/** @brief  Universal Asynchronous Receiver Transmitter 0 (UART0) register structure definition */
typedef struct {
    union {
        __I uint8_t RBR;
        __O uint8_t THR;
        __IO uint8_t DLL;
        uint32_t RESERVED0;
    };
    union {
        __IO uint8_t DLM;
        __IO uint32_t IER;
    };
    union {
        __I uint32_t IIR;
        __O uint8_t FCR;
    };
    __IO uint8_t LCR;
    uint8_t RESERVED1[7];
    __I uint8_t LSR;
    uint8_t RESERVED2[7];
    __IO uint8_t SCR;
    uint8_t RESERVED3[3];
    __IO uint32_t ACR;
    __IO uint8_t ICR;
    uint8_t RESERVED4[3];
    __IO uint8_t FDR;
    uint8_t RESERVED5[7];
    __IO uint8_t TER;
    uint8_t RESERVED6[39];
    __I uint8_t FIFOLVL;
} LPC_UART0_TypeDef;

/** @brief  Universal Asynchronous Receiver Transmitter 1 (UART1) register structure definition */
typedef struct {
    union {
        __I uint8_t RBR;
        __O uint8_t THR;
        __IO uint8_t DLL;
        uint32_t RESERVED0;
    };
    union {
        __IO uint8_t DLM;
        __IO uint32_t IER;
    };
    union {
        __I uint32_t IIR;
        __O uint8_t FCR;
    };
    __IO uint8_t LCR;
    uint8_t RESERVED1[3];
    __IO uint8_t MCR;
    uint8_t RESERVED2[3];
    __I uint8_t LSR;
    uint8_t RESERVED3[3];
    __I uint8_t MSR;
    uint8_t RESERVED4[3];
    __IO uint8_t SCR;
    uint8_t RESERVED5[3];
    __IO uint32_t ACR;
    uint32_t RESERVED6;
    __IO uint32_t FDR;
    uint32_t RESERVED7;
    __IO uint8_t TER;
    uint8_t RESERVED8[27];
    __IO uint8_t RS485CTRL;
    uint8_t RESERVED9[3];
    __IO uint8_t ADRMATCH;
    uint8_t RESERVED10[3];
    __IO uint8_t RS485DLY;
    uint8_t RESERVED11[3];
    __I uint8_t FIFOLVL;
} LPC_UART1_TypeDef;
/** @brief  Inter-Integrated Circuit (I2C) register structure definition */
typedef struct {
    __IO uint32_t I2CONSET;
    __I uint32_t I2STAT;
    __IO uint32_t I2DAT;
    __IO uint32_t I2ADR0;
    __IO uint32_t I2SCLH;
    __IO uint32_t I2SCLL;
    __O uint32_t I2CONCLR;
    __IO uint32_t MMCTRL;
    __IO uint32_t I2ADR1;
    __IO uint32_t I2ADR2;
    __IO uint32_t I2ADR3;
    __I uint32_t I2DATA_BUFFER;
    __IO uint32_t I2MASK0;
    __IO uint32_t I2MASK1;
    __IO uint32_t I2MASK2;
    __IO uint32_t I2MASK3;
} LPC_I2C_TypeDef;

/*------------- Real-Time Clock (RTC) ----------------------------------------*/
/** @brief  Real-Time Clock (RTC) register structure definition */
typedef struct {
    __IO uint8_t ILR;
    uint8_t RESERVED0[7];
    __IO uint8_t CCR;
    uint8_t RESERVED1[3];
    __IO uint8_t CIIR;
    uint8_t RESERVED2[3];
    __IO uint8_t AMR;
    uint8_t RESERVED3[3];
    __I uint32_t CTIME0;
    __I uint32_t CTIME1;
    __I uint32_t CTIME2;
    __IO uint8_t SEC;
    uint8_t RESERVED4[3];
    __IO uint8_t MIN;
    uint8_t RESERVED5[3];
    __IO uint8_t HOUR;
    uint8_t RESERVED6[3];
    __IO uint8_t DOM;
    uint8_t RESERVED7[3];
    __IO uint8_t DOW;
    uint8_t RESERVED8[3];
    __IO uint16_t DOY;
    uint16_t RESERVED9;
    __IO uint8_t MONTH;
    uint8_t RESERVED10[3];
    __IO uint16_t YEAR;
    uint16_t RESERVED11;
    __IO uint32_t CALIBRATION;
    __IO uint32_t GPREG0;
    __IO uint32_t GPREG1;
    __IO uint32_t GPREG2;
    __IO uint32_t GPREG3;
    __IO uint32_t GPREG4;
    __IO uint8_t RTC_AUXEN;
    uint8_t RESERVED12[3];
    __IO uint8_t RTC_AUX;
    uint8_t RESERVED13[3];
    __IO uint8_t ALSEC;
    uint8_t RESERVED14[3];
    __IO uint8_t ALMIN;
    uint8_t RESERVED15[3];
    __IO uint8_t ALHOUR;
    uint8_t RESERVED16[3];
    __IO uint8_t ALDOM;
    uint8_t RESERVED17[3];
    __IO uint8_t ALDOW;
    uint8_t RESERVED18[3];
    __IO uint16_t ALDOY;
    uint16_t RESERVED19;
    __IO uint8_t ALMON;
    uint8_t RESERVED20[3];
    __IO uint16_t ALYEAR;
    uint16_t RESERVED21;
} LPC_RTC_TypeDef;

extern LPC_UART0_TypeDef *LPC_UART0;
extern LPC_UART1_TypeDef *LPC_UART1;
extern uint32_t SystemCoreClock;
int SysTick_Config(int);
void PIN_Configure(int, int, int, int, int);
void GPIO_PinWrite(int, int, int);
int GPIO_PinRead(int, int);
void LED_On(int led);
void LED_Off(int led);
void LED_Initialize();
int LED_GetCount();
void init_ILI9325(void);
void lcdConfiguration(void);
void lcdSend(uint16_t byte);
void wait_delay(int count);
uint16_t lcdRead(void);
void lcdWriteIndex(uint16_t index);
void lcdWriteData(uint16_t data);
uint16_t lcdReadData(void);
void lcdWriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
uint16_t lcdReadReg(uint16_t LCD_Reg);
void lcdSetCursor(uint16_t Xpos, uint16_t Ypos);
#endif
