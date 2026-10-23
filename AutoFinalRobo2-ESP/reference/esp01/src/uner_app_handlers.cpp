#include "uner_app_handlers.h"

#include <string.h>

#include "config.h"
#include "boot_mode.h"
#include "uner_app_ports.h"
#include "wifi_scan_arbiter.h"

struct UnerApplicationState {
    bool scan_active = false;
    bool scan_stopped_by_command = false;
    bool scan_timed_out = false;
    uint32_t scan_started_ms = 0;
    uint8_t scan_notify_dst = UNER_NODE_MCU;
    uint8_t scan_notify_enabled = 0;
    uint8_t scan_finalizer_status = 0;
    uint8_t scan_last_count = 0;
    uint32_t scan_last_log_ms = 0;
    WifiScanSnapshot scan_cache{};
    WifiScanArbiter scan_arbiter{};
};
static UnerApplicationState s_uart;
static const UNER_AppServices *s_services = nullptr;
void UNER_App_SetServices(const UNER_AppServices &services) { s_services = &services; }
static const UNER_AppServices &webServices() {
  return s_services ? *s_services : UNER_App_DefaultServices();
}

static const uint32_t UNER_SCAN_WINDOW_MS = 10000u;

static const uint8_t UNER_SCAN_FINALIZER = 0xFEu;
static const uint8_t UNER_CONNECT_FINALIZER = 0xFEu;
static const uint8_t NET_IF_STA = 0x01u;
static const uint8_t NET_IF_AP = 0x02u;
static const uint8_t ESP_BOOT_DIAG_MARKER = 0xE5u;
static const uint8_t ESP_BOOT_DIAG_VERSION = 0x01u;

static AppConfig current_config_copy()
{
  const AppConfig *cached = webServices().config();
  return cached ? *cached : AppConfig{};
}

#ifndef UNER_SCAN_DEBUG
#define UNER_SCAN_DEBUG 0
#endif

#if UNER_SCAN_DEBUG
#define UNER_SCAN_LOG(...) Serial.printf(__VA_ARGS__)
#define UNER_SCAN_LOGLN(msg) Serial.println(msg)
#else
#define UNER_SCAN_LOG(...)
#define UNER_SCAN_LOGLN(msg)
#endif

static void clear_scan_cache()
{
  WifiScanSnapshot_Init(&s_uart.scan_cache);
}

void UNER_App_UpdateScanSnapshotFromDriver(WifiScanSnapshot *snapshot, int network_count)
{
  WifiScanSnapshot_Init(snapshot);
  if (!snapshot || network_count <= 0) return;

  uint8_t count = (uint8_t)((network_count > WIFI_SCAN_SNAPSHOT_MAX) ? WIFI_SCAN_SNAPSHOT_MAX : network_count);
  for (uint8_t i = 0; i < count; ++i)
  {
    UNER_WifiScanCacheEntry entry{};
    if (!webServices().scan_entry(i, &entry)) continue;
    (void)WifiScanSnapshot_Append(snapshot, &entry);
  }
}

void UNER_App_UpdateScanCacheFromDriver(int network_count)
{
  UNER_App_UpdateScanSnapshotFromDriver(&s_uart.scan_cache, network_count);
}

WifiScanAcquireResult UNER_App_TryAcquireWifiScan(WifiScanOwner owner)
{
  return WifiScanArbiter_TryAcquire(&s_uart.scan_arbiter, owner);
}

uint8_t UNER_App_ReleaseWifiScan(WifiScanOwner owner)
{
  return WifiScanArbiter_Release(&s_uart.scan_arbiter, owner);
}

uint8_t UNER_App_IsWifiScanOwner(WifiScanOwner owner)
{
  return WifiScanArbiter_IsOwner(&s_uart.scan_arbiter, owner);
}

WifiScanOwner UNER_App_GetWifiScanOwner(void)
{
  return WifiScanArbiter_GetOwner(&s_uart.scan_arbiter);
}

uint8_t UNER_App_GetScanCacheCount(void)
{
  return WifiScanSnapshot_Count(&s_uart.scan_cache);
}

uint8_t UNER_App_GetScanCacheEntry(uint8_t index, UNER_WifiScanCacheEntry *out)
{
  return WifiScanSnapshot_Get(&s_uart.scan_cache, index, out);
}

uint8_t UNER_App_FindScanCacheBySsid(const char *ssid, UNER_WifiScanCacheEntry *out)
{
  return WifiScanSnapshot_FindBySsid(&s_uart.scan_cache, ssid, out);
}

static uint8_t append_ipv4(uint8_t *out, uint8_t max_len, WifiIpv4 ip)
{
  if (!out || max_len < 4) return 0;
  out[0] = ip[0];
  out[1] = ip[1];
  out[2] = ip[2];
  out[3] = ip[3];
  return 4;
}

static uint8_t append_wifi_info(uint8_t *out, uint8_t max_len, bool prefer_sta)
{
  if (!out || max_len < 8) return 0;

  const AppConfig *c = webServices().config();
  const char *ssid = "";
  uint8_t mode = (uint8_t)webServices().platform->radio_mode();
  WifiIpv4 ip(0, 0, 0, 0);

  if (prefer_sta && webServices().platform->radio_status() == WIFI_LINK_CONNECTED)
  {
    ip = webServices().platform->sta_ip();
    ssid = c ? c->staSsid : "";
  }
  else if (mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA)
  {
    ip = webServices().platform->ap_ip();
    ssid = c ? c->apSsid : "";
  }
  else
  {
    ip = webServices().platform->sta_ip();
    ssid = c ? c->staSsid : "";
  }

  uint8_t ssid_len = 0;
  while (ssid[ssid_len] != '\0' && ssid_len < 32 && (uint8_t)(6u + ssid_len) < max_len)
  {
    ssid_len++;
  }

  uint8_t i = 0;
  out[i++] = mode;
  out[i++] = (uint8_t)webServices().platform->radio_status();
  out[i++] = ip[0];
  out[i++] = ip[1];
  out[i++] = ip[2];
  out[i++] = ip[3];
  out[i++] = ssid_len;
  for (uint8_t k = 0; k < ssid_len; ++k) out[i++] = (uint8_t)ssid[k];
  return i;
}

static void send_evt(UNER_App *app, uint8_t evt_cmd, const uint8_t *payload, uint8_t len)
{
//   Serial.printf("[UNER][EVT] cmd=0x%02X len=%u\n", evt_cmd, len);
  UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, evt_cmd, payload, len);
}

static void flush_uner_tx(UNER_App *app, uint16_t timeout_ms)
{
  if (!app) return;
  uint32_t start = webServices().platform->now_ms();
  while (app->handle.tx_q_count > 0 && (uint32_t)(webServices().platform->now_ms() - start) < timeout_ms)
  {
    UNER_Handle_TxPump(&app->handle);
    webServices().flush_serial();
    webServices().cooperate();
  }
  webServices().flush_serial();
}

static bool send_reboot_ack_and_flush(UNER_App *app, const UNER_Packet *pkt)
{
  if (!app || !pkt) return false;

  UNER_Status status = UNER_Handle_SendAck(&app->handle, pkt->dst, pkt->src,
                                           UNER_CMD_REBOOT_ESP, 0u);
  if (status == UNER_ERR_BUSY)
  {
    flush_uner_tx(app, 50u);
    status = UNER_Handle_SendAck(&app->handle, pkt->dst, pkt->src,
                                 UNER_CMD_REBOOT_ESP, 0u);
  }
  if (status != UNER_OK) return false;

  flush_uner_tx(app, 200u);
  return app->handle.tx_q_count == 0u;
}

static void send_reboot_nack_and_flush(UNER_App *app, const UNER_Packet *pkt,
                                       UNER_Status reason)
{
  if (!app || !pkt) return;

  UNER_Status status = UNER_Handle_SendNack(&app->handle, pkt->dst, pkt->src,
                                            UNER_CMD_REBOOT_ESP, (uint8_t)reason);
  if (status == UNER_ERR_BUSY)
  {
    flush_uner_tx(app, 50u);
    status = UNER_Handle_SendNack(&app->handle, pkt->dst, pkt->src,
                                  UNER_CMD_REBOOT_ESP, (uint8_t)reason);
  }
  if (status == UNER_OK) flush_uner_tx(app, 200u);
}

static void service_reboot_state_machine(UNER_App *app)
{
  if (!app || !app->reboot_pending) return;

  const uint32_t now = webServices().platform->now_ms();
  if (app->handle.tx_q_count > 0u)
  {
    UNER_Handle_TxPump(&app->handle);
    // La cola UART solo puede demorar el reboot hasta el deadline. El ACK del
    // comando UART ya fue vaciado sincronicamente antes de programarlo y el
    // reboot Web no depende de esta cola; un frame viejo no debe bloquear
    // webServices().restart() para siempre.
    if (app->handle.tx_q_count > 0u &&
        (int32_t)(now - app->reboot_not_before_ms) < 0)
      return;
  }
  if ((int32_t)(now - app->reboot_not_before_ms) < 0) return;

  UNER_SCAN_LOG("[UNER][REBOOT_ESP] executing boot_mode=%u\n", app->reboot_boot_mode);
  webServices().flush_serial();
  webServices().delay_ms(5u);
  webServices().restart();
}

static bool send_response_with_retry(UNER_App *app,
                                     const UNER_Packet *pkt,
                                     uint8_t status,
                                     const uint8_t *data,
                                     uint8_t data_len,
                                     uint16_t flush_timeout_ms)
{
  if (!app || !pkt) return false;

  UNER_Status send_status = UNER_Handle_SendResponse(&app->handle, pkt, status, data, data_len);
  if (send_status == UNER_ERR_BUSY)
  {
    flush_uner_tx(app, flush_timeout_ms);
    send_status = UNER_Handle_SendResponse(&app->handle, pkt, status, data, data_len);
  }

  if (send_status != UNER_OK)
  {
    return false;
  }

  flush_uner_tx(app, flush_timeout_ms);
  return true;
}

