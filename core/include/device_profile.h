/*
 * S5LAndBox — early multi-device bring-up registry.
 *
 * This catalog describes target hardware; it does NOT claim that each target's
 * SoC is emulated. In particular, iPod4,1 is catalogued for staged bring-up
 * while the current machine core remains S5L8900-only.
 *
 * Copyright (c) 2026 MarcioGames-pixel. MIT licensed.
 */
#ifndef S5LBOX_DEVICE_PROFILE_H
#define S5LBOX_DEVICE_PROFILE_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    S5L_SOC_S5L8900 = 1,
    S5L_SOC_S5L8930 = 2
} s5l_soc_family_t;

typedef enum {
    S5L_CPU_ARMV6 = 6,
    S5L_CPU_ARMV7 = 7
} s5l_cpu_family_t;

typedef struct {
    const char *identifier;       /* e.g. iPhone1,2 */
    const char *board_id;         /* Apple board identifier */
    const char *marketing_name;
    const char *soc_name;
    s5l_soc_family_t soc;
    s5l_cpu_family_t cpu;
    unsigned logical_width;
    unsigned logical_height;
    unsigned pixel_width;
    unsigned pixel_height;
    bool boot_supported;          /* true only for the currently implemented SoC */
} s5l_device_profile_t;

/* Returns the immutable catalog and optionally its element count. */
const s5l_device_profile_t *s5l_device_profiles(size_t *count);

/* Exact, case-sensitive lookup by Apple's hardware identifier. */
const s5l_device_profile_t *s5l_device_profile_find(const char *identifier);

/* True only when this tree has a machine implementation for the profile. */
bool s5l_device_profile_can_boot(const s5l_device_profile_t *profile);

#ifdef __cplusplus
}
#endif
#endif
