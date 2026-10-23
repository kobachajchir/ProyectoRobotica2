#pragma once
#include <stdint.h>

typedef enum { WIFI_RUN_AP_STA = 0, WIFI_RUN_AP_ONLY, WIFI_RUN_STA_ONLY } WifiRunMode;

typedef enum {
    WIFI_SERVICE_IDLE = 0,
    WIFI_SERVICE_STA_CONNECTING,
    WIFI_SERVICE_AP_CREATING,
    WIFI_SERVICE_AP_RUNNING,
    WIFI_SERVICE_STA_CONNECTED
} WifiServiceState;


// Platform-neutral driver values. The ESP adapter maps its third-party enums.
enum WifiRadioMode { WIFI_RADIO_OFF = 0, WIFI_RADIO_STA = 1,
                     WIFI_RADIO_AP = 2, WIFI_RADIO_AP_STA = 3 };
enum WifiLinkStatus { WIFI_LINK_IDLE = 0, WIFI_LINK_NO_SSID = 1,
    WIFI_LINK_SCAN_COMPLETED = 2, WIFI_LINK_CONNECTED = 3,
    WIFI_LINK_CONNECT_FAILED = 4, WIFI_LINK_CONNECTION_LOST = 5,
    WIFI_LINK_DISCONNECTED = 6, WIFI_LINK_WRONG_PASSWORD = 7 };
static const int WIFI_DRIVER_SCAN_RUNNING = -1;
struct WifiIpv4 {
    uint8_t octets[4];
    WifiIpv4(uint8_t a=0, uint8_t b=0, uint8_t c=0, uint8_t d=0) : octets{a,b,c,d} {}
    uint8_t operator[](unsigned index) const { return octets[index]; }
    explicit operator uint32_t() const {
        // Preserve the ESP client's original little-endian reconnect key.
        return (uint32_t)octets[0] | ((uint32_t)octets[1] << 8) |
               ((uint32_t)octets[2] << 16) | ((uint32_t)octets[3] << 24);
    }
    bool operator!=(const WifiIpv4 &other) const { return !(*this == other); }
    bool operator==(const WifiIpv4 &other) const {
        return octets[0]==other.octets[0] && octets[1]==other.octets[1] &&
               octets[2]==other.octets[2] && octets[3]==other.octets[3];
    }
};
