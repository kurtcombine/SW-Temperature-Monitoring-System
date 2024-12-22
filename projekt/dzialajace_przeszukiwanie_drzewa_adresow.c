// inspired by https://github.com/PaulStoffregen/OneWire/blob/v2.3.8/OneWire.cpp
// and https://github.com/Hotaman/OneWireSpark/blob/0b2380e9b1f5b561772edd46e0a6be5884538251/src/DS18.cpp

// for reference https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/DallasTemperature.cpp

/*
for testing without uVision: https://github.com/ARM-software/NXP_LPC/tree/master/LPC1700/CMSIS/Driver
*/
// #include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include <stdbool.h>
#include <stdio.h>

void DEBUG(char *str);
void onewire_reset_search();
int SysTick_setup() {
    return SysTick_Config(SystemCoreClock / 1000000);  // SystemCoreClock changes accordingly to clock divisor CCLKCFG
}
volatile int usTicks = 0;
void SysTick_Handler(void) {
    usTicks++;
}
void delayMicroseconds(int millis) {
    int start = usTicks;
    while(start + millis < usTicks);  // wait for int overflow
    while(usTicks < start + millis);
}
char port;
char pin;

char ROM_NO[8];
char LastDiscrepancy;
char LastFamilyDiscrepancy;
bool LastDeviceFlag;

void noInterrupts() {}
void interrupts() {}

void onewire_setup(char DS18B20__port, char DS18B20__pin) {
    port = DS18B20__port;
    pin = DS18B20__pin;
    PIN_Configure(port, pin, 0, 0, 1);
    onewire_reset_search();
}

char onewire_reset() { // true - good
    // assert: we are the only master and bus is high
    noInterrupts();
    GPIO_PinWrite(port, pin, 0);
    interrupts();
    delayMicroseconds(480);
    noInterrupts();
    GPIO_PinWrite(port, pin, 1);
    delayMicroseconds(70);
    char presence = !GPIO_PinRead(port, pin);
    interrupts();
    delayMicroseconds(410);
    return presence;
}

void onewire_write_bit(char bit) {
    if(bit) {
        noInterrupts();
        GPIO_PinWrite(port, pin, 0);
        delayMicroseconds(10);
        GPIO_PinWrite(port, pin, 1);
        interrupts();
        delayMicroseconds(55);
    } else {
        noInterrupts();
        GPIO_PinWrite(port, pin, 0);
        delayMicroseconds(65);
        GPIO_PinWrite(port, pin, 1);
        interrupts();
        delayMicroseconds(5);
    }
}

char onewire_read_bit(void) {
    noInterrupts();
    GPIO_PinWrite(port, pin, 0);
    delayMicroseconds(3);
    GPIO_PinWrite(port, pin, 1);
    delayMicroseconds(10);
    char bit = GPIO_PinRead(port, pin);
    interrupts();
    delayMicroseconds(53);
    return bit;
}

void onewire_write(char byte) {
    for(char i = 0; i < 8; i++) {
        onewire_write_bit(byte & 1);
        byte >>= 1;
    }
}

char onewire_read() {
    char byte = 0;

    for(char mask = 1; mask != 0; mask <<= 1)
        if(onewire_read_bit()) byte |= mask;
    return byte;
}

void onewire_select(const char rom[8]) {
    onewire_write(0x55);  // Choose ROM
    for(char i = 0; i < 8; i++) onewire_write(rom[i]);
}

void onewire_skip() {
    onewire_write(0xCC);  // Skip ROM
}

void onewire_reset_search() {
    LastDiscrepancy = 0;
    LastDeviceFlag = false;
    LastFamilyDiscrepancy = 0;
    for(char i = 0; i < 8; i++) ROM_NO[i] = 0;
}

