#include "f4_extension_profiles.h"

#include <stddef.h>

#include "uner_cmdspec.h"

bool F4_ExtensionProfile_IsValid(uint8_t profile_id)
{
    return profile_id == F4_EXTENSION_PROFILE_NRF24 ||
           profile_id == F4_EXTENSION_PROFILE_BEEPER;
}

static bool build_selection_request(uint8_t command_id, uint8_t profile_id,
                                    F4_ExtensionProfileRequest *out)
{
    if (out == NULL || !F4_ExtensionProfile_IsValid(profile_id)) return false;
    out->command_id = command_id;
    out->payload[0] = profile_id;
    out->payload_len = 1u;
    return true;
}

bool F4_ExtensionProfile_BuildGetRequest(F4_ExtensionProfileRequest *out)
{
    if (out == NULL) return false;
    out->command_id = UNER_CMD_GET_EXTENSION_PROFILE;
    out->payload[0] = 0u;
    out->payload_len = 0u;
    return true;
}

bool F4_ExtensionProfile_BuildSetRequest(uint8_t profile_id,
                                        F4_ExtensionProfileRequest *out)
{
    return build_selection_request(UNER_CMD_SET_EXTENSION_PROFILE,
                                   profile_id, out);
}

bool F4_ExtensionProfile_BuildRebootRequest(uint8_t profile_id,
                                           F4_ExtensionProfileRequest *out)
{
    return build_selection_request(UNER_CMD_REBOOT_INTO_PROFILE,
                                   profile_id, out);
}

bool F4_ExtensionProfile_DecodeCurrent(const uint8_t *payload, uint8_t len,
                                       uint8_t *profile_id)
{
    if (payload == NULL || profile_id == NULL || len != 2u ||
        payload[0] != 0u || !F4_ExtensionProfile_IsValid(payload[1])) return false;
    *profile_id = payload[1];
    return true;
}

bool F4_ExtensionProfile_DecodeResult(const uint8_t *payload, uint8_t len,
                                      bool *success)
{
    if (payload == NULL || success == NULL || len != 1u || payload[0] > 3u)
        return false;
    *success = payload[0] == 0u;
    return true;
}
