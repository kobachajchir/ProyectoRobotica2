#include "wifi_service.h"
#include <stdio.h>

static const int WIFI_SERVICE_SCAN_RUNNING = -1;
static const uint16_t WIFI_CONNECT_TIMEOUT_TICKS = 1000u;
static const uint16_t STA_RETRY_PERIOD_TICKS = 500u;
void WifiService_Init(WifiServiceHandle &h, const WifiDriverPort &driver,
    const WifiServiceOps &services, volatile t_bitMap *flags,
    volatile uint16_t *sta_wait, volatile uint16_t *sta_retry) {
    h = WifiServiceHandle{};
    h.driver = driver;
    h.services = services;
    h.flags = flags;
    h.sta_wait = sta_wait;
    h.sta_retry = sta_retry;
}

static bool parseIpv4Text(const char *text, WifiIpv4 &out)
{
    if (!text || !text[0])
        return false;
    int parts[4] = {-1, -1, -1, -1};
    char tail = '\0';
    if (sscanf(text, "%d.%d.%d.%d%c", &parts[0], &parts[1], &parts[2], &parts[3], &tail) != 4)
        return false;
    for (uint8_t i = 0; i < 4u; ++i)
    {
        if (parts[i] < 0 || parts[i] > 255)
            return false;
    }
    out = WifiIpv4((uint8_t)parts[0], (uint8_t)parts[1],
                    (uint8_t)parts[2], (uint8_t)parts[3]);
    return true;
}

static size_t boundedLength(const char *value, size_t max_length)
{
    if (!value)
        return 0;
    size_t length = 0;
    while (length < max_length && value[length] != '\0')
        ++length;
    return length;
}

static bool modeHasAP(WifiRadioMode mode)
{
    return mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA;
}

static void setApRunning(WifiServiceHandle &h, bool running)
{
    h.ap_started = running;
    CLEAR_FLAG((*h.flags), h.flags->bits.b4);
    if (running)
        SET_FLAG((*h.flags), h.flags->bits.b5);
    else
        CLEAR_FLAG((*h.flags), h.flags->bits.b5);
}

static void setStaIdleFlags(WifiServiceHandle &h)
{
    CLEAR_FLAG((*h.flags), h.flags->bits.b2);
    CLEAR_FLAG((*h.flags), h.flags->bits.b3);
    CLEAR_FLAG((*h.flags), h.flags->bits.b1);
    CLEAR_FLAG((*h.flags), h.flags->bits.b0);
    (*h.sta_wait) = 0;
    (*h.sta_retry) = 0;
}

static void setStaConnectedFlags(WifiServiceHandle &h)
{
    CLEAR_FLAG((*h.flags), h.flags->bits.b2);
    SET_FLAG((*h.flags), h.flags->bits.b3);
    CLEAR_FLAG((*h.flags), h.flags->bits.b1);
    CLEAR_FLAG((*h.flags), h.flags->bits.b0);
    (*h.sta_wait) = 0;
    (*h.sta_retry) = 0;
    h.ap_fallback_ready_pending = false;
    h.sta_attempt_failed_pending = false;
}

static void armStaAttempt(WifiServiceHandle &h)
{
    h.ap_fallback_ready_pending = false;
    h.sta_attempt_failed_pending = false;
    SET_FLAG((*h.flags), h.flags->bits.b2);
    CLEAR_FLAG((*h.flags), h.flags->bits.b3);
    CLEAR_FLAG((*h.flags), h.flags->bits.b1);
    CLEAR_FLAG((*h.flags), h.flags->bits.b0);
    (*h.sta_wait) = WIFI_CONNECT_TIMEOUT_TICKS;
    (*h.sta_retry) = STA_RETRY_PERIOD_TICKS;
    h.sta_start_tick = h.services.ticks(h.services.context);
    h.last_retry_tick = h.sta_start_tick;
    h.state = WIFI_SERVICE_STA_CONNECTING;
}

static void enterApFallbackState(WifiServiceHandle &h)
{
    const AppConfig *cfg = h.services.config(h.services.context);
    if (cfg && !cfg->apFallbackEnabled)
    {
        // Stop this STA attempt. A manually started AP is not an automatic fallback.
        h.driver.disconnect(false, false);
        if (!h.ap_started) h.driver.mode(WIFI_RADIO_STA);
        CLEAR_FLAG((*h.flags), h.flags->bits.b4);
        h.ap_fallback_ready_pending = false;
        h.state = h.ap_started ? WIFI_SERVICE_AP_RUNNING : WIFI_SERVICE_IDLE;
        return;
    }

    // Si el SoftAP ya sigue fisicamente activo, no volver a llamar softAP():
    // recrearlo cambia el canal/PCB y expulsa clientes justo al vencer STA.
    if (h.ap_started || modeHasAP(h.driver.getMode()))
    {
        setApRunning(h, true);
        h.state = WIFI_SERVICE_AP_RUNNING;
        h.ap_fallback_ready_pending = true;
        return;
    }

    SET_FLAG((*h.flags), h.flags->bits.b4);
    h.state = WIFI_SERVICE_AP_CREATING;
}

