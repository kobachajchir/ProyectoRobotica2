#include "UNERUtils.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ESP8266WiFi.h"
#include "uner_device.h"
#include "globals.h"

// =================== Estado interno ===================
static UNER_Device s_uner;
static AsyncWebSocket *s_ws = nullptr;
static AsyncWebSocketClient *s_wsLastClient = nullptr; // último cliente que envió datos
static uint8_t s_wifiMode = 0;                         // 0=AP, 1=STATION (puedes setearlo externamente)

// =================== Helpers HAL ===================

// TX: enviar un frame UNER por WS
static uint8_t hal_uner_send_bytes(const uint8_t *data, uint16_t len)
{
  if (s_ws == nullptr)
    return (uint8_t)UNER_STATUS_ERR_TX_FAIL;

  // Responder preferentemente al último cliente que habló
  if (s_wsLastClient && s_wsLastClient->status() == WS_CONNECTED)
  {
    s_wsLastClient->binary((const char *)data, (size_t)len);
    return (uint8_t)UNER_ERR_OK;
  }

  // Fallback: broadcast
  s_ws->binaryAll((const char *)data, (size_t)len);
  return (uint8_t)UNER_ERR_OK;
}

// Wi-Fi: modo actual
static uint8_t hal_wifi_get_mode(void)
{
  return s_wifiMode ? 1 : 0;
}

// Wi-Fi: pedir escaneo asíncrono
static void hal_wifi_request_scan(void)
{
  WiFi.scanDelete();
  WiFi.scanNetworks(true /* async */);
}

// Wi-Fi: aplicar AP
static uint8_t hal_wifi_set_ap(const uint8_t *ssid, uint8_t ssid_len,
                               const uint8_t *pass, uint8_t pass_len,
                               const uint8_t ip[4])
{
  if (ssid_len > 32)
    return (uint8_t)UNER_ERR_ARG;
  if (pass_len != 0 && pass_len < 8)
    return (uint8_t)UNER_ERR_ARG;

  String sSsid = String((const char *)ssid).substring(0, ssid_len);
  String sPass = String((const char *)pass).substring(0, pass_len);

  IPAddress apIP(ip[0], ip[1], ip[2], ip[3]);
  IPAddress gw(ip[0], ip[1], ip[2], 1);
  IPAddress mask(255, 255, 255, 0);
  if (apIP != IPAddress(0, 0, 0, 0))
  {
    (void)WiFi.softAPConfig(apIP, gw, mask); // si falla, seguimos igual
  }

  bool ok = (pass_len == 0)
                ? WiFi.softAP(sSsid.c_str())                 // OPEN
                : WiFi.softAP(sSsid.c_str(), sPass.c_str()); // WPA/WPA2/WPA3

  if (!ok)
    return (uint8_t)UNER_ERR_APPLY_FAIL;

  s_wifiMode = 0; // AP
  return (uint8_t)UNER_ERR_OK;
}

// Wi-Fi: aplicar STA
static uint8_t hal_wifi_set_sta(const uint8_t *ssid, uint8_t ssid_len,
                                const uint8_t *pass, uint8_t pass_len,
                                uint8_t fixed_ip, const uint8_t ip[4])
{
  if (ssid_len == 0 || ssid_len > 32)
    return (uint8_t)UNER_ERR_ARG;
  if (pass_len < 8)
    return (uint8_t)UNER_ERR_ARG;

  String sSsid = String((const char *)ssid).substring(0, ssid_len);
  String sPass = String((const char *)pass).substring(0, pass_len);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);

  if (fixed_ip)
  {
    IPAddress local(ip[0], ip[1], ip[2], ip[3]);
    IPAddress gw(ip[0], ip[1], ip[2], 1);
    IPAddress mask(255, 255, 255, 0);
    if (!WiFi.config(local, gw, mask))
    {
      return (uint8_t)UNER_ERR_APPLY_FAIL;
    }
  }
  else
  {
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
  }

  WiFi.begin(sSsid.c_str(), sPass.c_str());
  s_wifiMode = 1; // STATION
  return (uint8_t)UNER_ERR_OK;
}

// (Opcional) Telemetría automática por callback (si querés usarla)
// Devuelve true si llenó out_payload/out_len y la lib enviará TELEMETRY_DATA.
static bool hal_telemetry_fill(uint8_t *out_payload, uint8_t *out_len)
{
  (void)out_payload;
  (void)out_len;
  return false; // demo: no enviar automático, usaremos telemetry_due
}

// =================== API pública ===================

