#ifndef APP_PREFERENCES_STM32_H
#define APP_PREFERENCES_STM32_H
#include "main.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "automicro_boot_contract.h"
static inline const void *App_FlashView(void *ctx,uint32_t address,size_t len) { (void)ctx;(void)len;return (const void *)(uintptr_t)address; }
static inline AM_Result App_FlashUnlock(void *ctx) { (void)ctx;return HAL_FLASH_Unlock(); }
static inline AM_Result App_FlashLock(void *ctx) { (void)ctx;return HAL_FLASH_Lock(); }
static inline AM_Result App_FlashErase(void *ctx) {
    (void)ctx; FLASH_EraseInitTypeDef erase={0}; uint32_t sectorError=0;
    erase.TypeErase=FLASH_TYPEERASE_SECTORS; erase.Sector=FLASH_SECTOR_1;
    erase.NbSectors=1u; erase.VoltageRange=FLASH_VOLTAGE_RANGE_3;
    return HAL_FLASHEx_Erase(&erase,&sectorError);
}
static inline AM_Result App_FlashProgram(void *ctx,uint32_t address,uint32_t word) { (void)ctx;return HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,address,word); }
/* A large production image may contain code where the old preference log
 * lived. Never interpret that code as records. Migration is allowed only
 * while a packaged image demonstrably ends before sector 7. */
static inline bool AppPreferences_LegacySectorIsOutsideImage(void) {
    const AutoMicroAppManifest *manifest=
        (const AutoMicroAppManifest *)(uintptr_t)AUTOMICRO_APP_MANIFEST_ADDR;
    return manifest->magic==AUTOMICRO_APP_MANIFEST_MAGIC &&
        manifest->schema_version==AUTOMICRO_APP_MANIFEST_SCHEMA &&
        manifest->struct_size==AUTOMICRO_APP_MANIFEST_SIZE_BYTES &&
        manifest->app_base==AUTOMICRO_APP_BASE_ADDR &&
        manifest->image_size>=AUTOMICRO_APP_MANIFEST_OFFSET+AUTOMICRO_APP_MANIFEST_SIZE_BYTES &&
        manifest->image_size<=AUTOMICRO_APP_MIGRATION_MAX_BYTES &&
        manifest->image_crc32!=UINT32_MAX;
}
static inline void AppPreferences_BindStm32(void) {
    static const AppPreferencesStoragePort primary={NULL,
        AUTOMICRO_PREFERENCES_BASE_ADDR,
        AUTOMICRO_PREFERENCES_BASE_ADDR+AUTOMICRO_PREFERENCES_SIZE_BYTES,
        0u,0u,App_FlashView,App_FlashUnlock,App_FlashLock,
        App_FlashErase,App_FlashProgram,AppApConfig_Ipv4IsValid,AppApConfig_ApplyPatch};
    static const AppPreferencesStoragePort migration={NULL,
        AUTOMICRO_PREFERENCES_BASE_ADDR,
        AUTOMICRO_PREFERENCES_BASE_ADDR+AUTOMICRO_PREFERENCES_SIZE_BYTES,
        AUTOMICRO_PREFERENCES_LEGACY_BASE_ADDR,
        AUTOMICRO_PREFERENCES_LEGACY_BASE_ADDR+AUTOMICRO_PREFERENCES_LEGACY_LOG_BYTES,
        App_FlashView,App_FlashUnlock,App_FlashLock,
        App_FlashErase,App_FlashProgram,AppApConfig_Ipv4IsValid,AppApConfig_ApplyPatch};
    appPreferencesHandle.storage=AppPreferences_LegacySectorIsOutsideImage()?&migration:&primary;
}
#endif
