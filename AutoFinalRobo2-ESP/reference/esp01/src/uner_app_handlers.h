#pragma once
#include "config.h"
#include "uner_handle.h"
#include "wifi_scan_arbiter.h"
#include "wifi_scan_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UNER_App {
  UNER_Handle handle;
  UNER_Transport serial_transport;
  uint8_t this_node;
  uint8_t peer_node;
  uint8_t last_sta_status;
  uint8_t last_wifi_mode;
  uint8_t last_ap_clients;
  uint8_t boot_complete_sent;
  uint8_t reboot_pending;
  uint8_t reboot_boot_mode;
  uint32_t reboot_not_before_ms;
} UNER_App;

typedef WifiScanSnapshotEntry UNER_WifiScanCacheEntry;

void UNER_App_Init(UNER_App *app, uint8_t this_node, uint8_t peer_node);
void UNER_App_Loop(UNER_App *app);
void UNER_App_OnCommandTable(UNER_App *app);
void UNER_App_NotifyWebserverUp(UNER_App *app);
void UNER_App_NotifyWsClientConnected(UNER_App *app, uint32_t client_id);
void UNER_App_NotifyWsClientDisconnected(UNER_App *app, uint32_t client_id);
void UNER_App_NotifyWebPageReady(UNER_App *app, uint32_t client_id, uint8_t client_number);
void UNER_App_NotifyWebPageClosed(UNER_App *app, uint32_t client_id, uint8_t client_number);
void UNER_App_NotifyApClientJoined(UNER_App *app, uint8_t ap_clients, uint8_t client_number);
void UNER_App_NotifyApClientLeft(UNER_App *app, uint8_t ap_clients, uint8_t client_number);
/* Re-publica el SoftAP que quedo operativo despues de fallar/expirar un
 * intento STA iniciado desde AP+STA. Incluye finalizador START_AP e IP. */
void UNER_App_NotifyApFallbackReady(UNER_App *app);
UNER_Status UNER_App_SendRawToPeer(UNER_App *app, uint8_t cmd, const uint8_t *payload, uint8_t len);
UNER_Status UNER_App_SendRawFromNode(UNER_App *app, uint8_t src_node, uint8_t dst_node, uint8_t cmd, const uint8_t *payload, uint8_t len);
UNER_Status UNER_App_SendEventToPeer(UNER_App *app, uint8_t evt_cmd, const uint8_t *payload, uint8_t len);
void UNER_App_UpdateScanCacheFromDriver(int network_count);
void UNER_App_UpdateScanSnapshotFromDriver(WifiScanSnapshot *snapshot, int network_count);
uint8_t UNER_App_GetScanCacheCount(void);
uint8_t UNER_App_GetScanCacheEntry(uint8_t index, UNER_WifiScanCacheEntry *out);
uint8_t UNER_App_FindScanCacheBySsid(const char *ssid, UNER_WifiScanCacheEntry *out);
WifiScanAcquireResult UNER_App_TryAcquireWifiScan(WifiScanOwner owner);
uint8_t UNER_App_ReleaseWifiScan(WifiScanOwner owner);
uint8_t UNER_App_IsWifiScanOwner(WifiScanOwner owner);
WifiScanOwner UNER_App_GetWifiScanOwner(void);

#ifdef __cplusplus
}
#endif