void WifiService_NotifySTAConfigurationFailure(WifiServiceHandle &h)
{
    const AppConfig *cfg = h.services.config(h.services.context);
    h.mode = (!cfg || cfg->apFallbackEnabled) ? WIFI_RUN_AP_STA : WIFI_RUN_STA_ONLY;
    setStaIdleFlags(h);
    h.sta_attempt_failed_pending = true;
    // Uses the same gate as timeout/lost-link, preserving an explicit AP.
    enterApFallbackState(h);
}

static WifiLinkStatus startStaAttempt(WifiServiceHandle &h, const char *ssid, const char *password,
                                   bool keep_ap, bool disconnect_first,
                                   const uint8_t *bssid = nullptr)
{
    if (!ssid || boundedLength(ssid, SSID_MAX_LEN + 1u) == 0u ||
        boundedLength(ssid, SSID_MAX_LEN + 1u) > SSID_MAX_LEN)
    {
        WifiService_NotifySTAConfigurationFailure(h);
        return WIFI_LINK_CONNECT_FAILED;
    }

    if (disconnect_first)
        h.driver.disconnect(false, false);

    h.driver.mode(keep_ap ? WIFI_RADIO_AP_STA : WIFI_RADIO_STA);
    WifiLinkStatus status = bssid
                             ? h.driver.begin(ssid, password ? password : "", 0, bssid, true)
                             : h.driver.begin(ssid, password ? password : "");
    armStaAttempt(h);
    return status;
}

static bool startAPInternal(WifiServiceHandle &h, const char *ssid, const char *password,
                            bool enable_sta_interface, uint8_t channel,
                            bool hidden, uint8_t max_clients,
                            bool ap_protected = true)
{
    size_t ssid_length = boundedLength(ssid, SSID_MAX_LEN + 1u);
    size_t pass_length = boundedLength(password, PASS_MAX_LEN + 1u);
    if (ssid_length == 0u || ssid_length > SSID_MAX_LEN ||
        pass_length > PASS_MAX_LEN || (pass_length > 0u && pass_length < 8u))
    {
        // Una solicitud invalida no modifica el driver ni puede declarar que
        // un AP ya activo se detuvo. Conservamos la FSM que refleja el estado
        // fisico anterior.
        return false;
    }

    h.driver.mode(enable_sta_interface ? WIFI_RADIO_AP_STA : WIFI_RADIO_AP);
    const char *ap_password = (!ap_protected || pass_length == 0u) ? nullptr : password;
    bool started = h.driver.softAP(ssid, ap_password, channel, hidden ? 1 : 0, max_clients);
    setApRunning(h, started);
    if (started)
    {
        h.state = h.driver.status() == WIFI_LINK_CONNECTED
                      ? WIFI_SERVICE_STA_CONNECTED
                      : WIFI_SERVICE_AP_RUNNING;
    }
    else
    {
        h.state = h.driver.status() == WIFI_LINK_CONNECTED
                      ? WIFI_SERVICE_STA_CONNECTED
                      : WIFI_SERVICE_IDLE;
    }
    return started;
}

