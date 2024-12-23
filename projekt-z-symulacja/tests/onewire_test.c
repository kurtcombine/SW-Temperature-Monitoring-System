#include "ds18b20/onewire_DS18B20.h"


int main() {
    global_setup();
    onewire_setup();
    onewire_DS18B20_setup();

    uint8_t addr[8];
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
        for(uint8_t i = 0; i < 8; i++) {
            uint8_t buf[4];
            sprintf(buf, "%02X:", addr[i]);
            if(i == 7) buf[2] = '\0';
            DEBUG(buf);
        }
        DEBUG("] Temperature: ");
        uint8_t buf[10];
        sprintf(buf, "%f", onewire_DS18B20_get_celsius());
        DEBUG(buf);
        DEBUG("(_");
        DEBUG(onewire_DS18B20_get_precision());
        DEBUG(")");
        DEBUG("\r\n");
    }
    onewire_DS18B20_cleanup();
}
