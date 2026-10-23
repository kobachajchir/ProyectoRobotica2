// Private bridge implementation; included once by ws_json_bridge.cpp.
static void dispatch_get_mode(uint32_t client_id, const char *request_id, const char *command)
{
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Wi-Fi mode read");
    add_wifi_info(data, true);
    send_doc_to_client(client_id, doc);
}

static void dispatch_get_status(uint32_t client_id, const char *request_id, const char *command)
{
    StaticJsonDocument<1536> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Device status read");
    add_wifi_info(data, true);
    const bool sta_connected = s_bridge.platform.radio_status() == WIFI_LINK_CONNECTED;
    data["staConnected"] = sta_connected;
    data["connectedSsid"] = sta_connected ? s_bridge.platform.sta_ssid() : String();
    data["rssi"] = sta_connected ? s_bridge.platform.sta_rssi() : 0;
    data["rssiAvailable"] = sta_connected;
    data["apActive"] = s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA;
    data["wsClients"] = s_bridge.ws ? s_bridge.ws->count() : 0;
    WsmSession *status_session = session_for_client(client_id);
    data["sessionOnly"] = status_session && status_session->esp_debug_wifi;
    data["storage"] = s_bridge.debug_wifi.active ? "session" : "persistent";
    data["staReconnect"] = s_bridge.wifi.IsSTAReconnectEnabled();
    data["uartQueueDepth"] = utm_queued_count(&s_bridge.transactions);
    data["uartInFlight"] = utm_inflight(&s_bridge.transactions) != nullptr;
    data["wsBackpressureDrops"] = s_bridge.ws_backpressure_drops;
    WsClientState *state = find_client(client_id);
    data["clientBackpressureDrops"] = state ? state->backpressure_drops : 0u;
    data["apiV1Requests"] = s_bridge.api_v1_requests;
    data["legacyEnvelopes"] = s_bridge.legacy_envelopes;
    data["legacyRawRequests"] = s_bridge.legacy_raw_requests;
    data["f4Alive"] = f4_alive_now();
    JsonObject firmware = data.createNestedObject("firmware");
    firmware["name"] = "FirmESP01MicroArduino2025";
    firmware["version"] = ESP_FIRMWARE_VERSION;
    firmware["target"] = "ESP8266 ESP-01S";
    JsonObject build = data.createNestedObject("build");
    build["date"] = __DATE__;
    build["time"] = __TIME__;
#if defined(DEBUG_ESP_PORT)
    build["profile"] = "debug";
#else
    build["profile"] = "release";
#endif
    build["compiler"] = __VERSION__;
    build["arduinoCore"] = s_bridge.platform.core_version();
    build["sdk"] = s_bridge.platform.sdk_version();
    JsonObject runtime = data.createNestedObject("runtime");
    runtime["freeHeap"] = s_bridge.platform.free_heap();
    runtime["maxFreeBlock"] = s_bridge.platform.max_free_block();
    runtime["heapFragmentation"] = s_bridge.platform.heap_fragmentation();
    const UtmMetrics *metrics = utm_metrics(&s_bridge.transactions);
    if (metrics)
    {
        data["uartTimeouts"] = metrics->timed_out;
        data["uartRetries"] = metrics->retries;
        data["uartUnmatchedFrames"] = metrics->unmatched_frames;
    }
    send_doc_to_client(client_id, doc);
}

