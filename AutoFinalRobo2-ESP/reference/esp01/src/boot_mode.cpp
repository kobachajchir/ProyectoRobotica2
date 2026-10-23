#include "boot_mode.h"

bool BootMode_IsValid(uint8_t mode)
{
    return mode == APP_BOOT_MODE_NORMAL || mode == APP_BOOT_MODE_AP_ONLY;
}

uint8_t BootMode_GetRequested(const AppConfig *config)
{
    if (!config || !BootMode_IsValid(config->nextBootMode))
        return APP_BOOT_MODE_NORMAL;
    return config->nextBootMode;
}

bool BootMode_ApplyRequest(const AppConfig *current, uint8_t mode, AppConfig *next)
{
    if (!current || !next || !BootMode_IsValid(mode))
        return false;

    *next = *current;
    next->nextBootMode = mode;
    return true;
}
