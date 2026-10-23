#ifndef AUTOMICRO_BOOT_CONTRACT_H
#define AUTOMICRO_BOOT_CONTRACT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* STM32F411CEU6 memory contract shared by the bootloader and application. */
#define AUTOMICRO_BOOTLOADER_BASE_ADDR        UINT32_C(0x08000000)
#define AUTOMICRO_BOOTLOADER_SIZE_BYTES       UINT32_C(0x00004000)
#define AUTOMICRO_PREFERENCES_BASE_ADDR       UINT32_C(0x08004000)
#define AUTOMICRO_PREFERENCES_SIZE_BYTES      UINT32_C(0x00004000)
#define AUTOMICRO_APP_BASE_ADDR               UINT32_C(0x08008000)
#define AUTOMICRO_APP_MANIFEST_OFFSET         UINT32_C(0x00000200)
#define AUTOMICRO_APP_MANIFEST_ADDR           (AUTOMICRO_APP_BASE_ADDR + AUTOMICRO_APP_MANIFEST_OFFSET)

/* Production application owns all sectors 2..7. The migration limit remains
 * part of the manifest ABI for explicitly protected transitional images. */
#define AUTOMICRO_APP_MIGRATION_MAX_BYTES     UINT32_C(0x00058000)
#define AUTOMICRO_APP_PHYSICAL_MAX_BYTES      UINT32_C(0x00078000)
#define AUTOMICRO_PREFERENCES_LEGACY_BASE_ADDR UINT32_C(0x08060000)
#define AUTOMICRO_PREFERENCES_LEGACY_LOG_BYTES UINT32_C(0x00001000)

#define AUTOMICRO_SRAM_BASE_ADDR              UINT32_C(0x20000000)
#define AUTOMICRO_SRAM_SIZE_BYTES             UINT32_C(0x00020000)
#define AUTOMICRO_BOOT_MAILBOX_RESERVED_BYTES UINT32_C(0x00000100)
#define AUTOMICRO_BOOT_MAILBOX_ADDR           UINT32_C(0x2001FF00)

#define AUTOMICRO_APP_MANIFEST_MAGIC          UINT32_C(0x50414D41) /* "AMAP" little-endian. */
#define AUTOMICRO_APP_MANIFEST_SCHEMA         UINT16_C(1)
#define AUTOMICRO_APP_MANIFEST_SIZE_BYTES     UINT16_C(64)

#define AUTOMICRO_BOOT_MAILBOX_MAGIC          UINT32_C(0x4C424D41) /* "AMBL" little-endian. */
#define AUTOMICRO_BOOT_MAILBOX_SCHEMA         UINT16_C(1)
#define AUTOMICRO_BOOT_MAILBOX_SIZE_BYTES     UINT16_C(64)

#define AUTOMICRO_VERSION_PACK(major, minor, patch) \
    ((((uint32_t)(major) & UINT32_C(0xFF)) << 16) | \
     (((uint32_t)(minor) & UINT32_C(0xFF)) << 8) | \
     ((uint32_t)(patch) & UINT32_C(0xFF)))
#define AUTOMICRO_VERSION_MAJOR(version) (((uint32_t)(version) >> 16) & UINT32_C(0xFF))
#define AUTOMICRO_VERSION_MINOR(version) (((uint32_t)(version) >> 8) & UINT32_C(0xFF))
#define AUTOMICRO_VERSION_PATCH(version) ((uint32_t)(version) & UINT32_C(0xFF))

typedef enum
{
    AUTOMICRO_BOOT_RESET_NONE                 = 0U,
    AUTOMICRO_BOOT_RESET_LOW_POWER            = (1U << 0),
    AUTOMICRO_BOOT_RESET_WINDOW_WATCHDOG      = (1U << 1),
    AUTOMICRO_BOOT_RESET_INDEPENDENT_WATCHDOG = (1U << 2),
    AUTOMICRO_BOOT_RESET_SOFTWARE             = (1U << 3),
    AUTOMICRO_BOOT_RESET_POWER_ON             = (1U << 4),
    AUTOMICRO_BOOT_RESET_PIN                  = (1U << 5),
    AUTOMICRO_BOOT_RESET_BROWN_OUT            = (1U << 6)
} AutoMicroBootResetFlags;

