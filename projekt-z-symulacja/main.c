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
#define LAYOUT_X_LEFT_OFFSET 32
#define LAYOUT_X_RIGHT_OFFSET (LCD_SCREEN_WIDTH - 32)
#define LAYOUT_Y_TOP_OFFSET 22
#define LAYOUT_Y_BOTTOM_OFFSET (LCD_SCREEN_HEIGHT - (LCD_SCREEN_HEIGHT - (LAYOUT_Y_TOP_OFFSET + 12 * 16 + 6)) / 2 - 4)

void border_layout() {
    int x1 = LAYOUT_X_LEFT_OFFSET - 3;
    int x2 = LAYOUT_X_RIGHT_OFFSET + 3;
    int y1 = LAYOUT_Y_TOP_OFFSET - 3;
    int y2 = LAYOUT_Y_TOP_OFFSET + 12 * 16 + 3;
    LCD_Line(x1, y1, x2, y1, LCDWhite);
    LCD_Line(x1, y1, x1, y2, LCDWhite);
    LCD_Line(x2, y1, x2, y2, LCDWhite);
    LCD_Line(x1, y2, x2, y2, LCDWhite);
}

void prompt_no_devices() {
    LCD_Text(LCD_SCREEN_WIDTH / 2 - 60, LAYOUT_Y_TOP_OFFSET, "NO DEVICES", LCDRed);
}

char layout_buf[8];

int last_mem_size = 0;
int error_device = 0;

void prompt_error_device(const uint8_t addr[8]) {
    for(int j = 0; j < 12; j++) {
        for(int i = 0; i < 200; i++) { LCD_Pixel(i, LAYOUT_Y_BOTTOM_OFFSET - 1 + j, LCDBlue); }
    }
    if(addr == NULL) { return; }
    LCD_Addr(4, LAYOUT_Y_BOTTOM_OFFSET, addr, LCDRed);
    error_device++;
    snprintf(layout_buf, 8, "%d", error_device);
    LCD_Text(20 * 8 + 8, LAYOUT_Y_BOTTOM_OFFSET, layout_buf, LCDRed);
}

void prompt_pages(int dev_idx) {
    for(int j = 0; j < 12; j++) {
        for(int i = 200; i < LCD_SCREEN_WIDTH; i++) { LCD_Pixel(i, LAYOUT_Y_BOTTOM_OFFSET - 1 + j, LCDBlue); }
    }
    for(int j = 0; j < 12; j++) {
        for(int i = 100; i < LCD_SCREEN_WIDTH - 100; i++) { LCD_Pixel(i, 3 + j, LCDBlue); }
    }
    int page = dev_idx / 16 + 1, allpages = (MEM_size() + 15) / 16;
    snprintf(layout_buf, 8, "%d/%d", page, allpages);
    LCD_Text(LCD_SCREEN_WIDTH / 2 - 8 / 2 * strlen(layout_buf), 3, layout_buf, LCDWhite);

		int size = MEM_size();
		DEBUG("---------------");
		DEBUG_int(size);
		DEBUG("---------------");
    snprintf(layout_buf, 8, dev_idx == last_mem_size - 1 ? "/%u" : size == last_mem_size ? "%u" : "+%u", size);
    LCD_Text(LCD_SCREEN_WIDTH - 8 * strlen(layout_buf) - 4, LAYOUT_Y_BOTTOM_OFFSET, layout_buf, LCDWhite);
}

void on_last_page() {
    if(MEM_size() == 0) LCD_Text(360 / 2 - 60, 0, "NO DEVICES", LCDRed);
    prompt_pages(MEM_idx() - 1);

    for(int ui = ui_line; ui < 16; ui++) {
        for(int j = 0; j < 12; j++) {
            for(int i = LAYOUT_X_LEFT_OFFSET; i < LAYOUT_X_RIGHT_OFFSET; i++) {
                LCD_Pixel(i, ui * 12 - 1 + j + LAYOUT_Y_TOP_OFFSET, LCDBlue);
            }
        }
    }
}

