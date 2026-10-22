/*
 * S5LAndBox — explicit device-to-machine dispatch.
 *
 * The dispatcher is intentionally fail-closed: a device profile is not proof
 * that its SoC is implemented. Never construct an S5L8900 machine for an A4
 * target merely because both products run ARM-family code.
 *
 * Copyright (c) 2026 MarcioGames-pixel. MIT licensed.
 */
#ifndef S5LBOX_MACHINE_FACTORY_H
#define S5LBOX_MACHINE_FACTORY_H

#include "soc.h"
#include "device_profile.h"
#include "s5l8930.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    S5L_MACHINE_SELECT_OK = 0,
    S5L_MACHINE_SELECT_INVALID_ARGUMENT,
    S5L_MACHINE_SELECT_UNKNOWN_DEVICE,
    S5L_MACHINE_SELECT_BOARD_MISMATCH,
    S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC,
    S5L_MACHINE_SELECT_MACHINE_INIT_FAILED
} s5l_machine_select_status_t;

/*
 * Resolve an explicit Apple hardware identifier to a profile that this tree
 * can actually boot. On failure, *out_profile is set to NULL. This API does
 * not allocate a machine or modify guest RAM.
 */
s5l_machine_select_status_t
s5l_machine_select_profile(const char *identifier,
                           const s5l_device_profile_t **out_profile);

/*
 * Resolve a firmware manifest identity. ProductType must be a known hardware
 * identifier and BoardConfig must match its board ID (ASCII case-insensitive,
 * as Restore.plist commonly stores lower-case board names). A mismatch is
 * rejected before any machine state or guest RAM is touched.
 */
s5l_machine_select_status_t
s5l_machine_select_firmware_profile(const char *identifier,
                                    const char *board_id,
                                    const s5l_device_profile_t **out_profile);

const char *s5l_machine_select_status_name(s5l_machine_select_status_t status);

/* Initialize only the experimental S5L8930 platform scaffold. This does not
 * make the target bootable and must not be used as a substitute for a complete
 * machine backend. The returned profile is descriptive, not boot approval. */
s5l_machine_select_status_t
s5l_machine_init_s5l8930_scaffold_for_device(
    const char *identifier, s5l8930_t *soc, uint32_t dram_base,
    uint32_t dram_size, const s5l_device_profile_t **out_profile);

s5l_machine_select_status_t
s5l_machine_init_s5l8930_scaffold_for_firmware(
    const char *identifier, const char *board_id, s5l8930_t *soc,
    uint32_t dram_base, uint32_t dram_size,
    const s5l_device_profile_t **out_profile);

/* Resolve the identifier and initialize its implemented machine backend.
 * The output profile is optional; on any failure it is set to NULL. */
s5l_machine_select_status_t
s5l_machine_init_for_device(const char *identifier, s5l8900_t *machine,
                            uint32_t ram_base, uint32_t ram_size,
                            const s5l_device_profile_t **out_profile);

/* Firmware-aware counterpart: validates ProductType AND BoardConfig before
 * calling the S5L8900 initializer. */
s5l_machine_select_status_t
s5l_machine_init_for_firmware(const char *identifier, const char *board_id,
                              s5l8900_t *machine, uint32_t ram_base,
                              uint32_t ram_size,
                              const s5l_device_profile_t **out_profile);

#ifdef __cplusplus
}
#endif
#endif
