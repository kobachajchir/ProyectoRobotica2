#include "f4_command_registry.h"

#include <string.h>

#if defined(ARDUINO_ARCH_ESP8266) || defined(ESP8266)
#include <pgmspace.h>
#define F4_REGISTRY_STORAGE PROGMEM
#define F4_REGISTRY_NAME(_name) NULL
#define F4_REGISTRY_COPY(_dst, _src) memcpy_P((_dst), (_src), sizeof(F4_CommandSpec))
#else
#define F4_REGISTRY_STORAGE
#define F4_REGISTRY_NAME(_name) (_name)
#define F4_REGISTRY_COPY(_dst, _src) memcpy((_dst), (_src), sizeof(F4_CommandSpec))
#endif

#define SPEC(_id, _name, _owner, _req_min, _req_max, _resp_min, _resp_max, _policy, _timeout, _idem, _priv, _impl, _sensitive) \
    { (_id), F4_REGISTRY_NAME(_name), (_owner), (_req_min), (_req_max), (_resp_min), (_resp_max), (_policy), (_timeout), (_idem), (_priv), (_impl), (_sensitive) }

#define RESERVED(_id) \
    SPEC((_id), "RESERVED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false)

static const F4_CommandSpec kF4Commands[] F4_REGISTRY_STORAGE = {
    SPEC(0x10u, "SET_MODE_AP", F4_COMMAND_OWNER_ESP, 0u, 0u, 1u, 255u, F4_RESPONSE_SAME_COMMAND, 300u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x11u, "SET_MODE_STA", F4_COMMAND_OWNER_ESP, 0u, 0u, 1u, 255u, F4_RESPONSE_SAME_COMMAND, 300u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x12u, "SET_CREDENTIALS", F4_COMMAND_OWNER_ESP, 0u, 255u, 1u, 255u, F4_RESPONSE_SAME_COMMAND, 300u, false, F4_PRIVILEGE_ESP_ADMIN, true, true),
    SPEC(0x13u, "CLEAR_CREDENTIALS", F4_COMMAND_OWNER_ESP, 0u, 0u, 1u, 1u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x14u, "START_SCAN", F4_COMMAND_OWNER_ESP, 0u, 0u, 2u, 255u, F4_RESPONSE_SAME_COMMAND, 200u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x15u, "GET_SCAN_RESULTS", F4_COMMAND_OWNER_ESP, 0u, 255u, 2u, 255u, F4_RESPONSE_SAME_COMMAND, 100u, true, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x16u, "REBOOT_ESP", F4_COMMAND_OWNER_ESP, 1u, 1u, 0u, 0u, F4_RESPONSE_ACK, 50u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x17u, "FACTORY_RESET", F4_COMMAND_OWNER_ESP, 0u, 0u, 1u, 1u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 200u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x18u, "STOP_SCAN", F4_COMMAND_OWNER_ESP, 0u, 0u, 1u, 2u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x19u, "RESET_MCU", F4_COMMAND_OWNER_F4, 0u, 0u, 0u, 0u, F4_RESPONSE_ACK, 250u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x1Au, "WIFI_GET_DETAIL", F4_COMMAND_OWNER_ESP, 1u, 36u, 4u, 36u, F4_RESPONSE_SAME_COMMAND, 150u, true, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x1Bu, "SET_AP_FALLBACK", F4_COMMAND_OWNER_ESP, 1u, 1u, 2u, 2u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 1000u, true, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x1Cu, "GET_AP_FALLBACK", F4_COMMAND_OWNER_F4, 0u, 0u, 3u, 3u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false), 
    SPEC(0x1Du, "LINE_CALIBRATION_CONTROL", F4_COMMAND_OWNER_F4, 5u, 5u, 168u, 168u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x1Eu, "GET_LINE_STATE", F4_COMMAND_OWNER_F4, 0u, 0u, 168u, 168u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x1Fu, "SET_LINE_FOLLOW", F4_COMMAND_OWNER_F4, 1u, 1u, 168u, 168u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    RESERVED(0x20u), RESERVED(0x21u), RESERVED(0x22u),
    SPEC(0x23u, "stm.goto.get", F4_COMMAND_OWNER_F4, 0u, 0u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x24u, "stm.goto.start", F4_COMMAND_OWNER_F4, 8u, 8u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x25u, "stm.goto.cancel", F4_COMMAND_OWNER_F4, 0u, 0u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x26u, "stm.goto.irCalibration.set", F4_COMMAND_OWNER_F4, 186u, 186u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x27u, "stm.motorCalibration.get", F4_COMMAND_OWNER_F4, 0u, 0u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x28u, "stm.motorCalibration.control", F4_COMMAND_OWNER_F4, 13u, 13u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x29u, "stm.motorCalibration.profile.get", F4_COMMAND_OWNER_F4, 0u, 0u, 88u, 88u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x2au, "stm.motorCalibration.profile.set", F4_COMMAND_OWNER_F4, 88u, 88u, 88u, 88u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
     SPEC(0x2Bu, "MPU_CALIBRATION_REQUEST", F4_COMMAND_OWNER_F4, 0u, 0u, 128u, 128u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false), RESERVED(0x2Cu), RESERVED(0x2Du), RESERVED(0x2Eu), RESERVED(0x2Fu),
    SPEC(0x30u, "GET_STATUS", F4_COMMAND_OWNER_ESP, 0u, 255u, 18u, 255u, F4_RESPONSE_SAME_COMMAND, 100u, true, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x31u, "PING_ALIVE", F4_COMMAND_OWNER_DUAL, 0u, 16u, 0u, 16u, F4_RESPONSE_SAME_COMMAND, 150u, true, F4_PRIVILEGE_PUBLIC, true, false),
    RESERVED(0x32u), RESERVED(0x33u), RESERVED(0x34u), RESERVED(0x35u), RESERVED(0x36u), RESERVED(0x37u), RESERVED(0x38u), RESERVED(0x39u),
    RESERVED(0x3Au), RESERVED(0x3Bu), RESERVED(0x3Cu), RESERVED(0x3Du), RESERVED(0x3Eu), RESERVED(0x3Fu),
    SPEC(0x40u, "GET_PREFERENCES", F4_COMMAND_OWNER_F4, 0u, 1u, 1u, 255u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x41u, "REQUEST_FIRMWARE", F4_COMMAND_OWNER_ESP, 0u, 64u, 1u, 33u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 200u, true, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x42u, "ECHO", F4_COMMAND_OWNER_F4, 0u, 255u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x43u, "SET_ENCODER_FAST", F4_COMMAND_OWNER_F4, 1u, 1u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_STM_SESSION, false, false),
    SPEC(0x44u, "GET_CONNECTED_USERS", F4_COMMAND_OWNER_ESP, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, true, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0x45u, "GET_USER_INFO", F4_COMMAND_OWNER_ESP, 1u, 1u, 0u, 0u, F4_RESPONSE_NONE, 0u, true, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0x46u, "GET_INTERFACES_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, true, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0x47u, "GET_CREDENTIALS", F4_COMMAND_OWNER_ESP, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, true, F4_PRIVILEGE_ESP_ADMIN, false, true),
    SPEC(0x48u, "CONNECT_WIFI", F4_COMMAND_OWNER_ESP, 0u, 255u, 5u, 5u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 300u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x49u, "DISCONNECT_WIFI", F4_COMMAND_OWNER_ESP, 0u, 255u, 1u, 1u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x4Au, "SET_AP_CONFIG", F4_COMMAND_OWNER_ESP, 8u, 102u, 1u, 1u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 200u, false, F4_PRIVILEGE_ESP_ADMIN, true, true),
    SPEC(0x4Bu, "START_AP", F4_COMMAND_OWNER_ESP, 0u, 255u, 5u, 5u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 300u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x4Cu, "STOP_AP", F4_COMMAND_OWNER_ESP, 0u, 255u, 1u, 1u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x4Du, "GET_CONNECTED_USERS_MODE", F4_COMMAND_OWNER_ESP, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, true, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0x4Eu, "SET_AUTO_RECONNECT", F4_COMMAND_OWNER_ESP, 1u, 1u, 2u, 2u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 100u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    RESERVED(0x4Fu), RESERVED(0x50u),
    SPEC(0x51u, "AUTH_VALIDATE_PIN", F4_COMMAND_OWNER_F4, 6u, 10u, 9u, 9u, F4_RESPONSE_SAME_COMMAND, 1500u, false, F4_PRIVILEGE_PUBLIC, true, true),
    SPEC(0x52u, "GET_CURRENT_SCREEN", F4_COMMAND_OWNER_F4, 0u, 0u, 5u, 45u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x53u, "MENU_ITEM_CLICK", F4_COMMAND_OWNER_F4, 4u, 5u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x54u, "TRIGGER_ENCODER_BUTTON", F4_COMMAND_OWNER_F4, 4u, 5u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x55u, "TRIGGER_USER_BUTTON", F4_COMMAND_OWNER_F4, 4u, 5u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x56u, "REQUEST_SCREEN_PAGE", F4_COMMAND_OWNER_F4, 4u, 5u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x57u, "TRIGGER_ENCODER_ROTATE_LEFT", F4_COMMAND_OWNER_F4, 4u, 4u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x58u, "TRIGGER_ENCODER_ROTATE_RIGHT", F4_COMMAND_OWNER_F4, 4u, 4u, 0u, 0u, F4_RESPONSE_ACK, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x59u, "AUTH_PIN_GRANTED_DEPRECATED", F4_COMMAND_OWNER_F4, 4u, 4u, 0u, 0u, F4_RESPONSE_NACK_ONLY, 50u, false, F4_PRIVILEGE_PUBLIC, false, false),
    SPEC(0x5Au, "BINARY_ASSET_STREAM", F4_COMMAND_OWNER_ESP, 1u, 255u, 9u, 255u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 1000u, true, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0x5Bu, "GET_CAR_MODE", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 1u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x5Cu, "WIFI_CREDENTIALS_WEB_RESULT", F4_COMMAND_OWNER_ESP, 2u, 96u, 0u, 0u, F4_RESPONSE_ACK, 200u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x5Du, "AUTH_REMOTE_RESULT", F4_COMMAND_OWNER_F4, 1u, 1u, 6u, 6u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_PUBLIC, true, true),
    SPEC(0x5Eu, "SET_I2C_POLICY", F4_COMMAND_OWNER_F4, 1u, 1u, 2u, 2u, F4_RESPONSE_SAME_COMMAND, 50u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x5Fu, "GET_I2C_POLICY", F4_COMMAND_OWNER_F4, 0u, 0u, 2u, 2u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x60u, "GET_MPU_SNAPSHOT", F4_COMMAND_OWNER_F4, 0u, 0u, 42u, 42u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x61u, "SET_MPU_STREAM", F4_COMMAND_OWNER_F4, 1u, 3u, 4u, 4u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x62u, "STOP_MPU_STREAM", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 1u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x63u, "SET_PREFERENCES", F4_COMMAND_OWNER_F4, 4u, 64u, 5u, 5u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 250u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x64u, "GET_TELEMETRY_SNAPSHOT", F4_COMMAND_OWNER_F4, 0u, 0u, 92u, 92u, F4_RESPONSE_SAME_COMMAND, 80u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x65u, "GET_BALANCE_PID", F4_COMMAND_OWNER_F4, 0u, 0u, 13u, 13u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x66u, "SET_BALANCE_PID", F4_COMMAND_OWNER_F4, 5u, 13u, 14u, 14u, F4_RESPONSE_ACK_THEN_SAME_COMMAND, 80u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x67u, "GET_CONTROL_SNAPSHOT", F4_COMMAND_OWNER_F4, 0u, 0u, 64u, 64u, F4_RESPONSE_SAME_COMMAND, 80u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x68u, "GET_FIRMWARE_INFO", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 196u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_PUBLIC, true, false),
    SPEC(0x69u, "GET_BUILD_INFO", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 149u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_PUBLIC, true, false),
    SPEC(0x6Au, "GET_IR_SNAPSHOT", F4_COMMAND_OWNER_F4, 0u, 0u, 56u, 56u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x6Bu, "SET_IR_STREAM", F4_COMMAND_OWNER_F4, 1u, 3u, 4u, 4u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x6Cu, "STOP_IR_STREAM", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 1u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x6Du, "OLED_CANVAS_TRANSFER", F4_COMMAND_OWNER_F4, 3u, 118u, 10u, 10u, F4_RESPONSE_SAME_COMMAND, 1000u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x6Eu, "SET_CAR_MODE", F4_COMMAND_OWNER_F4, 1u, 1u, 2u, 2u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x6Fu, "GET_BOOT_REPORT", F4_COMMAND_OWNER_F4, 0u, 0u, 22u, 22u, F4_RESPONSE_SAME_COMMAND, 50u, true, F4_PRIVILEGE_PUBLIC, true, false),
    SPEC(0x70u, "WIFI_CREDENTIALS_FLOW", F4_COMMAND_OWNER_ESP, 1u, 255u, 3u, 255u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_ESP_ADMIN, true, true),
    SPEC(0x71u, "AP_CLIENTS_PAGE", F4_COMMAND_OWNER_ESP, 2u, 2u, 3u, 255u, F4_RESPONSE_SAME_COMMAND, 250u, true, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x72u, "AP_CLIENT_DISCONNECT", F4_COMMAND_OWNER_ESP, 6u, 6u, 2u, 2u, F4_RESPONSE_SAME_COMMAND, 250u, false, F4_PRIVILEGE_ESP_ADMIN, true, false),
    SPEC(0x73u, "GET_EXTENSION_PROFILE", F4_COMMAND_OWNER_F4, 0u, 0u, 2u, 2u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x74u, "SET_EXTENSION_PROFILE", F4_COMMAND_OWNER_F4, 1u, 1u, 1u, 1u, F4_RESPONSE_SAME_COMMAND, 3500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x76u, "REBOOT_INTO_PROFILE", F4_COMMAND_OWNER_F4, 1u, 1u, 0u, 0u, F4_RESPONSE_ACK, 3500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x75u, "GET_EXTENSION_PIN_MAP", F4_COMMAND_OWNER_F4, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_STM_SESSION, false, false),
    SPEC(0x77u, "GET_AP_CONFIG",    F4_COMMAND_OWNER_F4,  0u,   0u, 10u, 41u, F4_RESPONSE_SAME_COMMAND,           200u, true,  F4_PRIVILEGE_STM_SESSION, true,  false),
    SPEC(0x78u, "SET_AP_CONFIG_F4", F4_COMMAND_OWNER_F4,  1u, 103u,  3u,  3u, F4_RESPONSE_ACK_THEN_SAME_COMMAND,  500u, false, F4_PRIVILEGE_STM_SESSION, true,  true),
    SPEC(0x79u, "GET_IR_EMITTERS", F4_COMMAND_OWNER_F4, 0u, 0u, 3u, 3u, F4_RESPONSE_SAME_COMMAND, 500u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Au, "SET_IR_EMITTERS", F4_COMMAND_OWNER_F4, 3u, 3u, 4u, 4u, F4_RESPONSE_SAME_COMMAND, 500u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Bu, "GET_MOTION_STATE", F4_COMMAND_OWNER_F4, 0u, 0u, 8u, 8u, F4_RESPONSE_SAME_COMMAND, 100u, true, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Cu, "SET_BALANCE_ENABLED", F4_COMMAND_OWNER_F4, 1u, 1u, 8u, 8u, F4_RESPONSE_SAME_COMMAND, 100u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Du, "SET_MOTION", F4_COMMAND_OWNER_F4, 10u, 10u, 8u, 8u, F4_RESPONSE_SAME_COMMAND, 100u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Eu, "STOP_MOTION", F4_COMMAND_OWNER_F4, 1u, 1u, 8u, 8u, F4_RESPONSE_SAME_COMMAND, 100u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x7Fu, "SET_MOTOR_TEST", F4_COMMAND_OWNER_F4, 10u, 10u, 8u, 8u, F4_RESPONSE_SAME_COMMAND, 100u, false, F4_PRIVILEGE_STM_SESSION, true, false),

    SPEC(0x80u, "EVT_BOOT", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x81u, "EVT_MODE_CHANGED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x82u, "EVT_STA_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x83u, "EVT_STA_DISCONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x84u, "EVT_AP_CLIENT_JOIN", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x85u, "EVT_AP_CLIENT_LEAVE", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x86u, "EVT_APP_USER_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x87u, "EVT_APP_USER_DISCONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x88u, "EVT_WEBSERVER_UP", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x89u, "EVT_WEBSERVER_CLIENT_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Au, "EVT_WEBSERVER_CLIENT_DISCONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Bu, "EVT_LAST_WIFI_NOT_FOUND", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Cu, "EVT_CONTROLLER_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Du, "EVT_CONTROLLER_DISCONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Eu, "EVT_USB_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x8Fu, "EVT_USB_DISCONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x90u, "EVT_MPU_STREAM", F4_COMMAND_OWNER_F4, 0u, 0u, 42u, 42u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x91u, "EVT_IR_STREAM", F4_COMMAND_OWNER_F4, 0u, 0u, 56u, 56u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x92u, "EVT_WIFI_CONNECTED", F4_COMMAND_OWNER_ESP, 0u, 255u, 0u, 255u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x93u, "EVT_NETWORK_IP", F4_COMMAND_OWNER_ESP, 5u, 5u, 5u, 5u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x94u, "EVT_BOOT_COMPLETE", F4_COMMAND_OWNER_ESP, 5u, 5u, 5u, 5u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0x95u, "EVT_SCREEN_CHANGED", F4_COMMAND_OWNER_F4, 0u, 0u, 5u, 9u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x96u, "EVT_MENU_SELECTION_CHANGED", F4_COMMAND_OWNER_F4, 0u, 0u, 7u, 7u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x97u, "EVT_CAR_MODE_CHANGED", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 1u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x98u, "EVT_IR_EMITTERS_CHANGED", F4_COMMAND_OWNER_F4, 0u, 0u, 3u, 3u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x9Eu, "EVT_QT_USB_STATE", F4_COMMAND_OWNER_F4, 0u, 0u, 1u, 1u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),
    SPEC(0x9Fu, "EVT_MCU_BOOT_REPORT", F4_COMMAND_OWNER_F4, 0u, 0u, 22u, 22u, F4_RESPONSE_EVENT, 0u, false, F4_PRIVILEGE_STM_SESSION, true, false),

    SPEC(0xA4u, "LEGACY_WIFI_SCAN_STARTED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xA5u, "LEGACY_WIFI_SCAN_COMPLETED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xA6u, "LEGACY_WIFI_STA_CONNECT_REQUESTED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xA7u, "LEGACY_WIFI_STA_DISCONNECT_REQUESTED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xA8u, "LEGACY_WIFI_AP_STATE_REQUESTED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xA9u, "LEGACY_WEB_COMMAND_REJECTED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),
    SPEC(0xAAu, "LEGACY_CREDENTIALS_UPDATED", F4_COMMAND_OWNER_RESERVED, 0u, 0u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, false, false),

    SPEC(0xE0u, "ACK", F4_COMMAND_OWNER_DUAL, 1u, 2u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
    SPEC(0xE1u, "NACK", F4_COMMAND_OWNER_DUAL, 1u, 2u, 0u, 0u, F4_RESPONSE_NONE, 0u, false, F4_PRIVILEGE_INTERNAL, true, false),
};

const F4_CommandSpec *F4_CommandRegistry_Find(uint8_t id)
{
#if defined(ARDUINO_ARCH_ESP8266) || defined(ESP8266)
    static F4_CommandSpec scratch;
    return F4_CommandRegistry_Lookup(id, &scratch) ? &scratch : NULL;
#else
    size_t lo = 0u;
    size_t hi = sizeof(kF4Commands) / sizeof(kF4Commands[0]);

    while (lo < hi) {
        size_t mid = lo + ((hi - lo) / 2u);
        if (kF4Commands[mid].id == id) return &kF4Commands[mid];
        if (kF4Commands[mid].id < id) lo = mid + 1u;
        else hi = mid;
    }
    return NULL;
#endif
}

const F4_CommandSpec *F4_CommandRegistry_At(size_t index)
{
#if defined(ARDUINO_ARCH_ESP8266) || defined(ESP8266)
    static F4_CommandSpec scratch;
    if (index >= F4_CommandRegistry_Count()) return NULL;
    F4_REGISTRY_COPY(&scratch, &kF4Commands[index]);
    return &scratch;
#else
    return index < F4_CommandRegistry_Count() ? &kF4Commands[index] : NULL;
#endif
}

bool F4_CommandRegistry_Lookup(uint8_t id, F4_CommandSpec *out)
{
    size_t lo = 0u;
    size_t hi = sizeof(kF4Commands) / sizeof(kF4Commands[0]);
    if (out == NULL) return false;

    while (lo < hi) {
        F4_CommandSpec candidate;
        size_t mid = lo + ((hi - lo) / 2u);
        F4_REGISTRY_COPY(&candidate, &kF4Commands[mid]);
        if (candidate.id == id) {
            *out = candidate;
            return true;
        }
        if (candidate.id < id) lo = mid + 1u;
        else hi = mid;
    }
    return false;
}

size_t F4_CommandRegistry_Count(void)
{
    return sizeof(kF4Commands) / sizeof(kF4Commands[0]);
}

bool F4_CommandRegistry_IsRequestLengthValid(const F4_CommandSpec *spec, size_t len)
{
    if (spec == NULL) return false;
    /* El contrato contiene dominios disjuntos que min/max no puede expresar.
     * Mantenerlos aqui evita que raw o un futuro dispatcher acepten los huecos
     * aunque hoy los builders tipados solo produzcan longitudes validas. */
    if (spec->id == 0x51u) return len == 6u || len == 10u;
    if (spec->id == 0x61u || spec->id == 0x6Bu) return len == 1u || len == 3u;
    if (spec->id == 0x6Du) return len == 3u || len == 12u || len == 22u || len == 118u;
    return len >= spec->request_min_len && len <= spec->request_max_len;
}

bool F4_CommandRegistry_IsResponseLengthValid(const F4_CommandSpec *spec, size_t len)
{
    if (spec == NULL) return false;
    if (spec->id == 0x52u) {
        return len == 5u || len == 7u || len == 9u || (len >= 13u && len <= 45u);
    }
    if (spec->id == 0x95u) return len == 5u || len == 9u;
    return len >= spec->response_min_len && len <= spec->response_max_len;
}

bool F4_CommandRegistry_IsWireAllowed(const F4_CommandSpec *spec)
{
    return spec != NULL && spec->implemented && spec->owner != F4_COMMAND_OWNER_RESERVED;
}
