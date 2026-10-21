#include "machine_factory.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    const s5l_device_profile_t *profile = (const s5l_device_profile_t *)1;

    assert(s5l_machine_select_profile("iPhone1,2", &profile) ==
           S5L_MACHINE_SELECT_OK);
    assert(profile != NULL);
    assert(strcmp(profile->identifier, "iPhone1,2") == 0);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_select_profile("iPod4,1", &profile) ==
           S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC);
    assert(profile == NULL);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_select_profile("iPhone99,9", &profile) ==
           S5L_MACHINE_SELECT_UNKNOWN_DEVICE);
    assert(profile == NULL);

    assert(s5l_machine_select_profile(NULL, &profile) ==
           S5L_MACHINE_SELECT_INVALID_ARGUMENT);
    assert(profile == NULL);
    assert(s5l_machine_select_profile("iPhone1,2", NULL) ==
           S5L_MACHINE_SELECT_INVALID_ARGUMENT);
    assert(strcmp(s5l_machine_select_status_name(
                      S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC),
                  "unimplemented-soc") == 0);
    return 0;
}
