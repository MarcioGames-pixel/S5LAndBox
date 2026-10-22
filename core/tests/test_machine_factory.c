#include "machine_factory.h"
#include "s5l8930.h"

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

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

    /* Restore.plist commonly spells BoardConfig as lower-case n82ap. */
    profile = NULL;
    assert(s5l_machine_select_firmware_profile("iPhone1,2", "n82ap",
                                                &profile) ==
           S5L_MACHINE_SELECT_OK);
    assert(profile != NULL && strcmp(profile->board_id, "N82AP") == 0);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_select_firmware_profile("iPhone1,2", "N81AP",
                                                &profile) ==
           S5L_MACHINE_SELECT_BOARD_MISMATCH);
    assert(profile == NULL);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_select_firmware_profile("iPod4,1", "n81ap",
                                                &profile) ==
           S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC);
    assert(profile == NULL);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_select_firmware_profile("iPhone1,2", NULL,
                                                &profile) ==
           S5L_MACHINE_SELECT_INVALID_ARGUMENT);
    assert(profile == NULL);

    assert(s5l_machine_init_for_device("iPod4,1", NULL, 0x08000000u,
                                       0x08000000u, &profile) ==
           S5L_MACHINE_SELECT_INVALID_ARGUMENT);
    assert(profile == NULL);

    /*
     * A known-but-unimplemented target must be rejected before touching the
     * machine object. Sentinel bytes make this an integration test for the
     * factory's fail-closed behavior, not just a profile lookup test.
     */
    static s5l8900_t untouched_machine;
    memset(&untouched_machine, 0xa5, sizeof untouched_machine);
    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_init_for_device("iPod4,1", &untouched_machine,
                                       0x08000000u, 0x08000000u,
                                       &profile) ==
           S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC);
    assert(profile == NULL);
    const unsigned char *machine_bytes =
        (const unsigned char *)&untouched_machine;
    assert(machine_bytes[0] == 0xa5);
    assert(machine_bytes[sizeof untouched_machine - 1u] == 0xa5);

    /* A mismatched board must likewise be rejected before initialization. */
    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_init_for_firmware("iPhone1,2", "n81ap",
                                         &untouched_machine,
                                         0x08000000u, 0x08000000u,
                                         &profile) ==
           S5L_MACHINE_SELECT_BOARD_MISMATCH);
    assert(profile == NULL);
    assert(machine_bytes[0] == 0xa5);
    assert(machine_bytes[sizeof untouched_machine - 1u] == 0xa5);

    /* The experimental scaffold can now be initialized explicitly without
     * routing iPod4,1 into the S5L8900 machine or claiming it can boot iOS. */
    s5l8930_t a4;
    memset(&a4, 0x5a, sizeof a4);
    profile = NULL;
    assert(s5l_machine_init_s5l8930_scaffold_for_device(
               "iPod4,1", &a4, S5L8930_DRAM_BASE, 0x08000000u,
               &profile) == S5L_MACHINE_SELECT_OK);
    assert(profile != NULL && strcmp(profile->identifier, "iPod4,1") == 0);
    assert(a4.initialized);
    assert(a4.dram_base == S5L8930_DRAM_BASE);
    assert(a4.dram_size == 0x08000000u);

    profile = (const s5l_device_profile_t *)1;
    assert(s5l_machine_init_s5l8930_scaffold_for_firmware(
               "iPod4,1", "N82AP", &a4, S5L8930_DRAM_BASE,
               0x08000000u, &profile) ==
           S5L_MACHINE_SELECT_BOARD_MISMATCH);
    assert(profile == NULL);

    profile = NULL;
    assert(s5l_machine_init_s5l8930_scaffold_for_firmware(
               "iPod4,1", "n81ap", &a4, S5L8930_DRAM_BASE,
               0u, &profile) ==
           S5L_MACHINE_SELECT_MACHINE_INIT_FAILED);
    assert(profile == NULL);

    assert(strcmp(s5l_machine_select_status_name(
                      S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC),
                  "unimplemented-soc") == 0);
    assert(strcmp(s5l_machine_select_status_name(
                      S5L_MACHINE_SELECT_BOARD_MISMATCH),
                  "board-mismatch") == 0);
    return 0;
}