static void format_ap_client_mac(const uint8_t mac[6], char out[18])
{
    snprintf(out, 18u, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void format_ap_client_name(const uint8_t mac[6], char out[24])
{
    // El SDK ESP8266 expone MAC e IP de la estacion asociada, pero no su
    // hostname DHCP. Se entrega un nombre estable y honesto derivado de MAC.
    snprintf(out, 24u, "Dispositivo %02X%02X", mac[4], mac[5]);
}

static bool parse_ap_client_mac(const char *text, uint8_t out[6])
{
    unsigned int values[6];
    char tail = '\0';
    if (!text ||
        sscanf(text, "%2x:%2x:%2x:%2x:%2x:%2x%c",
               &values[0], &values[1], &values[2], &values[3],
               &values[4], &values[5], &tail) != 6)
        return false;
    for (uint8_t i = 0u; i < 6u; ++i) out[i] = (uint8_t)values[i];
    return true;
}

static bool ap_client_is_connected(const uint8_t mac[6])
{
    bool found = false;
    void *station = s_bridge.platform.station_first();
    void *cursor = station;
    while (cursor)
    {
        if (memcmp(s_bridge.platform.station_mac(cursor), mac, 6u) == 0)
        {
            found = true;
            break;
        }
        cursor = s_bridge.platform.station_next(cursor);
    }
    s_bridge.platform.station_free();
    return found;
}

uint8_t WSJsonBridge_BuildApClientsPage(uint8_t cursor,
                                        uint8_t maximum,
                                        uint8_t *out,
                                        uint8_t capacity,
                                        uint8_t *out_len)
{
    if (!out || !out_len || maximum == 0u || maximum > 4u || capacity < 2u) return 1u;
    *out_len = 0u;
    if (!(s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA)) return 2u;

    void *station = s_bridge.platform.station_first();
    void *item = station;
    uint8_t skipped = 0u;
    while (item && skipped < cursor) {
        item = s_bridge.platform.station_next(item);
        skipped++;
    }
    uint8_t offset = 2u;
    uint8_t count = 0u;
    while (item && count < maximum) {
        char name[24];
        format_ap_client_name(s_bridge.platform.station_mac(item), name);
        const uint8_t name_len = bounded_strlen_u8(name, sizeof(name));
        if ((uint16_t)offset + 11u + name_len > capacity) break;
        memcpy(&out[offset], s_bridge.platform.station_mac(item), 6u);
        offset = (uint8_t)(offset + 6u);
        const uint32_t raw_ip = s_bridge.platform.station_ip(item);
        out[offset++] = (uint8_t)(raw_ip & 0xFFu);
        out[offset++] = (uint8_t)((raw_ip >> 8) & 0xFFu);
        out[offset++] = (uint8_t)((raw_ip >> 16) & 0xFFu);
        out[offset++] = (uint8_t)((raw_ip >> 24) & 0xFFu);
        out[offset++] = name_len;
        memcpy(&out[offset], name, name_len);
        offset = (uint8_t)(offset + name_len);
        count++;
        item = s_bridge.platform.station_next(item);
    }
    out[0] = item ? (uint8_t)(cursor + count) : 0xFFu;
    out[1] = count;
    *out_len = offset;
    s_bridge.platform.station_free();
    return 0u;
}

uint8_t WSJsonBridge_ScheduleApClientDisconnect(const uint8_t mac[6])
{
    if (!mac) return 1u;
    if (!(s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA)) return 2u;
    if (s_bridge.ap_client_disconnect.active) return 3u;
    if (!ap_client_is_connected(mac)) return 4u;
    memcpy(s_bridge.ap_client_disconnect.mac, mac, 6u);
    s_bridge.ap_client_disconnect.execute_at_ms = s_bridge.platform.now_ms() + AP_CLIENT_DEAUTH_FLUSH_MS;
    s_bridge.ap_client_disconnect.active = 1u;
    return 0u;
}

static void add_ap_clients(JsonObject data){
    JsonArray clients = data.createNestedArray("clients");
    void *station = s_bridge.platform.station_first();
    void *cursor = station;
    uint8_t count = 0u;
    while (cursor && count < 4u)
    {
        char mac[18];
        char name[24];
        char ip[16];
        uint32_t raw_ip = s_bridge.platform.station_ip(cursor);
        format_ap_client_mac(s_bridge.platform.station_mac(cursor), mac);
        format_ap_client_name(s_bridge.platform.station_mac(cursor), name);
        snprintf(ip, sizeof(ip), "%u.%u.%u.%u",
                 (unsigned int)(raw_ip & 0xFFu),
                 (unsigned int)((raw_ip >> 8) & 0xFFu),
                 (unsigned int)((raw_ip >> 16) & 0xFFu),
                 (unsigned int)((raw_ip >> 24) & 0xFFu));
        JsonObject item = clients.createNestedObject();
        item["name"] = name;
        item["nameSource"] = "generated-from-mac";
        item["mac"] = mac;
        item["ip"] = ip;
        count++;
        cursor = s_bridge.platform.station_next(cursor);
    }
    s_bridge.platform.station_free();
    data["count"] = count;
    data["apActive"] = s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA;
}

static void dispatch_ap_clients_list(uint32_t client_id, const char *request_id,
                                     const char *command)
{
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "SoftAP clients read");
    add_ap_clients(data);
    send_doc_to_client(client_id, doc);
}

static void dispatch_ap_client_disconnect(uint32_t client_id,
                                          const char *request_id,
                                          const char *command,
                                          JsonObject params)
{
    uint8_t mac[6];
    if (!parse_ap_client_mac(params["mac"] | "", mac))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "mac must use AA:BB:CC:DD:EE:FF");
        return;
    }
    if (!(s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "SoftAP is not active");
        return;
    }
    if (s_bridge.ap_client_disconnect.active)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "Another SoftAP client disconnect is pending");
        return;
    }
    if (!ap_client_is_connected(mac))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "SoftAP client is no longer connected");
        return;
    }

    memcpy(s_bridge.ap_client_disconnect.mac, mac, sizeof(mac));
    s_bridge.ap_client_disconnect.execute_at_ms = s_bridge.platform.now_ms() + AP_CLIENT_DEAUTH_FLUSH_MS;
    s_bridge.ap_client_disconnect.active = 1u;

    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "SoftAP client disconnect scheduled");
    char canonical_mac[18];
    format_ap_client_mac(mac, canonical_mac);
    data["mac"] = canonical_mac;
    data["state"] = "scheduled";
    data["disconnectInMs"] = AP_CLIENT_DEAUTH_FLUSH_MS;
    send_doc_to_client(client_id, doc);
}

