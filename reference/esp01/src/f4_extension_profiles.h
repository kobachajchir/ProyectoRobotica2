#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    F4_EXTENSION_PROFILE_NRF24 = 0x00u,
    F4_EXTENSION_PROFILE_BEEPER = 0x01u,
} F4_ExtensionProfileId;

typedef struct {
    uint8_t command_id;
    uint8_t payload[1];
    uint8_t payload_len;
} F4_ExtensionProfileRequest;

bool F4_ExtensionProfile_IsValid(uint8_t profile_id);
bool F4_ExtensionProfile_BuildGetRequest(F4_ExtensionProfileRequest *out);
bool F4_ExtensionProfile_BuildSetRequest(uint8_t profile_id,
                                        F4_ExtensionProfileRequest *out);
bool F4_ExtensionProfile_BuildRebootRequest(uint8_t profile_id,
                                           F4_ExtensionProfileRequest *out);
bool F4_ExtensionProfile_DecodeCurrent(const uint8_t *payload, uint8_t len,
                                       uint8_t *profile_id);
bool F4_ExtensionProfile_DecodeResult(const uint8_t *payload, uint8_t len,
                                      bool *success);

#ifdef __cplusplus
}
#endif
