#include "kmalloc.h"

extern void kprintf(const char* format, ...);

struct block_header {
    size_t size;              /* size of the usable memory (excludes header) */
    int is_free;
    struct block_header* next;
};

static struct block_header* heap_start_block = NULL;
static uint32_t heap_end = 0;      /* current end of used heap region */
static uint32_t heap_limit = 0;    /* max address heap can grow to */

#define HEADER_SIZE sizeof(struct block_header)
#define ALIGN4(x) (((x) + 3) & ~3)

void kmalloc_init(uint32_t heap_start, uint32_t heap_size) {
    heap_start_block = (struct block_header*) heap_start;
    heap_start_block->size = 0;
    heap_start_block->is_free = 0;
    heap_start_block->next = NULL;

    heap_end = heap_start + HEADER_SIZE;
    heap_limit = heap_start + heap_size;

    kprintf("Heap initialized: start=%x limit=%x\n", heap_start, heap_limit);
}

static struct block_header* find_free_block(size_t size) {
    struct block_header* current = heap_start_block;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void* kmalloc(size_t size) {
    size = ALIGN4(size);

    /* Try to reuse a freed block first */
    struct block_header* reuse = find_free_block(size);
    if (reuse) {
        reuse->is_free = 0;
        return (void*)((uint32_t)reuse + HEADER_SIZE);
    }

    /* Otherwise, bump-allocate a new block */
    uint32_t needed = HEADER_SIZE + size;
    if (heap_end + needed > heap_limit) {
        kprintf("kmalloc: OUT OF MEMORY (requested %d bytes)\n", (int)size);
        return NULL;
    }

    struct block_header* new_block = (struct block_header*) heap_end;
    new_block->size = size;
    new_block->is_free = 0;
    new_block->next = NULL;

    /* Link it into the list (find the last block) */
    struct block_header* current = heap_start_block;
    while (current->next) {
        current = current->next;
    }
    current->next = new_block;

    heap_end += needed;

    return (void*)((uint32_t)new_block + HEADER_SIZE);
}

void kfree(void* ptr) {
    if (!ptr) return;

    struct block_header* block = (struct block_header*)((uint32_t)ptr - HEADER_SIZE);
    block->is_free = 1;
}