static void dispatch_reboot_esp(uint32_t client_id, const char *request_id,
                                const char *command, JsonObject params)
{
    if (!s_bridge.app)
    {
        send_error(client_id, request_id, "esp", command, "internal_error",
                   "UNER app is not initialized");
        return;
    }

    const char *mode_name = params["mode"] | "";
    const uint8_t boot_mode = strcmp(mode_name, "ap") == 0
                                  ? APP_BOOT_MODE_AP_ONLY
                                  : APP_BOOT_MODE_NORMAL;

    if ((*s_bridge.backend.reboot_pending))
    {
        if ((*s_bridge.backend.reboot_boot_mode) != boot_mode)
        {
            send_error(client_id, request_id, "esp", command, "busy",
                       "A reboot is already scheduled with another mode");
            return;
        }

        StaticJsonDocument<512> repeated_doc;
        JsonObject repeated = make_response_base(
            repeated_doc, client_id, request_id, "esp", command, true, "OK",
            "ESP reboot was already scheduled");
        repeated["mode"] = mode_name;
        repeated["bootMode"] = boot_mode;
        repeated["rebootInMs"] = 0u;
        repeated["persisted"] = true;
        repeated["alreadyScheduled"] = true;
        add_reboot_target_info(repeated, current_config_copy(), boot_mode);
        send_doc_to_client(client_id, repeated_doc);
        return;
    }

    const AppConfig previous = current_config_copy();
    AppConfig next{};
    if (!BootMode_ApplyRequest(&previous, boot_mode, &next))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "Unsupported ESP boot mode");
        return;
    }
    if (!s_bridge.backend.save_config(s_bridge.backend.context, next))
    {
        send_error(client_id, request_id, "esp", command, "persistence_error",
                   "ESP boot mode could not be persisted");
        return;
    }

    // La respuesta se encola antes de armar el reinicio. El retardo deja que
    // AsyncWebSocket la entregue al navegador antes de que ESP.restart corte
    // el enlace.
    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "ESP reboot scheduled");
    data["mode"] = mode_name;
    data["bootMode"] = boot_mode;
    data["rebootInMs"] = WEB_REBOOT_FLUSH_DELAY_MS;
    data["persisted"] = true;
    data["alreadyScheduled"] = false;
    add_reboot_target_info(data, next, boot_mode);
    send_doc_to_client(client_id, doc);

    (*s_bridge.backend.reboot_boot_mode) = boot_mode;
    (*s_bridge.backend.reboot_not_before_ms) = s_bridge.platform.now_ms() + WEB_REBOOT_FLUSH_DELAY_MS;
    (*s_bridge.backend.reboot_pending) = 1u;
}

static void dispatch_get_scan(uint32_t client_id, const char *request_id, const char *command)
{
    WsClientState *origin = find_client(client_id);
    WsmSession *origin_session = origin
        ? wsm_find(&s_bridge.sessions, origin->id, origin->generation)
        : nullptr;
    if (!origin || !origin_session)
    {
        send_error(client_id, request_id, "esp", command, "unauthorized",
                   "WebSocket session is not available");
        return;
    }

    if (s_bridge.json_scan_result_pending)
    {
        if (s_bridge.json_scan_result_ip_key == origin_session->ip_key)
            (void)deliver_pending_json_scan_result(client_id,
                                                   origin->preferred_api_v1,
                                                   true);
        if (s_bridge.json_scan_result_pending)
        {
            send_error(client_id, request_id, "esp", command, "busy",
                       "Previous Wi-Fi scan result is waiting for its client");
            return;
        }
    }

    if (s_bridge.json_scan_active || s_bridge.json_scan_cleanup_pending)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   s_bridge.json_scan_active
                       ? "Wi-Fi scan already running"
                       : "Previous Wi-Fi scan is still finishing in the driver");
        return;
    }
    WifiScanAcquireResult ownership =
        s_bridge.backend.acquire_scan(s_bridge.backend.context, WIFI_SCAN_OWNER_WEB);
    if (ownership != WIFI_SCAN_ACQUIRED)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "Wi-Fi scan driver is in use by another origin");
        return;
    }

    /* scanDelete libera el snapshot nativo del barrido anterior. Medir antes
     * producia falsos rechazos: el heap recuperable seguia ocupado justo al
     * evaluar si el siguiente scan era seguro. */
    s_bridge.wifi.DeleteScanResults();
    delay(0);
    const uint32_t free_heap = s_bridge.platform.free_heap();
    const uint32_t max_block = s_bridge.platform.max_free_block();
    const uint8_t fragmentation = s_bridge.platform.heap_fragmentation();
    if (free_heap < JSON_SCAN_MIN_FREE_HEAP ||
        max_block < JSON_SCAN_MIN_CONTIGUOUS_HEAP)
    {
        char message[144];
        snprintf(message, sizeof(message),
                 "Memoria insuficiente para escanear: libre=%lu B, bloque=%lu B, fragmentacion=%u%%",
                 (unsigned long)free_heap, (unsigned long)max_block,
                 (unsigned int)fragmentation);
        (void)s_bridge.backend.release_scan(s_bridge.backend.context, WIFI_SCAN_OWNER_WEB);
        send_error(client_id, request_id, "esp", command, "busy", message);
        return;
    }
    s_bridge.json_scan_previous_auto_reconnect =
        s_bridge.wifi.IsSTAReconnectEnabled() ? 1u : 0u;
    s_bridge.wifi.SetSTAReconnectEnabled(false);
    int rc = s_bridge.wifi.StartScan(true);
    WS_SCAN_LOG("[WS][SCAN] START rc=%d active=%u client=%lu request='%s'\n",
                rc, s_bridge.json_scan_active, (unsigned long)client_id, request_id);
    if (!(rc == WIFI_DRIVER_SCAN_RUNNING || rc >= 0))
    {
        finish_json_scan();
        send_error(client_id, request_id, "esp", command, "ERR_INTERNAL", "Could not start Wi-Fi scan");
        return;
    }
    s_bridge.json_scan_active = 1;
    s_bridge.json_scan_started_ms = s_bridge.platform.now_ms();
    s_bridge.json_scan_last_log_ms = s_bridge.json_scan_started_ms;
    s_bridge.json_scan_client_id = client_id;
    s_bridge.json_scan_generation = origin ? origin->generation : 0u;
    s_bridge.json_scan_origin_ip_key = origin_session->ip_key;
    s_bridge.json_scan_origin_api_v1 = origin->preferred_api_v1;
    s_bridge.json_scan_reconnect_until_ms = 0u;
    copy_cstr(s_bridge.json_scan_request_id, sizeof(s_bridge.json_scan_request_id), request_id);
    send_response(client_id, request_id, "esp", command, true, "OK", "Wi-Fi scan started");
}

