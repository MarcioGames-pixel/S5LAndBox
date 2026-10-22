#ifndef S5LBOX_S5L8930_MACHINE_H
#define S5LBOX_S5L8930_MACHINE_H

#include "arm.h"
#include "s5l8930.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    s5l8930_t soc;
    arm_cpu_t cpu;
    arm_bus_t bus;
    bool initialized;
} s5l8930_machine_t;

/* Caller owns DRAM and optional ROM buffers; no memory is allocated here.
 * This is an experimental CPU+memory harness, not a complete Apple A4 model. */
bool s5l8930_machine_init(s5l8930_machine_t *machine,
                          uint8_t *dram, size_t dram_backing_size,
                          uint32_t dram_base, uint32_t dram_size,
                          const uint8_t *bootrom, size_t bootrom_size);
void s5l8930_machine_reset(s5l8930_machine_t *machine);

#ifdef __cplusplus
}
#endif
#endif
