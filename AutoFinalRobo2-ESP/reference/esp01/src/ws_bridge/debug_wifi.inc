// Volatile Wi-Fi configuration, owned by one resumable WebSocket session.
static bool debug_wifi_begin(WsmSession *session)
{
    DebugWifiScope &scope = s_bridge.debug_wifi;
    if (!session || !s_bridge.wifi.ops->SetSessionConfig || !s_bridge.wifi.ops->BeginSessionMode) return false;
    if (scope.active) return scope.owner_node == session->node;
    if (s_bridge.wifi_credentials_web.requested || s_bridge.ap_credentials_web.active ||
        s_bridge.json_scan_active || s_bridge.ap_provisioning_exit.active ||
        (s_bridge.backend.reboot_pending && *s_bridge.backend.reboot_pending)) return false;
    scope.config = current_config_copy();
    scope.previous_mode = s_bridge.platform.radio_mode();
    scope.previous_reconnect = s_bridge.wifi.IsSTAReconnectEnabled();
    scope.owner_node = session->node;
    scope.active = 1u;
    s_bridge.wifi.SetSessionConfig(&scope.config);
    return true;
}

static void debug_wifi_release()
{
    DebugWifiScope &scope = s_bridge.debug_wifi;
    if (!scope.active) return;
    const bool radio_dirty = scope.radio_dirty != 0u;
    const WifiRadioMode previous_mode = scope.previous_mode;
    const bool previous_reconnect = scope.previous_reconnect != 0u;
    if (s_bridge.wifi_credentials_web.session_only)
        memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
    if (s_bridge.ap_credentials_web.session_only)
        memset(&s_bridge.ap_credentials_web, 0, sizeof(s_bridge.ap_credentials_web));
    if (s_bridge.json_scan_active) finish_json_scan();
    s_bridge.ap_client_disconnect.active = 0u;
    s_bridge.wifi.SetSessionConfig(nullptr);
    // Clear credentials from RAM, then restore the latest authoritative cache.
    memset(&scope, 0, sizeof(scope));
    if (!radio_dirty) { s_bridge.wifi.SetSTAReconnectEnabled(previous_reconnect); return; }
    const AppConfig config = current_config_copy();
    const WifiRunMode mode = previous_mode == WIFI_RADIO_AP ? WIFI_RUN_AP_ONLY :
        (previous_mode == WIFI_RADIO_AP_STA ? WIFI_RUN_AP_STA : WIFI_RUN_STA_ONLY);
    (void)s_bridge.wifi.DisconnectSTA(false, false);
    (void)s_bridge.wifi.StopAP(false);
    (void)s_bridge.wifi.BeginSessionMode(config, mode);
    s_bridge.wifi.SetSTAReconnectEnabled(previous_reconnect);
}

static void service_debug_wifi_scope()
{
    DebugWifiScope &scope = s_bridge.debug_wifi;
    if (!scope.active) return;
    WsmSession *owner = wsm_find_by_node(&s_bridge.sessions, scope.owner_node);
    const uint32_t now = s_bridge.platform.now_ms();
    if (!owner || !owner->esp_debug_wifi ||
        (owner->state != WSM_SLOT_ACTIVE &&
         (!owner->resume_until_ms || deadline_reached(now, owner->resume_until_ms))))
    {
        debug_wifi_release();
        return;
    }
    if (scope.mode_pending && deadline_reached(now, scope.mode_apply_at_ms))
    {
        scope.mode_pending = 0u;
        scope.radio_dirty = 1u;
        const bool ap = scope.config.nextBootMode == APP_BOOT_MODE_AP_ONLY;
        (void)s_bridge.wifi.DisconnectSTA(false, false);
        (void)s_bridge.wifi.StopAP(false);
        const bool ok = s_bridge.wifi.BeginSessionMode(scope.config,
            ap ? WIFI_RUN_AP_ONLY : (scope.config.apFallbackEnabled ? WIFI_RUN_AP_STA : WIFI_RUN_STA_ONLY));
        StaticJsonDocument<384> doc;
        JsonObject data = make_event_base(doc, "wifi.debug.mode.result", "esp", nullptr);
        data["success"] = ok;
        data["sessionOnly"] = true;
        data["persisted"] = false;
        data["mode"] = ap ? "AP" : "RED";
        broadcast_doc(doc);
    }
}

static void cmd_debug_ap_set(uint32_t client_id, const char *request_id,
                             const char *command, JsonObject params)
{
    dispatch_set_ap(client_id, request_id, command, params);
}

static void cmd_debug_ap_get(uint32_t client_id, const char *request_id,
                             const char *command, JsonObject params)
{
    (void)params;
    StaticJsonDocument<640> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Session AP configuration");
    const AppConfig config = current_config_copy();
    data["status"] = 0;
    data["ssid"] = config.apSsid;
    data["ip"] = config.apIp;
    data["protected"] = config.apProtected != 0u;
    data["sessionOnly"] = true;
    data["persisted"] = false;
    if (s_bridge.ap_credentials_result.available)
        data["lastApplyStatus"] = s_bridge.ap_credentials_result.status;
    send_doc_to_client(client_id, doc);
}

static void cmd_debug_mode_set(uint32_t client_id, const char *request_id,
                               const char *command, JsonObject params)
{
    if (s_bridge.wifi_credentials_web.connecting || s_bridge.ap_credentials_web.active ||
        s_bridge.json_scan_active || s_bridge.debug_wifi.mode_pending)
    {
        send_error(client_id, request_id, "esp", command, "busy", "Wi-Fi operation in progress");
        return;
    }
    const char *mode = params["mode"] | "";
    s_bridge.debug_wifi.config.nextBootMode = strcmp(mode, "ap") == 0 ? APP_BOOT_MODE_AP_ONLY : APP_BOOT_MODE_NORMAL;
    s_bridge.debug_wifi.mode_pending = 1u;
    s_bridge.debug_wifi.mode_apply_at_ms = s_bridge.platform.now_ms() + AP_APPLY_FLUSH_DELAY_MS;
    StaticJsonDocument<768> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Session radio mode scheduled; no reboot");
    data["sessionOnly"] = true;
    data["persisted"] = false;
    data["reboot"] = false;
    data["state"] = "scheduled";
    add_reboot_target_info(data, s_bridge.debug_wifi.config, s_bridge.debug_wifi.config.nextBootMode);
    send_doc_to_client(client_id, doc);
}

static void cmd_debug_fallback(uint32_t client_id, const char *request_id,
                               const char *command, JsonObject params)
{
    if (!params["enabled"].isNull())
    {
        if (!params["enabled"].is<bool>())
        {
            send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "enabled must be boolean");
            return;
        }
        s_bridge.debug_wifi.config.apFallbackEnabled = params["enabled"].as<bool>() ? 1u : 0u;
    }
    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Session fallback policy");
    data["status"] = 0;
    data["enabled"] = s_bridge.debug_wifi.config.apFallbackEnabled != 0u;
    data["syncState"] = 2;
    data["sessionOnly"] = true;
    data["persisted"] = false;
    send_doc_to_client(client_id, doc);
}
