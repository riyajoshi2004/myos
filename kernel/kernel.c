#include <stdint.h>
#include <stddef.h>

void kernel_main(void) {
    const char *str = "Hello, kernel World!";
    uint16_t *vga = (uint16_t*) 0xB8000;

    for (size_t i = 0; str[i] != '\0'; i++) {
        vga[i] = (uint16_t) str[i] | (uint16_t) 0x0F00;
    }

    for (;;) {}
}
