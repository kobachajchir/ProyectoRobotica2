#include "nvs_store.h"
#include <string.h>

void NvsStore_Init(NvsStoreHandle &h, const NvsStoreOps &ops) {
    h = NvsStoreHandle{};
    h.ops = ops;
}
static bool mountFs(NvsStoreHandle &h) {
    if (h.mounted) return true;
    if (!h.ops.mount || !h.ops.exists || !h.ops.remove || !h.ops.rename ||
        !h.ops.load || !h.ops.write) return false;
    h.mounted = h.ops.mount(h.ops.context);
    return h.mounted;
}

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

static void normalizeConfig(const AppConfig &input, AppConfig &output)
{
    memset(&output, 0, sizeof(output));
    output.schemaVersion = CONFIG_SCHEMA_VERSION;
    output.initializedNvs = NVS_MAGIC_VALUE;
    cpy(output.apSsid, input.apSsid, sizeof(output.apSsid));
    cpy(output.apPass, input.apPass, sizeof(output.apPass));
    output.apProtected = input.apProtected ? 1u : 0u;
    output.apFallbackEnabled = input.apFallbackEnabled ? 1u : 0u;
    cpy(output.staSsid, input.staSsid, sizeof(output.staSsid));
    if (output.staSsid[0] == '\0')
    {
        cpy(output.staSsid, DEFAULT_STA_SSID, sizeof(output.staSsid));
        cpy(output.staPass, DEFAULT_STA_PASSWORD, sizeof(output.staPass));
    }
    else
    {
        cpy(output.staPass, input.staPass, sizeof(output.staPass));
    }
    cpy(output.staIp, input.staIp, sizeof(output.staIp));
    cpy(output.staGateway, input.staGateway, sizeof(output.staGateway));
    cpy(output.staSubnet, input.staSubnet, sizeof(output.staSubnet));
    cpy(output.apIp, input.apIp, sizeof(output.apIp));
    if (output.staIp[0] == '\0') cpy(output.staIp, "0.0.0.0", sizeof(output.staIp));
    if (output.staGateway[0] == '\0') cpy(output.staGateway, "0.0.0.0", sizeof(output.staGateway));
    if (output.staSubnet[0] == '\0') cpy(output.staSubnet, "255.255.255.0", sizeof(output.staSubnet));
    if (output.apIp[0] == '\0') cpy(output.apIp, "0.0.0.0", sizeof(output.apIp));
    output.nextBootMode = input.nextBootMode == APP_BOOT_MODE_AP_ONLY
                              ? APP_BOOT_MODE_AP_ONLY
                              : APP_BOOT_MODE_NORMAL;
}

static void fillDefaults(AppConfig &cfg)
{
    memset(&cfg, 0, sizeof(cfg));
    cfg.schemaVersion = CONFIG_SCHEMA_VERSION;
    cfg.initializedNvs = NVS_MAGIC_VALUE;
    cpy(cfg.apSsid, DEFAULT_AP_SSID, sizeof(cfg.apSsid));
    cpy(cfg.apPass, DEFAULT_AP_PASSWORD, sizeof(cfg.apPass));
    cfg.apProtected = 0u;
    cfg.apFallbackEnabled = 1u;
    cpy(cfg.staSsid, DEFAULT_STA_SSID, sizeof(cfg.staSsid));
    cpy(cfg.staPass, DEFAULT_STA_PASSWORD, sizeof(cfg.staPass));
    cpy(cfg.staIp, "0.0.0.0", sizeof(cfg.staIp));
    cpy(cfg.staGateway, "0.0.0.0", sizeof(cfg.staGateway));
    cpy(cfg.staSubnet, "255.255.255.0", sizeof(cfg.staSubnet));
    cpy(cfg.apIp, "0.0.0.0", sizeof(cfg.apIp));
    cfg.nextBootMode = APP_BOOT_MODE_NORMAL;
}

