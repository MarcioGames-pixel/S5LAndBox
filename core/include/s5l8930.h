/*
 * S5LAndBox — Apple A4 / S5L8930 bring-up boundary.
 *
 * This module records the independently selected A4 platform identity and
 * known architectural windows. It intentionally does not alias S5L8900
 * peripherals or claim an iOS boot path until each device is modelled.
 */
#ifndef S5LBOX_S5L8930_H
#define S5LBOX_S5L8930_H

#include <stdbool.h>
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

typedef struct {
    uint32_t dram_base;
    uint32_t dram_size;
    bool initialized;
} s5l8930_t;

/* Minimal platform-state setup only; this does not execute guest code. */
bool s5l8930_init(s5l8930_t *soc, uint32_t dram_base, uint32_t dram_size);
void s5l8930_reset(s5l8930_t *soc);
s5l8930_region_t s5l8930_region_at(const s5l8930_t *soc, uint32_t address);

#ifdef __cplusplus
}
#endif
#endif
