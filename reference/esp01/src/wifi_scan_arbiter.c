#include "wifi_scan_arbiter.h"

#include <string.h>

static uint8_t owner_valid(WifiScanOwner owner)
{
    return (uint8_t)(owner == WIFI_SCAN_OWNER_STM ||
                     owner == WIFI_SCAN_OWNER_WEB);
}

void WifiScanArbiter_Init(WifiScanArbiter *arbiter)
{
    if (!arbiter) return;
    memset(arbiter, 0, sizeof(*arbiter));
}

WifiScanAcquireResult WifiScanArbiter_TryAcquire(WifiScanArbiter *arbiter,
                                                WifiScanOwner owner)
{
    if (!arbiter || !owner_valid(owner)) return WIFI_SCAN_INVALID;
    if (arbiter->owner == owner) return WIFI_SCAN_ALREADY_OWNED;
    if (arbiter->owner != WIFI_SCAN_OWNER_NONE)
    {
        arbiter->busy_rejections++;
        return WIFI_SCAN_BUSY;
    }
    arbiter->owner = owner;
    arbiter->acquisitions++;
    return WIFI_SCAN_ACQUIRED;
}

uint8_t WifiScanArbiter_Release(WifiScanArbiter *arbiter,
                               WifiScanOwner owner)
{
    if (!arbiter || !owner_valid(owner) || arbiter->owner != owner)
    {
        if (arbiter) arbiter->invalid_releases++;
        return 0u;
    }
    arbiter->owner = WIFI_SCAN_OWNER_NONE;
    arbiter->releases++;
    return 1u;
}

uint8_t WifiScanArbiter_IsOwner(const WifiScanArbiter *arbiter,
                               WifiScanOwner owner)
{
    return (uint8_t)(arbiter && owner_valid(owner) &&
                     arbiter->owner == owner);
}

WifiScanOwner WifiScanArbiter_GetOwner(const WifiScanArbiter *arbiter)
{
    return arbiter ? arbiter->owner : WIFI_SCAN_OWNER_NONE;
}
