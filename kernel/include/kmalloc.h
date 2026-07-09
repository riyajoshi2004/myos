#ifndef KMALLOC_H
#define KMALLOC_H

#include <stdint.h>
#include <stddef.h>

void kmalloc_init(uint32_t heap_start, uint32_t heap_size);
void* kmalloc(size_t size);
void kfree(void* ptr);

#endif
