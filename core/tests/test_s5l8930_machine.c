#include "s5l8930_machine.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    s5l8930_machine_t machine;
    uint8_t ram[256];
    uint8_t rom[16];
    memset(ram, 0, sizeof ram);
    memset(rom, 0, sizeof rom);
    rom[0] = 0x78u; rom[1] = 0x56u; rom[2] = 0x34u; rom[3] = 0x12u;

    assert(!s5l8930_machine_init(NULL, ram, sizeof ram, S5L8930_DRAM_BASE,
                                 sizeof ram, rom, sizeof rom));
    assert(s5l8930_machine_init(&machine, ram, sizeof ram, S5L8930_DRAM_BASE,
                                sizeof ram, rom, sizeof rom));
    assert(machine.initialized);
    assert(machine.cpu.bus == &machine.bus);
    assert(machine.cpu.arch == ARM_ARCH_V7_CORTEX_A8);
    assert(machine.bus.read32(machine.bus.ctx, 0u) == 0x12345678u);
    machine.bus.write32(machine.bus.ctx, S5L8930_DRAM_BASE, 0xaabbccddu);
    assert(machine.bus.read32(machine.bus.ctx, S5L8930_DRAM_BASE) == 0xaabbccddu);

    s5l8930_machine_reset(&machine);
    assert(machine.initialized);
    assert(machine.cpu.bus == &machine.bus);
    assert(machine.cpu.arch == ARM_ARCH_V7_CORTEX_A8);
    return 0;
}
