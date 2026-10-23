#pragma once
#include "config.h"

// File codec and storage operations are supplied by the application adapter.
struct NvsStoreOps {
    void *context = nullptr;
    bool (*mount)(void *) = nullptr;
    bool (*exists)(void *, const char *) = nullptr;
    bool (*remove)(void *, const char *) = nullptr;
    bool (*rename)(void *, const char *, const char *) = nullptr;
    bool (*load)(void *, const char *, AppConfig &, bool *requires_migration) = nullptr;
    bool (*write)(void *, const char *, const AppConfig &) = nullptr;
};
struct NvsStoreHandle {
    NvsStoreOps ops;
    AppConfig cached{};
    bool valid = false;
    bool mounted = false;
};
void NvsStore_Init(NvsStoreHandle &h, const NvsStoreOps &ops);
bool NvsStore_InitAndLoad(NvsStoreHandle &h, AppConfig *out);
bool NvsStore_Save(NvsStoreHandle &h, const AppConfig &config);
bool NvsStore_RestoreBackup(NvsStoreHandle &h, AppConfig *out);
bool NvsStore_StorageReady(const NvsStoreHandle &h);
const AppConfig *NvsStore_GetCached(const NvsStoreHandle &h);
