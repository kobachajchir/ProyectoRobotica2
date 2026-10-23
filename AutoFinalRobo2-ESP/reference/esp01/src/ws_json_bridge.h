#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "bridge_ports.h"
struct UNER_App;

void WSJsonBridge_Init(AsyncWebSocket *socket, UNER_App *app);
bool WSJsonBridge_OnConnect(AsyncWebSocketClient *client, const char *resume_token = nullptr);
bool WSJsonBridge_OnDisconnect(uint32_t client_id);
void WSJsonBridge_OnData(AsyncWebSocketClient *client, AwsFrameInfo *info, uint8_t *data, size_t len);
void WSJsonBridge_Loop();
uint8_t WSJsonBridge_OnUNERPacket(void *ctx, const UNER_Packet *pkt);
void WSJsonBridge_RequestWifiCredentials(const char *ssid);

/* Adaptador binario UNER para reutilizar el mismo estado WiFi/AP del bridge
 * JSON. Los status son 0=OK y valores no cero de validacion/estado; nunca se
 * copian passwords a las respuestas. */
uint8_t WSJsonBridge_GetWifiCredentialsState(uint8_t *flags,
                                             char *sta_ssid,
                                             size_t sta_ssid_capacity,
                                             char *ap_ssid,
                                             size_t ap_ssid_capacity);
uint8_t WSJsonBridge_SubmitWifiCredentials(const char *ssid,
                                           const char *password,
                                           bool reuse_password,
                                           const char *ip,
                                           const char *gateway,
                                           const char *subnet,
                                           const char *bssid);
uint8_t WSJsonBridge_CancelWifiCredentials(const char *ssid);
uint8_t WSJsonBridge_BuildApClientsPage(uint8_t cursor,
                                        uint8_t maximum,
                                        uint8_t *out,
                                        uint8_t capacity,
                                        uint8_t *out_len);
uint8_t WSJsonBridge_ScheduleApClientDisconnect(const uint8_t mac[6]);

// Composition entrypoint; pointers and callbacks must outlive the bridge.
void WSJsonBridge_InitWithPorts(AsyncWebSocket *socket,
    const BridgeBackendOps &backend, const BridgeWifiPort &wifi,
    const BridgePlatformOps &platform);
void *WSJsonBridge_GetContext();
