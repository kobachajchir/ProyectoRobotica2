#include "wifi_scan_snapshot.h"

#include <string.h>

void WifiScanSnapshot_Init(WifiScanSnapshot *snapshot)
{
    if (!snapshot) return;
    memset(snapshot, 0, sizeof(*snapshot));
}

uint8_t WifiScanSnapshot_Append(WifiScanSnapshot *snapshot,
                                const WifiScanSnapshotEntry *entry)
{
    if (!snapshot || !entry || snapshot->count >= WIFI_SCAN_SNAPSHOT_MAX)
        return 0u;
    snapshot->entries[snapshot->count++] = *entry;
    return 1u;
}

uint8_t WifiScanSnapshot_Count(const WifiScanSnapshot *snapshot)
{
    return snapshot ? snapshot->count : 0u;
}

uint8_t WifiScanSnapshot_Get(const WifiScanSnapshot *snapshot, uint8_t index,
                             WifiScanSnapshotEntry *out)
{
    if (!snapshot || !out || index >= snapshot->count) return 0u;
    *out = snapshot->entries[index];
    return 1u;
}

uint8_t WifiScanSnapshot_FindBySsid(const WifiScanSnapshot *snapshot,
                                    const char *ssid,
                                    WifiScanSnapshotEntry *out)
{
    if (!snapshot || !ssid || !ssid[0]) return 0u;
    for (uint8_t i = 0u; i < snapshot->count; ++i)
    {
        if (strcmp(snapshot->entries[i].ssid, ssid) != 0) continue;
        if (out) *out = snapshot->entries[i];
        return 1u;
    }
    return 0u;
}
