#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE 256

void MEM_setup();                        /// initialize the memory from the disk
const uint8_t *MEM_top();                /// get the current address
void MEM_pop();                          /// go to next address
void MEM_push(const uint8_t addr[8]);    /// push a new address to the memory in this place, prev<addr<top
void MEM_remove(const uint8_t addr[8]);  /// remove the address from the memory, implicit reset
void MEM_reset();                        /// if there were new adresses added, sync them with the memory
void MEM_drop();                         /// drop all adresses from memory
void MEM_print();                        /// print all adresses from memory
size_t MEM_size();                       /// get the number of adresses in memory
size_t MEM_idx();                        /// get the index of the current address
