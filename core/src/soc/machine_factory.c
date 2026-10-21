#include "machine_factory.h"

const char *s5l_machine_select_status_name(s5l_machine_select_status_t status)
{
    switch (status) {
    case S5L_MACHINE_SELECT_OK:                  return "ok";
    case S5L_MACHINE_SELECT_INVALID_ARGUMENT:    return "invalid-argument";
    case S5L_MACHINE_SELECT_UNKNOWN_DEVICE:      return "unknown-device";
    case S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC:   return "unimplemented-soc";
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
