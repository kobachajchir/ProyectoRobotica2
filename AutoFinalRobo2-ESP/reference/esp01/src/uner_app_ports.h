#pragma once
#include <stdint.h>
#include <stddef.h>
#include "bridge_ports.h"

// Web/JSON capabilities borrowed by the UART command handlers.
struct UNER_AppServices {
    BridgeWifiPort wifi;
    const BridgePlatformOps *platform = nullptr;
    const AppConfig *(*config)() = nullptr;
    bool (*save_config)(const AppConfig &) = nullptr;
    bool (*scan_entry)(uint8_t, WifiScanSnapshotEntry *) = nullptr;
    void (*reset_diagnostic)(uint8_t *reason, uint8_t *exception) = nullptr;
    void (*flush_serial)() = nullptr;
    void (*cooperate)() = nullptr;
    void (*delay_ms)(uint32_t) = nullptr;
    void (*restart)() = nullptr;
    void (*initialize_serial_transport)(UNER_Transport *) = nullptr;
    uint8_t (*connected_count)() = nullptr;
    bool (*client_info)(uint8_t, uint32_t *, uint8_t *) = nullptr;
    void (*request_credentials)(const char *) = nullptr;
    uint8_t (*credentials_state)(uint8_t *, char *, size_t, char *, size_t) = nullptr;
    uint8_t (*submit_credentials)(const char *, const char *, bool,
        const char *, const char *, const char *, const char *) = nullptr;
    uint8_t (*cancel_credentials)(const char *) = nullptr;
    uint8_t (*ap_clients_page)(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t *) = nullptr;
    uint8_t (*disconnect_ap_client)(const uint8_t *) = nullptr;
};
void UNER_App_SetServices(const UNER_AppServices &services);
const UNER_AppServices &UNER_App_DefaultServices();
