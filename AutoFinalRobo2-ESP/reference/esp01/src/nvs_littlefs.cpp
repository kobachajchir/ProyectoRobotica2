#include "nvs_littlefs.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <string.h>

static void cpy(char *dst, const char *src, size_t n)
{
    if (!dst || n == 0)
        return;
    if (!src)
    {
        dst[0] = 0;
        return;
    }

    size_t length = 0;
    while ((length + 1u) < n && src[length] != '\0')
        ++length;
    memcpy(dst, src, length);
    dst[length] = 0;
}

static bool loadConfigFile(void *, const char *path, AppConfig &out, bool *requires_migration)
{
    if (!path || !LittleFS.exists(path))
        return false;

    File file = LittleFS.open(path, "r");
    if (!file)
        return false;
    if (file.size() == 0 || file.size() > 1024)
    {
        file.close();
        return false;
    }

    DynamicJsonDocument doc(768);
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    if (error)
        return false;

    uint16_t stored_schema = doc["schemaVersion"] | 1u;
    if (stored_schema == 0u || stored_schema > CONFIG_SCHEMA_VERSION)
        return false;
    if ((uint8_t)(doc["initializedNvs"] | 0u) != NVS_MAGIC_VALUE)
        return false;

    AppConfig loaded{};
    loaded.schemaVersion = CONFIG_SCHEMA_VERSION;
    loaded.initializedNvs = NVS_MAGIC_VALUE;
    cpy(loaded.apSsid, doc["apSsid"] | DEFAULT_AP_SSID, sizeof(loaded.apSsid));
    cpy(loaded.apPass, doc["apPass"] | DEFAULT_AP_PASSWORD, sizeof(loaded.apPass));
    loaded.apProtected = doc["apProtected"] | 0u;
    loaded.apFallbackEnabled = (doc["apFallbackEnabled"] | 1u) ? 1u : 0u;
    if (stored_schema < 4u)
        loaded.apProtected = (loaded.apPass[0] != '\0') ? 1u : 0u;
    cpy(loaded.staSsid, doc["staSsid"] | DEFAULT_STA_SSID, sizeof(loaded.staSsid));
    if (loaded.staSsid[0] == '\0')
    {
        cpy(loaded.staSsid, DEFAULT_STA_SSID, sizeof(loaded.staSsid));
        cpy(loaded.staPass, DEFAULT_STA_PASSWORD, sizeof(loaded.staPass));
    }
    else
    {
        cpy(loaded.staPass, doc["staPass"] | DEFAULT_STA_PASSWORD, sizeof(loaded.staPass));
    }
    cpy(loaded.staIp, doc["staIp"] | "0.0.0.0", sizeof(loaded.staIp));
    cpy(loaded.staGateway, doc["staGateway"] | "0.0.0.0", sizeof(loaded.staGateway));
    cpy(loaded.staSubnet, doc["staSubnet"] | "255.255.255.0", sizeof(loaded.staSubnet));
    cpy(loaded.apIp, doc["apIp"] | "0.0.0.0", sizeof(loaded.apIp));
    loaded.nextBootMode = doc["nextBootMode"] | APP_BOOT_MODE_NORMAL;
    if (loaded.nextBootMode != APP_BOOT_MODE_NORMAL && loaded.nextBootMode != APP_BOOT_MODE_AP_ONLY)
        loaded.nextBootMode = APP_BOOT_MODE_NORMAL;

    if (requires_migration)
    {
        // Schema v1 stored testModePin. The field is only detected so the file
        // can be scrubbed; its value is never read into the application state.
        *requires_migration = stored_schema != CONFIG_SCHEMA_VERSION || !doc["testModePin"].isNull() || doc["apFallbackEnabled"].isNull();
    }
    out = loaded;
    return true;
}

static bool writeConfigFile(void *, const char *path, const AppConfig &cfg)
{
    if (!path)
        return false;
    if (LittleFS.exists(path) && !LittleFS.remove(path))
        return false;

    File file = LittleFS.open(path, "w");
    if (!file)
        return false;

    StaticJsonDocument<640> doc;
    doc["schemaVersion"] = CONFIG_SCHEMA_VERSION;
    doc["initializedNvs"] = NVS_MAGIC_VALUE;
    doc["apSsid"] = cfg.apSsid;
    doc["apPass"] = cfg.apPass;
    doc["apProtected"] = cfg.apProtected;
    doc["apFallbackEnabled"] = cfg.apFallbackEnabled;
    doc["staSsid"] = cfg.staSsid;
    doc["staPass"] = cfg.staPass;
    doc["staIp"] = cfg.staIp;
    doc["staGateway"] = cfg.staGateway;
    doc["staSubnet"] = cfg.staSubnet;
    doc["apIp"] = cfg.apIp;
    doc["nextBootMode"] = cfg.nextBootMode;

    size_t written = serializeJson(doc, file);
    file.flush();
    file.close();
    return written > 0u;
}


static bool mount(void *) { return LittleFS.begin(); }
static bool exists(void *, const char *path) { return LittleFS.exists(path); }
static bool removeFile(void *, const char *path) { return LittleFS.remove(path); }
static bool renameFile(void *, const char *from, const char *to) { return LittleFS.rename(from, to); }
const NvsStoreOps &NVS_LittleFSOps() {
    static const NvsStoreOps ops = [] {
        NvsStoreOps value;
        value.mount = mount;
        value.exists = exists;
        value.remove = removeFile;
        value.rename = renameFile;
        value.load = loadConfigFile;
        value.write = writeConfigFile;
        return value;
    }();
    return ops;
}