typedef enum
{
    AUTOMICRO_APP_VALIDATION_NONE           = 0U,
    AUTOMICRO_APP_VALIDATION_STACK          = (1U << 0),
    AUTOMICRO_APP_VALIDATION_RESET_HANDLER  = (1U << 1),
    AUTOMICRO_APP_VALIDATION_MANIFEST       = (1U << 2),
    AUTOMICRO_APP_VALIDATION_IMAGE_RANGE    = (1U << 3),
    AUTOMICRO_APP_VALIDATION_CRC32          = (1U << 4),
    AUTOMICRO_APP_VALIDATION_READY =
        AUTOMICRO_APP_VALIDATION_STACK |
        AUTOMICRO_APP_VALIDATION_RESET_HANDLER |
        AUTOMICRO_APP_VALIDATION_MANIFEST |
        AUTOMICRO_APP_VALIDATION_IMAGE_RANGE |
        AUTOMICRO_APP_VALIDATION_CRC32
} AutoMicroAppValidationFlags;

typedef enum
{
    AUTOMICRO_BOOT_HANDOFF_NONE = 0U,
    AUTOMICRO_BOOT_HANDOFF_APPLICATION = 1U,
    AUTOMICRO_BOOT_HANDOFF_ROM_BOOTLOADER = 2U,
    AUTOMICRO_BOOT_HANDOFF_RECOVERY_INVALID_APP = 3U
} AutoMicroBootHandoff;

typedef enum
{
    AUTOMICRO_BOOT_REQUEST_NONE = 0U,
    AUTOMICRO_BOOT_REQUEST_VISUAL_MENU = 1U,
    AUTOMICRO_BOOT_REQUEST_ROM_BOOTLOADER = 2U
} AutoMicroBootRequest;

typedef enum
{
    AUTOMICRO_APP_FLAG_PRESERVE_LEGACY_SECTOR7 = (1U << 0)
} AutoMicroAppManifestFlags;

typedef struct
{
    uint32_t magic;
    uint16_t schema_version;
    uint16_t struct_size;
    uint32_t app_base;
    uint32_t image_size;
    uint32_t image_crc32;
    uint32_t firmware_version;
    uint32_t build_id;
    uint32_t flags;
    uint32_t reserved[8];
} AutoMicroAppManifest;

typedef struct
{
    uint32_t magic;
    uint16_t schema_version;
    uint16_t struct_size;
    uint32_t bootloader_version;
    uint32_t bootloader_build_id;
    uint32_t reset_reason_flags;
    uint32_t app_validation_flags;
    uint32_t app_base;
    uint32_t app_size;
    uint32_t app_version;
    uint32_t app_build_id;
    uint32_t app_crc32;
    uint32_t handoff;
    uint32_t request;
    uint32_t reserved[2];
    uint32_t mailbox_crc32;
} AutoMicroBootMailbox;

#if defined(__cplusplus)
static_assert(sizeof(AutoMicroAppManifest) == AUTOMICRO_APP_MANIFEST_SIZE_BYTES,
              "AutoMicroAppManifest ABI changed");
static_assert(sizeof(AutoMicroBootMailbox) == AUTOMICRO_BOOT_MAILBOX_SIZE_BYTES,
              "AutoMicroBootMailbox ABI changed");
#else
_Static_assert(sizeof(AutoMicroAppManifest) == AUTOMICRO_APP_MANIFEST_SIZE_BYTES,
               "AutoMicroAppManifest ABI changed");
_Static_assert(sizeof(AutoMicroBootMailbox) == AUTOMICRO_BOOT_MAILBOX_SIZE_BYTES,
               "AutoMicroBootMailbox ABI changed");
#endif

const AutoMicroBootMailbox *AutoMicroBootMailbox_Get(void);
bool AutoMicroBootMailbox_Validate(const AutoMicroBootMailbox *mailbox);
bool AutoMicroBootMailbox_Consume(AutoMicroBootMailbox *out);
bool AutoMicroBootMailbox_WriteRequest(AutoMicroBootRequest request);
AutoMicroBootRequest AutoMicroBootMailbox_ConsumeRequest(void);
uint32_t AutoMicroBoot_Crc32(const void *data, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* AUTOMICRO_BOOT_CONTRACT_H */
