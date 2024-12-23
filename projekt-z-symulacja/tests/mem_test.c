#include "mem/mem.h"

void assert(const uint8_t *exp, const uint8_t *got, size_t line) {
    if(exp == NULL && got == NULL) return;
    if(exp == NULL || got == NULL || memcmp(exp, got, 8) != 0) {
        DEBUG("assertion failed at line ");
        DEBUG_int(line);
        DEBUG("\r\nexpected: ");
        DEBUG_addr(exp);
        DEBUG("\r\ngot: ");
        DEBUG_addr(got);
        DEBUG("\r\n");
        DEBUG_halt();
    }
}

void assert_int(size_t exp, size_t got, size_t line) {
    if(exp != got) {
        DEBUG("assertion failed at line ");
        DEBUG_int(line);
        DEBUG("\r\nexpected: ");
        DEBUG_int(exp);
        DEBUG("\r\ngot: ");
        DEBUG_int(got);
        DEBUG("\r\n");
        DEBUG_halt();
    }
}

void asserted_state(uint8_t *exp, size_t idx, size_t line) {
    assert(exp, MEM_top(), line);
    assert_int(idx, MEM_idx(), line);
}

void asserted_push(uint8_t addr[8], size_t idx, size_t line) {
    MEM_push(addr);
    assert(addr, MEM_top(), line);
    assert_int(idx, MEM_idx(), line);
}

int main() {
    global_setup();
    MEM_setup();
    asserted_state(NULL, 0, __LINE__);
    asserted_push("\xd7\xef\xd4\xc0\xb5\x0d\x87\x62", 0, __LINE__);
    asserted_push("\x7e\xbe\x6b\x30\xe1\xaf\xf7\xfc", 0, __LINE__);
    asserted_push("\x67\xa3\x54\xfb\xe1\x3a\x63\xd0", 0, __LINE__);
    asserted_push("\x21\x2b\x61\xf2\x19\x54\x83\xc4", 0, __LINE__);
    asserted_push("\x09\x3e\xcb\x50\xda\x1a\x9b\xf1", 0, __LINE__);
    MEM_pop();
    asserted_state("\x21\x2b\x61\xf2\x19\x54\x83\xc4", 1, __LINE__);
    MEM_pop();
    asserted_state("\x67\xa3\x54\xfb\xe1\x3a\x63\xd0", 2, __LINE__);
    MEM_pop();
    asserted_state("\x7e\xbe\x6b\x30\xe1\xaf\xf7\xfc", 3, __LINE__);
    MEM_pop();
    asserted_state("\xd7\xef\xd4\xc0\xb5\x0d\x87\x62", 4, __LINE__);
    MEM_pop();
    asserted_state(NULL, 5, __LINE__);
    MEM_pop();
    asserted_state(NULL, 5, __LINE__);
    MEM_reset();
    asserted_state("\x09\x3e\xcb\x50\xda\x1a\x9b\xf1", 0, __LINE__);
    MEM_pop();
    asserted_state("\x21\x2b\x61\xf2\x19\x54\x83\xc4", 1, __LINE__);
    MEM_pop();
    asserted_state("\x67\xa3\x54\xfb\xe1\x3a\x63\xd0", 2, __LINE__);
    MEM_reset();
    asserted_state("\x09\x3e\xcb\x50\xda\x1a\x9b\xf1", 0, __LINE__);
    MEM_pop();
    asserted_state("\x21\x2b\x61\xf2\x19\x54\x83\xc4", 1, __LINE__);
    asserted_push("\x1f\x06\x7a\x07\xbf\x61\x16\x8f", 1, __LINE__);
    MEM_pop();
    asserted_state("\x21\x2b\x61\xf2\x19\x54\x83\xc4", 2, __LINE__);
    MEM_pop();
    asserted_state("\x67\xa3\x54\xfb\xe1\x3a\x63\xd0", 3, __LINE__);
    MEM_reset();
    asserted_state("\x09\x3e\xcb\x50\xda\x1a\x9b\xf1", 0, __LINE__);
    MEM_pop();
    asserted_state("\x1f\x06\x7a\x07\xbf\x61\x16\x8f", 1, __LINE__);
    MEM_print();
    MEM_drop();
    asserted_state(NULL, 0, __LINE__);
    DEBUG("+------------------+\r\n");
    DEBUG("| All tests passed.|\r\n");
    DEBUG("+------------------+\r\n");
    return 0;
}
