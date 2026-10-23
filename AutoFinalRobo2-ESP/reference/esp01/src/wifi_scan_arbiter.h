#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WIFI_SCAN_OWNER_NONE = 0,
    WIFI_SCAN_OWNER_STM = 1,
    WIFI_SCAN_OWNER_WEB = 2
} WifiScanOwner;

typedef enum {
    WIFI_SCAN_ACQUIRED = 0,
    WIFI_SCAN_ALREADY_OWNED,
    WIFI_SCAN_BUSY,
    WIFI_SCAN_INVALID
} WifiScanAcquireResult;

typedef struct {
    WifiScanOwner owner;
    uint32_t acquisitions;
    uint32_t releases;
    uint32_t busy_rejections;
    uint32_t invalid_releases;
} WifiScanArbiter;

void WifiScanArbiter_Init(WifiScanArbiter *arbiter);
WifiScanAcquireResult WifiScanArbiter_TryAcquire(WifiScanArbiter *arbiter,
                                                WifiScanOwner owner);
uint8_t WifiScanArbiter_Release(WifiScanArbiter *arbiter,
                               WifiScanOwner owner);
uint8_t WifiScanArbiter_IsOwner(const WifiScanArbiter *arbiter,
                               WifiScanOwner owner);
WifiScanOwner WifiScanArbiter_GetOwner(const WifiScanArbiter *arbiter);

#ifdef __cplusplus
}
#endif
