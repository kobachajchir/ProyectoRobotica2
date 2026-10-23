#pragma once

#include <stdint.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_SCAN_SNAPSHOT_MAX 16u

typedef struct
{
    char ssid[SSID_MAX_LEN + 1];
    uint8_t bssid[6];
    int8_t signal_strength;
    uint8_t encryption_type;
    uint8_t channel;
} WifiScanSnapshotEntry;

typedef struct
{
    WifiScanSnapshotEntry entries[WIFI_SCAN_SNAPSHOT_MAX];
    uint8_t count;
} WifiScanSnapshot;

void WifiScanSnapshot_Init(WifiScanSnapshot *snapshot);
uint8_t WifiScanSnapshot_Append(WifiScanSnapshot *snapshot,
                                const WifiScanSnapshotEntry *entry);
uint8_t WifiScanSnapshot_Count(const WifiScanSnapshot *snapshot);
uint8_t WifiScanSnapshot_Get(const WifiScanSnapshot *snapshot, uint8_t index,
                             WifiScanSnapshotEntry *out);
uint8_t WifiScanSnapshot_FindBySsid(const WifiScanSnapshot *snapshot,
                                    const char *ssid,
                                    WifiScanSnapshotEntry *out);

#ifdef __cplusplus
}
#endif