bool WifiService_Begin(WifiServiceHandle &h, const AppConfig &cfg, WifiRunMode mode)
{
    h.mode = mode;
    h.state = WIFI_SERVICE_IDLE;
    h.ap_started = false;
    h.keep_ap_with_sta = false;
    h.sta_reconnect_enabled = false;
    h.scan_previous_mode = WIFI_RADIO_OFF;
    h.scan_mode_restore_pending = false;
    h.scan_keep_ap_hybrid = false;
    h.ap_fallback_ready_pending = false;
    h.sta_attempt_failed_pending = false;

    h.driver.initialize();
    setStaIdleFlags(h);
    setApRunning(h, false);

    if (mode == WIFI_RUN_AP_ONLY)
        return WifiService_ApplyAPConfig(h, cfg) &&
               startAPInternal(h, cfg.apSsid, cfg.apPass, false, 1, false, 4);

    if (cfg.staSsid[0] == '\0')
    {
        h.sta_attempt_failed_pending = true;
        if (mode == WIFI_RUN_AP_STA && cfg.apFallbackEnabled)
            return WifiService_ApplyAPConfig(h, cfg) &&
                   startAPInternal(h, cfg.apSsid, cfg.apPass, true, 1, false, 4);
        h.driver.mode(WIFI_RADIO_STA);
        return false;
    }

    if (!WifiService_ApplySTAConfig(h, cfg))
    {
        h.sta_attempt_failed_pending = true;
        if (mode == WIFI_RUN_AP_STA) enterApFallbackState(h);
        return false;
    }
    WifiLinkStatus status = startStaAttempt(h, cfg.staSsid, cfg.staPass, false, false);
    return status != WIFI_LINK_CONNECT_FAILED;
}

void WifiService_Loop(WifiServiceHandle &h)
{
    bool driver_ap_active = modeHasAP(h.driver.getMode());
    if (driver_ap_active != h.ap_started)
        setApRunning(h, driver_ap_active);

    switch (h.state)
    {
    case WIFI_SERVICE_STA_CONNECTING:
        if (h.driver.status() == WIFI_LINK_CONNECTED)
        {
            setStaConnectedFlags(h);
            h.state = WIFI_SERVICE_STA_CONNECTED;
            break;
        }

        if (h.sta_reconnect_enabled && (*h.sta_retry) == 0)
        {
            (*h.sta_retry) = STA_RETRY_PERIOD_TICKS;
            h.driver.reconnect();
        }

        if ((uint32_t)(h.services.ticks(h.services.context) - h.sta_start_tick) >= WIFI_CONNECT_TIMEOUT_TICKS)
        {
            CLEAR_FLAG((*h.flags), h.flags->bits.b2);
            SET_FLAG((*h.flags), h.flags->bits.b1);
            (*h.sta_wait) = 0;
            (*h.sta_retry) = 0;
            h.sta_attempt_failed_pending = true;
            if (h.mode == WIFI_RUN_AP_STA)
            {
                enterApFallbackState(h);
            }
            else
            {
                h.driver.disconnect(false, false);
                h.state = WIFI_SERVICE_IDLE;
            }
        }
        break;

    case WIFI_SERVICE_AP_CREATING:
    {
        if (h.driver.status() == WIFI_LINK_CONNECTED)
        {
            setStaConnectedFlags(h);
            h.state = WIFI_SERVICE_STA_CONNECTED;
            break;
        }
        const AppConfig *cfg = h.services.config(h.services.context);
        if (cfg && !cfg->apFallbackEnabled)
        {
            enterApFallbackState(h);
            break;
        }
        const char *ssid = cfg ? cfg->apSsid : DEFAULT_AP_SSID;
        const char *password = cfg ? cfg->apPass : DEFAULT_AP_PASSWORD;
        if (cfg && !WifiService_ApplyAPConfig(h, *cfg))
        {
            setApRunning(h, false);
            h.state = WIFI_SERVICE_IDLE;
            break;
        }
        bool ap_prot = cfg ? (cfg->apProtected != 0u) : true;
        (void)startAPInternal(h, ssid, password, h.mode == WIFI_RUN_AP_STA, 1, false, 4, ap_prot);
        break;
    }

    case WIFI_SERVICE_AP_RUNNING:
        if (h.sta_reconnect_enabled &&
            (uint32_t)(h.services.ticks(h.services.context) - h.last_retry_tick) >= STA_RETRY_PERIOD_TICKS)
        {
            h.last_retry_tick = h.services.ticks(h.services.context);
            h.driver.reconnect();
        }
        if (h.driver.status() == WIFI_LINK_CONNECTED)
        {
            setStaConnectedFlags(h);
            h.state = WIFI_SERVICE_STA_CONNECTED;
        }
        break;

    case WIFI_SERVICE_STA_CONNECTED:
        if (h.driver.status() != WIFI_LINK_CONNECTED)
        {
            setStaIdleFlags(h);
            if (h.sta_reconnect_enabled)
            {
                const AppConfig *cfg = h.services.config(h.services.context);
                if (cfg && cfg->staSsid[0] != '\0')
                {
                    (void)startStaAttempt(h, cfg->staSsid, cfg->staPass, h.ap_started, false);
                    break;
                }
            }
            h.sta_attempt_failed_pending = true;
            if (h.mode == WIFI_RUN_AP_STA)
            {
                enterApFallbackState(h);
            }
            else
            {
                h.state = h.ap_started ? WIFI_SERVICE_AP_RUNNING : WIFI_SERVICE_IDLE;
            }
            break;
        }
        if (h.ap_started && !h.keep_ap_with_sta)
            WifiService_StopAP(h, false);
        break;

    case WIFI_SERVICE_IDLE:
    default:
        if (h.driver.status() == WIFI_LINK_CONNECTED)
        {
            setStaConnectedFlags(h);
            h.state = WIFI_SERVICE_STA_CONNECTED;
        }
        else if (h.ap_started)
        {
            h.state = WIFI_SERVICE_AP_RUNNING;
        }
        break;
    }

    // La finalizacion puede pertenecer a Web o a F4. Centralizarla evita que
    // un timeout deje para siempre pendiente el estado del driver de scan.
    WifiService_RestoreModeAfterScan(h);
    if (h.services.cooperate) h.services.cooperate(h.services.context);
}

