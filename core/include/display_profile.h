/*
 * S5LAndBox — host presentation mapping for device display profiles.
 *
 * This is not a panel/SoC emulator: guest scanout remains in logical guest
 * coordinates. The helper maps a logical pixel to the physical panel pixel
 * covered by that logical pixel (top-left convention), for frontends that
 * present Retina-class panels.
 */
#ifndef S5LBOX_DISPLAY_PROFILE_H
#define S5LBOX_DISPLAY_PROFILE_H

#include <stdbool.h>
#include <stdint.h>
#include "device_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Map one in-bounds logical pixel to its top-left physical pixel.
 * Returns false for null arguments, zero dimensions, or out-of-range input.
 * Uses 64-bit intermediates so profile dimensions cannot overflow arithmetic.
 */
bool s5l_display_map_logical_pixel(const s5l_device_profile_t *profile,
                                   uint32_t logical_x, uint32_t logical_y,
                                   uint32_t *pixel_x, uint32_t *pixel_y);

/*
 * Convert a physical panel coordinate back to its logical guest coordinate.
 * Physical coordinates must be in bounds; output pointers are unchanged on
 * failure. This is intended for mapping touch input back to the guest UI.
 */
bool s5l_display_map_physical_pixel(const s5l_device_profile_t *profile,
                                    uint32_t pixel_x, uint32_t pixel_y,
                                    uint32_t *logical_x, uint32_t *logical_y);

#ifdef __cplusplus
}
#endif
#endif
