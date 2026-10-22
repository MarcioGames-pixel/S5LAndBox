#include "display_profile.h"
#include <assert.h>
#include <stddef.h>

int main(void) {
    const s5l_device_profile_t *iphone = s5l_device_profile_find("iPhone1,2");
    const s5l_device_profile_t *ipod = s5l_device_profile_find("iPod4,1");
    uint32_t x = 0, y = 0;
    assert(iphone && ipod);
    assert(s5l_display_map_logical_pixel(iphone, 319, 479, &x, &y));
    assert(x == 319 && y == 479);
    assert(s5l_display_map_logical_pixel(ipod, 1, 1, &x, &y));
    assert(x == 2 && y == 2);
    assert(s5l_display_map_logical_pixel(ipod, 319, 479, &x, &y));
    assert(x == 638 && y == 958);
    x = 123; y = 456;
    assert(!s5l_display_map_logical_pixel(ipod, 320, 0, &x, &y));
    assert(x == 123 && y == 456);
    assert(!s5l_display_map_logical_pixel(NULL, 0, 0, &x, &y));

    /* Retina panel coordinates map back to the same logical touch cell. */
    assert(s5l_display_map_physical_pixel(ipod, 2, 3, &x, &y));
    assert(x == 1 && y == 1);
    assert(s5l_display_map_physical_pixel(ipod, 639, 959, &x, &y));
    assert(x == 319 && y == 479);
    assert(s5l_display_map_physical_pixel(iphone, 319, 479, &x, &y));
    assert(x == 319 && y == 479);
    x = 77; y = 88;
    assert(!s5l_display_map_physical_pixel(ipod, 640, 0, &x, &y));
    assert(x == 77 && y == 88);
    assert(!s5l_display_map_physical_pixel(NULL, 0, 0, &x, &y));
    return 0;
}