int WifiService_StartScan(WifiServiceHandle &h, bool async, bool show_hidden)
{
    if (!h.scan_mode_restore_pending)
    {
        h.scan_previous_mode = h.driver.getMode();
        h.scan_mode_restore_pending = true;
        h.scan_keep_ap_hybrid = modeHasAP(h.scan_previous_mode);

        // ESP8266 usa una sola radio. Un scan necesita la interfaz STA, pero
        // apagar/recrear el SoftAP corta sockets existentes. Si el AP estaba
        // activo, se entra una sola vez en AP+STA y se conserva el AP durante
        // todo el barrido (y despues de el) para mantener DHCP/TCP/WS vivos.
        if (h.scan_keep_ap_hybrid && h.driver.getMode() != WIFI_RADIO_AP_STA)
            h.driver.mode(WIFI_RADIO_AP_STA);
    }
    return (int)h.driver.scanNetworks(async, show_hidden);
}

int WifiService_ScanComplete(WifiServiceHandle &h)
{
    return (int)h.driver.scanComplete();
}

void WifiService_DeleteScanResults(WifiServiceHandle &h)
{
    h.driver.scanDelete();
}

void WifiService_RestoreModeAfterScan(WifiServiceHandle &h)
{
    if (!h.scan_mode_restore_pending || h.driver.scanComplete() == WIFI_SERVICE_SCAN_RUNNING)
        return;

    // En un scan iniciado desde SoftAP no se vuelve de AP+STA a AP: esa
    // segunda mutacion del opmode es innecesaria y puede resetear conexiones
    // TCP del AP. Para STA/OFF se conserva el comportamiento anterior.
    WifiRadioMode target_mode = h.scan_keep_ap_hybrid
                                 ? WIFI_RADIO_AP_STA
                                 : h.scan_previous_mode;
    if (h.driver.getMode() != target_mode)
        h.driver.mode(target_mode);

    h.scan_previous_mode = WIFI_RADIO_OFF;
    h.scan_mode_restore_pending = false;
    h.scan_keep_ap_hybrid = false;
}

WifiLinkStatus WifiService_ConnectSTA(WifiServiceHandle &h, const char *ssid, const char *password,
                                 bool keep_ap, bool disconnect_first,
                                 const uint8_t *bssid)
{
    const AppConfig *cfg = h.services.config(h.services.context);
    h.mode = (keep_ap || !cfg || cfg->apFallbackEnabled) ? WIFI_RUN_AP_STA : WIFI_RUN_STA_ONLY;
    h.keep_ap_with_sta = keep_ap;
    return startStaAttempt(h, ssid, password, keep_ap, disconnect_first, bssid);
}

bool WifiService_DisconnectSTA(WifiServiceHandle &h, bool radio_off, bool erase_sdk_credentials)
{
    WifiService_SetSTAReconnectEnabled(h, false);
    bool disconnected = h.driver.disconnect(radio_off, erase_sdk_credentials);
    setStaIdleFlags(h);
    bool ap_active = modeHasAP(h.driver.getMode());
    setApRunning(h, ap_active);
    h.state = ap_active ? WIFI_SERVICE_AP_RUNNING : WIFI_SERVICE_IDLE;
    return disconnected;
}

bool WifiService_ReconnectSTA(WifiServiceHandle &h)
{
    const AppConfig *cfg = h.services.config(h.services.context);
    if (!cfg || cfg->staSsid[0] == '\0')
        return false;
    if (!WifiService_ApplySTAConfig(h, *cfg))
        return false;
    WifiLinkStatus status = startStaAttempt(h, cfg->staSsid, cfg->staPass,
                                         h.ap_started && h.mode == WIFI_RUN_AP_STA,
                                         false);
    return status != WIFI_LINK_CONNECT_FAILED;
}

