#include "device_profile.h"

#include <string.h>

/*
 * Resolution fields distinguish UIKit points from physical panel pixels.
 * The iPod touch 4's Retina panel is 640x960 pixels but its logical canvas is
 * 320x480 points, matching the current guest input coordinate system.
 *
 * S5L8930 is identified as a separate SoC family. No S5L8930 MMIO map or
 * boot path is implemented yet, so iPod4,1 deliberately remains non-bootable.
 */
static const s5l_device_profile_t PROFILES[] = {
    {
        "iPhone1,2", "N82AP", "iPhone 3G",
        "Samsung S5L8900", S5L_SOC_S5L8900, S5L_CPU_ARMV6,
        320u, 480u, 320u, 480u, true
    },
    {
        "iPod4,1", "N81AP", "iPod touch (4th generation)",
        "Apple A4 / S5L8930", S5L_SOC_S5L8930, S5L_CPU_ARMV7,
        320u, 480u, 640u, 960u, false
    }
};

const s5l_device_profile_t *s5l_device_profiles(size_t *count)
{
    if (count) *count = sizeof PROFILES / sizeof PROFILES[0];
    return PROFILES;
}

const s5l_device_profile_t *s5l_device_profile_find(const char *identifier)
{
    if (!identifier || !identifier[0]) return NULL;

    for (size_t i = 0; i < sizeof PROFILES / sizeof PROFILES[0]; ++i) {
        if (strcmp(PROFILES[i].identifier, identifier) == 0)
            return &PROFILES[i];
    }
    return NULL;
}

bool s5l_device_profile_can_boot(const s5l_device_profile_t *profile)
{
    return profile && profile->boot_supported &&
           profile->soc == S5L_SOC_S5L8900;
}