static void dispatch_stop_scan(uint32_t client_id, const char *request_id,
                               const char *command)
{
    if (!s_bridge.json_scan_active)
    {
        StaticJsonDocument<384> doc;
        JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                             true, "OK", "No Web Wi-Fi scan was active");
        data["stopped"] = false;
        data["active"] = false;
        send_doc_to_client(client_id, doc);
        return;
    }

    WsClientState *requester = find_client(client_id);
    if (!requester || client_id != s_bridge.json_scan_client_id ||
        requester->generation != s_bridge.json_scan_generation)
    {
        send_error(client_id, request_id, "esp", command, "forbidden",
                   "Only the originating WebSocket session may stop this scan");
        return;
    }

    finish_json_scan();
    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "Wi-Fi scan stopped");
    data["stopped"] = true;
    data["active"] = false;
    send_doc_to_client(client_id, doc);
}

static void dispatch_wifi_detail_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    const char *ssid = params["ssid"] | "";
    uint8_t ssid_len = bounded_strlen_u8(ssid, SSID_MAX_LEN + 1);
    if (ssid_len == 0 || ssid_len > SSID_MAX_LEN)
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "ssid length is invalid");
        return;
    }

    UNER_WifiScanCacheEntry entry;
    if (!WifiScanSnapshot_FindBySsid(&s_bridge.web_scan_cache, ssid, &entry))
    {
        send_error(client_id, request_id, "esp", command, "ERR_NOT_FOUND", "SSID not present in scan cache");
        return;
    }

    StaticJsonDocument<640> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Wi-Fi detail ready");
    add_scan_network_fields(data, entry);
    send_doc_to_client(client_id, doc);
}

static void dispatch_set_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    WsmSession *debug_session = session_for_client(client_id);
    if (!debug_session || !debug_session->esp_debug_wifi || !s_bridge.debug_wifi.active) {
        send_error(client_id, request_id, "esp", command, "ERR_AUTH_REQUIRED", "Session AP writes require local debug" ); return;
    }
    if (s_bridge.ap_credentials_web.active)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "Another AP credentials transaction is applying");
        return;
    }
    const char *ssid = params["ssid"] | "";
    const char *requested_password = params["password"] | "";
    const AppConfig previous_config = current_config_copy();
    const bool reuse_password = params["reusePassword"] | false;
    if (reuse_password && strcmp(ssid, previous_config.apSsid) != 0)
    {
        send_error(client_id, request_id, "esp", command, "ERR_SSID_MISMATCH",
                   "Saved AP password may only be reused with the stored SSID");
        return;
    }
    const char *password = reuse_password ? previous_config.apPass : requested_password;
    const char *ip_text = params["ip"] | "0.0.0.0";
    uint8_t ssid_len = bounded_strlen_u8(ssid, SSID_MAX_LEN + 1);
    uint8_t pass_len = bounded_strlen_u8(password, PASS_MAX_LEN + 1);
    if (ssid_len == 0 || ssid_len > SSID_MAX_LEN || pass_len > PASS_MAX_LEN || (pass_len > 0 && pass_len < 8))
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "AP ssid/password length is invalid");
        return;
    }
    WifiIpv4 ap_ip;
    if (!parse_ipv4(ip_text, ap_ip))
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "AP ip must be IPv4");
        return;
    }
    WsmSession *session = session_for_client(client_id);
    memset(&s_bridge.ap_credentials_web, 0, sizeof(s_bridge.ap_credentials_web));
    s_bridge.ap_credentials_web.active = 1u;
    s_bridge.ap_credentials_web.session_only = 1u;
    s_bridge.ap_credentials_web.apply_radio = (s_bridge.wifi.GetMode() == WIFI_RADIO_AP || s_bridge.wifi.GetMode() == WIFI_RADIO_AP_STA) ? 1u : 0u;
    s_bridge.ap_credentials_web.client_id = client_id;
    s_bridge.ap_credentials_web.generation = session ? session->generation : 0u;
    s_bridge.ap_credentials_web.previous_config = previous_config;
    s_bridge.ap_credentials_web.candidate_config = s_bridge.ap_credentials_web.previous_config;
    copy_cstr(s_bridge.ap_credentials_web.candidate_config.apSsid,
              sizeof(s_bridge.ap_credentials_web.candidate_config.apSsid), ssid);
    copy_cstr(s_bridge.ap_credentials_web.candidate_config.apPass,
              sizeof(s_bridge.ap_credentials_web.candidate_config.apPass), password);
    copy_cstr(s_bridge.ap_credentials_web.candidate_config.apIp,
              sizeof(s_bridge.ap_credentials_web.candidate_config.apIp), ip_text);
    s_bridge.ap_credentials_web.candidate_config.apProtected = (params["protected"] | (pass_len > 0)) ? 1u : 0u;
    s_bridge.ap_credentials_web.previous_ip = s_bridge.wifi.GetAP_IP();
    s_bridge.ap_credentials_web.candidate_ip = ap_ip;
    s_bridge.ap_credentials_web.keep_sta =
        (s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA || s_bridge.platform.radio_status() == WIFI_LINK_CONNECTED) ? 1u : 0u;
    s_bridge.ap_credentials_web.apply_at_ms = s_bridge.platform.now_ms() + AP_APPLY_FLUSH_DELAY_MS;
    if (s_bridge.ap_credentials_web.apply_radio) s_bridge.debug_wifi.radio_dirty = 1u;
    copy_cstr(s_bridge.ap_credentials_web.request_id,
              sizeof(s_bridge.ap_credentials_web.request_id), request_id);
    copy_cstr(s_bridge.ap_credentials_web.command,
              sizeof(s_bridge.ap_credentials_web.command), command);

    memset(&s_bridge.ap_credentials_result, 0, sizeof(s_bridge.ap_credentials_result));
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "AP credentials accepted for deferred apply");
    data["status"] = "accepted";
    data["state"] = "applying";
    data["applyDelayMs"] = AP_APPLY_FLUSH_DELAY_MS;
    data["sessionOnly"] = true;
    data["persisted"] = false;
    add_wifi_info(data, false);
    data["configuredIp"] = ip_to_string(ap_ip);
    send_doc_to_client(client_id, doc);
}