bool WifiService_ConfigureSTA(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet)
{
    return h.driver.config(local_ip, gateway, subnet);
}

bool WifiService_ConfigureAP(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet)
{
    return h.driver.softAPConfig(local_ip, gateway, subnet);
}

bool WifiService_ApplySTAConfig(WifiServiceHandle &h, const AppConfig &cfg)
{
    WifiIpv4 local_ip;
    WifiIpv4 gateway;
    WifiIpv4 subnet;
    if (!parseIpv4Text(cfg.staIp, local_ip) ||
        !parseIpv4Text(cfg.staGateway, gateway) ||
        !parseIpv4Text(cfg.staSubnet, subnet))
        return false;

    if (local_ip == WifiIpv4(0, 0, 0, 0))
        return WifiService_ConfigureSTA(h, WifiIpv4(0, 0, 0, 0),
                                      WifiIpv4(0, 0, 0, 0),
                                      WifiIpv4(0, 0, 0, 0));
    if (gateway == WifiIpv4(0, 0, 0, 0) ||
        subnet == WifiIpv4(0, 0, 0, 0))
        return false;
    return WifiService_ConfigureSTA(h, local_ip, gateway, subnet);
}

bool WifiService_ApplyAPConfig(WifiServiceHandle &h, const AppConfig &cfg)
{
    WifiIpv4 local_ip;
    if (!parseIpv4Text(cfg.apIp, local_ip))
        return false;
    if (local_ip == WifiIpv4(0, 0, 0, 0))
        local_ip = WifiIpv4(192, 168, 4, 1);
    return WifiService_ConfigureAP(h, local_ip, local_ip,
                                 WifiIpv4(255, 255, 255, 0));
}

bool WifiService_StartAP(WifiServiceHandle &h, const char *ssid, const char *password,
                       bool keep_sta, uint8_t channel,
                       bool hidden, uint8_t max_clients,
                       bool ap_protected)
{
    h.mode = keep_sta ? WIFI_RUN_AP_STA : WIFI_RUN_AP_ONLY;
    h.keep_ap_with_sta = keep_sta;
    return startAPInternal(h, ssid, password, keep_sta, channel, hidden, max_clients, ap_protected);
}

bool WifiService_StopAP(WifiServiceHandle &h, bool radio_off)
{
    bool stopped = h.driver.softAPdisconnect(radio_off);
    setApRunning(h, false);
    h.keep_ap_with_sta = false;
    if (h.driver.status() == WIFI_LINK_CONNECTED)
    {
        setStaConnectedFlags(h);
        h.state = WIFI_SERVICE_STA_CONNECTED;
    }
    else
    {
        h.state = WIFI_SERVICE_IDLE;
    }
    return stopped;
}

void WifiService_SetSTAReconnectEnabled(WifiServiceHandle &h, bool enabled)
{
    h.sta_reconnect_enabled = enabled;
    h.driver.setAutoReconnect(enabled);
}

bool WifiService_IsSTAReconnectEnabled(WifiServiceHandle &h)
{
    return h.sta_reconnect_enabled;
}

WifiServiceState WifiService_GetState(WifiServiceHandle &h) { return h.state; }
WifiIpv4 WifiService_GetSTA_IP(WifiServiceHandle &h) { return h.driver.localIP(); }
WifiIpv4 WifiService_GetAP_IP(WifiServiceHandle &h) { return h.driver.softAPIP(); }
bool WifiService_STA_Connected(WifiServiceHandle &h) { return h.driver.status() == WIFI_LINK_CONNECTED; }
WifiRadioMode WifiService_GetMode(WifiServiceHandle &h) { return h.driver.getMode(); }
WifiLinkStatus WifiService_GetSTAStatus(WifiServiceHandle &h) { return h.driver.status(); }
uint8_t WifiService_GetAPClientCount(WifiServiceHandle &h) { return h.driver.softAPgetStationNum(); }

bool WifiService_ConsumeAPFallbackReady(WifiServiceHandle &h)
{
    bool pending = h.ap_fallback_ready_pending;
    h.ap_fallback_ready_pending = false;
    return pending;
}

bool WifiService_ConsumeSTAAttemptFailure(WifiServiceHandle &h) {
    const bool pending = h.sta_attempt_failed_pending;
    h.sta_attempt_failed_pending = false;
    return pending;
}
