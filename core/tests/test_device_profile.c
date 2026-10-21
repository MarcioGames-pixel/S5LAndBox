#include "device_profile.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

int main(void)
{
    size_t count = 0;
    const s5l_device_profile_t *profiles = s5l_device_profiles(&count);
    assert(profiles != NULL);
    assert(count == 2u);

    const s5l_device_profile_t *iphone = s5l_device_profile_find("iPhone1,2");
    assert(iphone != NULL);
    assert(strcmp(iphone->board_id, "N82AP") == 0);
    assert(iphone->soc == S5L_SOC_S5L8900);
    assert(iphone->cpu == S5L_CPU_ARMV6);
    assert(iphone->logical_width == 320u && iphone->logical_height == 480u);
    assert(s5l_device_profile_can_boot(iphone));

    const s5l_device_profile_t *ipod = s5l_device_profile_find("iPod4,1");
    assert(ipod != NULL);
    assert(strcmp(ipod->board_id, "N81AP") == 0);
    assert(ipod->soc == S5L_SOC_S5L8930);
    assert(ipod->cpu == S5L_CPU_ARMV7);
    assert(ipod->logical_width == 320u && ipod->logical_height == 480u);
    assert(ipod->pixel_width == 640u && ipod->pixel_height == 960u);
    assert(!s5l_device_profile_can_boot(ipod));

    assert(s5l_device_profile_find("iPhone1,1") == NULL);
    assert(s5l_device_profile_find(NULL) == NULL);
    assert(!s5l_device_profile_can_boot(NULL));
    return 0;
}