UNER_Status UNER_App_SendRawToPeer(UNER_App *app, uint8_t cmd, const uint8_t *payload, uint8_t len)
{
  if (!app) return UNER_ERR_LEN;
  return UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, cmd, payload, len);
}

UNER_Status UNER_App_SendRawFromNode(UNER_App *app, uint8_t src_node, uint8_t dst_node, uint8_t cmd, const uint8_t *payload, uint8_t len)
{
  if (!app) return UNER_ERR_LEN;
  return UNER_Handle_SendRaw(&app->handle, src_node, dst_node, cmd, payload, len);
}

UNER_Status UNER_App_SendEventToPeer(UNER_App *app, uint8_t evt_cmd, const uint8_t *payload, uint8_t len)
{
  if (!app) return UNER_ERR_LEN;
  return UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, evt_cmd, payload, len);
}

static void log_cmd(const char* name, const UNER_Packet *pkt)
{
//   Serial.printf("[UNER][CMD] %s (0x%02X) len=%u src=%u dst=%u\n", name, pkt->cmd, pkt->len, pkt->src, pkt->dst);
}

static uint8_t fill_network_ip_payload(uint8_t out[5], uint8_t net_if, const WifiIpv4 &ip)
{
  out[0] = net_if;
  out[1] = ip[0];
  out[2] = ip[1];
  out[3] = ip[2];
  out[4] = ip[3];
  return 5;
}

static void emit_network_ip_evt_and_cmd(UNER_App *app, uint8_t net_if, const WifiIpv4 &ip)
{
  uint8_t payload[5];
  uint8_t len = fill_network_ip_payload(payload, net_if, ip);
  send_evt(app, UNER_EVT_NETWORK_IP, payload, len);
  UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, UNER_CMD_NETWORK_IP, payload, len);
}

static void send_boot_complete_evt_and_cmd(UNER_App *app, uint8_t net_if, const WifiIpv4 &ip)
{
  if (!app || app->boot_complete_sent) return;

  uint8_t payload[5];
  uint8_t len = fill_network_ip_payload(payload, net_if, ip);
  send_evt(app, UNER_EVT_BOOT_COMPLETE, payload, len);
  UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, UNER_CMD_BOOT_COMPLETE, payload, len);
  app->boot_complete_sent = 1u;
}

static void fill_interfaces_connected(uint8_t out[4])
{
  // [PC, WEB, RF, ESP]
  out[0] = 1; // PC/UART bridge assumed present while receiving commands
  out[1] = webServices().connected_count() > 0 ? 1 : 0;
  out[2] = 0; // RF not implemented yet
  out[3] = 1; // ESP itself alive
}

static uint8_t build_scan_results_payload(uint8_t *out, uint8_t max_len)
{
  if (!out || max_len == 0) return 0;
  uint8_t i = 0;
  uint8_t count = 0;
  out[i++] = 0; // count placeholder

  const uint8_t cached_count = WifiScanSnapshot_Count(&s_uart.scan_cache);
  for (uint8_t idx = 0; idx < cached_count; ++idx)
  {
    UNER_WifiScanCacheEntry entry;
    if (!WifiScanSnapshot_Get(&s_uart.scan_cache, idx, &entry)) continue;
    const char *ssid = entry.ssid;
    uint8_t slen = 0;
    while (ssid[slen] && slen < SSID_MAX_LEN) slen++;
    if (slen > 32) slen = 32;
    if ((uint16_t)(i + 1u + slen) > max_len) break;
    out[i++] = slen;
    for (uint8_t k = 0; k < slen; ++k) out[i++] = (uint8_t)ssid[k];
    count++;
  }

  out[0] = count;
  return i;
}

static uint8_t append_reset_diagnostic(uint8_t *out, uint8_t max_len)
{
  if (!out || max_len < 4u) return 0u;

  uint8_t reason = 0xFFu;
  uint8_t exception = 0xFFu;
  webServices().reset_diagnostic(&reason, &exception);
  out[0] = ESP_BOOT_DIAG_MARKER;
  out[1] = ESP_BOOT_DIAG_VERSION;
  out[2] = reason;
  out[3] = exception;
  return 4u;
}

static uint8_t current_network_interface(WifiIpv4 &ip)
{
  const WifiRadioMode mode = webServices().platform->radio_mode();
  if (mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA)
  {
    ip = webServices().platform->ap_ip();
    return NET_IF_AP;
  }

  ip = webServices().platform->radio_status() == WIFI_LINK_CONNECTED ? webServices().platform->sta_ip() : WifiIpv4(0, 0, 0, 0);
  return NET_IF_STA;
}

static uint8_t append_wifi_detail_payload(const UNER_WifiScanCacheEntry *entry, uint8_t *out, uint8_t max_len)
{
  if (!entry || !out) return 0;
  uint8_t ssid_len = 0;
  while (entry->ssid[ssid_len] && ssid_len < SSID_MAX_LEN) ssid_len++;
  if ((uint16_t)(ssid_len + 4u) > max_len) return 0;

  uint8_t i = 0;
  out[i++] = ssid_len;
  for (uint8_t k = 0; k < ssid_len; ++k) out[i++] = (uint8_t)entry->ssid[k];
  out[i++] = (uint8_t)entry->signal_strength;
  out[i++] = entry->encryption_type;
  out[i++] = entry->channel;
  return i;
}

static void send_scan_results_async(UNER_App *app, uint8_t status)
{
  if (!s_uart.scan_notify_enabled || !app)
  {
    UNER_SCAN_LOG("[UNER][SCAN] finalizer skip enabled=%u app=%u status=%u active=%u dst=%u\n",
                  s_uart.scan_notify_enabled, app ? 1u : 0u, status, s_uart.scan_active ? 1u : 0u, s_uart.scan_notify_dst);
    return;
  }

  uint8_t payload[1] = {UNER_SCAN_FINALIZER}; // solo terminador; resultados se piden con GET_SCAN_RESULTS
  s_uart.scan_finalizer_status = status;
  UNER_Status st = UNER_Handle_SendRaw(&app->handle, app->this_node, s_uart.scan_notify_dst, UNER_CMD_GET_SCAN_RESULTS, payload, 1);
  if (st == UNER_OK)
  {
    UNER_SCAN_LOG("[UNER][SCAN] finalizer sent cmd=0x%02X dst=%u status=%u payload=[0x%02X]\n",
                  UNER_CMD_GET_SCAN_RESULTS, s_uart.scan_notify_dst, status, UNER_SCAN_FINALIZER);
    s_uart.scan_notify_enabled = 0;
  }
  else
  {
    UNER_SCAN_LOG("[UNER][SCAN] finalizer retry pending send_status=%d dst=%u status=%u\n",
                  (int)st, s_uart.scan_notify_dst, status);
  }
}

static void service_scan_state_machine(UNER_App *app)
{
  if (!s_uart.scan_active)
  {
    if (s_uart.scan_notify_enabled) send_scan_results_async(app, s_uart.scan_finalizer_status);
    return;
  }

  if (!UNER_App_IsWifiScanOwner(WIFI_SCAN_OWNER_STM))
  {
    /* El driver ya pertenece a otro origen. No consumir ni borrar sus
     * resultados desde la maquina de estados STM. */
    s_uart.scan_active = false;
    s_uart.scan_notify_enabled = 0u;
    return;
  }

  int n = webServices().wifi.ScanComplete();
  uint32_t now = webServices().platform->now_ms();
  if ((uint32_t)(now - s_uart.scan_last_log_ms) >= 500u)
  {
    UNER_SCAN_LOG("[UNER][SCAN] poll scanComplete=%d active=%u notify_enabled=%u notify_dst=%u elapsed=%lu\n",
                  n, s_uart.scan_active ? 1u : 0u, s_uart.scan_notify_enabled, s_uart.scan_notify_dst,
                  (unsigned long)(now - s_uart.scan_started_ms));
    s_uart.scan_last_log_ms = now;
  }
  if (n >= 0)
  {
    s_uart.scan_active = false;
    s_uart.scan_timed_out = false;
    s_uart.scan_last_count = (uint8_t)((n > 255) ? 255 : (n < 0 ? 0 : n));
    UNER_App_UpdateScanCacheFromDriver(n);
    UNER_SCAN_LOG("[UNER][SCAN] completed within window networks=%d notify_enabled=%u dst=%u\n",
                  n, s_uart.scan_notify_enabled, s_uart.scan_notify_dst);

    webServices().wifi.DeleteScanResults();
    (void)UNER_App_ReleaseWifiScan(WIFI_SCAN_OWNER_STM);
    send_scan_results_async(app, 0);
    return;
  }

  if ((uint32_t)(now - s_uart.scan_started_ms) >= UNER_SCAN_WINDOW_MS)
  {
    int n_end = webServices().wifi.ScanComplete();
    s_uart.scan_active = false;
    s_uart.scan_timed_out = n_end < 0;
    s_uart.scan_last_count = (uint8_t)((n_end > 255) ? 255 : (n_end < 0 ? 0 : n_end));
    UNER_App_UpdateScanCacheFromDriver(n_end);
    UNER_SCAN_LOG("[UNER][SCAN] timeout window=%lu scanComplete=%d notify_enabled=%u dst=%u\n",
                  (unsigned long)UNER_SCAN_WINDOW_MS, n_end, s_uart.scan_notify_enabled, s_uart.scan_notify_dst);

    webServices().wifi.DeleteScanResults();
    (void)UNER_App_ReleaseWifiScan(WIFI_SCAN_OWNER_STM);
    send_scan_results_async(app, s_uart.scan_timed_out ? 3u : 0u);
  }
}

