#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    F4_COMMAND_OWNER_F4 = 0,
    F4_COMMAND_OWNER_ESP,
    F4_COMMAND_OWNER_DUAL,
    F4_COMMAND_OWNER_RESERVED,
} F4_CommandOwner;

typedef enum {
    F4_RESPONSE_NONE = 0,
    F4_RESPONSE_ACK,
    F4_RESPONSE_SAME_COMMAND,
    F4_RESPONSE_ACK_THEN_SAME_COMMAND,
    F4_RESPONSE_EVENT,
    F4_RESPONSE_NACK_ONLY,
} F4_ResponsePolicy;

typedef enum {
    F4_PRIVILEGE_PUBLIC = 0,
    F4_PRIVILEGE_STM_SESSION,
    F4_PRIVILEGE_ESP_ADMIN,
    F4_PRIVILEGE_DIAGNOSTIC,
    F4_PRIVILEGE_INTERNAL,
} F4_CommandPrivilege;

typedef struct {
    uint8_t id;
    const char *name;
    F4_CommandOwner owner;
    uint8_t request_min_len;
    uint8_t request_max_len;
    uint8_t response_min_len;
    uint8_t response_max_len;
    F4_ResponsePolicy response_policy;
    uint16_t timeout_ms;
    bool idempotent;
    F4_CommandPrivilege privilege;
    bool implemented;
    bool sensitive;
} F4_CommandSpec;

/*
 * response_min_len/response_max_len describe a same-command response or event.
 * ACK/NACK payloads are transport control frames and are validated separately.
 */
const F4_CommandSpec *F4_CommandRegistry_Find(uint8_t id);
const F4_CommandSpec *F4_CommandRegistry_At(size_t index);
bool F4_CommandRegistry_Lookup(uint8_t id, F4_CommandSpec *out);
size_t F4_CommandRegistry_Count(void);

bool F4_CommandRegistry_IsRequestLengthValid(const F4_CommandSpec *spec, size_t len);
bool F4_CommandRegistry_IsResponseLengthValid(const F4_CommandSpec *spec, size_t len);
bool F4_CommandRegistry_IsWireAllowed(const F4_CommandSpec *spec);

#ifdef __cplusplus
}
#endif