//
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// onewire_address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use onewire_reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
bool onewire_search(char *newAddr, bool search_mode) {
    char id_bit_number;
    char last_zero, rom_byte_number;
    bool search_result;
    char id_bit, cmp_id_bit;

    unsigned char rom_byte_mask, search_direction;

    // initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = false;

    // if the last call was not the last one
    if(!LastDeviceFlag) {
        // 1-Wire reset
        if(!onewire_reset()) {
            // reset the search
            LastDiscrepancy = 0;
            LastDeviceFlag = false;
            LastFamilyDiscrepancy = 0;
            return false;
        }

        // issue the search command
        if(search_mode == true) {
            onewire_write(0xF0);  // NORMAL SEARCH
        } else {
            onewire_write(0xEC);  // CONDITIONAL SEARCH
        }

        // loop to do the search
        do {
            // read a bit and its complement
            id_bit = onewire_read_bit();
            cmp_id_bit = onewire_read_bit();

            // check for no devices on 1-wire
            if((id_bit == 1) && (cmp_id_bit == 1)) {
                break;
            } else {
                // all devices coupled have 0 or 1
                if(id_bit != cmp_id_bit) {
                    search_direction = id_bit;  // bit write value for search
                } else {
                    // if this discrepancy if before the Last Discrepancy
                    // on a previous next then pick the same as last time
                    if(id_bit_number < LastDiscrepancy) {
                        search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                    } else {
                        // if equal to last pick 1, if not then pick 0
                        search_direction = (id_bit_number == LastDiscrepancy);
                    }
                    // if 0 was picked then record its position in LastZero
                    if(search_direction == 0) {
                        last_zero = id_bit_number;

                        // check for Last discrepancy in family
                        if(last_zero < 9) LastFamilyDiscrepancy = last_zero;
                    }
                }

                // set or clear the bit in the ROM byte rom_byte_number
                // with mask rom_byte_mask
                if(search_direction == 1) ROM_NO[rom_byte_number] |= rom_byte_mask;
                else
                    ROM_NO[rom_byte_number] &= ~rom_byte_mask;

                // serial number search direction write bit
                onewire_write_bit(search_direction);

                // increment the byte counter id_bit_number
                // and shift the mask rom_byte_mask
                id_bit_number++;
                rom_byte_mask <<= 1;

                // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
                if(rom_byte_mask == 0) {
                    rom_byte_number++;
                    rom_byte_mask = 1;
                }
            }
        } while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

        // if the search was successful then
        if(!(id_bit_number < 65)) {
            // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
            LastDiscrepancy = last_zero;

            // check for last device
            if(LastDiscrepancy == 0) LastDeviceFlag = true;
            search_result = true;
        }
    }

    // if no device found then reset counters so next 'search' will be like a first
    if(!search_result || !ROM_NO[0]) {
        LastDiscrepancy = 0;
        LastDeviceFlag = false;
        LastFamilyDiscrepancy = 0;
        search_result = false;
    } else {
        for(int i = 0; i < 8; i++) newAddr[i] = ROM_NO[i];
    }
    return search_result;
}

// The 1-Wire CRC scheme is described in Maxim Application Note 27:
// "Understanding and Using Cyclic Redundancy Checks with Maxim iButton Products"
//
#define ONEWIRE_CRC8_TABLE 1
#if ONEWIRE_CRC8_TABLE
// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 2x16 entry CRC table created by Arjen Lentz
// See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
static const char dscrc2x16_table[] = {0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83, 0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F,
    0x41, 0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8, 0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74};

// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (Use tiny 2x16 entry CRC table)
char onewire_crc8(const char *addr, char len) {
    char crc = 0;

    while(len--) {
        crc = *addr++ ^ crc;  // just re-using crc as intermediate
        crc = (*(dscrc2x16_table + (crc & 0x0f))) ^ (*(dscrc2x16_table + 16 + ((crc >> 4) & 0x0f)));
    }

    return crc;
}
#else
//
// Compute a Dallas Semiconductor 8 bit CRC directly.
// this is much slower, but a little smaller, than the lookup table.
//
char onewire_crc8(const char *addr, char len) {
    char crc = 0;

    while(len--) {
        char inbyte = *addr++;
        for(char i = 8; i; i--) {
            char mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if(mix) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}
#endif

////////////////////////////////////////

char DS18B20__data[9];
bool DS18B20__presence = false;
uint16_t DS18B20__conversionTime = 1000;

void onewire_DS18B20_setup() {
    for(char i = 0; i < 9; i++) DS18B20__data[i] = 0;
}

bool onewire_DS18B20_read_scratchpad(char addr[8]) {
    DS18B20__presence = onewire_reset();
    if(!DS18B20__presence) return false;
	if(addr == NULL)onewire_skip();
		else onewire_select(addr);
    onewire_write(0xBE);  // Read Scratchpad
    for(char i = 0; i < 9; i++) DS18B20__data[i] = onewire_read();
    if(onewire_crc8(DS18B20__data, 8) != DS18B20__data[8]) return false;
    return true;
}

bool onewire_DS18B20_convert(char addr[8]) {
    DS18B20__presence = onewire_reset();
    if(!DS18B20__presence) return false;
    if(addr == NULL) onewire_skip();
		else onewire_select(addr);
    onewire_write(0x44);

    delayMicroseconds(1000 * DS18B20__conversionTime);
    return true;
}

char onewire_DS18B20_get_bits_resolution() {
    switch(DS18B20__data[4] & 0x60) {
        case 0x00: return 9;
        case 0x20: return 10;
        case 0x40: return 11;
        case 0x60:
        default: return 12;
    }
}

char presisions[4][8] = {"0.25", "0.125", "0.0625", "0.03125"};

char *onewire_DS18B20_get_precision() {
    switch(onewire_DS18B20_get_bits_resolution()) {
        case 9: return presisions[0];
        case 10: return presisions[1];
        case 11: return presisions[2];
        default: return presisions[3];
    }
}

float onewire_DS18B20_get_celsius() {
    int16_t DS18B20__raw = (DS18B20__data[1] << 8) | DS18B20__data[0];

    // at lower resolution, the low bits are undefined, so let's meet half-way
    switch(onewire_DS18B20_get_bits_resolution()) {
        case 9: DS18B20__raw = (DS18B20__raw & ~0b111) | 0b100; break;  // ± 0.25000 ?, 93.75 ms
        case 10: DS18B20__raw = (DS18B20__raw & ~0b11) | 0b10; break;   // ± 0.12500 ?, 187.5 ms
        case 11: DS18B20__raw = DS18B20__raw & 0b1; break;            // ± 0.06250 ?, 375 ms
        default: break;                                               // ± 0.03125 ?, 750 ms
    }
    return (float)DS18B20__raw * 0.0625;
}

float onewire_DS18B20_get_fahrenheit() {
    return onewire_DS18B20_get_celsius() * 1.8 + 32.0;
}

void onewire_DS18B20_setConversionTime(uint16_t ms) {
    DS18B20__conversionTime = ms;
}

#define UART LPC_UART0
void UART0_Init() {
    PIN_Configure(0, 2, 1, 0, 0);
    PIN_Configure(0, 3, 1, 0, 0);
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b10000111;
    UART->DLL = 163;  // bitrate=9600
    UART->LCR = (UART->LCR & 0xFFFFFF00) | 0b00000111;
}
void UART_Char_Send(char c) {
    while(!(UART->LSR & 1 << 5));  // wait for THRE flag (5th bit of LSR reg) to reset
    UART->THR = c;
}
void UART_Send(const char *str) {
    for(int i = 0; str[i]; i++) UART_Char_Send(str[i]);
}

void DEBUG(char *str) {
    UART_Send(str);
}

int main() {
    SysTick_setup();
    UART0_Init();
    onewire_setup(0, 0);
    onewire_DS18B20_setup();

	/*
    while(true) {

        if(!onewire_DS18B20_convert(NULL)) {
            DEBUG("No device on bus. Skipping.\r\n");
            continue;
        }

        if(!onewire_DS18B20_read_scratchpad(NULL)) {
            if(!DS18B20__presence) {
                DEBUG("No device on bus. Skipping.\r\n");
                continue;
            }
            DEBUG("Data CRC mismatch. Skipping.\r\n");
						
        for(char i = 0; i < 9; i++) {
            char buf[4];
            sprintf(buf, "%02X:", DS18B20__data[i]);
            if(i == 8) buf[2] = '\0';
            DEBUG(buf);
        }
					DEBUG("\r\n");
            continue;
        }
        DEBUG("Temperature: ");
        char buf[10];
        sprintf(buf, "%f", onewire_DS18B20_get_celsius());
        DEBUG(buf);
        DEBUG("(_");
        DEBUG(onewire_DS18B20_get_precision());
        DEBUG(")");
        DEBUG("\r\n");
    }
	*/
	
	
    char addr[8];
	for(char i=0; i < 8; i++) addr[i] = 0;
	
    while(true) {
        if(!onewire_search(addr, false)) {
            DEBUG("No more addresses. Restarting search.\r\n");
            continue;
        }

        if(onewire_crc8(addr, 7) != addr[7]) {
            DEBUG("Address CRC mismatch. Skipping.\r\n");
            continue;
        }

        if(!onewire_DS18B20_convert(addr)) {
            DEBUG("No device on bus. Skipping.\r\n");
            continue;
        }

        if(!onewire_DS18B20_read_scratchpad(addr)) {
            if(!DS18B20__presence) {
                DEBUG("No device on bus. Skipping.\r\n");
                continue;
            }
            DEBUG("Data CRC mismatch. Skipping.\r\n");
            continue;
        }

        DEBUG("[");
        for(char i = 0; i < 8; i++) {
            char buf[4];
            sprintf(buf, "%02X:", addr[i]);
            if(i == 7) buf[2] = '\0';
            DEBUG(buf);
        }
        DEBUG("] Temperature: ");
        char buf[10];
        sprintf(buf, "%f", onewire_DS18B20_get_celsius());
        DEBUG(buf);
        DEBUG("(_");
        DEBUG(onewire_DS18B20_get_precision());
        DEBUG(")");
        DEBUG("\r\n");
    }
}
