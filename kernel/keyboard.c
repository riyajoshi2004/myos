#include <stdint.h>
#include "pic.h"

extern void terminal_putchar(char c);
extern void kprintf(const char* format, ...);

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* US QWERTY scancode set 1 -> ASCII (unshifted) */
static char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,
    '-', 0,0,0,
    '+', 0,0,0,0,0,0,0,0,0,0,0,0,0
};

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    /* Ignore key releases (top bit set) */
    if (scancode & 0x80) {
        return;
    }

    if (scancode < 128) {
        char c = scancode_to_ascii[scancode];
        if (c) {
            terminal_putchar(c);
        }
    }
}

struct registers_irq {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

void irq_handler(struct registers_irq regs) {
    if (regs.int_no == 33) {
        keyboard_handler();
    }

    pic_send_eoi((uint8_t)(regs.int_no - 32));
}