#define O 12
uint8_t buf_f[10];

void feed_ui(int state) {
    DEBUG("FEED: ");

    if(ui_line == 16) {
        ui_line = 0;
        prompt_pages(MEM_idx() - 1);
        delay_ms(500);
        prompt_pages(MEM_idx());
    }

    int X = 20 * 8 + LAYOUT_X_LEFT_OFFSET, Y = ui_line * 12 + LAYOUT_Y_TOP_OFFSET;
    for(int j = 0; j < 12; j++) {
        for(int i = LAYOUT_X_LEFT_OFFSET; i < LAYOUT_X_RIGHT_OFFSET; i++) LCD_Pixel(i, Y - 1 + j, LCDBlue);
    }
    if(state == SEARCH_GOOD) LCD_Addr(LAYOUT_X_LEFT_OFFSET, Y, MEM_top(), LCDWhite);
    else
        LCD_Addr(LAYOUT_X_LEFT_OFFSET, Y, MEM_top(), LCDRed);
    switch(state) {
        case SEARCH_ADDR_CRC_ERR:
            DEBUG(" ADDR CRC ERR\r\n");
            LCD_Text(X, Y, "ADDR CRC ERR", LCDRed);
            break;
        case SEARCH_BUS_ERR:
            DEBUG(" BUS ERR\r\n");
            LCD_Text(X + 4, Y, "  BUS ERR", LCDRed);
            break;
        case SEARCH_DATA_CRC_ERR:
            DEBUG(" DATA CRC ERR\r\n");
            LCD_Text(X, Y, "DATA CRC ERR", LCDRed);
            break;
        case SEARCH_MISSING:
            DEBUG(" MISSING\r\n");
            LCD_Text(X + 4, Y, "  MISSING", LCDRed);
            break;
        case SEARCH_GOOD:
            DEBUG(" GOOD\r\n");
            sprintf(buf_f, "%6.2f", onewire_DS18B20_get_celsius());
            LCD_Text(X + O, Y, buf_f, LCDWhite);
            LCD_Char(X + O + 8 * 6, Y, '\x7f', LCDWhite);
            LCD_Char(X + O + 8 * 6 + 7, Y, 'C', LCDWhite);
            break;
        default:
            DEBUG("unreachable!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
            DEBUG_int(state);
            DEBUG("\r\n");
    }
    if(state != SEARCH_GOOD) prompt_error_device(MEM_top());
    ui_line++;
}

uint8_t history_fixup[25] = {};

int main() {
    global_setup();
    onewire_setup();
    onewire_DS18B20_setup();
    LCD_setup();
    MEM_setup();

    LCD_Background(LCDBlue);
    border_layout();

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
#ifndef MOCKED_EMBEDDED
            for(x = history_fixup; *x != 0; ++x)
                if(addr[7] == *x) ok = 0;
            if(ok) {
                *x = addr[7];
#else
            if(true) {
#endif
                DEBUG("PUSH ");
                DEBUG_addr(addr);
                DEBUG("\r\n");
                MEM_push(addr);  // cmp => 0
            }
        }
        if(MEM_top() != NULL) feed_ui(state);
        MEM_pop();

        if(state == SEARCH_RESET) {
            DEBUG("RESET ");
            DEBUG_addr(MEM_top());
            if(MEM_size() == 0) prompt_no_devices();
            last_mem_size = MEM_size();
            on_last_page();
            if(MEM_size() > 16) delay_ms(500);
            if(error_device == 0) prompt_error_device(NULL);
            error_device = 0;
            prompt_pages(0);
            ui_line = 0;
            MEM_reset();
            DEBUG(" ");
            DEBUG_addr(MEM_top());
            DEBUG("\r\n");
        }
    }
}
/// zamiast czerwonego czerwonego boxa to adres
/// liczba zepsutych
