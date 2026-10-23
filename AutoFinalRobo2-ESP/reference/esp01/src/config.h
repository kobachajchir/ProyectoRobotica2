#pragma once
#include <stdint.h>

#define CONFIG_FILE_PATH             "/config.json"
#define CONFIG_TEMP_FILE_PATH        "/config.json.tmp"
#define CONFIG_BACKUP_FILE_PATH      "/config.json.bak"
#define CONFIG_BACKUP_TEMP_FILE_PATH "/config.json.bak.tmp"
#define CONFIG_SCHEMA_VERSION        4u
#define NVS_MAGIC_VALUE              7
#define ESP_FIRMWARE_VERSION         "v2.4"

// Provisioning defaults only. Production Wi-Fi credentials must be supplied
// at runtime and persisted in LittleFS; they are intentionally not embedded.
// An empty AP password means the provisioning AP is open and therefore does
// not provide confidentiality.
#define DEFAULT_AP_SSID        "AutoMicro-Setup"
#define DEFAULT_AP_PASSWORD    ""
#define DEFAULT_STA_SSID       ""
#define DEFAULT_STA_PASSWORD   ""

#define SSID_MAX_LEN  32
#define PASS_MAX_LEN  63
#define IPV4_TEXT_MAX_LEN 15

#define APP_BOOT_MODE_NORMAL   0u
#define APP_BOOT_MODE_AP_ONLY  1u

// AP apply states (reported by F4 0x77 and local operations)
#define AP_APPLY_STATE_SYNCED    0u
#define AP_APPLY_STATE_PENDING   1u
#define AP_APPLY_STATE_APPLYING  2u
#define AP_APPLY_STATE_FAILED    3u

// AP changed-field mask bits (used by F4 0x78 response)
#define AP_FIELD_SSID       0x01u
#define AP_FIELD_PASSWORD   0x02u
#define AP_FIELD_PROTECTED  0x04u
#define AP_FIELD_IPV4       0x08u
#define AP_FIELD_MASK_ALL   0x0Fu

// 0x4A operational response codes (ESP → MCU)
#define AP_OP_OK                   0u
#define AP_OP_PAYLOAD_INVALID      1u
#define AP_OP_CONFIG_INVALID       2u
#define AP_OP_STORAGE_FAILED       3u

// Car modes
#define CAR_MODE_IDLE   0u
#define CAR_MODE_FOLLOW 1u
#define CAR_MODE_TEST   2u
#define CAR_MODE_GOTO   3u
#define CAR_MODE_MAX    3u

// Reservar UART0 para el protocolo comando/respuesta.
// Las notificaciones/eventos asíncronos quedan deshabilitados por defecto.
#define UNER_SERIAL_EMIT_ASYNC_NOTIFICATIONS 0

// Raw UNER access is intentionally absent from production builds. Diagnostic
// builds may opt in explicitly, but the bridge still requires a live F4 PIN
// session and rejects sensitive/stream-control command IDs.
#ifndef ENABLE_DIAGNOSTIC_RAW_UNER
#define ENABLE_DIAGNOSTIC_RAW_UNER 0
#endif

struct AppConfig {
  uint16_t schemaVersion;
  uint8_t initializedNvs; // =7 cuando OK
  char    apSsid[SSID_MAX_LEN + 1];
  char    apPass[PASS_MAX_LEN + 1];
  uint8_t apProtected;              // 1 = WPA2, 0 = open AP (password still stored)
  char    staSsid[SSID_MAX_LEN + 1];
  char    staPass[PASS_MAX_LEN + 1];
  // "0.0.0.0" disables the fixed address. STA gateway/subnet are persisted
  // with the address because ESP8266 needs all three values for static IPv4.
  char    staIp[IPV4_TEXT_MAX_LEN + 1];
  char    staGateway[IPV4_TEXT_MAX_LEN + 1];
  char    staSubnet[IPV4_TEXT_MAX_LEN + 1];
  char    apIp[IPV4_TEXT_MAX_LEN + 1];
  uint8_t nextBootMode;
#ifdef __cplusplus
  uint8_t apFallbackEnabled = 1u;
#else
  uint8_t apFallbackEnabled;
#endif
  // 1 = automatic AP after STA failure; explicit AP is independent
};
