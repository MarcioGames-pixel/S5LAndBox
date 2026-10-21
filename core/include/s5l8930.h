/*
 * S5LAndBox — Apple A4 / S5L8930 bring-up boundary.
 *
 * This is an experimental memory-bus scaffold, not a complete SoC emulator.
 * Address constants remain provisional until checked against iPod4,1 firmware.
 */
#ifndef S5LBOX_S5L8930_H
#define S5LBOX_S5L8930_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define S5L8930_DRAM_BASE       0x40000000u
#define S5L8930_DRAM_DEFAULT   0x10000000u
#define S5L8930_BOOTROM_BASE   0x00000000u
#define S5L8930_BOOTROM_SIZE   0x00010000u

typedef enum {
    S5L8930_REGION_UNMAPPED = 0,
    S5L8930_REGION_BOOTROM,
    S5L8930_REGION_DRAM
} s5l8930_region_t;

typedef enum {
    S5L8930_ACCESS_OK = 0,
    S5L8930_ACCESS_INVALID_ARGUMENT,
    S5L8930_ACCESS_UNMAPPED,
    S5L8930_ACCESS_READ_ONLY,
    S5L8930_ACCESS_NO_BACKING
} s5l8930_access_status_t;

typedef struct {
    uint32_t dram_base;
    uint32_t dram_size;
    uint8_t *dram;
    size_t dram_backing_size;
    const uint8_t *bootrom;
    size_t bootrom_backing_size;
    bool initialized;
} s5l8930_t;

/* Minimal platform state; this does not create a CPU or execute guest code. */
bool s5l8930_init(s5l8930_t *soc, uint32_t dram_base, uint32_t dram_size);
void s5l8930_reset(s5l8930_t *soc);
s5l8930_region_t s5l8930_region_at(const s5l8930_t *soc, uint32_t address);

/* Attach caller-owned backing buffers. No memory is allocated or freed here.
 * DRAM backing must cover the entire configured DRAM range. A boot-ROM image
 * may be shorter than its aperture; accesses beyond its supplied size fail. */
s5l8930_access_status_t
s5l8930_attach_memory(s5l8930_t *soc, uint8_t *dram, size_t dram_size,
                      const uint8_t *bootrom, size_t bootrom_size);

/* Little-endian byte/word accesses. Unmapped and unbacked reads fail closed;
 * boot-ROM writes are rejected. 16/32-bit accesses must be fully in one region. */
s5l8930_access_status_t
s5l8930_read8(const s5l8930_t *soc, uint32_t address, uint8_t *value);
s5l8930_access_status_t
s5l8930_read16(const s5l8930_t *soc, uint32_t address, uint16_t *value);
s5l8930_access_status_t
s5l8930_read32(const s5l8930_t *soc, uint32_t address, uint32_t *value);
s5l8930_access_status_t
s5l8930_write8(s5l8930_t *soc, uint32_t address, uint8_t value);
s5l8930_access_status_t
s5l8930_write16(s5l8930_t *soc, uint32_t address, uint16_t value);
s5l8930_access_status_t
s5l8930_write32(s5l8930_t *soc, uint32_t address, uint32_t value);
const char *s5l8930_access_status_name(s5l8930_access_status_t status);

#ifdef __cplusplus
}
#endif
#endif
