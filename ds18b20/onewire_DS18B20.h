#include "../global/global.h"

#define DS18B20_TEMP_MIN -55
#define DS18B20_TEMP_MAX 125

bool onewire_reset();  // true = good
void onewire_write_bit(uint8_t bit);
uint8_t onewire_read_bit(void);
void onewire_write(uint8_t byte);
uint8_t onewire_read();
void onewire_select(const uint8_t rom[8]);
void onewire_skip();

void onewire_setup();
uint8_t onewire_crc8(const uint8_t *data, uint8_t length);
void onewire_reset_search();
// FIXME: mock 1-Wire search algorithm and check if it returns all devices in sorted order
bool onewire_search(uint8_t *new_addr, bool search_mode);

extern uint8_t DS18B20__data[9];
extern bool DS18B20__presence;
extern uint16_t DS18B20__conversionTime;

void onewire_DS18B20_setup();
void onewire_DS18B20_cleanup();

bool onewire_DS18B20_read_scratchpad(uint8_t addr[8]);
bool onewire_DS18B20_convert(uint8_t addr[8]);

uint8_t onewire_DS18B20_get_bits_resolution();
uint8_t *onewire_DS18B20_get_precision();
float onewire_DS18B20_get_celsius();
float onewire_DS18B20_get_fahrenheit();
void onewire_DS18B20_setConversionTime(uint16_t ms);
