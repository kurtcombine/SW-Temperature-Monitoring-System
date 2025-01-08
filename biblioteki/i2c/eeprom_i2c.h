#include "Driver_I2C.h"
 
#define EEPROM_I2C_ADDR       0x51      /* EEPROM I2C address */
 
/* I2C Signal Event function callback */
void I2C_SignalEvent (uint32_t event);
 
/* Read I2C connected EEPROM (event driven example) */
int32_t EEPROM_Read_Event (uint16_t addr, uint8_t *buf, uint32_t len);
int32_t EEPROM_Write_Event (uint16_t addr, uint8_t *buf, uint32_t len);

int32_t EEPROM_Initialize ();