void UNERUtils_Init(AsyncWebSocket &ws)
{
  s_ws = &ws;

  uner_dev_init(&s_uner,
                hal_uner_send_bytes,
                hal_wifi_get_mode,
                hal_wifi_request_scan,
                hal_wifi_set_ap,
                hal_wifi_set_sta,
                /*telemetry_fill*/ NULL /* o hal_telemetry_fill */);
}

void UNERUtils_OnWSBinary(AsyncWebSocketClient *client,
                          AwsFrameInfo *info, uint8_t *data, size_t len)
{
  (void)info; // si necesitás fragmentación avanzada, podés usar info->index/final

  // recordar el cliente para responderle
  s_wsLastClient = client;

  // inyectar bytes en el ring y procesar
  if (len > 0)
  {
    uner_dev_push_bytes(&s_uner, data, len);
  }
  uner_dev_process_rx(&s_uner);
}

void UNERUtils_Loop()
{
  // 1) Tick periódico UNER (heartbeat + telemetría periódica)
  static uint32_t last_ms = 0;
  uint32_t now = millis();
  uint16_t dt = (uint16_t)(now - last_ms);
  last_ms = now;
  uner_dev_tick(&s_uner, dt);

  // 2) Telemetría manual si no usamos telemetry_fill()
  if (s_uner.telemetry_due)
  {
    // Reemplazá por tu adquisición real (IMU/posición)
    int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0, t = 0;
    uner_dev_send_telemetry_mpu6050(&s_uner, s_uner.telemetry_seq,
                                    ax, ay, az, gx, gy, gz, t);
  }

  // 3) Si se completó el escaneo Wi-Fi asíncrono, empaquetar y enviar WIFI_SCAN_LIST
  int n = WiFi.scanComplete();
  if (n >= 0)
  {
    UNER_WifiNetworkDesc nets[8];
    uint8_t used = 0;
    static uint8_t ssid_bufs[8][32]; // no malloc

    for (int i = 0; i < n && used < 8; ++i)
    {
      String s = WiFi.SSID(i);
      uint8_t sl = (uint8_t)min((int)s.length(), 32);
      for (uint8_t k = 0; k < sl; k++)
        ssid_bufs[used][k] = (uint8_t)s[k];

      nets[used].ssid_len = sl;
      nets[used].ssid = ssid_bufs[used];
      nets[used].rssi_dbm = (int8_t)WiFi.RSSI(i);

      // mapear seguridad (ESP32/ESP8266 difieren en enums; ajustamos lo común)
#if defined(ESP32)
      wifi_auth_mode_t auth = WiFi.encryptionType(i);
#elif defined(ESP8266)
      uint8_t auth = WiFi.encryptionType(i);
#endif
      uint8_t sec = UNER_WIFI_SEC_OPEN;

#if defined(ESP32)
      if (auth == WIFI_AUTH_WEP)
        sec = UNER_WIFI_SEC_WEP;
      else if (auth == WIFI_AUTH_WPA_PSK)
        sec = UNER_WIFI_SEC_WPA_PSK;
      else if (auth == WIFI_AUTH_WPA2_PSK)
        sec = UNER_WIFI_SEC_WPA2_PSK;
      else if (auth == WIFI_AUTH_WPA_WPA2_PSK)
        sec = UNER_WIFI_SEC_WPA_WPA2_MIXED;
#ifdef WIFI_AUTH_WPA3_PSK
      else if (auth == WIFI_AUTH_WPA3_PSK)
        sec = UNER_WIFI_SEC_WPA3_PERSONAL;
#endif
#elif defined(ESP8266)
      if (auth == ENC_TYPE_WEP)
        sec = UNER_WIFI_SEC_WEP;
      else if (auth == ENC_TYPE_TKIP)
        sec = UNER_WIFI_SEC_WPA_PSK; // aprox
      else if (auth == ENC_TYPE_CCMP)
        sec = UNER_WIFI_SEC_WPA2_PSK; // aprox
      else if (auth == ENC_TYPE_AUTO)
        sec = UNER_WIFI_SEC_WPA_WPA2_MIXED; // aprox
#endif
      nets[used].security = sec;
      used++;
    }

    (void)uner_dev_send_wifi_scan_list(&s_uner, nets, used);
    WiFi.scanDelete();
  }
}

void UNERUtils_SetWifiMode(uint8_t modeAP0_STA1)
{
  s_wifiMode = (modeAP0_STA1 ? 1 : 0);
}

UNER_Device *UNERUtils_GetDevice()
{
  return &s_uner;
}