static void emit_mode_and_link_events(UNER_App *app)
{
  uint8_t mode = (uint8_t)webServices().platform->radio_mode();
  uint8_t sta = (uint8_t)webServices().platform->radio_status();
  uint8_t ap_clients = webServices().platform->ap_clients();

  if (mode != app->last_wifi_mode)
  {
    uint8_t payload[64];
    uint8_t len = append_wifi_info(payload, sizeof(payload), false);
    send_evt(app, UNER_EVT_MODE_CHANGED, payload, len);

    if (mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA)
    {
      WifiIpv4 apIp = webServices().platform->ap_ip();
      uint8_t ap_done[5] = {UNER_CONNECT_FINALIZER, apIp[0], apIp[1], apIp[2], apIp[3]};
      UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, UNER_CMD_START_AP, ap_done, sizeof(ap_done));
      emit_network_ip_evt_and_cmd(app, NET_IF_AP, apIp);
      send_boot_complete_evt_and_cmd(app, NET_IF_AP, apIp);
//       Serial.printf("[UNER][START_AP] finalizador enviado=0x%02X ip=%u.%u.%u.%u\n", UNER_CONNECT_FINALIZER, apIp[0], apIp[1], apIp[2], apIp[3]);
    }

    app->last_wifi_mode = mode;
  }

  if (sta != app->last_sta_status)
  {
    uint8_t payload[64];
    uint8_t len = append_wifi_info(payload, sizeof(payload), true);
    if (sta == WIFI_LINK_CONNECTED)
    {
      send_evt(app, UNER_EVT_STA_CONNECTED, payload, len);
      uint8_t wifi_payload[64];
      uint8_t wlen = append_wifi_info(wifi_payload, sizeof(wifi_payload), true);
      send_evt(app, UNER_EVT_WIFI_CONNECTED, wifi_payload, wlen);

      WifiIpv4 staIp = webServices().platform->sta_ip();
      uint8_t sta_done[5] = {UNER_CONNECT_FINALIZER, staIp[0], staIp[1], staIp[2], staIp[3]};
      UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node, UNER_CMD_CONNECT_WIFI, sta_done, sizeof(sta_done));
      emit_network_ip_evt_and_cmd(app, NET_IF_STA, staIp);
      send_boot_complete_evt_and_cmd(app, NET_IF_STA, staIp);
//       Serial.printf("[UNER][CONNECT_WIFI] finalizador enviado=0x%02X ip=%u.%u.%u.%u\n", UNER_CONNECT_FINALIZER, staIp[0], staIp[1], staIp[2], staIp[3]);
    }
    else
    {
      send_evt(app, UNER_EVT_STA_DISCONNECTED, payload, len);
    }
    app->last_sta_status = sta;
  }

  if (ap_clients != app->last_ap_clients)
  {
    uint8_t p[1] = {ap_clients};
    if (ap_clients > app->last_ap_clients)
      send_evt(app, UNER_EVT_AP_CLIENT_JOIN, p, 1);
    else
      send_evt(app, UNER_EVT_AP_CLIENT_LEAVE, p, 1);
    app->last_ap_clients = ap_clients;
  }
}

static void h_ping(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("PING", pkt);
  // El wire no posee un bit request/response. F4 WEB envia request vacio
  // legacy o U16 periodMs. La response contiene status (len=1) y no debe
  // volver a responderse para evitar un eco infinito.
  if (!pkt || (pkt->len != 0u && pkt->len != 2u)) return;
  UNER_Handle_SendResponse(&app->handle, pkt, 0, nullptr, 0);
}

static void h_echo(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("ECHO", pkt);
  UNER_Handle_SendResponse(&app->handle, pkt, 0, pkt->payload, pkt->len);
}

static void h_get_status(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_STATUS", pkt);

  uint8_t data[104];
  uint8_t idx = 0;
  uint8_t ap_active = (webServices().platform->radio_mode() == WIFI_RADIO_AP || webServices().platform->radio_mode() == WIFI_RADIO_AP_STA) ? 1u : 0u;
  uint8_t wifi_active = (webServices().platform->radio_status() == WIFI_LINK_CONNECTED) ? 1u : 0u;
  data[idx++] = ap_active;
  data[idx++] = wifi_active;

  uint8_t rest = append_wifi_info(&data[idx], (uint8_t)(sizeof(data) - idx), true);
  idx = (uint8_t)(idx + rest);

  WifiIpv4 staIp = webServices().platform->sta_ip();
  WifiIpv4 apIp = webServices().platform->ap_ip();
  idx = (uint8_t)(idx + append_ipv4(&data[idx], (uint8_t)(sizeof(data) - idx), staIp));
  idx = (uint8_t)(idx + append_ipv4(&data[idx], (uint8_t)(sizeof(data) - idx), apIp));

  // Extension GET_STATUS para la pantalla fisica Info AP:
  // [ap_ssid_len, ap_ssid, ap_pass_preview_len, ap_pass_preview, clients].
  // El password se limita a los 16 caracteres que entran en la OLED; nunca
  // se publica en el JSON Web.
  const AppConfig *config = webServices().config();
  const char *ap_ssid = config ? config->apSsid : "";
  const char *ap_pass = config ? config->apPass : "";
  uint8_t ap_ssid_len = 0u;
  uint8_t ap_pass_len = 0u;
  while (ap_ssid[ap_ssid_len] != '\0' && ap_ssid_len < SSID_MAX_LEN) ap_ssid_len++;
  while (ap_pass[ap_pass_len] != '\0' && ap_pass_len < 16u) ap_pass_len++;
  data[idx++] = ap_ssid_len;
  memcpy(&data[idx], ap_ssid, ap_ssid_len);
  idx = (uint8_t)(idx + ap_ssid_len);
  data[idx++] = ap_pass_len;
  memcpy(&data[idx], ap_pass, ap_pass_len);
  idx = (uint8_t)(idx + ap_pass_len);
  data[idx++] = webServices().platform->ap_clients();

  uint8_t len = idx;
//   Serial.printf("[UNER][GET_STATUS] ap_active=%u wifi_active=%u mode=%u wl=%u\n", ap_active, wifi_active, webServices().platform->radio_mode(), webServices().platform->radio_status());

  UNER_Handle_SendResponse(&app->handle, pkt, 0, data, len);
}

static void h_get_preferences(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_PREFERENCES", pkt);
  const AppConfig *c = webServices().config();
  uint8_t p[96];
  uint8_t i = 0;
  const char *ap = c ? c->apSsid : "";
  const char *sta = c ? c->staSsid : "";
  uint8_t ap_len = 0, sta_len = 0;
  while (ap[ap_len] && ap_len < 32) ap_len++;
  while (sta[sta_len] && sta_len < 32) sta_len++;
  p[i++] = ap_len;
  for (uint8_t k = 0; k < ap_len; ++k) p[i++] = (uint8_t)ap[k];
  p[i++] = sta_len;
  for (uint8_t k = 0; k < sta_len; ++k) p[i++] = (uint8_t)sta[k];
  UNER_Handle_SendResponse(&app->handle, pkt, 0, p, i);
}

static void h_set_mode_ap(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_MODE_AP", pkt);
  const AppConfig *c = webServices().config();
  const char *apSsid = c ? c->apSsid : DEFAULT_AP_SSID;
  const char *apPass = c ? c->apPass : DEFAULT_AP_PASSWORD;

  const bool configured = !c || webServices().wifi.ApplyAPConfig(*c);
  const bool started = configured && webServices().wifi.StartAP(apSsid, apPass, false);
  const WifiRadioMode mode = webServices().wifi.GetMode();
  const bool ok = started && (mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA);

  uint8_t data[64];
  uint8_t len = append_wifi_info(data, sizeof(data), false);
  UNER_Handle_SendResponse(&app->handle, pkt, ok ? 0u : 1u, data, len);
  if (ok) send_evt(app, UNER_EVT_MODE_CHANGED, data, len);
}

static void h_set_mode_sta(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_MODE_STA", pkt);
  const AppConfig *c = webServices().config();
  const char *staSsid = c ? c->staSsid : DEFAULT_STA_SSID;
  const char *staPass = c ? c->staPass : DEFAULT_STA_PASSWORD;

  webServices().wifi.SetSTAReconnectEnabled(false);
  const bool configured = staSsid[0] != '\0' && (!c || webServices().wifi.ApplySTAConfig(*c));
  const WifiLinkStatus start_status = configured
                                       ? webServices().wifi.ConnectSTA(staSsid, staPass, false, false)
                                       : WIFI_LINK_CONNECT_FAILED;
  const bool started = start_status != WIFI_LINK_CONNECT_FAILED;

  uint8_t data[64];
  uint8_t len = append_wifi_info(data, sizeof(data), true);
  UNER_Handle_SendResponse(&app->handle, pkt, started ? 0u : 1u, data, len);
  if (started) send_evt(app, UNER_EVT_MODE_CHANGED, data, len);
}

