#include "s5l8930_machine.h"

#include <string.h>

static uint32_t bus_read32(void *ctx, uint32_t address)
{
    uint32_t value = 0u;
    (void)s5l8930_read32(&((s5l8930_machine_t *)ctx)->soc, address, &value);
    return value;
}
static uint16_t bus_read16(void *ctx, uint32_t address)
{
    uint16_t value = 0u;
    (void)s5l8930_read16(&((s5l8930_machine_t *)ctx)->soc, address, &value);
    return value;
}
static uint8_t bus_read8(void *ctx, uint32_t address)
{
    uint8_t value = 0u;
    (void)s5l8930_read8(&((s5l8930_machine_t *)ctx)->soc, address, &value);
    return value;
}
static void bus_write32(void *ctx, uint32_t address, uint32_t value)
{ (void)s5l8930_write32(&((s5l8930_machine_t *)ctx)->soc, address, value); }
static void bus_write16(void *ctx, uint32_t address, uint16_t value)
{ (void)s5l8930_write16(&((s5l8930_machine_t *)ctx)->soc, address, value); }
static void bus_write8(void *ctx, uint32_t address, uint8_t value)
{ (void)s5l8930_write8(&((s5l8930_machine_t *)ctx)->soc, address, value); }

bool s5l8930_machine_init(s5l8930_machine_t *machine,
                          uint8_t *dram, size_t dram_backing_size,
                          uint32_t dram_base, uint32_t dram_size,
                          const uint8_t *bootrom, size_t bootrom_size)
{
    if (!machine || !dram) return false;
    memset(machine, 0, sizeof(*machine));
    if (!s5l8930_init(&machine->soc, dram_base, dram_size)) return false;
    if (s5l8930_attach_memory(&machine->soc, dram, dram_backing_size,
                              bootrom, bootrom_size) != S5L8930_ACCESS_OK) {
        memset(machine, 0, sizeof(*machine));
        return false;
    }

    machine->bus.ctx = machine;
    machine->bus.read32 = bus_read32;
    machine->bus.read16 = bus_read16;
    machine->bus.read8 = bus_read8;
    machine->bus.write32 = bus_write32;
    machine->bus.write16 = bus_write16;
    machine->bus.write8 = bus_write8;
    arm_reset(&machine->cpu, &machine->bus);
    machine->cpu.arch = ARM_ARCH_V7_CORTEX_A8;
    machine->initialized = true;
    return true;
}

void s5l8930_machine_reset(s5l8930_machine_t *machine)
{
    if (!machine || !machine->initialized) return;
    arm_reset(&machine->cpu, &machine->bus);
    machine->cpu.arch = ARM_ARCH_V7_CORTEX_A8;
}
