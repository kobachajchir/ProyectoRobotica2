/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
// AFR2-20261023-11. STA/AP service; no AT and no SDK credential persistence.
#pragma once
#include "storage.h"
struct WifiConfig {
    char mode[7]={}, ssid[33]={}, password[65]={};
    bool persist=true;
};
/* Snapshot for the wired OLED channel. Never serialize this struct to HTTP/WS. */
struct WifiDisplayInfo {
    uint8_t mode,staConnected,apReady,clients;
    int16_t rssi;
    uint8_t staIp[4],apIp[4];
    char staSsid[33],apSsid[33],apPassword[64];
};
class WifiService {
public:
    explicit WifiService(Storage &s):storage_(s) {}
    void begin();
    void tick(uint32_t now);
    bool stage(const WifiConfig &config, uint32_t now);
    bool busy() const { return applyPending_ || scanPending_ || scanReady_; }
    bool scan();
    bool retry();
    bool disconnectStation();
    bool changeMode(uint8_t mode);
    bool takeScan(JsonObject out);
    void status(JsonObject out) const;
    void displayInfo(WifiDisplayInfo &out) const;
    static bool valid(const WifiConfig &config);
private:
    Storage &storage_;
    WifiConfig pending_;
    String apSsid_;
    char apPassword_[64]={}; // AP only; STA credentials are never assigned here.
    uint32_t stagedAt_=0, staStarted_=0, lastRetry_=0, scanStarted_=0;
    bool applyPending_=false, scanPending_=false, scanReady_=false, scanTimedOut_=false, staWanted_=false, apOn_=false;
    bool fallbackActive_=false,scanRestoreAp_=false;
    bool disconnectedByUser_=false;
    uint8_t scanBssid_[16][6]={},scanAuth_[16]={},authCount_=0;
    uint32_t authScanAt_=0;
    int8_t persisted_=-1; // -1 unknown, 0 RAM, 1 saved
    const char *configError_=nullptr;
    void apply(const WifiConfig &config);
    void startFallback();
};
