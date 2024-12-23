#include "lcd/lcd.h"
#include "ds18b20/onewire_DS18B20.h"

void OnMockDestroyWindow() {
    onewire_DS18B20_cleanup();
}

typedef enum { SEARCH_RESET, SEARCH_ADDR_CRC_ERR, SEARCH_BUS_ERR, SEARCH_DATA_CRC_ERR, SEARCH_GOOD } searchstate;
searchstate gather_device(uint8_t addr[8]) {
    if(!onewire_search(addr, false)) {
        DEBUG("SEARCH: RESET\r\n");
        return SEARCH_RESET;
    }

    if(onewire_crc8(addr, 7) != addr[7]) {
        DEBUG("SEARCH: ADDR CRC ERR\r\n");
        return SEARCH_ADDR_CRC_ERR;
    }

    if(!onewire_DS18B20_convert(addr)) {
        DEBUG("SEARCH: BUS ERR\r\n");
        return SEARCH_BUS_ERR;
    }

    if(!onewire_DS18B20_read_scratchpad(addr)) {
        if(!DS18B20__presence) {
            DEBUG("SEARCH: BUS ERR\r\n");
            return SEARCH_BUS_ERR;
        }
        DEBUG("SEARCH: DATA CRC ERR\r\n");
        return SEARCH_DATA_CRC_ERR;
    }
    uint8_t buf[50];
    sprintf(buf, "SEARCH: GOOD %f_%f\r\n", onewire_DS18B20_get_celsius(), onewire_DS18B20_get_precision());
    DEBUG(buf);
    return SEARCH_GOOD;
}

int main() {
    global_setup();
    LCD_setup();
    onewire_setup();
    onewire_DS18B20_setup();
    LCD_Background(LCDBlue);
    int j = 0;
    for(int j = 0; j < LCD_SCREEN_HEIGHT; j++) {
        for(int i = 0; i < LCD_SCREEN_WIDTH; i++) { LCD_Pixel(i, j, LCDWhite); }
        if(j % 10 == 9) LCD_Background(LCDBlue);
    }

    LCD_Background(LCDBlue);
    uint8_t addr[8];
    int line = 0;
    while(true) {
        searchstate state = gather_device(addr);

        if(state == SEARCH_RESET) {
            LCD_Background(LCDBlue);
            line = 0;
            continue;
        }
        if(state == SEARCH_ADDR_CRC_ERR) {
            LCD_Text(0, (line++) * 12, "ACRC", LCDWhite);
            continue;
        }

        LCD_Addr(0, line * 12, addr, LCDWhite);

        if(state == SEARCH_BUS_ERR) {
            LCD_Text(20 * 8, (line++) * 12, "BUS", LCDWhite);
            continue;
        }

        if(state == SEARCH_DATA_CRC_ERR) {
            LCD_Text(20 * 8, (line++) * 12, "DCRC", LCDWhite);
            continue;
        }

        if(state == SEARCH_GOOD) {
            uint8_t buf[10];
            sprintf(buf, "%f", onewire_DS18B20_get_celsius());
            LCD_Text(20 * 8, (line++) * 12, buf, LCDWhite);
        }
    }
}
