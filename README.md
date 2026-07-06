# MyOS — A Hobby Operating System Built From Scratch

A minimal x86 operating system built from the ground up in C and x86 Assembly, without relying on any existing OS kernel or standard library. This project was built to understand how operating systems actually work — from boot to memory management.

## Features Implemented

- **Custom Cross-Compiler Toolchain** — GCC + Binutils built from source, targeting `i686-elf`
- **Multiboot-compliant bootloader integration** — boots via GRUB
- **VGA Text Mode Driver** — custom terminal with scrolling and color support
- **`kprintf`** — a hand-written `printf`-style formatted output function (`%d`, `%x`, `%s`, `%c`)
- **Global Descriptor Table (GDT)** — custom kernel/user code and data segments
- **Interrupt Descriptor Table (IDT)** — full CPU exception handling (divide-by-zero, invalid opcode, page fault, etc.)
- **PIC Remapping** — hardware interrupts properly mapped to avoid CPU exception conflicts
- **PS/2 Keyboard Driver** — live keyboard input via IRQ1
- **Physical Memory Manager (PMM)** — bitmap-based frame allocator using the Multiboot memory map
- **Paging** — virtual memory enabled with identity-mapped first 4MB
- **Heap Allocator (`kmalloc`/`kfree`)** — dynamic memory allocation with free-block reuse

## Screenshots

![Booting OS](screenshot1.png)
![Terminal Output](screenshot2.png)
![Memory Management](screenshot3.png)
![Screenshot 4](screenshot4.png)
![Screenshot 5](screenshot5.png)
![Screenshot 6](screenshot6.png)
![Screenshot 7](screenshot7.png)
![Screenshot 8](screenshot8.png)

## Project Structure
```text
myos/
├── boot/
│   ├── boot.s              # Entry point, multiboot header, stack setup
│   ├── gdt_flush.s          # Loads the GDT
│   ├── idt_flush.s          # Loads the IDT
│   ├── isr.s                 # CPU exception stubs (0-31)
│   ├── irq.s                 # Hardware interrupt stubs (32-47)
│   └── paging_enable.s       # Enables paging via CR0/CR3
├── kernel/
│   ├── kernel.c               # kernel_main, VGA terminal, kprintf
│   ├── gdt.c / gdt.h
│   ├── idt.c / idt.h
│   ├── isr.c
│   ├── pic.c / pic.h
│   ├── keyboard.c
│   ├── pmm.c / pmm.h
│   ├── paging.c / paging.h
│   ├── kmalloc.c / kmalloc.h
│   └── multiboot.h
├── linker.ld                  # Custom memory layout linker script
└── isodir/                    # GRUB ISO staging directory
```
## Building From Source

### Prerequisites

You need a custom cross-compiler targeting `i686-elf` (this cannot use your system GCC, since we're building a freestanding kernel).

```bash
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo grub-pc-bin grub-common xorriso qemu-system-x86
```

Cross-compiler setup (Binutils + GCC targeting `i686-elf`) is documented separately — see [OSDev Wiki: GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler).

### Build Steps

```bash
# Assemble
i686-elf-as boot/boot.s -o boot/boot.o
i686-elf-as boot/gdt_flush.s -o boot/gdt_flush.o
i686-elf-as boot/idt_flush.s -o boot/idt_flush.o
i686-elf-as boot/isr.s -o boot/isr.o
i686-elf-as boot/irq.s -o boot/irq.o
i686-elf-as boot/paging_enable.s -o boot/paging_enable.o

# Compile
i686-elf-gcc -c kernel/kernel.c -o kernel/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/gdt.c -o kernel/gdt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/idt.c -o kernel/idt.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/isr.c -o kernel/isr.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/pic.c -o kernel/pic.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/keyboard.c -o kernel/keyboard.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/pmm.c -o kernel/pmm.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/paging.c -o kernel/paging.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -c kernel/kmalloc.c -o kernel/kmalloc.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Link
i686-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib \
    boot/boot.o boot/gdt_flush.o boot/idt_flush.o boot/isr.o boot/irq.o boot/paging_enable.o \
    kernel/kernel.o kernel/gdt.o kernel/idt.o kernel/isr.o kernel/pic.o kernel/keyboard.o \
    kernel/pmm.o kernel/paging.o kernel/kmalloc.o -lgcc

# Create bootable ISO
cp myos.bin isodir/boot/myos.bin
grub-mkrescue -o myos.iso isodir
```

### Running

```bash
qemu-system-i386 -cdrom myos.iso
```

## Roadmap

- [ ] Backspace/Enter handling + basic shell
- [ ] PIT timer + basic multitasking
- [ ] Shift/Caps Lock keyboard support
- [ ] Filesystem support
- [ ] User mode processes

## Acknowledgments

Built while learning from the [OSDev Wiki](https://wiki.osdev.org) .

## License

MIT License 