static void dispatch_ap_credentials_result_get(uint32_t client_id,
                                               const char *request_id,
                                               const char *command)
{
    StaticJsonDocument<640> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "Latest AP apply result read");
    data["available"] = s_bridge.ap_credentials_result.available != 0u;
    data["applying"] = s_bridge.ap_credentials_web.active != 0u;
    if (s_bridge.ap_credentials_result.available)
    {
        data["success"] = s_bridge.ap_credentials_result.success != 0u;
        data["status"] = s_bridge.ap_credentials_result.status;
        data["reason"] = s_bridge.ap_credentials_result.reason;
        data["ssid"] = s_bridge.ap_credentials_result.ssid;
        data["configuredIp"] = s_bridge.ap_credentials_result.configured_ip;
        data["originalRequestId"] = s_bridge.ap_credentials_result.request_id;
        data["completedMs"] = s_bridge.ap_credentials_result.completed_ms;
    }
    send_doc_to_client(client_id, doc);
}

static void dispatch_connect_wifi(uint32_t client_id, const char *request_id, const char *command)
{
    const AppConfig *c = s_bridge.backend.config(s_bridge.backend.context);
    s_bridge.wifi.SetSTAReconnectEnabled(false);
    bool configured = !c || s_bridge.wifi.ApplySTAConfig(*c);
    WifiLinkStatus status = configured
        ? s_bridge.wifi.ConnectSTA(c ? c->staSsid : DEFAULT_STA_SSID,
                               c ? c->staPass : DEFAULT_STA_PASSWORD,
                               false, false)
        : WIFI_LINK_CONNECT_FAILED;
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         status != WIFI_LINK_CONNECT_FAILED,
                                         status != WIFI_LINK_CONNECT_FAILED ? "OK" : "ERR_BAD_PARAMS",
                                         status != WIFI_LINK_CONNECT_FAILED ? "STA connection started" : "Stored STA IP configuration is invalid");
    data["status"] = status != WIFI_LINK_CONNECT_FAILED ? 0 : 1;
    data["sessionOnly"] = s_bridge.debug_wifi.active != 0u;
    add_wifi_info(data, true);
    send_doc_to_client(client_id, doc);
}

static void dispatch_wifi_credentials_get_current(uint32_t client_id,
                                                  const char *request_id,
                                                  const char *command)
{
    const bool pending = s_bridge.wifi_credentials_web.requested != 0u;
    const bool connecting = pending && s_bridge.wifi_credentials_web.connecting != 0u;
    uint32_t timeout_ms = 0u;
    if (connecting)
    {
        const uint32_t now = s_bridge.platform.now_ms();
        timeout_ms = (int32_t)(s_bridge.wifi_credentials_web.deadline_ms - now) > 0
                         ? (s_bridge.wifi_credentials_web.deadline_ms - now)
                         : 0u;
    }

    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK",
                                         "Pending Wi-Fi credentials state read");
    data["pending"] = pending;
    data["connecting"] = connecting;
    data["ssid"] = pending ? s_bridge.wifi_credentials_web.candidate_config.staSsid : "";
    data["reason"] = !pending
                         ? "none"
                         : (connecting ? "testing"
                                       : (s_bridge.wifi_credentials_web.notify_stm
                                              ? "stm_request"
                                              : "web_request"));
    data["timeoutMs"] = timeout_ms;
    data["requestId"] = pending ? s_bridge.wifi_credentials_web.request_id : "";
    data["source"] = pending && s_bridge.wifi_credentials_web.notify_stm ? "stm" : "web";
    send_doc_to_client(client_id, doc);
}

