#include "mem.h"
#define MEM_SIZE 256

struct node {
    uint8_t addr[8];
    struct node *next;
} MEM[MEM_SIZE], *head, *current, *prev;

size_t size;
size_t idx;

#ifdef MOCKED_EMBEDDED
void fram_load() {
    // TODO
}
void fram_save() {
    // TODO
}
void fram_erase() {
    // TODO
}
#else
void fram_load() {}
void fram_save() {}
void fram_erase() {}
#endif

void MEM_setup() {
    memset(MEM, 0, sizeof(MEM));
    head = prev = current = NULL;
    size = idx = 0;

    fram_load();
}

void MEM_reset() {
    prev = NULL;
    current = head;
    idx = 0;

    if(false /**TODO: if needed */) fram_save();
}

void MEM_drop() {
    memset(MEM, 0, sizeof(MEM));
    head = prev = current = NULL;
    size = idx = 0;
    fram_erase();
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
    MEM_reset();  // save
}

void MEM_print() {
    for(struct node *n = head; n != NULL; n = n->next) { DEBUG_addr(n->addr); }
    DEBUG("NULL\r\n");
}

size_t MEM_size() {
    return size;
}

size_t MEM_idx() {
    return idx;
}

