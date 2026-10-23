#pragma once

#include <stdint.h>

#include "config.h"

bool BootMode_IsValid(uint8_t mode);
uint8_t BootMode_GetRequested(const AppConfig *config);
bool BootMode_ApplyRequest(const AppConfig *current, uint8_t mode, AppConfig *next);
