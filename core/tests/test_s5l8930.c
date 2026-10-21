#include "s5l8930.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    s5l8930_t soc;
    memset(&soc, 0, sizeof soc);

    assert(!s5l8930_init(NULL, S5L8930_DRAM_BASE, S5L8930_DRAM_DEFAULT));
    assert(!s5l8930_init(&soc, S5L8930_DRAM_BASE, 0u));
    assert(!s5l8930_init(&soc, 0u, 0x20000u));
    assert(!soc.initialized);

    assert(s5l8930_init(&soc, S5L8930_DRAM_BASE, S5L8930_DRAM_DEFAULT));
    assert(soc.initialized);
    assert(s5l8930_region_at(&soc, 0u) == S5L8930_REGION_BOOTROM);
    assert(s5l8930_region_at(&soc, S5L8930_BOOTROM_SIZE - 1u) ==
           S5L8930_REGION_BOOTROM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE) == S5L8930_REGION_DRAM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE +
                             S5L8930_DRAM_DEFAULT - 1u) ==
           S5L8930_REGION_DRAM);
    assert(s5l8930_region_at(&soc, S5L8930_DRAM_BASE +
                             S5L8930_DRAM_DEFAULT) ==
           S5L8930_REGION_UNMAPPED);

    s5l8930_reset(&soc);
    assert(!soc.initialized);
    assert(s5l8930_region_at(&soc, 0u) == S5L8930_REGION_UNMAPPED);
    return 0;
}