static bool configsEqual(const AppConfig &a, const AppConfig &b)
{
    return a.schemaVersion == b.schemaVersion &&
           a.initializedNvs == b.initializedNvs &&
           strcmp(a.apSsid, b.apSsid) == 0 &&
           strcmp(a.apPass, b.apPass) == 0 &&
           a.apProtected == b.apProtected &&
           strcmp(a.staSsid, b.staSsid) == 0 &&
           strcmp(a.staPass, b.staPass) == 0 &&
           strcmp(a.staIp, b.staIp) == 0 &&
           strcmp(a.staGateway, b.staGateway) == 0 &&
           strcmp(a.staSubnet, b.staSubnet) == 0 &&
           strcmp(a.apIp, b.apIp) == 0 &&
           a.nextBootMode == b.nextBootMode &&
           a.apFallbackEnabled == b.apFallbackEnabled;
}

static bool verifyConfigFile(NvsStoreHandle &h, const char *path, const AppConfig &expected)
{
    AppConfig actual{};
    bool requires_migration = false;
    return h.ops.load(h.ops.context, path, actual, &requires_migration) &&
           !requires_migration && configsEqual(actual, expected);
}

static bool writeSanitizedBackup(NvsStoreHandle &h, const AppConfig &cfg)
{
    if (!h.ops.write(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH, cfg) ||
        !verifyConfigFile(h, CONFIG_BACKUP_TEMP_FILE_PATH, cfg))
    {
        if (h.ops.exists(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH))
            h.ops.remove(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH);
        // Never retain a legacy backup containing the removed PIN field.
        if (h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH))
            h.ops.remove(h.ops.context, CONFIG_BACKUP_FILE_PATH);
        return false;
    }

    if (h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH) && !h.ops.remove(h.ops.context, CONFIG_BACKUP_FILE_PATH))
    {
        h.ops.remove(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH);
        return false;
    }
    if (!h.ops.rename(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH, CONFIG_BACKUP_FILE_PATH))
    {
        h.ops.remove(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH);
        return false;
    }
    return true;
}

bool NvsStore_Save(NvsStoreHandle &h, const AppConfig &cfg)
{
    if (!mountFs(h))
        return false;

    AppConfig normalized{};
    normalizeConfig(cfg, normalized);
    if (!h.ops.write(h.ops.context, CONFIG_TEMP_FILE_PATH, normalized) ||
        !verifyConfigFile(h, CONFIG_TEMP_FILE_PATH, normalized))
    {
        if (h.ops.exists(h.ops.context, CONFIG_TEMP_FILE_PATH))
            h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
        return false;
    }

    bool had_primary = h.ops.exists(h.ops.context, CONFIG_FILE_PATH);
    if (had_primary)
    {
        if (h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH) && !h.ops.remove(h.ops.context, CONFIG_BACKUP_FILE_PATH))
        {
            h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
            return false;
        }
        if (!h.ops.rename(h.ops.context, CONFIG_FILE_PATH, CONFIG_BACKUP_FILE_PATH))
        {
            h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
            return false;
        }
    }

    if (!h.ops.rename(h.ops.context, CONFIG_TEMP_FILE_PATH, CONFIG_FILE_PATH))
    {
        if (had_primary && h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH))
            h.ops.rename(h.ops.context, CONFIG_BACKUP_FILE_PATH, CONFIG_FILE_PATH);
        h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
        return false;
    }

    if (!verifyConfigFile(h, CONFIG_FILE_PATH, normalized))
    {
        h.ops.remove(h.ops.context, CONFIG_FILE_PATH);
        if (had_primary && h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH))
            h.ops.rename(h.ops.context, CONFIG_BACKUP_FILE_PATH, CONFIG_FILE_PATH);
        return false;
    }

    h.cached = normalized;
    h.valid = true;
    return true;
}

