#pragma once
#include "config.h"

// Historical API name kept for compatibility. Persistence is JSON in LittleFS,
// with schema migration and a last-known-good backup; it is not hardware NVS.
bool            NVS_InitAndLoad(AppConfig* out);
bool            NVS_Save(const AppConfig& cfg);
bool            NVS_RestoreBackup(AppConfig* out);
bool            NVS_StorageReady();
const AppConfig* NVS_GetCached();