static void dispatch_wifi_credentials_submit(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    const char *ssid = params["ssid"] | "";
    const char *requested_password = params["password"] | "";
    AppConfig previous_config = current_config_copy();
    const bool reuse_password = params["reusePassword"] | false;
    if (reuse_password && strcmp(ssid, previous_config.staSsid) != 0)
    {
        send_error(client_id, request_id, "esp", command, "ERR_SSID_MISMATCH",
                   "Saved STA password may only be reused with the stored SSID");
        return;
    }
    const char *password = reuse_password ? previous_config.staPass : requested_password;
    uint8_t ssid_len = bounded_strlen_u8(ssid, SSID_MAX_LEN + 1);
    uint8_t pass_len = bounded_strlen_u8(password, PASS_MAX_LEN + 1);
    bool has_pending_request = s_bridge.wifi_credentials_web.requested != 0;
    WsmSession *session = session_for_client(client_id);
    const bool session_only = session && session->esp_debug_wifi;
    if (session_only && s_bridge.wifi_credentials_web.notify_stm) {
        send_error(client_id, request_id, "esp", command, "busy", "F4 owns the pending credentials request" ); return;
    }

    if (has_pending_request && s_bridge.wifi_credentials_web.connecting)
    {
        send_error(client_id, request_id, "esp", command, "ERR_BUSY", "Wi-Fi credentials are already being tested");
        return;
    }
    if (ssid_len == 0 || ssid_len > SSID_MAX_LEN || pass_len > PASS_MAX_LEN)
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "ssid/password length is invalid");
        return;
    }
    if (has_pending_request &&
        strcmp(ssid, s_bridge.wifi_credentials_web.candidate_config.staSsid) != 0)
    {
        send_error(client_id, request_id, "esp", command, "ERR_SSID_MISMATCH", "SSID does not match pending request");
        return;
    }

    AppConfig candidate_config = previous_config;
    const bool ap_provisioning = ap_provisioning_active();
    copy_cstr(candidate_config.staSsid, sizeof(candidate_config.staSsid), ssid);
    copy_cstr(candidate_config.staPass, sizeof(candidate_config.staPass), password);
    const char *ip_text = params["ip"] | previous_config.staIp;
    const char *gateway_text = params["gateway"] | previous_config.staGateway;
    const char *subnet_text = params["subnet"] | previous_config.staSubnet;
    WifiIpv4 fixed_ip;
    WifiIpv4 gateway;
    WifiIpv4 subnet;
    if (!parse_ipv4(ip_text, fixed_ip) ||
        !parse_ipv4(gateway_text, gateway) ||
        !parse_ipv4(subnet_text, subnet) ||
        (fixed_ip != WifiIpv4(0, 0, 0, 0) &&
         (gateway == WifiIpv4(0, 0, 0, 0) || subnet == WifiIpv4(0, 0, 0, 0))))
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS",
                   "Static STA configuration requires valid ip, gateway and subnet");
        return;
    }
    copy_cstr(candidate_config.staIp, sizeof(candidate_config.staIp), ip_text);
    copy_cstr(candidate_config.staGateway, sizeof(candidate_config.staGateway), gateway_text);
    copy_cstr(candidate_config.staSubnet, sizeof(candidate_config.staSubnet), subnet_text);
    if (ap_provisioning)
        candidate_config.nextBootMode = APP_BOOT_MODE_NORMAL;
    if (session_only) s_bridge.debug_wifi.radio_dirty = 1u;
    if (!s_bridge.wifi.ApplySTAConfig(candidate_config))
    {
        (void)s_bridge.wifi.ApplySTAConfig(previous_config);
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS",
                   "STA IP configuration could not be applied");
        return;
    }

    if (!has_pending_request)
    {
        memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
        s_bridge.wifi_credentials_web.requested = 1;
        s_bridge.wifi_credentials_web.notify_stm = 0;
    }

    s_bridge.wifi_credentials_web.previous_config = previous_config;
    s_bridge.wifi_credentials_web.candidate_config = candidate_config;
    s_bridge.wifi_credentials_web.previous_auto_reconnect =
        s_bridge.wifi.IsSTAReconnectEnabled() ? 1u : 0u;
    s_bridge.wifi_credentials_web.session_only = session_only ? 1u : 0u;
    s_bridge.wifi_credentials_web.ap_provisioning = ap_provisioning && !session_only ? 1u : 0u;
    if (session_only) s_bridge.wifi_credentials_web.notify_stm = 0u;
    s_bridge.wifi_credentials_web.has_bssid = 0u;
    if (!params["bssid"].isNull())
    {
        s_bridge.wifi_credentials_web.has_bssid =
            parse_bssid(params["bssid"].as<const char *>(),
                        s_bridge.wifi_credentials_web.bssid) ? 1u : 0u;
    }

    copy_cstr(s_bridge.wifi_credentials_web.request_id, sizeof(s_bridge.wifi_credentials_web.request_id), request_id);
    s_bridge.wifi_credentials_web.client_id = client_id;
    s_bridge.wifi_credentials_web.generation = session ? session->generation : 0u;
    s_bridge.wifi_credentials_web.connecting = 1;
    s_bridge.wifi_credentials_web.started_ms = s_bridge.platform.now_ms();
    s_bridge.wifi_credentials_web.deadline_ms = s_bridge.wifi_credentials_web.started_ms + WIFI_CREDENTIALS_CONNECT_TIMEOUT_MS;

    s_bridge.wifi.SetSTAReconnectEnabled(false);
    WifiLinkStatus begin_status = s_bridge.wifi.ConnectSTA(
        s_bridge.wifi_credentials_web.candidate_config.staSsid,
        s_bridge.wifi_credentials_web.candidate_config.staPass,
        ap_provisioning, true,
        s_bridge.wifi_credentials_web.has_bssid ? s_bridge.wifi_credentials_web.bssid : nullptr);

    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Wi-Fi credentials accepted for testing");
    data["ssid"] = ssid;
    data["status"] = "accepted";
    data["state"] = "testing";
    data["sessionOnly"] = session_only;
    data["persisted"] = false;
    data["wlStatus"] = (uint8_t)begin_status;
    data["timeoutMs"] = WIFI_CREDENTIALS_CONNECT_TIMEOUT_MS;
    data["configuredIp"] = candidate_config.staIp;
    if (session_only) data["storage"] = "session";
    data["gateway"] = candidate_config.staGateway;
    data["subnet"] = candidate_config.staSubnet;
    send_doc_to_client(client_id, doc);

