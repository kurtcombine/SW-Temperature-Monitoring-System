#include "lcd/lcd.h"
#include "ds18b20/onewire_DS18B20.h"
#include "mem/mem.h"

void OnMockDestroyWindow() {
    onewire_DS18B20_cleanup();
}
typedef enum {
    SEARCH_RESET,
    SEARCH_ADDR_CRC_ERR,
    SEARCH_BUS_ERR,
    SEARCH_DATA_CRC_ERR,
    SEARCH_GOOD,
    SEARCH_MISSING
} searchstate;
searchstate gather_device(uint8_t addr[8]) {
    DEBUG("SEARCH: ");
    if(!onewire_search(addr, false)) {
        DEBUG("RESET\r\n");
        return SEARCH_RESET;
    }

    if(onewire_crc8(addr, 7) != addr[7]) {
        DEBUG("ADDR CRC ERR\r\n");
        return SEARCH_ADDR_CRC_ERR;
    }

    DEBUG_addr(addr);
    if(!onewire_DS18B20_convert(addr)) {
        DEBUG(" BUS ERR\r\n");
        return SEARCH_BUS_ERR;
    }

    if(!onewire_DS18B20_read_scratchpad(addr)) {
        if(!DS18B20__presence) {
            DEBUG(" BUS ERR\r\n");
            return SEARCH_BUS_ERR;
        }
        DEBUG(" DATA CRC ERR\r\n");
        return SEARCH_DATA_CRC_ERR;
    }
    DEBUG(" GOOD\r\nTEMPERATURE(");
    DEBUG_addr(addr);
    DEBUG("_");
    DEBUG_float(onewire_DS18B20_get_celsius());
    DEBUG("_");
    DEBUG(onewire_DS18B20_get_precision());
    DEBUG(")\r\n");
    return SEARCH_GOOD;
}

int MEM_cmp(const uint8_t addr[8]) {
    // addr < mem => -1, addr == mem => 0, addr > mem => +1
    // -1 => need to add address
    // +1 => address is missing
    const uint8_t *mem = MEM_top();
    int cmp;
    if(addr == NULL && mem == NULL) cmp = 0;
    else if(addr == NULL)
        cmp = 1;
    else if(mem == NULL)
        cmp = -1;
    else {
        cmp = memcmp(addr, mem, 8);
        cmp = cmp < 0 ? -1 : cmp > 0 ? 1 : 0;
    }
    DEBUG("CMP: ADDR=");
    DEBUG_addr(addr);
    DEBUG(" MEM=");
    DEBUG_addr(mem);
    DEBUG(" CMP=");
    DEBUG_float(cmp);
    DEBUG("\r\n");
    return cmp;
}

