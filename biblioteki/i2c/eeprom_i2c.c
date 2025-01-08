#include "Driver_I2C.h"
#include "eeprom_i2c.h"

/* I2C driver instance */
extern ARM_DRIVER_I2C            Driver_I2C0;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C0;
 
static volatile uint32_t I2C_Event;
 
/* I2C Signal Event function callback */
void I2C_SignalEvent (uint32_t event) {
 
  /* Save received events */
  I2C_Event |= event;
 
  /* Optionally, user can define specific actions for an event */
 
  if (event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) {
    /* Less data was transferred than requested */
  }
 
  if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
    /* Transfer or receive is finished */
  }
 
  if (event & ARM_I2C_EVENT_ADDRESS_NACK) {
    /* Slave address was not acknowledged */
  }
 
  if (event & ARM_I2C_EVENT_ARBITRATION_LOST) {
    /* Master lost bus arbitration */
  }
 
  if (event & ARM_I2C_EVENT_BUS_ERROR) {
    /* Invalid start/stop position detected */
  }
 
  if (event & ARM_I2C_EVENT_BUS_CLEAR) {
    /* Bus clear operation completed */
  }
 
  if (event & ARM_I2C_EVENT_GENERAL_CALL) {
    /* Slave was addressed with a general call address */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_RECEIVE) {
    /* Slave addressed as receiver but SlaveReceive operation is not started */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_TRANSMIT) {
    /* Slave addressed as transmitter but SlaveTransmit operation is not started */
  }
}
 
/* Read I2C connected EEPROM (event driven example) */
int32_t EEPROM_Read_Event (uint16_t addr, uint8_t *buf, uint32_t len) {
  uint8_t a[2];
 
  a[0] = (uint8_t)(addr >> 8);
  a[1] = (uint8_t)(addr & 0xFF);
 
  /* Clear event flags before new transfer */
  I2C_Event = 0U;
 
  I2Cdrv->MasterTransmit (EEPROM_I2C_ADDR, a, 2, true);
 
  /* Wait until transfer completed */
  while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);
  /* Check if all data transferred */
  if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) return -1;
 
  /* Clear event flags before new transfer */
  I2C_Event = 0U;
 
  I2Cdrv->MasterReceive (EEPROM_I2C_ADDR, buf, len, false);
 
  /* Wait until transfer completed */
  while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);
  /* Check if all data transferred */
  if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) return -1;
 
  return 0;
}
#include <stdlib.h>
uint8_t buf2[0x7ff+2];

int32_t EEPROM_Write_Event (uint16_t addr, uint8_t *buf, uint32_t len) {
  uint8_t a[2];
 
  a[0] = (uint8_t)(addr >> 8);
  a[1] = (uint8_t)(addr & 0xFF);
 
  /* Clear event flags before new transfer */
  I2C_Event = 0U;
	buf2[0] = a[0], buf2[1] = a[1];
	for(int i = 0; i < len; i++)
		buf2[i+2] = buf[i];
 
  I2Cdrv->MasterTransmit (EEPROM_I2C_ADDR ^ 1, buf2, 2+len, false);
 
  /* Wait until transfer completed */
  while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);
  /* Check if all data transferred */
  if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) return -1;
 
  /* Clear event flags before new transfer */
  //I2C_Event = 0U;
 
  //I2Cdrv->MasterTransmit (EEPROM_I2C_ADDR ^ 1, buf, len, false);
 
  /* Wait until transfer completed */
  //while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0U);
  /* Check if all data transferred */
  //if ((I2C_Event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) != 0U) return -1;
 
  return 0;
}
 
/* Initialize I2C connected EEPROM */
int32_t EEPROM_Initialize () {
  int32_t status;
  uint8_t val;
 
	I2Cdrv->Initialize (I2C_SignalEvent);
  I2Cdrv->PowerControl (ARM_POWER_FULL);
  I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);
 
	status = EEPROM_Read_Event (0x00, &val, 1);
 
  return (status);
}