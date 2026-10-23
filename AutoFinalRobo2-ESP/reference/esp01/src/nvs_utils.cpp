#include "nvs_utils.h"
#include "nvs_store.h"
#include "nvs_littlefs.h"

static NvsStoreHandle &store() {
    static NvsStoreHandle handle;
    if (!handle.ops.mount) NvsStore_Init(handle, NVS_LittleFSOps());
    return handle;
}
bool NVS_InitAndLoad(AppConfig *out) { return NvsStore_InitAndLoad(store(), out); }
bool NVS_Save(const AppConfig &config) { return NvsStore_Save(store(), config); }
bool NVS_RestoreBackup(AppConfig *out) { return NvsStore_RestoreBackup(store(), out); }
bool NVS_StorageReady() { return NvsStore_StorageReady(store()); }
const AppConfig *NVS_GetCached() { return NvsStore_GetCached(store()); }
