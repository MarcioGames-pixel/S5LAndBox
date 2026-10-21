#include "machine_factory.h"

static unsigned char ascii_lower(unsigned char c)
{
    if (c >= (unsigned char)'A' && c <= (unsigned char)'Z')
        return (unsigned char)(c + ((unsigned char)'a' - (unsigned char)'A'));
    return c;
}

static int board_id_equal(const char *a, const char *b)
{
    if (!a || !b || !a[0] || !b[0]) return 0;
    while (*a && *b) {
        if (ascii_lower((unsigned char)*a) !=
            ascii_lower((unsigned char)*b))
            return 0;
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}

const char *s5l_machine_select_status_name(s5l_machine_select_status_t status)
{
    switch (status) {
    case S5L_MACHINE_SELECT_OK:                  return "ok";
    case S5L_MACHINE_SELECT_INVALID_ARGUMENT:    return "invalid-argument";
    case S5L_MACHINE_SELECT_UNKNOWN_DEVICE:      return "unknown-device";
    case S5L_MACHINE_SELECT_BOARD_MISMATCH:      return "board-mismatch";
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

s5l_machine_select_status_t
s5l_machine_select_firmware_profile(const char *identifier,
                                    const char *board_id,
                                    const s5l_device_profile_t **out_profile)
{
    if (out_profile) *out_profile = NULL;
    if (!identifier || !identifier[0] || !board_id || !board_id[0] ||
        !out_profile)
        return S5L_MACHINE_SELECT_INVALID_ARGUMENT;

    const s5l_device_profile_t *profile =
        s5l_device_profile_find(identifier);
    if (!profile)
        return S5L_MACHINE_SELECT_UNKNOWN_DEVICE;

    if (!board_id_equal(profile->board_id, board_id))
        return S5L_MACHINE_SELECT_BOARD_MISMATCH;

    if (!s5l_device_profile_can_boot(profile))
        return S5L_MACHINE_SELECT_UNIMPLEMENTED_SOC;

    *out_profile = profile;
    return S5L_MACHINE_SELECT_OK;
}

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

s5l_machine_select_status_t
s5l_machine_init_for_firmware(const char *identifier, const char *board_id,
                              s5l8900_t *machine, uint32_t ram_base,
                              uint32_t ram_size,
                              const s5l_device_profile_t **out_profile)
{
    if (out_profile) *out_profile = NULL;
    if (!machine || !out_profile)
        return S5L_MACHINE_SELECT_INVALID_ARGUMENT;

    const s5l_device_profile_t *profile = NULL;
    s5l_machine_select_status_t status =
        s5l_machine_select_firmware_profile(identifier, board_id, &profile);
    if (status != S5L_MACHINE_SELECT_OK)
        return status;

    if (!s5l8900_init(machine, ram_base, ram_size))
        return S5L_MACHINE_SELECT_MACHINE_INIT_FAILED;

    *out_profile = profile;
    return S5L_MACHINE_SELECT_OK;
}
