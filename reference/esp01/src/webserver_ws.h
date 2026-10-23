#pragma once
#include <Arduino.h>

void WebServerWS_Init();                 // HTTP estático + WS /ws
void WebServerWS_SetUNERApp(void* app_ptr);
void WebServerWS_Loop();

uint8_t WebServerWS_MarkPageReady(uint32_t client_id);
uint8_t WebServerWS_GetConnectedCount();
bool WebServerWS_GetClientInfoByIndex(uint8_t index, uint32_t* id, uint8_t ip_out[4]);