static void h_set_credentials(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_CREDENTIALS", pkt);
  AppConfig c = current_config_copy();

  if (pkt->len < 3)
  {
    UNER_Handle_SendResponse(&app->handle, pkt, 1, nullptr, 0);
    return;
  }

  uint8_t off = 0;
  uint8_t target = pkt->payload[off++]; // 0:AP 1:STA
  uint8_t ssid_len = pkt->payload[off++];
  if ((uint16_t)off + ssid_len + 1u > pkt->len || ssid_len > SSID_MAX_LEN)
  {
    UNER_Handle_SendResponse(&app->handle, pkt, 2, nullptr, 0);
    return;
  }
  char ssid[SSID_MAX_LEN + 1];
  for (uint8_t i = 0; i < ssid_len; ++i) ssid[i] = (char)pkt->payload[off + i];
  ssid[ssid_len] = '\0';
  off += ssid_len;

  uint8_t pass_len = pkt->payload[off++];
  if ((uint16_t)off + pass_len > pkt->len || pass_len > PASS_MAX_LEN)
  {
    UNER_Handle_SendResponse(&app->handle, pkt, 3, nullptr, 0);
    return;
  }
  char pass[PASS_MAX_LEN + 1];
  for (uint8_t i = 0; i < pass_len; ++i) pass[i] = (char)pkt->payload[off + i];
  pass[pass_len] = '\0';

  if (target == 1 && strcmp(pass, "connRequest") == 0)
  {
    webServices().request_credentials(ssid);
    UNER_Handle_SendResponse(&app->handle, pkt, 0, nullptr, 0);
    return;
  }

  if (target == 0)
  {
    strncpy(c.apSsid, ssid, sizeof(c.apSsid));
    c.apSsid[sizeof(c.apSsid) - 1] = '\0';
    strncpy(c.apPass, pass, sizeof(c.apPass));
    c.apPass[sizeof(c.apPass) - 1] = '\0';
  }
  else
  {
    strncpy(c.staSsid, ssid, sizeof(c.staSsid));
    c.staSsid[sizeof(c.staSsid) - 1] = '\0';
    strncpy(c.staPass, pass, sizeof(c.staPass));
    c.staPass[sizeof(c.staPass) - 1] = '\0';
  }

  bool ok = webServices().save_config(c);
  // La confirmacion de 0x12 nunca refleja el SSID ni la clave recibida. El
  // resultado asincrono de la conexion se informa por 0x5C sin password.
  UNER_Handle_SendResponse(&app->handle, pkt, ok ? 0 : 4, nullptr, 0);
}

static void h_clear_credentials(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("CLEAR_CREDENTIALS", pkt);
  AppConfig c{};
  c.schemaVersion = CONFIG_SCHEMA_VERSION;
  c.initializedNvs = NVS_MAGIC_VALUE;
  strncpy(c.apSsid, DEFAULT_AP_SSID, sizeof(c.apSsid));
  strncpy(c.apPass, DEFAULT_AP_PASSWORD, sizeof(c.apPass));
  strncpy(c.staSsid, DEFAULT_STA_SSID, sizeof(c.staSsid));
  strncpy(c.staPass, DEFAULT_STA_PASSWORD, sizeof(c.staPass));
  strncpy(c.staIp, "0.0.0.0", sizeof(c.staIp));
  strncpy(c.staGateway, "0.0.0.0", sizeof(c.staGateway));
  strncpy(c.staSubnet, "255.255.255.0", sizeof(c.staSubnet));
  strncpy(c.apIp, "0.0.0.0", sizeof(c.apIp));
  c.nextBootMode = APP_BOOT_MODE_NORMAL;
  c.apSsid[sizeof(c.apSsid) - 1] = '\0';
  c.apPass[sizeof(c.apPass) - 1] = '\0';
  c.staSsid[sizeof(c.staSsid) - 1] = '\0';
  c.staPass[sizeof(c.staPass) - 1] = '\0';

  bool ok = webServices().save_config(c);
  UNER_Handle_SendResponse(&app->handle, pkt, ok ? 0 : 1, nullptr, 0);
}

static void h_factory_reset(void *ctx, const UNER_Packet *pkt)
{
  h_clear_credentials(ctx, pkt);
}

static void h_reboot_esp(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("REBOOT_ESP", pkt);
  if (!app || !pkt || pkt->len != 1u)
  {
    send_reboot_nack_and_flush(app, pkt, UNER_ERR_ARGS);
    return;
  }

  const uint8_t boot_mode = pkt->payload[0];
  if (!BootMode_IsValid(boot_mode))
  {
    send_reboot_nack_and_flush(app, pkt, UNER_ERR_ARGS);
    return;
  }
  if (app->reboot_pending)
  {
    if (app->reboot_boot_mode == boot_mode)
    {
      // Una retransmision identica confirma la misma operacion ya persistida.
      // No mueve el deadline ni programa un segundo reinicio.
      (void)send_reboot_ack_and_flush(app, pkt);
    }
    else
    {
      send_reboot_nack_and_flush(app, pkt, UNER_ERR_BUSY);
    }
    return;
  }

  const AppConfig previous = current_config_copy();
  AppConfig next{};
  if (!BootMode_ApplyRequest(&previous, boot_mode, &next))
  {
    send_reboot_nack_and_flush(app, pkt, UNER_ERR_ARGS);
    return;
  }
  if (!webServices().save_config(next))
  {
    UNER_SCAN_LOG("[UNER][REBOOT_ESP] boot_mode=%u persistence_failed\n", boot_mode);
    send_reboot_nack_and_flush(app, pkt, UNER_ERR_BUSY);
    return;
  }

  if (!send_reboot_ack_and_flush(app, pkt))
  {
    (void)webServices().save_config(previous);
    UNER_SCAN_LOG("[UNER][REBOOT_ESP] boot_mode=%u ack_failed rollback_requested\n", boot_mode);
    return;
  }

  UNER_SCAN_LOG("[UNER][REBOOT_ESP] boot_mode=%u saved=1 reboot_scheduled\n", boot_mode);
  send_evt(app, UNER_EVT_USB_DISCONNECTED, nullptr, 0);
  app->reboot_boot_mode = boot_mode;
  app->reboot_not_before_ms = webServices().platform->now_ms() + 25u;
  app->reboot_pending = 1u;
}

static void h_request_firmware(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("REQUEST_FIRMWARE", pkt);
  const uint8_t fw[] = ESP_FIRMWARE_VERSION;
  UNER_Handle_SendResponse(&app->handle, pkt, 0, fw, (uint8_t)(sizeof(fw) - 1));
}

#if 0
// Autoridad PIN legacy del ESP, conservada solo como referencia de migracion.
// No se compila ni se registra: 0x51/0x5D pertenecen exclusivamente a F4.
static bool stored_test_pin_matches(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  (void)d0;
  (void)d1;
  (void)d2;
  (void)d3;
  return false;
}

static bool validate_test_pin_ascii4(const uint8_t *ascii4)
{
  if (!ascii4)
    return false;

  if (ascii4[0] < '0' || ascii4[0] > '9')
    return false;
  if (ascii4[1] < '0' || ascii4[1] > '9')
    return false;
  if (ascii4[2] < '0' || ascii4[2] > '9')
    return false;
  if (ascii4[3] < '0' || ascii4[3] > '9')
    return false;

  return stored_test_pin_matches(
      (uint8_t)(ascii4[0] - '0'),
      (uint8_t)(ascii4[1] - '0'),
      (uint8_t)(ascii4[2] - '0'),
      (uint8_t)(ascii4[3] - '0'));
}

static bool is_pin_ascii4_format_valid(const uint8_t *ascii4)
{
  if (!ascii4)
    return false;

  return (ascii4[0] >= '0' && ascii4[0] <= '9' &&
          ascii4[1] >= '0' && ascii4[1] <= '9' &&
          ascii4[2] >= '0' && ascii4[2] <= '9' &&
          ascii4[3] >= '0' && ascii4[3] <= '9');
}

static bool save_test_pin_ascii4(const uint8_t *ascii4)
{
  (void)ascii4;
  return false;
}

static void send_auth_validate_pin_response(UNER_App *app, const UNER_Packet *pkt, uint8_t status,
                                            uint8_t request_id, uint8_t permission_id, bool granted,
                                            uint32_t ttl_ms, uint8_t attempts_left)
{
  uint8_t data[8] = {
      request_id,
      permission_id,
      (uint8_t)(granted ? 1u : 0u),
      (uint8_t)(ttl_ms & 0xFFu),
      (uint8_t)((ttl_ms >> 8) & 0xFFu),
      (uint8_t)((ttl_ms >> 16) & 0xFFu),
      (uint8_t)((ttl_ms >> 24) & 0xFFu),
      attempts_left};
  (void)send_response_with_retry(app, pkt, status, data, sizeof(data), 100);
}

static bool auth_permission_id_supported(uint8_t permission_id)
{
  (void)permission_id;
  return false;
}

static uint32_t auth_ttl_for_permission(uint8_t permission_id)
{
  (void)permission_id;
  return 0u;
}

static void h_auth_validate_pin(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("AUTH_VALIDATE_PIN", pkt);

  if (!pkt)
  {
    return;
  }

  if (!pkt->payload || pkt->len != 6u)
  {
    (void)send_response_with_retry(app, pkt, 1u, nullptr, 0u, 100);
    return;
  }

  const uint8_t request_id = pkt->payload[0];
  const uint8_t permission_id = pkt->payload[1];
  const bool permission_ok = auth_permission_id_supported(permission_id);
  const bool digits_ok =
      (pkt->payload[2] <= 9u &&
       pkt->payload[3] <= 9u &&
       pkt->payload[4] <= 9u &&
       pkt->payload[5] <= 9u);

  bool granted = false;
  uint32_t ttl_ms = 0u;

  if (permission_ok && digits_ok)
  {
    uint8_t pin_ascii4[4] = {
        (uint8_t)('0' + pkt->payload[2]),
        (uint8_t)('0' + pkt->payload[3]),
        (uint8_t)('0' + pkt->payload[4]),
        (uint8_t)('0' + pkt->payload[5]),
    };

    granted = validate_test_pin_ascii4(pin_ascii4);

    if (granted)
    {
      s_auth_pin_attempts_left = AUTH_PIN_MAX_ATTEMPTS;
      ttl_ms = auth_ttl_for_permission(permission_id);
    }
    else if (s_auth_pin_attempts_left > 0u)
    {
      s_auth_pin_attempts_left--;
    }
  }

  send_auth_validate_pin_response(app,
                                  pkt,
                                  0u,
                                  request_id,
                                  permission_id,
                                  granted,
                                  ttl_ms,
                                  s_auth_pin_attempts_left);
}

