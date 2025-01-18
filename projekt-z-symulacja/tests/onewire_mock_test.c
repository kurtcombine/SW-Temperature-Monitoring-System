#include "ds18b20/onewire_DS18B20.h"
#include "ds18b20/onewire_DS18B20.c"

void ___LCD_render() {}

int main() {
    global_setup();
    onewire_setup();
    onewire_DS18B20_setup();
    uint8_t addr[8];

    for(int i = 0; i < 10; i++) {
        while(onewire_search(addr, true)) {
            uint8_t buf[24];
            if(onewire_crc8(addr, 7) != addr[7]) DEBUG("Invalid address");
            else
                DEBUG_addr(addr);
            DEBUG(" ");
            DEBUG_float(___get_temperature(addr));
            DEBUG("\r\n");
        }
        DEBUG("No more devices\r\n");
    }

    onewire_DS18B20_cleanup();
    return 0;
}
