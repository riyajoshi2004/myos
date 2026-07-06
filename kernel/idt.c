#include "idt.h"

#define IDT_ENTRIES 256

struct idt_entry idt[IDT_ENTRIES];
struct idt_ptr ip;

extern void idt_flush(uint32_t);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector  = selector;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_install(void) {
    ip.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    ip.base  = (uint32_t)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_flush((uint32_t)&ip);
}
