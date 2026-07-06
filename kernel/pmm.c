#include "pmm.h"

extern void kprintf(const char* format, ...);

#define FRAME_SIZE 4096
#define MAX_FRAMES (1024 * 1024)  /* Supports up to 4GB RAM (1M frames * 4KB) */

static uint32_t bitmap[MAX_FRAMES / 32];
static uint32_t total_frames = 0;
static uint32_t used_frames = 0;

static void bitmap_set(uint32_t frame) {
    bitmap[frame / 32] |= (1 << (frame % 32));
}

static void bitmap_clear(uint32_t frame) {
    bitmap[frame / 32] &= ~(1 << (frame % 32));
}

static int bitmap_test(uint32_t frame) {
    return bitmap[frame / 32] & (1 << (frame % 32));
}

void pmm_init(struct multiboot_info* mbi, uint32_t kernel_end) {
    /* Step 1: mark ALL frames as used by default (safe default) */
    for (uint32_t i = 0; i < MAX_FRAMES / 32; i++) {
        bitmap[i] = 0xFFFFFFFF;
    }
    used_frames = MAX_FRAMES;
    total_frames = 0;

    /* Step 2: walk the multiboot memory map and mark AVAILABLE regions as free */
    struct multiboot_mmap_entry* entry = (struct multiboot_mmap_entry*)(uintptr_t)mbi->mmap_addr;
    uint32_t mmap_end = mbi->mmap_addr + mbi->mmap_length;

    while ((uint32_t)(uintptr_t)entry < mmap_end) {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t start = entry->addr;
            uint64_t length = entry->len;
            uint32_t start_frame = (uint32_t)(start / FRAME_SIZE);
            uint32_t frame_count = (uint32_t)(length / FRAME_SIZE);

            for (uint32_t f = start_frame; f < start_frame + frame_count && f < MAX_FRAMES; f++) {
                if (bitmap_test(f)) {
                    bitmap_clear(f);
                    used_frames--;
                }
                if (f + 1 > total_frames) total_frames = f + 1;
            }
        }
        entry = (struct multiboot_mmap_entry*)((uint32_t)(uintptr_t)entry + entry->size + sizeof(entry->size));
    }

    /* Step 3: reserve the first 1MB (BIOS, VGA, etc.) and the kernel itself */
    uint32_t kernel_end_frame = (kernel_end / FRAME_SIZE) + 1;
    for (uint32_t f = 0; f < kernel_end_frame; f++) {
        if (!bitmap_test(f)) {
            bitmap_set(f);
            used_frames++;
        }
    }

    kprintf("PMM initialized: %d total frames, %d used, %d free\n",
            (int)total_frames, (int)used_frames, (int)(total_frames - used_frames));
}

uint32_t pmm_alloc_frame(void) {
    for (uint32_t i = 0; i < total_frames; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            used_frames++;
            return i * FRAME_SIZE;
        }
    }
    return 0;  /* Out of memory */
}

void pmm_free_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / FRAME_SIZE;
    if (bitmap_test(frame)) {
        bitmap_clear(frame);
        used_frames--;
    }
}

uint32_t pmm_get_free_frame_count(void) {
    return total_frames - used_frames;
}
