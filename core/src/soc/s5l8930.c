#include "s5l8930.h"

#include <stddef.h>

bool s5l8930_init(s5l8930_t *soc, uint32_t dram_base, uint32_t dram_size)
{
    if (!soc || dram_size == 0u ||
        dram_base > UINT32_MAX - (dram_size - 1u))
        return false;

    /* Do not accept DRAM overlapping the boot-ROM aperture. */
    const uint32_t dram_last = dram_base + dram_size - 1u;
    const uint32_t rom_last = S5L8930_BOOTROM_BASE + S5L8930_BOOTROM_SIZE - 1u;
    if (dram_base <= rom_last && dram_last >= S5L8930_BOOTROM_BASE)
        return false;

    soc->dram_base = dram_base;
    soc->dram_size = dram_size;
    soc->initialized = true;
    return true;
}

void s5l8930_reset(s5l8930_t *soc)
{
    if (!soc) return;
    soc->initialized = false;
    soc->dram_base = 0u;
    soc->dram_size = 0u;
}

s5l8930_region_t s5l8930_region_at(const s5l8930_t *soc, uint32_t address)
{
    if (!soc || !soc->initialized) return S5L8930_REGION_UNMAPPED;
    if (address >= S5L8930_BOOTROM_BASE &&
        address < S5L8930_BOOTROM_BASE + S5L8930_BOOTROM_SIZE)
        return S5L8930_REGION_BOOTROM;
    if (address >= soc->dram_base &&
        (uint64_t)address < (uint64_t)soc->dram_base + soc->dram_size)
        return S5L8930_REGION_DRAM;
    return S5L8930_REGION_UNMAPPED;
}
