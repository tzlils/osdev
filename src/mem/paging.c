#include <stdint.h>
#include <mem/paging.h>

extern void enable_paging();
extern void load_page_table();

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute((aligned(4096)));

void blank_page_directory() {
    int i;
    for(i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002;
    }
}

void blank_table() {
    unsigned int i;
    for(i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
    }
}