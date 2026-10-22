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
    /* ARM: MOV r0, #42. Used as a tiny reset-vector execution probe. */
    rom[0] = 0x2au; rom[1] = 0x00u; rom[2] = 0xa0u; rom[3] = 0xe3u;

    assert(!s5l8930_machine_init(NULL, ram, sizeof ram, S5L8930_DRAM_BASE,
                                 sizeof ram, rom, sizeof rom));
    assert(s5l8930_machine_init(&machine, ram, sizeof ram, S5L8930_DRAM_BASE,
                                sizeof ram, rom, sizeof rom));
    assert(machine.initialized);
    assert(machine.cpu.bus == &machine.bus);
    assert(machine.cpu.arch == ARM_ARCH_V7_CORTEX_A8);
    assert(machine.bus.read32(machine.bus.ctx, 0u) == 0xe3a0002au);
    assert(machine.cpu.r[15] == 0u);
    assert(arm_step(&machine.cpu) == ARM_OK);
    assert(machine.cpu.r[0] == 42u);
    machine.bus.write32(machine.bus.ctx, S5L8930_DRAM_BASE, 0xaabbccddu);
    assert(machine.bus.read32(machine.bus.ctx, S5L8930_DRAM_BASE) == 0xaabbccddu);

    s5l8930_machine_reset(&machine);
    assert(machine.initialized);
    assert(machine.cpu.bus == &machine.bus);
    assert(machine.cpu.arch == ARM_ARCH_V7_CORTEX_A8);
    return 0;
}