#if UNER_BRIDGE_DEBUG
    Serial.printf("[WS][WIFI_CREDENTIALS] submit ssid='%s' begin_status=%u timeout_ms=%lu\n",
                  ssid, (uint8_t)begin_status, (unsigned long)WIFI_CREDENTIALS_CONNECT_TIMEOUT_MS);
#endif

    if (begin_status == WIFI_LINK_CONNECT_FAILED)
    {
        finish_wifi_credentials_flow(1, "failed", "auth_failed", WifiIpv4(0, 0, 0, 0), true);
    }
}

static void dispatch_wifi_credentials_cancel(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    const char *ssid = params["ssid"] | "";
    uint8_t ssid_len = bounded_strlen_u8(ssid, SSID_MAX_LEN + 1);

    if (!s_bridge.wifi_credentials_web.requested)
    {
        send_error(client_id, request_id, "esp", command, "ERR_NO_PENDING_REQUEST", "No pending Wi-Fi credentials request");
        return;
    }
    if (ssid_len == 0 || ssid_len > SSID_MAX_LEN)
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "ssid length is invalid");
        return;
    }
    if (strcmp(ssid, s_bridge.wifi_credentials_web.candidate_config.staSsid) != 0)
    {
        send_error(client_id, request_id, "esp", command, "ERR_SSID_MISMATCH", "SSID does not match pending request");
        return;
    }

    if (s_bridge.wifi_credentials_web.connecting)
    {
        WsClientState *state = find_client(client_id);
        if (!state || client_id != s_bridge.wifi_credentials_web.client_id ||
            state->generation != s_bridge.wifi_credentials_web.generation)
        {
            send_error(client_id, request_id, "esp", command, "forbidden",
                       "Only the originating WebSocket session may cancel this test");
            return;
        }
    }

    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Wi-Fi credentials request cancelled");
    data["ssid"] = ssid;
    send_doc_to_client(client_id, doc);

#if UNER_BRIDGE_DEBUG
    Serial.printf("[WS][WIFI_CREDENTIALS] cancelled ssid='%s' connecting=%u\n",
                  ssid, s_bridge.wifi_credentials_web.connecting);
#endif
    finish_wifi_credentials_flow(3, "cancelled", nullptr, WifiIpv4(0, 0, 0, 0), true);
}

static void dispatch_disconnect_wifi(uint32_t client_id, const char *request_id, const char *command)
{
    s_bridge.wifi.SetSTAReconnectEnabled(false);
    (void)s_bridge.wifi.DisconnectSTA(true, false);
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "STA disconnected");
    add_wifi_info(data, true);
    send_doc_to_client(client_id, doc);
}

static void dispatch_start_ap(uint32_t client_id, const char *request_id, const char *command)
{
    const AppConfig *c = s_bridge.backend.config(s_bridge.backend.context);
    bool ok = (!c || s_bridge.wifi.ApplyAPConfig(*c)) &&
              s_bridge.wifi.StartAP(c ? c->apSsid : DEFAULT_AP_SSID,
                                c ? c->apPass : DEFAULT_AP_PASSWORD,
                                false);
    StaticJsonDocument<1024> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, ok, ok ? "OK" : "ERR_INTERNAL",
                                         ok ? "AP started" : "AP start failed");
    add_wifi_info(data, false);
    send_doc_to_client(client_id, doc);
}

static void dispatch_stop_ap(uint32_t client_id, const char *request_id, const char *command)
{
    (void)s_bridge.wifi.StopAP(true);
    StaticJsonDocument<768> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "AP stopped");
    add_wifi_info(data, false);
    send_doc_to_client(client_id, doc);
}

