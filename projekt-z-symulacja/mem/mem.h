#include "../global/global.h"
#ifndef __MEM_H
#define __MEM_H

#define MEM_SIZE 256

/// initialize the memory from the disk
void MEM_setup();
/// get the current address
const uint8_t *MEM_top();
/// go to next address
void MEM_pop();
/// push a new address to the memory in this place, prev<addr<top
void MEM_push(const uint8_t addr[8]);
/// remove the address from the memory, implicit reset
void MEM_remove(const uint8_t addr[8]);
/// if there were new adresses added, sync them with the memory
void MEM_reset();
/// drop all adresses from memory
void MEM_drop();
/// print all adresses from memory
void MEM_print();
/// get the number of adresses in memory
size_t MEM_size();
/// get the index of the current address
size_t MEM_idx();

#endif
