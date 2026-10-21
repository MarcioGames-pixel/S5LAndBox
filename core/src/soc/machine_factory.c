#include "machine_factory.h"

const char *s5l_machine_select_status_name(s5l_machine_select_status_t status)
{
    switch (status) {
    case S5L_MACHINE_SELECT_OK:                  return "ok";
    case S5L_MACHINE_SELECT_INVALID_ARGUMENT:    return "invalid-argument";
    case S5L_MACHINE_SELECT_UNKNOWN_DEVICE:      return "unknown-device";
    case S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC:   return "unimplemented-soc";
    case S5L_MACHINE_SELECT_MACHINE_INIT_FAILED: return "machine-init-failed";
    }
    return "invalid-status";
}

s5l_machine_select_status_t
s5l_machine_select_profile(const char *identifier,
                           const s5l_device_profile_t **out_profile)
{
    if (out_profile) *out_profile = NULL;
    if (!identifier || !identifier[0] || !out_profile)
        return S5L_MACHINE_SELECT_INVALID_ARGUMENT;

    const s5l_device_profile_t *profile =
        s5l_device_profile_find(identifier);
    if (!profile)
        return S5L_MACHINE_SELECT_UNKNOWN_DEVICE;

    if (!s5l_device_profile_can_boot(profile))
        return S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC;

    *out_profile = profile;
    return S5L_MACHINE_SELECT_OK;
}

/*
 * End-to-end entry point for frontends: do not let UI selection bypass the
 * capability gate and call the S5L8900 initializer for an unrelated device.
 */
s5l_machine_select_status_t
s5l_machine_init_for_device(const char *identifier, s5l8900_t *machine,
                            uint32_t ram_base, uint32_t ram_size,
                            const s5l_device_profile_t **out_profile)
{
    if (out_profile) *out_profile = NULL;
    if (!machine || !out_profile)
        return S5L_MACHINE_SELECT_INVALID_ARGUMENT;

    const s5l_device_profile_t *profile = NULL;
    s5l_machine_select_status_t status =
        s5l_machine_select_profile(identifier, &profile);
    if (status != S5L_MACHINE_SELECT_OK)
        return status;

    if (!s5l8900_init(machine, ram_base, ram_size))
        return S5L_MACHINE_SELECT_MACHINE_INIT_FAILED;

    *out_profile = profile;
    return S5L_MACHINE_SELECT_OK;
}