static void dispatch_auto_reconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    if (!params["enabled"].is<bool>())
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "enabled must be boolean");
        return;
    }
    bool enabled = params["enabled"];
    s_bridge.wifi.SetSTAReconnectEnabled(enabled);
    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Auto reconnect updated");
    data["enabled"] = enabled;
    send_doc_to_client(client_id, doc);
}

static void dispatch_telemetry_set_rate(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    if (!params["periodMs"].is<uint16_t>())
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "periodMs must be uint16");
        return;
    }
    uint16_t period_ms = params["periodMs"];
    if (!dispatch_stream_subscription(client_id, request_id, command,
                                      SB_STREAM_MPU, period_ms > 0u, period_ms))
        return;
    if (params["durationMs"].is<uint32_t>() && period_ms > 0)
    {
        uint32_t duration_ms = params["durationMs"];
        s_bridge.telem_stop.active = duration_ms > 0 ? 1u : 0u;
        s_bridge.telem_stop.client_id = client_id;
        copy_cstr(s_bridge.telem_stop.request_id, sizeof(s_bridge.telem_stop.request_id), request_id);
        s_bridge.telem_stop.deadline_ms = s_bridge.platform.now_ms() + duration_ms;
    }
    else if (period_ms == 0)
    {
        s_bridge.telem_stop.active = 0;
    }
}

static void dispatch_get_current_screen(uint32_t client_id, const char *request_id, const char *command)
{
    if (!start_stm_request(client_id, request_id, command,
                           UNER_CMD_GET_CURRENT_SCREEN, UNER_CMD_GET_CURRENT_SCREEN,
                           nullptr, 0, 0))
    {
        return;
    }
}

static void dispatch_pin_auth_command(uint32_t client_id, const char *request_id, const char *command,
                                      JsonObject params, uint8_t scope)
{
    uint8_t pin_ascii4[4];
    uint8_t payload[10];
    uint8_t payload_len = 6u;
    WsmSession *session = session_for_client(client_id);
    if (!session || !wsm_allow_auth_attempt(&s_bridge.sessions, client_id, session->generation, s_bridge.platform.now_ms()))
    {
        send_error(client_id, request_id, "esp", command, "busy", "Authentication rate limit exceeded");
        return;
    }

    if (!read_pin_ascii4(params, pin_ascii4))
    {
        send_error(client_id, request_id, "esp", command, "ERR_BAD_PARAMS", "pin must be a 4-digit string");
        return;
    }

    payload[0] = s_bridge.auth_request_id++;
    if (s_bridge.auth_request_id == 0u) s_bridge.auth_request_id = 1u;
    payload[1] = scope;
    for (uint8_t i = 0; i < 4u; ++i) payload[2u + i] = (uint8_t)(pin_ascii4[i] - (uint8_t)'0');
    if (scope == STM_AUTH_SCOPE_SCREEN)
    {
        uint32_t screen_code = 0;
        if (!read_json_u32(params, "screenCode", &screen_code))
        {
            send_error(client_id, request_id, "esp", command, "invalid_request", "screenCode is required");
            return;
        }
        write_u32_le(screen_code, &payload[6]);
        payload_len = 10u;
    }
    (void)start_stm_request(client_id, request_id, command,
                            UNER_CMD_AUTH_VALIDATE_PIN, UNER_CMD_AUTH_VALIDATE_PIN,
                            payload, payload_len, 0u);
}

static void dispatch_pin_change_command(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    WsmSession *session = session_for_client(client_id);
    uint8_t current_pin[4];
    uint8_t new_pin[4];
    if (!session || !wsm_allow_auth_attempt(&s_bridge.sessions, client_id, session->generation, s_bridge.platform.now_ms()))
    {
        send_error(client_id, request_id, "esp", command, "busy", "Authentication rate limit exceeded");
        return;
    }
    if (!read_pin_key_ascii4(params, "currentPin", current_pin) || !read_pin_key_ascii4(params, "newPin", new_pin))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request", "currentPin and newPin are required");
        return;
    }
    uint8_t payload[10];
    payload[0] = s_bridge.auth_request_id++;
    if (s_bridge.auth_request_id == 0u) s_bridge.auth_request_id = 1u;
    payload[1] = STM_AUTH_SCOPE_CHANGE;
    for (uint8_t i = 0; i < 4u; ++i)
    {
        payload[2u + i] = (uint8_t)(current_pin[i] - (uint8_t)'0');
        payload[6u + i] = (uint8_t)(new_pin[i] - (uint8_t)'0');
    }
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_AUTH_VALIDATE_PIN,
                            UNER_CMD_AUTH_VALIDATE_PIN, payload, sizeof(payload), 0u);
}

static void dispatch_auth_session_command(uint32_t client_id, const char *request_id,
                                          const char *command, uint8_t action)
{
    WsmSession *session = session_for_client(client_id);
    if (!session)
    {
        send_error(client_id, request_id, "esp", command, "unauthorized", "WebSocket session is not active");
        return;
    }
    if (action == 0u)
    {
        (void)srw_remove_client(&s_bridge.stream_waiters, client_id, session->generation);
        (void)sb_remove_client(&s_bridge.streams, client_id, session->generation);
    }
    (void)start_stm_request(client_id, request_id, command, STM_CMD_AUTH_REMOTE_RESULT,
                            STM_CMD_AUTH_REMOTE_RESULT, &action, 1u, 0u);
}

