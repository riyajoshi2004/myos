#include <stdint.h>
#include "paging.h"

extern void kprintf(const char* format, ...);

#define PAGE_PRESENT 0x1
#define PAGE_WRITE   0x2

/* Must be 4KB aligned */
static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t first_page_table[1024] __attribute__((aligned(4096)));

extern void paging_enable(uint32_t page_directory_addr);

void paging_init(void) {
    /* Identity map the first 4MB (0x00000000 - 0x00400000) */
    for (uint32_t i = 0; i < 1024; i++) {
        first_page_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
    }

    /* Clear the page directory */
    for (uint32_t i = 0; i < 1024; i++) {
        page_directory[i] = 0;  /* not present */
    }

    /* First entry points to our identity-mapped page table */
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_WRITE;

    paging_enable((uint32_t)page_directory);

    kprintf("Paging enabled: identity-mapped first 4MB\n");
}
