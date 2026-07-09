CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Ikernel/include
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

BOOT_SRCS = boot/boot.s boot/gdt_flush.s boot/idt_flush.s boot/isr.s boot/irq.s boot/paging_enable.s
BOOT_OBJS = $(BOOT_SRCS:.s=.o)

KERNEL_SRCS = $(wildcard kernel/src/*.c)
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)

all: myos.iso

%.o: %.s
	$(AS) $< -o $@

kernel/src/%.o: kernel/src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

myos.bin: $(BOOT_OBJS) $(KERNEL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

myos.iso: myos.bin
	cp myos.bin isodir/boot/myos.bin
	grub-mkrescue -o myos.iso isodir

run: myos.iso
	qemu-system-i386 -cdrom myos.iso

clean:
	rm -f $(BOOT_OBJS) $(KERNEL_OBJS) myos.bin myos.iso isodir/boot/myos.bin

.PHONY: all run clean
