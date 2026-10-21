#include "s5l8930.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    s5l8930_t soc;
    uint8_t dram[256];
    uint8_t rom[16];
    memset(&soc, 0, sizeof soc);
    memset(dram, 0, sizeof dram);
    for (size_t i = 0; i < sizeof rom; ++i) rom[i] = (uint8_t)(0xa0u + i);

    assert(!s5l8930_init(NULL, S5L8930_DRAM_BASE, sizeof dram));
    assert(!s5l8930_init(&soc, S5L8930_DRAM_BASE, 0u));
    assert(!s5l8930_init(&soc, 0u, 0x20000u));
    assert(!soc.initialized);

    assert(s5l8930_init(&soc, S5L8930_DRAM_BASE, sizeof dram));
    assert(soc.initialized);
    assert(s5l8930_region_at(&soc, 0u) == S5L8930_REGION_BOOTROM);
    assert(s5l8930_region_at(&soc, S5L8930_BOOTROM_SIZE - 1u) ==
           S5L8930_REGION_BOOTROM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE) == S5L8930_REGION_DRAM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE + sizeof dram - 1u) ==
           S5L8930_REGION_DRAM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE + sizeof dram) ==
           S5L8930_REGION_UNMAPPED);

    assert(s5l8930_read8(&soc, S5L8930_DRAM_BASE, &rom[0]) ==
           S5L8930_ACCESS_NO_BACKING);
    assert(s5l8930_attach_memory(&soc, dram, sizeof dram, rom, sizeof rom) ==
           S5L8930_ACCESS_OK);

    assert(s5l8930_write8(&soc, S5L8930_DRAM_BASE, 0x12u) == S5L8930_ACCESS_OK);
    assert(s5l8930_write16(&soc, S5L8930_DRAM_BASE + 2u, 0x3456u) ==
           S5L8930_ACCESS_OK);
    assert(s5l8930_write32(&soc, S5L8930_DRAM_BASE + 4u, 0x789abcdeu) ==
           S5L8930_ACCESS_OK);
    uint8_t b = 0u; uint16_t h = 0u; uint32_t w = 0u;
    assert(s5l8930_read8(&soc, S5L8930_DRAM_BASE, &b) == S5L8930_ACCESS_OK);
    assert(b == 0x12u);
    assert(s5l8930_read16(&soc, S5L8930_DRAM_BASE + 2u, &h) == S5L8930_ACCESS_OK);
    assert(h == 0x3456u);
    assert(s5l8930_read32(&soc, S5L8930_DRAM_BASE + 4u, &w) == S5L8930_ACCESS_OK);
    assert(w == 0x789abcdeu);

    assert(s5l8930_read32(&soc, S5L8930_BOOTROM_BASE, &w) == S5L8930_ACCESS_OK);
    assert(w == 0xa3a2a1a0u);
    assert(s5l8930_write8(&soc, S5L8930_BOOTROM_BASE, 1u) ==
           S5L8930_ACCESS_READ_ONLY);
    assert(s5l8930_read32(&soc, S5L8930_BOOTROM_SIZE - 2u, &w) ==
           S5L8930_ACCESS_NO_BACKING);
    assert(s5l8930_read32(&soc, S5L8930_DRAM_BASE + sizeof dram - 2u, &w) ==
           S5L8930_ACCESS_UNMAPPED);
    assert(s5l8930_read8(&soc, 0x20000000u, &b) == S5L8930_ACCESS_UNMAPPED);
    assert(s5l8930_read32(&soc, UINT32_MAX - 1u, &w) ==
           S5L8930_ACCESS_UNMAPPED);
    assert(s5l8930_write32(&soc, S5L8930_DRAM_BASE + 1u, 0x12345678u) ==
           S5L8930_ACCESS_OK);

    s5l8930_reset(&soc);
    assert(!soc.initialized);
    assert(s5l8930_region_at(&soc, 0u) == S5L8930_REGION_UNMAPPED);
    assert(s5l8930_read8(&soc, S5L8930_DRAM_BASE, &b) ==
           S5L8930_ACCESS_INVALID_ARGUMENT);
    return 0;
}
