/**********************************************************************
* $Id$		pca8581_test.c  				2010-05-21
*//**
* @file		pca8581_test.c
* @brief	An example of I2C using polling mode to test the I2C driver.
* 			Using EEPROM PCA8581 to transfer a number of data byte.
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/
#include "lpc17xx_i2c.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "systen_delay.h"
/* Example group ----------------------------------------------------------- */
/** @defgroup I2C_pca8581_polling	pca8581_polling
 * @ingroup I2C_Examples
 * @{
 */

//#define Open_I2C0
//#define Open_I2C1
#define Open_I2C2

/************************** PRIVATE DEFINITIONS *************************/
/** Used I2C device definition, should be 0 or 2 */
#ifdef Open_I2C0
	#define USEDI2CDEV	0
#endif

#ifdef Open_I2C1
	#define USEDI2CDEV	1
#endif

#ifdef Open_I2C2
	#define USEDI2CDEV	2
#endif

/* Definition of internal register of EEPROM PCA8581 */

/* 7 bit address */
#define PCA8581_SLVADDR		(0xA0>>1)

#if (USEDI2CDEV == 0)
#define I2CDEV LPC_I2C0
#elif (USEDI2CDEV == 1)
#define I2CDEV LPC_I2C1
#elif (USEDI2CDEV == 2)
#define I2CDEV LPC_I2C2

#else
#error "I2C device not defined!"
#endif


uint8_t menu2[] = "Demo terminated! \r\n";

/* Data using for transferring to PCA8581 */
uint8_t pca8581_wrdat[] = {
		(0x01<<3),	// Memory Address, data will be written at this address
					// This internal memory address should be 8-byte alignment
		0x00,	// Data 0
		0x11,	// Data 1
		0x22,	// Data 2
		0x33,	// Data 3
		0x44,	// Data 4
		0x55,	// Data 5
		0x66,	// Data 6
		0x77,	// Data 7
};

uint8_t pca8581_rddat[] = {
		// Don't use Memory address here, the Memory address to read
		// is the same as Memory Address to write.
		0x00,	// Data 0
		0x00,	// Data 1
		0x00,	// Data 2
		0x00,	// Data 3
		0x00,	// Data 4
		0x00,	// Data 5
		0x00,	// Data 6
		0x00,	// Data 7
};

/* Transmit setup */
I2C_M_SETUP_Type txsetup;
/* Receive setup */
I2C_M_SETUP_Type rxsetup;

/************************** PRIVATE FUNCTIONS *************************/
int32_t AT24xx_Write(void);
int32_t AT24xx_Read(void);


/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Write a number of data byte into EEPROM PCA8581
 * @param[in]	None
 * @return 		0: if success, otherwise (-1) returned.
 **********************************************************************/
int32_t AT24xx_Write(void)
{
	txsetup.sl_addr7bit = PCA8581_SLVADDR;
	txsetup.tx_data = pca8581_wrdat;
	txsetup.tx_length = sizeof(pca8581_wrdat);
//	txsetup.tx_length =2;
	txsetup.rx_data = NULL;
	txsetup.rx_length = 0;
	txsetup.retransmissions_max = 3;

	if (I2C_MasterTransferData(I2CDEV, &txsetup, I2C_TRANSFER_POLLING) == SUCCESS)
	{
		return (0);
	} else {
		return (-1);
	}
}

/*********************************************************************//**
 * @brief		Read a number of data byte from EEPROM PCA8581
 * @param[in]	None
 * @return 		0: if success, otherwise (-1) returned.
 **********************************************************************/
int32_t AT24xx_Read(void)
{

	rxsetup.sl_addr7bit = PCA8581_SLVADDR;
	rxsetup.tx_data = pca8581_wrdat;	// Get address to read at writing address
	rxsetup.tx_length = 1;
	rxsetup.rx_data = pca8581_rddat;
	rxsetup.rx_length = sizeof(pca8581_rddat);
	rxsetup.retransmissions_max = 3;

	if (I2C_MasterTransferData(I2CDEV, &rxsetup, I2C_TRANSFER_POLLING) == SUCCESS){
		return (0);
	} else {
		return (-1);
	}
}





/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t tmp;
	uint8_t *dp, *sp;

	debug_frmwrk_init();
	_printf("AT24Cxx test procedures:\r\n");
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

	/* I2C block ------------------------------------------------------------------- */
	// Initialize I2C peripheral
	I2C_Init(I2CDEV, 200000);

	/* Enable I2C1 operation */
	I2C_Cmd(I2CDEV, ENABLE);

	/* Transmit data ---------------------------------------------------------- */
	_printf("wirte data:\r\n");
	if (AT24xx_Write() == (-1))
		_printf("Error while sending data\r\n");
	else
	_printf("Data write OK \r\n");

	// wait for a while
	for (tmp = 0x100000; tmp; tmp--);

	/* Receive data ---------------------------------------------------------- */
	_printf("Read data:\r\n");
	if (AT24xx_Read() == (-1))
		_printf("Error while reading data\r\n");
	else
		_printf("read data OK\r\n");

	// Verify data
	dp = pca8581_rddat;
	sp = pca8581_wrdat;
	sp += 1;  // Offset +1

	for (tmp = sizeof(pca8581_rddat); tmp; tmp--)
	{
		if (*dp++ != *sp++)
		{
			_printf("Verify Data error!\r\n");
			break;
		}
		else
			_printf("Verify Data OK!\r\n");
	}

	return(1);
}

void LED_Configuration(void)
{
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;	
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);	
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(0, (1<<0)|(1<<1)|(1<<2)|(1<<3), 1);           /* LEDs on PORT0[0123] defined as Output    */
	GPIO_SetValue(0, (1<<0)|(1<<1)|(1<<2)|(1<<3));

}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
	delay_init();
    c_entry();
	 
	LED_Configuration();
    while(1)
	{
		GPIO_SetValue(0, (1<<0)|(1<<1)|(1<<2)|(1<<3));
		delay_ms(150);
		GPIO_ClearValue(0,(1<<0)|(1<<1)|(1<<2)|(1<<3));
		delay_ms(150);	
	}
}



#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/*
 * @}
 */