bool NvsStore_InitAndLoad(NvsStoreHandle &h, AppConfig *out)
{
    bool storage_ok = mountFs(h);
    if (!storage_ok)
    {
        fillDefaults(h.cached);
        h.valid = true;
        if (out)
            *out = h.cached;
        return false;
    }

    AppConfig loaded{};
    bool requires_migration = false;
    if (h.ops.load(h.ops.context, CONFIG_FILE_PATH, loaded, &requires_migration))
    {
        h.cached = loaded;
        h.valid = true;
        if (requires_migration)
        {
            storage_ok = NvsStore_Save(h, loaded);
            if (storage_ok)
                storage_ok = writeSanitizedBackup(h, h.cached);
        }
        else
        {
            AppConfig backup{};
            bool backup_requires_migration = false;
            bool backup_valid = h.ops.load(h.ops.context, CONFIG_BACKUP_FILE_PATH, backup, &backup_requires_migration);
            if (!backup_valid || backup_requires_migration)
                storage_ok = writeSanitizedBackup(h, loaded);
        }
    }
    else
    {
        bool backup_requires_migration = false;
        if (h.ops.load(h.ops.context, CONFIG_BACKUP_FILE_PATH, loaded, &backup_requires_migration))
        {
            if (h.ops.exists(h.ops.context, CONFIG_FILE_PATH))
                h.ops.remove(h.ops.context, CONFIG_FILE_PATH);
            h.cached = loaded;
            h.valid = true;
            storage_ok = NvsStore_Save(h, loaded);
            if (storage_ok && backup_requires_migration)
                storage_ok = writeSanitizedBackup(h, h.cached);
        }
        else
        {
            if (h.ops.exists(h.ops.context, CONFIG_FILE_PATH))
                h.ops.remove(h.ops.context, CONFIG_FILE_PATH);
            if (h.ops.exists(h.ops.context, CONFIG_BACKUP_FILE_PATH))
                h.ops.remove(h.ops.context, CONFIG_BACKUP_FILE_PATH);
            if (h.ops.exists(h.ops.context, CONFIG_TEMP_FILE_PATH))
                h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
            if (h.ops.exists(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH))
                h.ops.remove(h.ops.context, CONFIG_BACKUP_TEMP_FILE_PATH);

            fillDefaults(h.cached);
            h.valid = true;
            storage_ok = NvsStore_Save(h, h.cached);
            if (storage_ok)
                storage_ok = writeSanitizedBackup(h, h.cached);
        }
    }

    if (out)
        *out = h.cached;
    return h.valid && storage_ok;
}

bool NvsStore_RestoreBackup(NvsStoreHandle &h, AppConfig *out)
{
    if (!mountFs(h))
        return false;
    AppConfig backup{};
    bool requires_migration = false;
    if (!h.ops.load(h.ops.context, CONFIG_BACKUP_FILE_PATH, backup, &requires_migration) || requires_migration)
        return false;
    if (!h.ops.write(h.ops.context, CONFIG_TEMP_FILE_PATH, backup) || !verifyConfigFile(h, CONFIG_TEMP_FILE_PATH, backup))
    {
        if (h.ops.exists(h.ops.context, CONFIG_TEMP_FILE_PATH)) h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
        return false;
    }
    if (h.ops.exists(h.ops.context, CONFIG_FILE_PATH) && !h.ops.remove(h.ops.context, CONFIG_FILE_PATH))
    {
        h.ops.remove(h.ops.context, CONFIG_TEMP_FILE_PATH);
        return false;
    }
    if (!h.ops.rename(h.ops.context, CONFIG_TEMP_FILE_PATH, CONFIG_FILE_PATH) || !verifyConfigFile(h, CONFIG_FILE_PATH, backup))
        return false;
    h.cached = backup;
    h.valid = true;
    if (out) *out = h.cached;
    return true;
}

bool NvsStore_StorageReady(const NvsStoreHandle &h) { return h.mounted; }

const AppConfig *NvsStore_GetCached(const NvsStoreHandle &h) { return h.valid ? &h.cached : nullptr; }