static void send_app_pin_config_response(UNER_App *app,
                                         const UNER_Packet *pkt,
                                         uint8_t status,
                                         uint8_t action,
                                         uint8_t code)
{
  uint8_t data[2] = {action, code};
  (void)send_response_with_retry(app, pkt, status, data, sizeof(data), 100);
}

static void h_app_pin_config(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("APP_PIN_CONFIG", pkt);

  if (!pkt || pkt->len < 1)
  {
    send_app_pin_config_response(app, pkt, 1, 0x00u, 1u);
    return;
  }

  const uint8_t action = pkt->payload[0];

  switch (action)
  {
  case 0x01u: /* LOGIN */
  case 0x03u: /* VALIDATE_SCREEN */
  {
    if (pkt->len != 5)
    {
      send_app_pin_config_response(app, pkt, 1, action, 1u);
      return;
    }

    const uint8_t *pin_ascii4 = &pkt->payload[1];
    const bool ok = validate_test_pin_ascii4(pin_ascii4);

    send_app_pin_config_response(app, pkt, 0, action, ok ? 0u : 2u);
    return;
  }

  case 0x02u: /* CHANGE */
  {
    if (pkt->len != 9)
    {
      send_app_pin_config_response(app, pkt, 1, action, 1u);
      return;
    }

    const uint8_t *current_pin = &pkt->payload[1];
    const uint8_t *new_pin = &pkt->payload[5];

    if (!validate_test_pin_ascii4(current_pin))
    {
      send_app_pin_config_response(app, pkt, 0, action, 2u);
      return;
    }

    if (!is_pin_ascii4_format_valid(new_pin))
    {
      send_app_pin_config_response(app, pkt, 0, action, 3u);
      return;
    }

    if (!save_test_pin_ascii4(new_pin))
    {
      send_app_pin_config_response(app, pkt, 0, action, 4u);
      return;
    }

    send_app_pin_config_response(app, pkt, 0, action, 0u);
    return;
  }

  default:
  {
    send_app_pin_config_response(app, pkt, 2, action, 1u);
    return;
  }
  }
}
#endif

static bool read_uner_text_field(const UNER_Packet *pkt,
                                 uint8_t *offset,
                                 char *out,
                                 uint8_t capacity,
                                 bool allow_empty)
{
  if (!pkt || !offset || !out || capacity == 0u || *offset >= pkt->len) return false;
  const uint8_t length = pkt->payload[(*offset)++];
  if ((!allow_empty && length == 0u) || length >= capacity ||
      (uint16_t)(*offset) + length > pkt->len) return false;
  memcpy(out, &pkt->payload[*offset], length);
  out[length] = '\0';
  *offset = (uint8_t)(*offset + length);
  return true;
}

static uint8_t build_wifi_credentials_flow_data(uint8_t action,
                                                uint8_t extra_flags,
                                                uint8_t *out,
                                                uint8_t capacity)
{
  char sta_ssid[SSID_MAX_LEN + 1u];
  char ap_ssid[SSID_MAX_LEN + 1u];
  uint8_t flags = 0u;
  if (!out || capacity < 4u ||
      webServices().credentials_state(&flags,
                                           sta_ssid, sizeof(sta_ssid),
                                           ap_ssid, sizeof(ap_ssid)) != 0u) return 0u;
  uint8_t offset = 0u;
  out[offset++] = action;
  out[offset++] = (uint8_t)(flags | extra_flags);
  const uint8_t sta_len = (uint8_t)strnlen(sta_ssid, SSID_MAX_LEN);
  const uint8_t ap_len = (uint8_t)strnlen(ap_ssid, SSID_MAX_LEN);
  if ((uint16_t)offset + 2u + sta_len + ap_len > capacity) return 0u;
  out[offset++] = sta_len;
  memcpy(&out[offset], sta_ssid, sta_len);
  offset = (uint8_t)(offset + sta_len);
  out[offset++] = ap_len;
  memcpy(&out[offset], ap_ssid, ap_len);
  return (uint8_t)(offset + ap_len);
}

static void h_wifi_credentials_flow(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  if (!app || !pkt || !pkt->payload || pkt->len < 1u) return;
  const uint8_t action = pkt->payload[0];
  uint8_t status = 0u;
  uint8_t extra_flags = 0u;
  if (action == 0u) {
    if (pkt->len != 1u) status = 1u;
  } else if (action == 1u) {
    char ssid[SSID_MAX_LEN + 1u];
    char password[PASS_MAX_LEN + 1u];
    char ip[16u];
    char gateway[16u];
    char subnet[16u];
    char bssid[18u];
    uint8_t offset = 2u;
    if (pkt->len < 2u ||
        !read_uner_text_field(pkt, &offset, ssid, sizeof(ssid), false) ||
        !read_uner_text_field(pkt, &offset, password, sizeof(password), true) ||
        !read_uner_text_field(pkt, &offset, ip, sizeof(ip), true) ||
        !read_uner_text_field(pkt, &offset, gateway, sizeof(gateway), true) ||
        !read_uner_text_field(pkt, &offset, subnet, sizeof(subnet), true) ||
        !read_uner_text_field(pkt, &offset, bssid, sizeof(bssid), true) ||
        offset != pkt->len) {
      status = 1u;
    } else {
      const bool reuse_password = (pkt->payload[1] & 0x01u) != 0u;
      status = webServices().submit_credentials(
          ssid, password, reuse_password, ip, gateway, subnet, bssid);
      if (status == 0u) {
        extra_flags |= 0x08u;
        if (reuse_password) extra_flags |= 0x10u;
      }
    }
  } else if (action == 2u) {
    char ssid[SSID_MAX_LEN + 1u];
    uint8_t offset = 1u;
    if (!read_uner_text_field(pkt, &offset, ssid, sizeof(ssid), false) ||
        offset != pkt->len) status = 1u;
    else status = webServices().cancel_credentials(ssid);
  } else {
    status = 1u;
  }

  uint8_t response[2u + 2u * (SSID_MAX_LEN + 1u)];
  const uint8_t response_len =
      build_wifi_credentials_flow_data(action, extra_flags, response, sizeof(response));
  UNER_Handle_SendResponse(&app->handle, pkt, status,
                           response_len > 0u ? response : nullptr, response_len);
}

static void h_ap_clients_page(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  uint8_t data[UNER_MAX_PAYLOAD - 1u];
  uint8_t data_len = 0u;
  const uint8_t status = webServices().ap_clients_page(
      pkt->payload[0], pkt->payload[1], data, sizeof(data), &data_len);
  UNER_Handle_SendResponse(&app->handle, pkt, status,
                           data_len > 0u ? data : nullptr, data_len);
}

static void h_ap_client_disconnect(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  const uint8_t status = webServices().disconnect_ap_client(pkt->payload);
  const uint8_t scheduled = status == 0u ? 1u : 0u;
  UNER_Handle_SendResponse(&app->handle, pkt, status, &scheduled, 1u);
}

static void h_get_connected_users(void *ctx, const UNER_Packet *pkt){
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_CONNECTED_USERS", pkt);
  uint8_t p[1] = { webServices().connected_count() };
  UNER_Handle_SendResponse(&app->handle, pkt, 0, p, 1);
}

static void h_get_user_info(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_USER_INFO", pkt);
  uint8_t index = pkt->payload[0];
  uint32_t id = 0;
  uint8_t ip[4] = {0,0,0,0};
  bool ok = webServices().client_info(index, &id, ip);
  uint8_t p[9] = {index, (uint8_t)(id & 0xFFu), (uint8_t)((id>>8)&0xFFu), (uint8_t)((id>>16)&0xFFu), (uint8_t)((id>>24)&0xFFu), ip[0], ip[1], ip[2], ip[3]};
  UNER_Handle_SendResponse(&app->handle, pkt, ok ? 0 : 1, p, 9);
}

static void h_start_scan(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("START_SCAN", pkt);

  WifiScanAcquireResult ownership =
      UNER_App_TryAcquireWifiScan(WIFI_SCAN_OWNER_STM);
  if (ownership != WIFI_SCAN_ACQUIRED)
  {
    /* status=2: el unico driver de scan esta ocupado (incluso por una
     * retransmision del mismo origen). No tocar WiFi.scanDelete(). */
    UNER_Handle_SendResponse(&app->handle, pkt, 2u, nullptr, 0u);
    return;
  }

  webServices().wifi.SetSTAReconnectEnabled(false);
  webServices().wifi.DeleteScanResults();
  int scan_rc = webServices().wifi.StartScan(true);
  bool ok = (scan_rc == WIFI_DRIVER_SCAN_RUNNING) || (scan_rc >= 0);
  if (!ok) (void)UNER_App_ReleaseWifiScan(WIFI_SCAN_OWNER_STM);

  s_uart.scan_active = ok;
  s_uart.scan_stopped_by_command = false;
  s_uart.scan_timed_out = false;
  s_uart.scan_started_ms = webServices().platform->now_ms();
  s_uart.scan_last_log_ms = s_uart.scan_started_ms;
  s_uart.scan_notify_dst = pkt->src;
  s_uart.scan_notify_enabled = ok ? 1u : 0u;
  s_uart.scan_finalizer_status = 0;

  UNER_SCAN_LOG("[UNER][SCAN] START rc=%d active=%u notify_enabled=%u notify_dst=%u\n",
                scan_rc, s_uart.scan_active ? 1u : 0u, s_uart.scan_notify_enabled, s_uart.scan_notify_dst);
  if (ok)
  {
    UNER_SCAN_LOGLN("[UNER][SCAN] async scan started, 10s window");
  }
  else
  {
    UNER_SCAN_LOGLN("[UNER][SCAN] ERROR could not start async scan");
  }

  uint8_t status = ok ? 0 : 1;
  UNER_Handle_SendResponse(&app->handle, pkt, status, nullptr, 0);
}

