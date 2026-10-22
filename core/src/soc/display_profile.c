#include "display_profile.h"

bool s5l_display_map_logical_pixel(const s5l_device_profile_t *profile,
                                   uint32_t logical_x, uint32_t logical_y,
                                   uint32_t *pixel_x, uint32_t *pixel_y)
{
    if (!profile || !pixel_x || !pixel_y ||
        profile->logical_width == 0u || profile->logical_height == 0u ||
        profile->pixel_width == 0u || profile->pixel_height == 0u ||
        logical_x >= profile->logical_width ||
        logical_y >= profile->logical_height)
        return false;

    *pixel_x = (uint32_t)(((uint64_t)logical_x * profile->pixel_width) /
                          profile->logical_width);
    *pixel_y = (uint32_t)(((uint64_t)logical_y * profile->pixel_height) /
                          profile->logical_height);
    return true;
}

bool s5l_display_map_physical_pixel(const s5l_device_profile_t *profile,
                                    uint32_t pixel_x, uint32_t pixel_y,
                                    uint32_t *logical_x, uint32_t *logical_y)
{
    if (!profile || !logical_x || !logical_y ||
        profile->logical_width == 0u || profile->logical_height == 0u ||
        profile->pixel_width == 0u || profile->pixel_height == 0u ||
        pixel_x >= profile->pixel_width || pixel_y >= profile->pixel_height)
        return false;

    *logical_x = (uint32_t)(((uint64_t)pixel_x * profile->logical_width) /
                            profile->pixel_width);
    *logical_y = (uint32_t)(((uint64_t)pixel_y * profile->logical_height) /
                            profile->pixel_height);
    return true;
}
