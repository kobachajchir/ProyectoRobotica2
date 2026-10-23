#pragma once
#include <stdint.h>
#include <stddef.h>
#include <ESPAsyncWebServer.h>

struct WebServerServices {
    void *context = nullptr;
    bool (*storage_ready)() = nullptr;
    void (*bridge_initialize)(void *, AsyncWebSocket *) = nullptr;
    bool (*bridge_connect)(AsyncWebSocketClient *, const char *) = nullptr;
    bool (*bridge_disconnect)(uint32_t) = nullptr;
    void (*bridge_data)(AsyncWebSocketClient *, AwsFrameInfo *, uint8_t *, size_t) = nullptr;
    void (*bridge_loop)() = nullptr;
    void (*notify_connected)(void *, uint32_t) = nullptr;
    void (*notify_disconnected)(void *, uint32_t) = nullptr;
    void (*notify_page_ready)(void *, uint32_t, uint8_t) = nullptr;
    void (*notify_page_closed)(void *, uint32_t, uint8_t) = nullptr;
    void (*notify_ap_join)(void *, uint8_t, uint8_t) = nullptr;
    void (*notify_ap_leave)(void *, uint8_t, uint8_t) = nullptr;
    void (*notify_server_up)(void *) = nullptr;
};
void WebServerWS_SetServices(const WebServerServices &services);
const WebServerServices &WebServer_DefaultServices();