static void h_stop_scan(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("STOP_SCAN", pkt);

  WifiScanOwner owner = UNER_App_GetWifiScanOwner();
  if (owner == WIFI_SCAN_OWNER_WEB)
  {
    const uint8_t info = 0u;
    UNER_Handle_SendResponse(&app->handle, pkt, 2u, &info, 1u);
    return;
  }
  if (owner == WIFI_SCAN_OWNER_NONE)
  {
    const uint8_t info = 0u;
    UNER_Handle_SendResponse(&app->handle, pkt, 0u, &info, 1u);
    return;
  }

  s_uart.scan_active = false;
  s_uart.scan_stopped_by_command = true;
  s_uart.scan_finalizer_status = 2;
  webServices().wifi.DeleteScanResults();
  (void)UNER_App_ReleaseWifiScan(WIFI_SCAN_OWNER_STM);
  UNER_SCAN_LOG("[UNER][SCAN] STOP active=%u notify_enabled=%u notify_dst=%u\n",
                s_uart.scan_active ? 1u : 0u, s_uart.scan_notify_enabled, s_uart.scan_notify_dst);
  send_scan_results_async(app, 2);

  const uint8_t info = 1; // stopped-by-command
  UNER_Handle_SendResponse(&app->handle, pkt, 0, &info, 1);
}

static void h_get_scan_results(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_SCAN_RESULTS", pkt);

  uint8_t payload[UNER_MAX_PAYLOAD];
  uint8_t plen = 0;
  uint8_t status = 0;

  WifiScanOwner owner = UNER_App_GetWifiScanOwner();
  const uint8_t cached_count = WifiScanSnapshot_Count(&s_uart.scan_cache);
  if (owner == WIFI_SCAN_OWNER_STM && s_uart.scan_active)
  {
    status = 1; // still running
    payload[0] = 0;
    plen = 1;
    UNER_SCAN_LOG("[UNER][SCAN] GET_SCAN_RESULTS busy owner=%u active=%u notify_enabled=%u dst=%u\n",
                  (unsigned)owner, s_uart.scan_active ? 1u : 0u,
                  s_uart.scan_notify_enabled, s_uart.scan_notify_dst);
  }
  else if (cached_count == 0u)
  {
    payload[0] = 0;
    plen = 1;
    if (s_uart.scan_timed_out)
    {
      status = 3;
      UNER_SCAN_LOGLN("[UNER][SCAN] GET_SCAN_RESULTS timed out");
      s_uart.scan_timed_out = false;
    }
    else if (s_uart.scan_stopped_by_command)
    {
      status = 2;
      UNER_SCAN_LOGLN("[UNER][SCAN] GET_SCAN_RESULTS stopped");
      s_uart.scan_stopped_by_command = false;
    }
    else
    {
      UNER_SCAN_LOGLN("[UNER][SCAN] GET_SCAN_RESULTS no networks");
    }
  }
  else
  {
    plen = build_scan_results_payload(payload, sizeof(payload));
    UNER_SCAN_LOG("[UNER][SCAN] GET_SCAN_RESULTS cached_networks=%u payload_len=%u\n",
                  cached_count, plen);
  }

  UNER_Handle_SendResponse(&app->handle, pkt, status, payload, plen);
}

static void h_get_wifi_detail(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("WIFI_GET_DETAIL", pkt);

  if (!pkt || pkt->len < 1)
  {
    UNER_Handle_SendNack(&app->handle, app->this_node, pkt ? pkt->src : app->peer_node, UNER_CMD_WIFI_GET_DETAIL, (uint8_t)UNER_ERR_ARGS);
    return;
  }

  uint8_t ssid_len = pkt->payload[0];
  if (ssid_len == 0 || ssid_len > SSID_MAX_LEN || pkt->len != (uint8_t)(1u + ssid_len))
  {
    UNER_Handle_SendNack(&app->handle, app->this_node, pkt->src, UNER_CMD_WIFI_GET_DETAIL, (uint8_t)UNER_ERR_ARGS);
    return;
  }

  char ssid[SSID_MAX_LEN + 1];
  for (uint8_t i = 0; i < ssid_len; ++i) ssid[i] = (char)pkt->payload[1u + i];
  ssid[ssid_len] = '\0';

  UNER_WifiScanCacheEntry entry;
  if (!UNER_App_FindScanCacheBySsid(ssid, &entry))
  {
    UNER_Handle_SendNack(&app->handle, app->this_node, pkt->src, UNER_CMD_WIFI_GET_DETAIL, (uint8_t)UNER_ERR_ARGS);
    return;
  }

  uint8_t resp[SSID_MAX_LEN + 4];
  uint8_t len = append_wifi_detail_payload(&entry, resp, sizeof(resp));
  UNER_Handle_SendRaw(&app->handle, app->this_node, pkt->src, UNER_CMD_WIFI_GET_DETAIL, resp, len);
}

static void h_set_encoder_fast(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_ENCODER_FAST", pkt);
  uint8_t accepted = pkt->payload[0];
  UNER_Handle_SendResponse(&app->handle, pkt, 0, &accepted, 1);
}

static void h_get_interfaces_connected(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_INTERFACES_CONNECTED", pkt);
  uint8_t interfaces[4];
  fill_interfaces_connected(interfaces);
  UNER_Handle_SendResponse(&app->handle, pkt, 0, interfaces, 4);
}

static void h_get_credentials(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  // 0x47 esta declarado en codigo legacy pero no es operativo en el F4 del
  // corte 2026-07-12. Aunque se enlazara por error, nunca expone secretos.
  UNER_Handle_SendNack(&app->handle, app->this_node, pkt->src,
                       UNER_CMD_GET_CREDENTIALS, (uint8_t)UNER_ERR_UNKNOWN_CMD);
}

static void h_connect_wifi(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("CONNECT_WIFI", pkt);
  const AppConfig *c = webServices().config();
  const char *staSsid = c ? c->staSsid : DEFAULT_STA_SSID;
  const char *staPass = c ? c->staPass : DEFAULT_STA_PASSWORD;
  webServices().wifi.SetSTAReconnectEnabled(false);
  const bool configured = staSsid[0] != '\0' && (!c || webServices().wifi.ApplySTAConfig(*c));
  const WifiLinkStatus start_status = configured
                                       ? webServices().wifi.ConnectSTA(staSsid, staPass, false, false)
                                       : WIFI_LINK_CONNECT_FAILED;
  if (!configured) webServices().wifi.NotifySTAConfigurationFailure();
  if (start_status == WIFI_LINK_CONNECT_FAILED)
      (void)webServices().wifi.ConsumeSTAAttemptFailure(); // this response reports the immediate error
//   Serial.println("[UNER][CONNECT_WIFI] intento iniciado, se notificara finalizador al conectar");
  uint8_t data[64]; uint8_t len = append_wifi_info(data, sizeof(data), true);
  UNER_Handle_SendResponse(&app->handle, pkt,
                           start_status != WIFI_LINK_CONNECT_FAILED ? 0u : 1u,
                           data, len);
}

static void h_disconnect_wifi(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("DISCONNECT_WIFI", pkt);
  webServices().wifi.SetSTAReconnectEnabled(false);
  (void)webServices().wifi.DisconnectSTA(true, false);
  uint8_t data[64]; uint8_t len = append_wifi_info(data, sizeof(data), true);
  UNER_Handle_SendResponse(&app->handle, pkt, 0, data, len);
}

