#include "mem.h"

#define MEM_SIZE 256

struct node {
    uint8_t addr[8];
    struct node *next;
} MEM[MEM_SIZE], *head, *current, *prev;

size_t size;
size_t idx;

void MEM_setup() {
    MEM_drop();
}

const uint8_t *MEM_top() {
    return current == NULL ? NULL : current->addr;
}

void MEM_pop() {
    if(current == NULL) return;
    prev = current;
    current = current->next;
    ++idx;
}

void MEM_push(const uint8_t addr[8]) {
    struct node *new_node = MEM;
    for(; new_node->addr[0] != 0; ++new_node);
    memcpy(new_node->addr, addr, 8);
    new_node->next = current;
    if(prev == NULL) head = new_node;
    else
        prev->next = new_node;
    current = new_node;
    ++size;
}

void MEM_remove(const uint8_t addr[8]) {
    // TODO: implement and write tests
}

void MEM_reset() {
    prev = NULL;
    current = head;
    idx = 0;
}

void MEM_drop() {
    memset(MEM, 0, sizeof(MEM));
    head = prev = current = NULL;
    size = idx = 0;
}

void MEM_print() {
    for(struct node *n = head; n != NULL; n = n->next) {
        for(int i = 0; i < 8; i++) printf("%02X%s", n->addr[i], i < 7 ? ":" : "->");
    }
    printf("NULL\n");
}

size_t MEM_size() {
    return size;
}

size_t MEM_idx() {
    return idx;
}

#ifdef LIB_TEST
void DEBUG(uint8_t *str) {
    printf("%s", str);
}

void printf_addr(const uint8_t addr[8]) {
    if(addr == NULL) printf("NULL");
    for(int i = 0; i < 8; i++) {
        printf("%02X", addr[i]);
        if(i < 7) printf(":");
    }
}

void assert(const uint8_t *exp, const uint8_t *got, size_t line) {
    if(exp == NULL && got == NULL) return;
    if(exp == NULL || got == NULL || memcmp(exp, got, 8) != 0) {
        printf("assertion failed at line %zu\n", line);
        printf("expected: ");
        printf_addr(exp);
        printf("\n");
        printf("got: ");
        printf_addr(got);
        printf("\n");
        exit(1);
    }
}

void assert_int(size_t exp, size_t got, size_t line) {
    if(exp != got) {
        printf("assertion failed at line %zu\n", line);
        printf("expected: %zu\n", exp);
        printf("got: %zu\n", got);
        exit(1);
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
    printf("All tests passed.\n");
    return 0;
}
#endif
