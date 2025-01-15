#include "lpc17xx_i2c.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"

#define Open_I2C0
//#define Open_I2C1
//#define Open_I2C2

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