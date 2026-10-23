#pragma once
#include "wifi_service_types.h"

struct WifiDriverOps {
    void (*initialize)(void *) = nullptr;
    bool (*set_mode)(void *, WifiRadioMode) = nullptr;
    WifiRadioMode (*get_mode)(void *) = nullptr;
    WifiLinkStatus (*status)(void *) = nullptr;
    WifiLinkStatus (*connect)(void *, const char *, const char *, const uint8_t *) = nullptr;
    bool (*disconnect)(void *, bool, bool) = nullptr;
    bool (*reconnect)(void *) = nullptr;
    bool (*start_ap)(void *, const char *, const char *, uint8_t, bool, uint8_t) = nullptr;
    bool (*stop_ap)(void *, bool) = nullptr;
    bool (*configure_sta)(void *, WifiIpv4, WifiIpv4, WifiIpv4) = nullptr;
    bool (*configure_ap)(void *, WifiIpv4, WifiIpv4, WifiIpv4) = nullptr;
    void (*auto_reconnect)(void *, bool) = nullptr;
    int (*scan_start)(void *, bool, bool) = nullptr;
    int (*scan_complete)(void *) = nullptr;
    void (*scan_delete)(void *) = nullptr;
    WifiIpv4 (*sta_ip)(void *) = nullptr;
    WifiIpv4 (*ap_ip)(void *) = nullptr;
    uint8_t (*ap_clients)(void *) = nullptr;
};
struct WifiDriverPort {
    const WifiDriverOps *ops = nullptr;
    void *context = nullptr;
    void initialize() const { ops->initialize(context); }
    bool mode(WifiRadioMode m) const { return ops->set_mode(context,m); }
    WifiRadioMode getMode() const { return ops->get_mode(context); }
    WifiLinkStatus status() const { return ops->status(context); }
    WifiLinkStatus begin(const char *s, const char *p, int = 0,
                         const uint8_t *bssid = nullptr, bool = true) const {
        return ops->connect(context,s,p,bssid);
    }
    bool disconnect(bool off, bool erase) const { return ops->disconnect(context,off,erase); }
    bool reconnect() const { return ops->reconnect(context); }
    bool softAP(const char *s,const char *p,uint8_t ch,bool hidden,uint8_t max) const {
        return ops->start_ap(context,s,p,ch,hidden,max);
    }
    bool softAPdisconnect(bool off) const { return ops->stop_ap(context,off); }
    bool config(WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) const {
        return ops->configure_sta(context,ip,gw,mask);
    }
    bool softAPConfig(WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) const {
        return ops->configure_ap(context,ip,gw,mask);
    }
    void setAutoReconnect(bool on) const { ops->auto_reconnect(context,on); }
    int scanNetworks(bool async,bool hidden) const { return ops->scan_start(context,async,hidden); }
    int scanComplete() const { return ops->scan_complete(context); }
    void scanDelete() const { ops->scan_delete(context); }
    WifiIpv4 localIP() const { return ops->sta_ip(context); }
    WifiIpv4 softAPIP() const { return ops->ap_ip(context); }
    uint8_t softAPgetStationNum() const { return ops->ap_clients(context); }
};