size_t ui_line;
#define LAYOUT_X_OFFSET 32
#define LAYOUT_Y_OFFSET 18
void feed_ui(int state) {
    DEBUG("FEED: ");

    for(int j = 0; j < 12; j++) {
        for(int i = LAYOUT_X_OFFSET; i < LCD_SCREEN_WIDTH - LAYOUT_X_OFFSET; i++) {
            LCD_Pixel(i, ui_line * 12 - 2 + j + LAYOUT_Y_OFFSET, LCDBlue);
        }
    }
    int X = 20 * 8 + LAYOUT_X_OFFSET, Y = ui_line * 12 + LAYOUT_Y_OFFSET;
    if(state == SEARCH_GOOD) LCD_Addr(LAYOUT_X_OFFSET, Y, MEM_top(), LCDWhite);
    else
        LCD_Addr(LAYOUT_X_OFFSET, Y, MEM_top(), LCDRed);
    switch(state) {
        case SEARCH_ADDR_CRC_ERR:
            DEBUG(" ADDR CRC ERR\r\n");
            LCD_Text(X, Y, "ADDR CRC ERR", LCDWhite);
            break;
        case SEARCH_BUS_ERR:
            DEBUG(" BUS ERR\r\n");
            LCD_Text(X, Y, "BUS ERR", LCDWhite);
            break;
        case SEARCH_DATA_CRC_ERR:
            DEBUG(" DATA CRC ERR\r\n");
            LCD_Text(X, Y, "DATA CRC ERR", LCDWhite);
            break;
        case SEARCH_MISSING:
            DEBUG(" MISSING\r\n");
            LCD_Text(X, Y, "MISSING", LCDWhite);
            break;
        case SEARCH_GOOD:
            DEBUG(" GOOD\r\n");
            uint8_t buf[10];
            sprintf(buf, "%6.2f", onewire_DS18B20_get_celsius());
            LCD_Text(X, Y, buf, LCDWhite);
            LCD_Char(X + 8 * 6, Y, '\x7f', LCDWhite);
            LCD_Char(X + 8 * 6 + 7, Y, 'C', LCDWhite);
            break;
        default:
            DEBUG("unreachable!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            DEBUG_int(state);
            DEBUG("\r\n");
    }
    ui_line++;
}

uint8_t history_fixup[25] = {};
char buf[8];

int main() {
    global_setup();
    onewire_setup();
    onewire_DS18B20_setup();
    LCD_setup();
    MEM_setup();
    LCD_Background(LCDBlue);
    int xo = 25;
    LCD_Line(xo, LAYOUT_Y_OFFSET - 3, LCD_SCREEN_WIDTH - xo, LAYOUT_Y_OFFSET - 3, LCDWhite);
    LCD_Line(xo, LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET + 3 - 12, LCD_SCREEN_WIDTH - xo,
        LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET + 3 - 12, LCDWhite);
    LCD_Line(xo, LAYOUT_Y_OFFSET - 3, xo, LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET + 3 - 12, LCDWhite);
    LCD_Line(LCD_SCREEN_WIDTH - xo, LAYOUT_Y_OFFSET - 3, LCD_SCREEN_WIDTH - xo, LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET + 3 - 12,
        LCDWhite);

#ifdef MOCKED_EMBEDDED
    onewire_DS18B20_setConversionTime(100);
#endif
    uint8_t addr_buf[8], *addr = NULL;
    ui_line = 0;
    while(true) {
        int state = gather_device(addr_buf);
        if(state == SEARCH_ADDR_CRC_ERR) continue;
        addr = state == SEARCH_RESET ? NULL : addr_buf;
        int cmp = MEM_cmp(addr);
        while(cmp == 1) {
            feed_ui(SEARCH_MISSING);
            MEM_pop();
            cmp = MEM_cmp(addr);
        }
        if(addr != NULL && cmp == -1) {
            uint8_t *x;
            int ok = 1;
            for(x = history_fixup; *x != 0; ++x)
                if(addr[7] == *x) ok = 0;
            if(ok) {
                *x = addr[7];
                DEBUG("PUSH ");
                DEBUG_addr(addr);
                DEBUG("\r\n");
                MEM_push(addr);  // cmp => 0
            }
        }
        if(MEM_top() != NULL) feed_ui(state);
        MEM_pop();

        if(state == SEARCH_RESET) {
            if(ui_line == 0) LCD_Text(360 / 2 - 60, 0, "NO DEVICES", LCDRed);
            ui_line = 0;
            DEBUG("RESET ");
            DEBUG_addr(MEM_top());
            MEM_reset();
            DEBUG(" ");
            DEBUG_addr(MEM_top());
            DEBUG("\r\n");

            for(int j = 0; j < 12; j++) {
                for(int i = 0; i < LCD_SCREEN_WIDTH; i++) {
                    LCD_Pixel(i, LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET - 2 + j, LCDBlue);
                }
            }
            snprintf(buf, 8, "100/100");
            strlen(buf);
            LCD_Text(LCD_SCREEN_WIDTH / 2 - 8 / 2 * strlen(buf), LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET, buf, LCDWhite);
            snprintf(buf, 8, "%d", MEM_size());
            LCD_Text(LCD_SCREEN_WIDTH - 8 * strlen(buf) - 4, LCD_SCREEN_HEIGHT - LAYOUT_Y_OFFSET, buf, LCDWhite);
            continue;
        }
    }
}