static void h_set_ap_config(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_AP_CONFIG", pkt);

  /* Reject any source other than MCU. */
  if (!pkt || !pkt->payload || pkt->src != UNER_NODE_MCU) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u);
    return;
  }

  /* --- Decode binary payload: ssidLen(1) ssid passLen(1) pass protected(1) ip[4] --- */
  const uint8_t *p = pkt->payload;
  const uint8_t total = pkt->len;
  uint8_t pos = 0u;

  /* SSID */
  if (pos >= total) { UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u); return; }
  const uint8_t ssid_len = p[pos++];
  if (ssid_len == 0u || ssid_len > SSID_MAX_LEN || pos + ssid_len > total) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u);
    return;
  }
  char ssid[SSID_MAX_LEN + 1u];
  memcpy(ssid, &p[pos], ssid_len);
  ssid[ssid_len] = '\0';
  pos += ssid_len;

  /* Password */
  if (pos >= total) { UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u); return; }
  const uint8_t pass_len = p[pos++];
  if (pass_len > PASS_MAX_LEN || pos + pass_len > total) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u);
    return;
  }
  char password[PASS_MAX_LEN + 1u];
  if (pass_len > 0u) memcpy(password, &p[pos], pass_len);
  password[pass_len] = '\0';
  pos += pass_len;

  /* Protected flag + IPv4 (5 bytes) */
  if (pos + 5u != total) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_PAYLOAD_INVALID, nullptr, 0u);
    return;
  }
  const uint8_t ap_protected = p[pos++];
  const uint8_t ip_bytes[4] = { p[pos], p[pos + 1u], p[pos + 2u], p[pos + 3u] };
  pos += 4u;

  /* --- Validate --- */
  /* SSID: printable ASCII 0x20..0x7E */
  for (uint8_t i = 0u; i < ssid_len; ++i) {
    const uint8_t ch = (uint8_t)ssid[i];
    if (ch < 0x20u || ch > 0x7Eu) {
      UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_CONFIG_INVALID, nullptr, 0u);
      return;
    }
  }
  /* Password: empty or 8..63 */
  if (pass_len > 0u && pass_len < 8u) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_CONFIG_INVALID, nullptr, 0u);
    return;
  }
  /* Protected: boolean 0 or 1 */
  if (ap_protected > 1u) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_CONFIG_INVALID, nullptr, 0u);
    return;
  }
  /* Protected=true requires a non-empty password */
  if (ap_protected != 0u && pass_len == 0u) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_CONFIG_INVALID, nullptr, 0u);
    return;
  }

  /* Convert IP bytes to text. 0.0.0.0 means use default 192.168.4.1. */
  char ip_text[16u];
  snprintf(ip_text, sizeof(ip_text), "%u.%u.%u.%u",
           ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);

  /* --- Build candidate config --- */
  const AppConfig previous = current_config_copy();
  AppConfig candidate = previous;
  strncpy(candidate.apSsid, ssid, sizeof(candidate.apSsid));
  candidate.apSsid[sizeof(candidate.apSsid) - 1u] = '\0';
  strncpy(candidate.apPass, password, sizeof(candidate.apPass));
  candidate.apPass[sizeof(candidate.apPass) - 1u] = '\0';
  candidate.apProtected = ap_protected;
  strncpy(candidate.apIp, ip_text, sizeof(candidate.apIp));
  candidate.apIp[sizeof(candidate.apIp) - 1u] = '\0';

  /* --- Persist to LittleFS --- */
  if (!webServices().save_config(candidate)) {
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_STORAGE_FAILED, nullptr, 0u);
    return;
  }

  /* --- Apply if AP is active --- */
  const WifiRadioMode prev_mode = webServices().platform->radio_mode();
  const bool was_active = (prev_mode == WIFI_RADIO_AP || prev_mode == WIFI_RADIO_AP_STA);
  if (!was_active) {
    /* AP inactive: config will be applied on next start. */
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_OK, nullptr, 0u);
    return;
  }

  /* Preserve STA if we were in AP_STA. */
  const bool keep_sta = (prev_mode == WIFI_RADIO_AP_STA);
  bool ok = webServices().wifi.ApplyAPConfig(candidate);
  if (ok) {
    (void)webServices().wifi.StopAP(false);
    ok = webServices().wifi.StartAP(candidate.apSsid, candidate.apPass,
                           keep_sta, 1, false, 4,
                           candidate.apProtected != 0u);
  }

  if (!ok) {
    /* Rollback: restore previous config, IP, and restart AP. */
    (void)webServices().save_config(previous);
    (void)webServices().wifi.ApplyAPConfig(previous);
    (void)webServices().wifi.StartAP(previous.apSsid, previous.apPass,
                            keep_sta, 1, false, 4,
                            previous.apProtected != 0u);
    UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_STORAGE_FAILED, nullptr, 0u);
    return;
  }

  UNER_Handle_SendResponse(&app->handle, pkt, AP_OP_OK, nullptr, 0u);
}

static void h_start_ap(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("START_AP", pkt);
  const AppConfig *c = webServices().config();
  const char *apSsid = c ? c->apSsid : DEFAULT_AP_SSID;
  const char *apPass = c ? c->apPass : DEFAULT_AP_PASSWORD;
  const bool configured = !c || webServices().wifi.ApplyAPConfig(*c);
  const bool ap_prot = c ? (c->apProtected != 0u) : true;
  const bool started = configured && webServices().wifi.StartAP(apSsid, apPass, false, 1, false, 4, ap_prot);
  const WifiRadioMode mode = webServices().wifi.GetMode();
  bool ok = started && (mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA);
//   Serial.printf("[UNER][START_AP] request procesado ok=%u, se notificara finalizador cuando se detecte modo AP\n", ok ? 1 : 0);
  uint8_t data[64]; uint8_t len = append_wifi_info(data, sizeof(data), false);
  UNER_Handle_SendResponse(&app->handle, pkt, ok ? 0 : 1, data, len);
}

static void h_stop_ap(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("STOP_AP", pkt);
  (void)webServices().wifi.StopAP(true);
  uint8_t data[64]; uint8_t len = append_wifi_info(data, sizeof(data), false);
  UNER_Handle_SendResponse(&app->handle, pkt, 0, data, len);
}

static void h_set_ap_fallback(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  const AppConfig *current = webServices().config();
  uint8_t enabled = current ? (current->apFallbackEnabled ? 1u : 0u) : 1u;
  uint8_t status = 1u;
  if (pkt->payload && pkt->len == 1u && pkt->payload[0] <= 1u && current)
  {
    AppConfig candidate = *current;
    candidate.apFallbackEnabled = pkt->payload[0];
    const bool saved = candidate.apFallbackEnabled == current->apFallbackEnabled ||
                       webServices().save_config(candidate);
    if (saved)
    {
      enabled = candidate.apFallbackEnabled;
      status = 0u;
      (void)UNER_Handle_SendAck(&app->handle, app->this_node, pkt->src, pkt->cmd, 0u);
    }
    else status = 3u;
  }
  (void)UNER_Handle_SendResponse(&app->handle, pkt, status, &enabled, 1u);
}

static void h_set_auto_reconnect(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("SET_AUTO_RECONNECT", pkt);

  uint8_t enable = pkt->payload[0] ? 1u : 0u;
  webServices().wifi.SetSTAReconnectEnabled(enable == 1u);

  uint8_t resp[1] = { (uint8_t)(webServices().wifi.IsSTAReconnectEnabled() ? 1u : 0u) };
  UNER_Handle_SendResponse(&app->handle, pkt, 0, resp, 1);
}

static void h_get_connected_users_mode(void *ctx, const UNER_Packet *pkt)
{
  UNER_App *app = static_cast<UNER_App *>(ctx);
  log_cmd("GET_CONNECTED_USERS_MODE", pkt);

  uint8_t p[3];
  uint8_t ap_mode = (webServices().platform->radio_mode() == WIFI_RADIO_AP || webServices().platform->radio_mode() == WIFI_RADIO_AP_STA) ? 1u : 0u;
  uint8_t ap_users = webServices().platform->ap_clients();
  uint8_t ws_users = webServices().connected_count();

  p[0] = ap_mode;
  p[1] = ap_users;
  p[2] = ws_users;
//   Serial.printf("[UNER][GET_CONNECTED_USERS_MODE] ap_mode=%u ap_users=%u ws_users=%u\n", ap_mode, ap_users, ws_users);
  UNER_Handle_SendResponse(&app->handle, pkt, 0, p, 3);
}

void UNER_App_OnCommandTable(UNER_App *app)
{
  for (uint8_t i = 0; i < app->handle.command_count; ++i)
  {
    UNER_CommandSpec *spec = (UNER_CommandSpec *)&app->handle.command_table[i];
    switch (spec->id)
    {
      case UNER_CMD_PING: spec->handler = h_ping; break;
      case UNER_CMD_GET_STATUS: spec->handler = h_get_status; break;
      case UNER_CMD_SET_MODE_AP: spec->handler = h_set_mode_ap; break;
      case UNER_CMD_SET_MODE_STA: spec->handler = h_set_mode_sta; break;
      case UNER_CMD_SET_CREDENTIALS: spec->handler = h_set_credentials; break;
      case UNER_CMD_CLEAR_CREDENTIALS: spec->handler = h_clear_credentials; break;
      case UNER_CMD_REBOOT_ESP: spec->handler = h_reboot_esp; break;
      case UNER_CMD_FACTORY_RESET: spec->handler = h_factory_reset; break;
      case UNER_CMD_GET_PREFERENCES: spec->handler = h_get_preferences; break;
      case UNER_CMD_REQUEST_FIRMWARE: spec->handler = h_request_firmware; break;
      case UNER_CMD_SET_ENCODER_FAST: spec->handler = h_set_encoder_fast; break;
      case UNER_CMD_GET_CONNECTED_USERS: spec->handler = h_get_connected_users; break;
      case UNER_CMD_GET_USER_INFO: spec->handler = h_get_user_info; break;
      case UNER_CMD_GET_INTERFACES_CONNECTED: spec->handler = h_get_interfaces_connected; break;
      case UNER_CMD_GET_CREDENTIALS: spec->handler = h_get_credentials; break;
      case UNER_CMD_START_SCAN: spec->handler = h_start_scan; break;
      case UNER_CMD_STOP_SCAN: spec->handler = h_stop_scan; break;
      case UNER_CMD_WIFI_GET_DETAIL: spec->handler = h_get_wifi_detail; break;
      case UNER_CMD_GET_SCAN_RESULTS: spec->handler = h_get_scan_results; break;
      case UNER_CMD_CONNECT_WIFI: spec->handler = h_connect_wifi; break;
      case UNER_CMD_DISCONNECT_WIFI: spec->handler = h_disconnect_wifi; break;
      case UNER_CMD_SET_AP_CONFIG: spec->handler = h_set_ap_config; break;
      case UNER_CMD_START_AP: spec->handler = h_start_ap; break;
      case UNER_CMD_STOP_AP: spec->handler = h_stop_ap; break;
      case UNER_CMD_GET_CONNECTED_USERS_MODE: spec->handler = h_get_connected_users_mode; break;
      case UNER_CMD_SET_AUTO_RECONNECT: spec->handler = h_set_auto_reconnect; break;
      case UNER_CMD_SET_AP_FALLBACK: spec->handler = h_set_ap_fallback; break;
      case UNER_CMD_WIFI_CREDENTIALS_FLOW: spec->handler = h_wifi_credentials_flow; break;
      case UNER_CMD_AP_CLIENTS_PAGE: spec->handler = h_ap_clients_page; break;
      case UNER_CMD_AP_CLIENT_DISCONNECT: spec->handler = h_ap_client_disconnect; break;
      default: break;
    }
  }
}

