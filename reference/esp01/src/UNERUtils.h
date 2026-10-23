#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "uner_device.h"

// Inicializa la librería UNER y registra los HAL callbacks.
// Debes pasar la referencia al WebSocket para que el transport pueda responder.
void UNERUtils_Init(AsyncWebSocket& ws);

// Alimenta el parser UNER con datos binarios provenientes del WebSocket.
// Llamar desde tu onWSEvent cuando (info->opcode == WS_BINARY).
void UNERUtils_OnWSBinary(AsyncWebSocketClient* client,
                          AwsFrameInfo* info, uint8_t* data, size_t len);

// Lazo de servicio de UNER: heartbeat/telémetría y envío de WIFI_SCAN_LIST.
// Llamar una vez por iteración desde tu loop() principal.
void UNERUtils_Loop();

// (Opcional) Fijar modo Wi-Fi externo (si tu proyecto lo mantiene).
void UNERUtils_SetWifiMode(uint8_t modeAP0_STA1);

// (Opcional) Exponer acceso al estado interno si te hace falta para debug.
UNER_Device* UNERUtils_GetDevice();