void UNER_App_Init(UNER_App *app, uint8_t this_node, uint8_t peer_node)
{
  app->this_node = this_node;
  app->peer_node = peer_node;
  WifiScanArbiter_Init(&s_uart.scan_arbiter);

  uint8_t count = 0;
  UNER_CommandSpec *table = UNER_MutableCommandTable(&count);

  UNER_CoreConfig c = {};
  c.this_node = this_node;
  // UART1 recibe frames originados por la F4 y también frames PC_QT que la F4
  // reenvía desde USB CDC sin alterar la ruta. Los nodos Web siguen siendo
  // destinos de respuestas, nunca fuentes aceptadas desde el cable UART.
  c.accept_src_mask = (uint16_t)((1u << UNER_NODE_MCU) |
                                 (1u << UNER_NODE_PC_QT));
  c.accept_dst_mask = (uint16_t)((1u << this_node) | (1u << UNER_NODE_WEB_APP));
  for (uint8_t node = UNER_NODE_WEB_CLIENT_MIN; node <= UNER_NODE_WEB_CLIENT_MAX; ++node)
  {
    c.accept_dst_mask = (uint16_t)(c.accept_dst_mask | (uint16_t)(1u << node));
  }
  c.accept_broadcast = 1;
  c.default_max_payload = UNER_MAX_PAYLOAD;

  UNER_Handle_Init(&app->handle, &c, table, count, app);
  UNER_App_OnCommandTable(app);

  webServices().initialize_serial_transport(&app->serial_transport);
  UNER_Handle_AddTransport(&app->handle, &app->serial_transport);

  app->last_wifi_mode = (uint8_t)webServices().platform->radio_mode();
  app->last_sta_status = (uint8_t)webServices().platform->radio_status();
  app->last_ap_clients = webServices().platform->ap_clients();
  app->boot_complete_sent = 0u;
  app->reboot_pending = 0u;
  app->reboot_boot_mode = APP_BOOT_MODE_NORMAL;
  app->reboot_not_before_ms = 0u;

  uint8_t boot_payload[64];
  const uint8_t wifi_info_len = append_wifi_info(boot_payload, sizeof(boot_payload), true);
  uint8_t boot_len = (uint8_t)(wifi_info_len + append_reset_diagnostic(
      &boot_payload[wifi_info_len], (uint8_t)(sizeof(boot_payload) - wifi_info_len)));
  send_evt(app, UNER_EVT_BOOT, boot_payload, boot_len);
  send_evt(app, UNER_EVT_USB_CONNECTED, nullptr, 0);
  flush_uner_tx(app, 200u);

  // F4 conserva sus flags durante el reset de la ESP. Publicar siempre el
  // modo y el estado de enlace evita que quede AP_ACTIVE de un boot anterior.
  send_evt(app, UNER_EVT_MODE_CHANGED, boot_payload, wifi_info_len);

  if (app->last_sta_status == WIFI_LINK_CONNECTED) {
    send_evt(app, UNER_EVT_STA_CONNECTED, boot_payload, wifi_info_len);
    uint8_t wifi_payload[64];
    uint8_t wlen = append_wifi_info(wifi_payload, sizeof(wifi_payload), true);
    send_evt(app, UNER_EVT_WIFI_CONNECTED, wifi_payload, wlen);
  }
  else
  {
    send_evt(app, UNER_EVT_STA_DISCONNECTED, boot_payload, wifi_info_len);
  }
  flush_uner_tx(app, 200u);

  WifiIpv4 initial_ip;
  uint8_t initial_net_if = current_network_interface(initial_ip);
  emit_network_ip_evt_and_cmd(app, initial_net_if, initial_ip);
  flush_uner_tx(app, 200u);
}

void UNER_App_Loop(UNER_App *app)
{
  if (!app) return;
  if (app->reboot_pending)
  {
    service_reboot_state_machine(app);
    return;
  }

  UNER_Handle_Tick(&app->handle);
  UNER_Handle_PollRx(&app->handle);
  UNER_Handle_Dispatch(&app->handle);
  if (!app->reboot_pending)
  {
    if (webServices().wifi.ConsumeSTAAttemptFailure())
    {
      const uint8_t failed = 1u;
      (void)UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node,
                               UNER_CMD_CONNECT_WIFI, &failed, 1u);
    }
    emit_mode_and_link_events(app);
    service_scan_state_machine(app);
  }
  for (uint8_t i = 0; i < UNER_TX_SLOTS; ++i)
  {
    uint8_t before = app->handle.tx_q_count;
    if (before == 0) break;
    UNER_Handle_TxPump(&app->handle);
    if (app->handle.tx_q_count == before) break;
  }
  service_reboot_state_machine(app);
}

void UNER_App_NotifyWebserverUp(UNER_App *app)
{
  if (!app) return;
  send_evt(app, UNER_EVT_WEBSERVER_UP, nullptr, 0);
  WifiIpv4 ip;
  uint8_t net_if = current_network_interface(ip);
  send_boot_complete_evt_and_cmd(app, net_if, ip);
  flush_uner_tx(app, 200u);
}

void UNER_App_NotifyWsClientConnected(UNER_App *app, uint32_t client_id)
{
  uint8_t p[4] = {(uint8_t)(client_id & 0xFFu), (uint8_t)((client_id >> 8) & 0xFFu), (uint8_t)((client_id >> 16) & 0xFFu), (uint8_t)((client_id >> 24) & 0xFFu)};
  send_evt(app, UNER_EVT_APP_USER_CONNECTED, p, 4);
}

void UNER_App_NotifyWsClientDisconnected(UNER_App *app, uint32_t client_id)
{
  uint8_t p[4] = {(uint8_t)(client_id & 0xFFu), (uint8_t)((client_id >> 8) & 0xFFu), (uint8_t)((client_id >> 16) & 0xFFu), (uint8_t)((client_id >> 24) & 0xFFu)};
  send_evt(app, UNER_EVT_APP_USER_DISCONNECTED, p, 4);
}

static void notify_web_page(UNER_App *app, uint8_t event, uint32_t id, uint8_t number)
{
  if (!app) return;
  uint8_t p[5] = {(uint8_t)id, (uint8_t)(id >> 8), (uint8_t)(id >> 16), (uint8_t)(id >> 24), number};
  send_evt(app, event, p, sizeof(p));
}

void UNER_App_NotifyWebPageReady(UNER_App *app, uint32_t client_id, uint8_t client_number)
{
  notify_web_page(app, UNER_EVT_WEBSERVER_CLIENT_CONNECTED, client_id, client_number);
}

void UNER_App_NotifyWebPageClosed(UNER_App *app, uint32_t client_id, uint8_t client_number)
{
  notify_web_page(app, UNER_EVT_WEBSERVER_CLIENT_DISCONNECTED, client_id, client_number);
}

void UNER_App_NotifyApClientJoined(UNER_App *app, uint8_t ap_clients, uint8_t client_number)
{
  if (!app) return;
  uint8_t p[2] = {ap_clients, client_number};
  send_evt(app, UNER_EVT_AP_CLIENT_JOIN, p, sizeof(p));
  app->last_ap_clients = ap_clients;
}

void UNER_App_NotifyApClientLeft(UNER_App *app, uint8_t ap_clients, uint8_t client_number)
{
  if (!app) return;
  uint8_t p[2] = {ap_clients, client_number};
  send_evt(app, UNER_EVT_AP_CLIENT_LEAVE, p, sizeof(p));
  app->last_ap_clients = ap_clients;
}

void UNER_App_NotifyApFallbackReady(UNER_App *app)
{
  if (!app) return;
  WifiRadioMode mode = webServices().platform->radio_mode();
  if (mode != WIFI_RADIO_AP && mode != WIFI_RADIO_AP_STA) return;

  // El resultado de credenciales puede ocupar ya un slot. Vaciamos primero
  // porque este snapshot usa exactamente los cuatro slots de la cola UNER y
  // no debe perder el ultimo NETWORK_IP cuando se informa el fallback.
  flush_uner_tx(app, 200u);

  // El modo puede seguir siendo AP+STA durante todo el intento, por lo que la
  // deteccion por cambio no vuelve a emitir nada al vencer el timeout. Este
  // snapshot explicito permite que F4 y Web vean el AP restaurado con su IP.
  uint8_t mode_payload[64];
  uint8_t mode_len = append_wifi_info(mode_payload, sizeof(mode_payload), false);
  send_evt(app, UNER_EVT_MODE_CHANGED, mode_payload, mode_len);

  WifiIpv4 ap_ip = webServices().platform->ap_ip();
  uint8_t ap_done[5] = {UNER_CONNECT_FINALIZER,
                        ap_ip[0], ap_ip[1], ap_ip[2], ap_ip[3]};
  UNER_Handle_SendRaw(&app->handle, app->this_node, app->peer_node,
                      UNER_CMD_START_AP, ap_done, sizeof(ap_done));
  emit_network_ip_evt_and_cmd(app, NET_IF_AP, ap_ip);
  flush_uner_tx(app, 200u);

  app->last_wifi_mode = (uint8_t)mode;
  app->last_sta_status = (uint8_t)webServices().platform->radio_status();
}
