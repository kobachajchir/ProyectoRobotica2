// Private bridge implementation; included once by ws_json_bridge.cpp.
static AppConfig current_config_copy()
{
    const AppConfig *cached = s_bridge.backend.config(s_bridge.backend.context);
    return cached ? *cached : AppConfig{};
}

static void clear_json_scan_request()
{
    s_bridge.json_scan_active = 0u;
    s_bridge.json_scan_client_id = 0u;
    s_bridge.json_scan_generation = 0u;
    s_bridge.json_scan_origin_ip_key = 0u;
    s_bridge.json_scan_reconnect_until_ms = 0u;
    s_bridge.json_scan_origin_api_v1 = 0u;
    s_bridge.json_scan_request_id[0] = '\0';
}

static void clear_json_scan_result()
{
    s_bridge.json_scan_result_pending = 0u;
    s_bridge.json_scan_result_timed_out = 0u;
    s_bridge.json_scan_result_failed = 0u;
    s_bridge.json_scan_result_count = 0u;
    s_bridge.json_scan_result_api_v1 = 0u;
    s_bridge.json_scan_result_ip_key = 0u;
    s_bridge.json_scan_result_expires_ms = 0u;
    s_bridge.json_scan_result_request_id[0] = '\0';
}

static bool deadline_reached(uint32_t now, uint32_t deadline)
{
    return deadline != 0u && (int32_t)(now - deadline) >= 0;
}

static void complete_json_scan_driver_cleanup()
{
    s_bridge.wifi.DeleteScanResults();
    s_bridge.wifi.RestoreModeAfterScan();
    (void)s_bridge.backend.release_scan(s_bridge.backend.context, WIFI_SCAN_OWNER_WEB);
    s_bridge.wifi.SetSTAReconnectEnabled(s_bridge.json_scan_previous_auto_reconnect != 0u);
    s_bridge.json_scan_cleanup_pending = 0u;
    s_bridge.json_scan_previous_auto_reconnect = 0u;
}

static void finish_json_scan()
{
    clear_json_scan_request();

    // En ESP8266 scanDelete() libera resultados terminados; no cancela de
    // forma segura un scan asincrono que aun devuelve WIFI_DRIVER_SCAN_RUNNING.
    // Conservamos el owner y el reconnect deshabilitado hasta que el driver
    // termine por si solo para que ningun segundo origen pise su memoria.
    if (s_bridge.wifi.ScanComplete() == WIFI_DRIVER_SCAN_RUNNING)
    {
        s_bridge.json_scan_cleanup_pending = 1u;
        return;
    }

    complete_json_scan_driver_cleanup();
}

static void service_json_scan_driver_cleanup()
{
    if (!s_bridge.json_scan_cleanup_pending) return;
    if (s_bridge.wifi.ScanComplete() == WIFI_DRIVER_SCAN_RUNNING) return;
    complete_json_scan_driver_cleanup();
}

static bool f4_alive_now()
{
    // F4 tolera diez heartbeats WEB perdidos antes de declarar caido el
    // backend. La ESP debe usar la misma ventana: con 1,5 periodos podia
    // revocar un login valido por una unica demora de UART/OLED.
    uint32_t timeout_ms = (uint32_t)s_bridge.f4_alive_period_ms * F4_WEB_ALIVE_MAX_MISSES;
    timeout_ms += F4_WEB_ALIVE_MARGIN_MS;
    if (timeout_ms < 150u) timeout_ms = 150u;
    return s_bridge.f4_alive_seen != 0u &&
           (uint32_t)(s_bridge.platform.now_ms() - s_bridge.last_f4_alive_ms) <= timeout_ms;
}

static uint16_t normalize_f4_alive_period(uint16_t period_ms)
{
    if (period_ms < F4_WEB_ALIVE_MIN_PERIOD_MS)
        return F4_WEB_ALIVE_MIN_PERIOD_MS;
    if (period_ms > F4_WEB_ALIVE_MAX_PERIOD_MS)
        return F4_WEB_ALIVE_MAX_PERIOD_MS;
    return period_ms;
}

static bool stream_broker_has_runtime_state()
{
    for (uint8_t stream = 0u; stream < SB_STREAM_COUNT; ++stream)
    {
        const SbStreamState *state = sb_stream_state(&s_bridge.streams, (SbStream)stream);
        if (state && (state->desired_active || state->applied_active || state->pending))
            return true;
    }
    return false;
}

static void mark_f4_backend_activity(uint32_t received_at_ms)
{
    // Una response/evento F4 valido tambien prueba que el backend esta vivo.
    // Esto evita invalidar en el mismo loop una sesion que acaba de recibir
    // AUTH_VALIDATE_PIN o AUTH_REMOTE_RESULT correctamente.
    s_bridge.last_f4_alive_ms = received_at_ms;
    s_bridge.f4_offline_since_ms = 0u;
    s_bridge.f4_alive_seen = 1u;
    if (!s_bridge.f4_backend_online && stream_broker_has_runtime_state())
        sb_backend_reconnected(&s_bridge.streams);
    s_bridge.f4_backend_online = 1u;
}

static void clear_cleanup_tracking(uint8_t index)
{
    if (index >= WSM_MAX_SESSIONS) return;
    s_bridge.logout_enqueued[index] = 0u;
    s_bridge.logout_retry_at_ms[index] = 0u;
    s_bridge.cleanup_release_at_ms[index] = 0u;
}

static void copy_cstr(char *dst, size_t dst_len, const char *src);
static WsmSession *session_for_client(uint32_t client_id);
static UtmSendResult transaction_send(void *ctx, const UtmRequest *request);
static void transaction_complete(void *ctx, const UtmRequest *request,
                                 UtmCompletionStatus status, const UtmFrame *frame);
static void dispatch_auth_session_command(uint32_t client_id, const char *request_id,
                                          const char *command, uint8_t action);

static const char *screen_source_name(uint8_t source)
{
    switch (source)
    {
    case 0x01: return "MENU";
    case 0x02: return "RENDER";
    case 0x03: return "NOTIFICATION";
    case 0x04: return "PERMISSION";
    case 0x05: return "SYSTEM";
    default: return "UNKNOWN";
    }
}

#if UNER_BRIDGE_DEBUG
static void debug_print_hex_payload(const uint8_t *payload, uint8_t len)
{
    if (!payload || len == 0)
    {
        Serial.print("(empty)");
        return;
    }
    for (uint8_t i = 0; i < len; ++i)
    {
        if (i > 0) Serial.print(' ');
        if (payload[i] < 0x10) Serial.print('0');
        Serial.print(payload[i], HEX);
    }
}

static void debug_log_ws_uner_packet(const char *stage, const UNER_Packet *pkt)
{
    if (!pkt) return;
    Serial.printf("[WS][%s] cmd=0x%02X len=%u src=%u dst=%u payload=",
                  stage ? stage : "UNER", pkt->cmd, pkt->len, pkt->src, pkt->dst);
    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command(pkt->cmd, spec_storage);
    if (spec && spec->sensitive) Serial.print("<redacted>");
    else debug_print_hex_payload(pkt->payload, pkt->len);
    Serial.println();
}

static void debug_log_ws_send(const char *destination, const char *type, const char *event_name, const UNER_Packet *pkt)
{
    Serial.printf("[WS][SEND] dst=%s type=%s event=%s cmd=0x%02X len=%u payload=",
                  destination ? destination : "unknown",
                  type ? type : "unknown",
                  event_name ? event_name : "",
                  pkt ? pkt->cmd : 0,
                  pkt ? pkt->len : 0);
    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = pkt ? lookup_f4_command(pkt->cmd, spec_storage) : nullptr;
    if (spec && spec->sensitive) Serial.print("<redacted>");
    else if (pkt) debug_print_hex_payload(pkt->payload, pkt->len);
    else Serial.print("(null)");
    Serial.println();
}
#else
static void debug_log_ws_uner_packet(const char *stage, const UNER_Packet *pkt)
{
    (void)stage;
    (void)pkt;
}

static void debug_log_ws_send(const char *destination, const char *type, const char *event_name, const UNER_Packet *pkt)
{
    (void)destination;
    (void)type;
    (void)event_name;
    (void)pkt;
}
#endif

static void copy_cstr(char *dst, size_t dst_len, const char *src)
{
    if (!dst || dst_len == 0) return;
    if (!src) src = "";
    size_t n = strlen(src);
    if (n >= dst_len) n = dst_len - 1;
    memcpy(dst, src, n);
    dst[n] = '\0';
}

static WsClientState *find_client(uint32_t id)
{
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (s_bridge.clients[i].used && s_bridge.clients[i].id == id) return &s_bridge.clients[i];
    }
    return nullptr;
}

static WsClientState *ensure_client(uint32_t id)
{
    WsClientState *existing = find_client(id);
    if (existing) return existing;
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used)
        {
            memset(&s_bridge.clients[i], 0, sizeof(s_bridge.clients[i]));
            s_bridge.clients[i].used = 1;
            s_bridge.clients[i].id = id;
            return &s_bridge.clients[i];
        }
    }
    return nullptr;
}

static AsyncWebSocketClient *client_by_id(uint32_t id)
{
    return s_bridge.ws ? s_bridge.ws->client(id) : nullptr;
}

static void send_doc_to_client(uint32_t client_id, JsonDocument &doc)
{
    AsyncWebSocketClient *client = client_by_id(client_id);
    if (!client) return;
    if (client->queueIsFull())
    {
        s_bridge.ws_backpressure_drops++;
        WsClientState *state = find_client(client_id);
        if (state) state->backpressure_drops++;
        return;
    }
    String out;
    serializeJson(doc, out);
    client->text(out);
}

static void send_event_doc_to_client(uint32_t client_id, JsonDocument &doc)
{
    WsClientState *state = find_client(client_id);
    if (!state || !state->preferred_api_v1 || doc["api"].is<int>() ||
        strcmp(doc["type"] | "", "device.event") != 0)
    {
        send_doc_to_client(client_id, doc);
        return;
    }

    DynamicJsonDocument v1_doc(3072);
    JsonObject legacy_payload = doc["payload"].as<JsonObject>();
    const char *event_name = legacy_payload["event"] | "";
    if (!event_name[0]) event_name = doc["event"] | "bridge.event";
    v1_doc["api"] = 1;
    v1_doc["type"] = "event";
    v1_doc["event"] = event_name;
    v1_doc["data"] = legacy_payload["data"];
    if (!v1_doc["data"].is<JsonObject>()) v1_doc.createNestedObject("data");
    JsonObject data = v1_doc["data"].as<JsonObject>();
    const char *origin = legacy_payload["origin"] | "";
    const char *request_id = legacy_payload["requestId"] | "";
    if (origin[0] && data["origin"].isNull()) data["origin"] = origin;
    if (request_id[0] && data["requestId"].isNull()) data["requestId"] = request_id;
    send_doc_to_client(client_id, v1_doc);
}

static void broadcast_doc(JsonDocument &doc)
{
    if (!s_bridge.ws) return;
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used) continue;
        const WsmSession *session = wsm_find_const(&s_bridge.sessions, s_bridge.clients[i].id, s_bridge.clients[i].generation);
        if (!session || !session->stm_authenticated) continue;
        send_event_doc_to_client(s_bridge.clients[i].id, doc);
    }
}

static void broadcast_doc_stm_authorized(JsonDocument &doc)
{
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used) continue;
        const WsmSession *session = wsm_find_const(&s_bridge.sessions, s_bridge.clients[i].id, s_bridge.clients[i].generation);
        if (!session || !session->stm_authenticated) continue;
        send_event_doc_to_client(s_bridge.clients[i].id, doc);
    }
}

static const char *stable_error_code(const char *code)
{
    if (!code || !code[0]) return "uart_error";
    if (strcmp(code, "invalid_request") == 0 || strcmp(code, "unauthorized") == 0 ||
        strcmp(code, "forbidden") == 0 || strcmp(code, "unsupported_by_f4") == 0 ||
        strcmp(code, "busy") == 0 || strcmp(code, "timeout") == 0 ||
        strcmp(code, "uart_error") == 0 || strcmp(code, "f4_nack") == 0 ||
        strcmp(code, "display_io_error") == 0) return code;
    if (strstr(code, "AUTH") != nullptr) return "unauthorized";
    if (strstr(code, "CONTRACT_MISSING") != nullptr ||
        strstr(code, "UNSUPPORTED") != nullptr) return "unsupported_by_f4";
    if (strstr(code, "BUSY") != nullptr || strstr(code, "PENDING") != nullptr) return "busy";
    if (strstr(code, "TIMEOUT") != nullptr) return "timeout";
    if (strstr(code, "STM_REJECT") != nullptr || strstr(code, "NACK") != nullptr) return "f4_nack";
    if (strstr(code, "UNKNOWN") != nullptr || strstr(code, "BAD_PARAMS") != nullptr ||
        strstr(code, "TARGET") != nullptr || strstr(code, "JSON") != nullptr ||
        strstr(code, "NOT_FOUND") != nullptr || strstr(code, "SSID") != nullptr) return "invalid_request";
    return "uart_error";
}

static JsonObject make_response_base_for_api(JsonDocument &doc, uint8_t api_v1,
                                             const char *request_id, const char *target,
                                             const char *command, bool ok,
                                             const char *code, const char *message)
{
    if (api_v1)
    {
        doc["api"] = 1;
        doc["type"] = ok ? "response" : "error";
        doc["requestId"] = request_id ? request_id : "";
        doc["command"] = command ? command : "";
        if (!ok)
        {
            doc["code"] = stable_error_code(code);
            doc["message"] = message ? message : "";
            JsonObject details = doc.createNestedObject("details");
            details["target"] = target ? target : "";
            return details;
        }
        doc["ok"] = true;
        return doc.createNestedObject("data");
    }
    doc["type"] = "device.response";
    JsonObject payload = doc.createNestedObject("payload");
    payload["requestId"] = request_id ? request_id : "";
    payload["target"] = target ? target : "";
    payload["command"] = command ? command : "";
    payload["ok"] = ok;
    payload["code"] = code ? code : "ERR_INTERNAL";
    payload["message"] = message ? message : "";
    return payload.createNestedObject("data");
}

static JsonObject make_response_base(JsonDocument &doc, uint32_t client_id, const char *request_id, const char *target, const char *command,
                                     bool ok, const char *code, const char *message)
{
    WsClientState *state = find_client(client_id);
    return make_response_base_for_api(doc,
                                      state && state->preferred_api_v1 ? 1u : 0u,
                                      request_id, target, command, ok, code, message);
}

static JsonObject make_event_base(JsonDocument &doc, const char *event, const char *origin, const char *request_id)
{
    doc["type"] = "device.event";
    JsonObject payload = doc.createNestedObject("payload");
    payload["event"] = event ? event : "bridge.event";
    payload["origin"] = origin ? origin : "esp";
    if (request_id && request_id[0]) payload["requestId"] = request_id;
    return payload.createNestedObject("data");
}

static void send_response(uint32_t client_id, const char *request_id, const char *target, const char *command,
                          bool ok, const char *code, const char *message)
{
    StaticJsonDocument<512> doc;
    make_response_base(doc, client_id, request_id, target, command, ok, code, message);
    send_doc_to_client(client_id, doc);
}

static void send_error(uint32_t client_id, const char *request_id, const char *target, const char *command,
                       const char *code, const char *message)
{
    send_response(client_id, request_id, target, command, false, code, message);
}

static void send_error_for_api(uint32_t client_id, uint8_t api_v1,
                               const char *request_id, const char *target,
                               const char *command, const char *code,
                               const char *message)
{
    StaticJsonDocument<512> doc;
    make_response_base_for_api(doc, api_v1, request_id, target, command,
                               false, code, message);
    send_doc_to_client(client_id, doc);
}

static void emit_f4_alive_event(uint32_t received_at_ms)
{
    StaticJsonDocument<256> doc;
    doc["api"] = 1;
    doc["type"] = "event";
    doc["event"] = "f4Alive";
    JsonObject data = doc.createNestedObject("data");
    data["receivedAtMs"] = received_at_ms;
    data["periodMs"] = s_bridge.f4_alive_period_ms;
    data["alive"] = true;

    for (uint8_t i = 0u; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used || !s_bridge.clients[i].preferred_api_v1) continue;
        const WsmSession *session = wsm_find_const(&s_bridge.sessions,
                                                   s_bridge.clients[i].id,
                                                   s_bridge.clients[i].generation);
        if (!session || session->state != WSM_SLOT_ACTIVE ||
            !session->stm_authenticated) continue;
        send_doc_to_client(s_bridge.clients[i].id, doc);
    }
}

static void emit_auth_session_invalidated(uint32_t client_id, const char *reason)
{
    WsClientState *state = find_client(client_id);
    if (!state || !state->preferred_api_v1) return;
    StaticJsonDocument<256> doc;
    doc["api"] = 1;
    doc["type"] = "event";
    doc["event"] = "auth.session.expired";
    JsonObject data = doc.createNestedObject("data");
    data["reason"] = reason ? reason : "ttl_expired";
    data["reauthRequired"] = true;
    send_doc_to_client(client_id, doc);
}

static const char *stream_name(SbStream stream)
{
    return stream == SB_STREAM_IR ? "ir" : "mpu";
}

static void deliver_stream_completion(const SrwCompletion *completion)
{
    if (!completion || completion->status == SRW_COMPLETED_CANCELED) return;
    const SrwWaiter *waiter = &completion->waiter;
    const WsmSession *session = wsm_find_const(&s_bridge.sessions,
                                               waiter->client_id,
                                               waiter->generation);
    if (!session || session->state != WSM_SLOT_ACTIVE) return;

    if (completion->status != SRW_COMPLETED_CONVERGED)
    {
        const char *code = "uart_error";
        const char *message = "Stream state could not be applied to STM32";
        if (completion->status == SRW_COMPLETED_TIMEOUT)
        {
            code = "timeout";
            message = "STM32 stream state did not converge before the deadline";
        }
        else if (completion->status == SRW_COMPLETED_F4_NACK)
        {
            code = "f4_nack";
            message = "STM32 rejected the stream state change";
        }
        else if (completion->status == SRW_COMPLETED_BUSY)
        {
            code = "busy";
            message = "UART transaction queue could not accept the stream state change";
        }
        send_error_for_api(waiter->client_id, waiter->api_v1,
                           waiter->request_id, "stm", waiter->command,
                           code, message);
        return;
    }

    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base_for_api(doc, waiter->api_v1,
                                                  waiter->request_id, "stm",
                                                  waiter->command, true, "OK",
                                                  "Stream subscription applied by STM32");
    const SbStreamState *state = sb_stream_state(&s_bridge.streams, waiter->stream);
    data["stream"] = stream_name(waiter->stream);
    data["subscribed"] = waiter->subscribed != 0u;
    data["periodMs"] = waiter->subscribed ? waiter->period_ms : 0u;
    data["physicalActive"] = state ? state->applied_active != 0u : false;
    data["physicalPeriodMs"] = state ? state->applied_period_ms : 0u;
    send_doc_to_client(waiter->client_id, doc);
}

static void complete_stream_waiters(SbStream stream, SrwCompletionStatus status)
{
    SrwCompletion completion;
    while (srw_take_for_stream(&s_bridge.stream_waiters, stream, status, &completion))
        deliver_stream_completion(&completion);
}

static void resolve_stream_waiters_if_converged(SbStream stream)
{
    if (sb_is_converged(&s_bridge.streams, stream))
        complete_stream_waiters(stream, SRW_COMPLETED_CONVERGED);
}

static void service_stream_waiter_timeouts()
{
    SrwCompletion completion;
    uint32_t now = s_bridge.platform.now_ms();
    while (srw_take_expired(&s_bridge.stream_waiters, now, &completion))
        deliver_stream_completion(&completion);
}

static void emit_bridge_error(uint32_t client_id, const char *code, const char *message)
{
    StaticJsonDocument<384> doc;
    WsClientState *state = find_client(client_id);
    if (state && state->preferred_api_v1)
    {
        doc["api"] = 1;
        doc["type"] = "error";
        doc["code"] = stable_error_code(code);
        doc["message"] = message ? message : "";
        doc.createNestedObject("details");
        send_doc_to_client(client_id, doc);
        return;
    }
    JsonObject data = make_event_base(doc, "bridge.error", "esp", nullptr);
    data["code"] = code;
    data["message"] = message;
    send_doc_to_client(client_id, doc);
}

static const char *wifi_mode_string(uint8_t mode)
{
    switch (mode)
    {
    case WIFI_RADIO_OFF: return "OFF";
    case WIFI_RADIO_STA: return "STA";
    case WIFI_RADIO_AP: return "AP";
    case WIFI_RADIO_AP_STA: return "AP_STA";
    default: return "UNKNOWN";
    }
}

static bool ap_provisioning_active()
{
    WifiRadioMode mode = s_bridge.platform.radio_mode();
    bool ap_active = mode == WIFI_RADIO_AP || mode == WIFI_RADIO_AP_STA;
    return ap_active && s_bridge.platform.radio_status() != WIFI_LINK_CONNECTED;
}

static bool is_ap_provisioning_command(const char *command)
{
    if (!command) return false;
    return strcmp(command, "wifi.scan.start") == 0 ||
           strcmp(command, "esp.wifi.getScan") == 0 ||
           strcmp(command, "wifi.scan.stop") == 0 ||
           strcmp(command, "wifi.detail.get") == 0 ||
           strcmp(command, "esp.wifi.getDetail") == 0 ||
           strcmp(command, "wifi.credentials.submit") == 0 ||
           strcmp(command, "esp.wifi.credentials.submit") == 0;
}

static String ip_to_string(WifiIpv4 ip)
{
    char buf[16];
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return String(buf);
}

static void add_wifi_info(JsonObject data, bool prefer_sta)
{
    uint8_t mode = (uint8_t)s_bridge.platform.radio_mode();
    WifiIpv4 ip = prefer_sta && s_bridge.platform.radio_status() == WIFI_LINK_CONNECTED ? s_bridge.platform.sta_ip() : s_bridge.platform.ap_ip();
    const AppConfig *c = s_bridge.backend.config(s_bridge.backend.context);
    data["mode"] = wifi_mode_string(mode);
    data["modeRaw"] = mode;
    data["wlStatus"] = (uint8_t)s_bridge.platform.radio_status();
    data["ip"] = ip_to_string(ip);
    data["staIp"] = ip_to_string(s_bridge.platform.sta_ip());
    data["apIp"] = ip_to_string(s_bridge.platform.ap_ip());
    data["ssid"] = prefer_sta ? (c ? c->staSsid : "") : (c ? c->apSsid : "");
    data["staSsid"] = c ? c->staSsid : "";
    data["apSsid"] = c ? c->apSsid : "";
    data["staConfiguredIp"] = c ? c->staIp : "0.0.0.0";
    data["staGateway"] = c ? c->staGateway : "0.0.0.0";
    data["staSubnet"] = c ? c->staSubnet : "255.255.255.0";
    data["staStaticIpEnabled"] = c && strcmp(c->staIp, "0.0.0.0") != 0;
    data["apConfiguredIp"] = c ? c->apIp : "0.0.0.0";
    data["apStaticIpEnabled"] = c && strcmp(c->apIp, "0.0.0.0") != 0;
    data["configuredBootMode"] =
        c && c->nextBootMode == APP_BOOT_MODE_AP_ONLY ? "ap" : "normal";
    data["apClients"] = s_bridge.platform.ap_clients();
}

static void add_reboot_target_info(JsonObject data, const AppConfig &config,
                                   uint8_t boot_mode)
{
    const bool ap_only = boot_mode == APP_BOOT_MODE_AP_ONLY;
    data["targetMode"] = ap_only ? "AP" : "RED";
    data["targetSsid"] = ap_only ? config.apSsid : config.staSsid;

    if (ap_only)
    {
        const bool default_ip = strcmp(config.apIp, "0.0.0.0") == 0;
        data["targetIp"] = default_ip ? "192.168.4.1" : config.apIp;
        data["targetIpSource"] = default_ip ? "default" : "fixed";
        return;
    }

    const bool dhcp = strcmp(config.staIp, "0.0.0.0") == 0;
    data["targetIp"] = dhcp ? "" : config.staIp;
    data["targetIpSource"] = dhcp ? "dhcp" : "fixed";
}

static bool parse_ipv4(const char *text, WifiIpv4 &out)
{
    if (!text || !text[0]) return false;
    int parts[4] = {-1, -1, -1, -1};
    char tail = '\0';
    if (sscanf(text, "%d.%d.%d.%d%c", &parts[0], &parts[1], &parts[2], &parts[3], &tail) != 4) return false;
    for (uint8_t i = 0; i < 4; ++i)
    {
        if (parts[i] < 0 || parts[i] > 255) return false;
    }
    out = WifiIpv4((uint8_t)parts[0], (uint8_t)parts[1], (uint8_t)parts[2], (uint8_t)parts[3]);
    return true;
}

static uint8_t bounded_strlen_u8(const char *s, uint8_t max_len)
{
    uint8_t n = 0;
    if (!s) return 0;
    while (s[n] && n < max_len) n++;
    return n;
}

static bool parse_bssid(const char *text, uint8_t out[6])
{
    if (!text || !out || strlen(text) != 17u) return false;
    for (uint8_t i = 0u; i < 6u; ++i)
    {
        char hi = text[i * 3u];
        char lo = text[i * 3u + 1u];
        if (i < 5u && text[i * 3u + 2u] != ':') return false;
        auto nibble = [](char c) -> int8_t {
            if (c >= '0' && c <= '9') return (int8_t)(c - '0');
            if (c >= 'a' && c <= 'f') return (int8_t)(c - 'a' + 10);
            if (c >= 'A' && c <= 'F') return (int8_t)(c - 'A' + 10);
            return -1;
        };
        int8_t high = nibble(hi);
        int8_t low = nibble(lo);
        if (high < 0 || low < 0) return false;
        out[i] = (uint8_t)(((uint8_t)high << 4) | (uint8_t)low);
    }
    return true;
}

static bool valid_sta_ip(WifiIpv4 ip)
{
    return ip != WifiIpv4(0, 0, 0, 0) && ip != WifiIpv4(255, 255, 255, 255);
}

static bool parse_u32_text(const char *text, uint32_t *out)
{
    if (!text || !text[0] || !out) return false;

    char *end = nullptr;
    unsigned long value = strtoul(text, &end, 0);
    if (end == text || (end && *end != '\0')) return false;

    *out = (uint32_t)value;
    return true;
}

static bool read_json_u32(JsonObject params, const char *key, uint32_t *out)
{
    if (!out) return false;
    JsonVariant value = params[key];
    if (value.is<uint32_t>())
    {
        *out = value.as<uint32_t>();
        return true;
    }
    if (value.is<unsigned long>())
    {
        *out = (uint32_t)value.as<unsigned long>();
        return true;
    }
    if (value.is<const char *>())
    {
        return parse_u32_text(value.as<const char *>(), out);
    }
    return false;
}

static void write_u32_le(uint32_t value, uint8_t out[4])
{
    if (!out) return;
    out[0] = (uint8_t)(value & 0xFFu);
    out[1] = (uint8_t)((value >> 8) & 0xFFu);
    out[2] = (uint8_t)((value >> 16) & 0xFFu);
    out[3] = (uint8_t)((value >> 24) & 0xFFu);
}

static bool read_pin_ascii4(JsonObject params, uint8_t ascii4[4])
{
    const char *pin = params["pin"] | "";
    if (!pin || bounded_strlen_u8(pin, 5u) != 4u) return false;

    for (uint8_t i = 0; i < 4u; ++i)
    {
        if (pin[i] < '0' || pin[i] > '9') return false;
        ascii4[i] = (uint8_t)pin[i];
    }
    return true;
}

static bool read_pin_key_ascii4(JsonObject params, const char *key, uint8_t ascii4[4])
{
    const char *pin = params[key] | "";
    if (!pin || bounded_strlen_u8(pin, 5u) != 4u) return false;
    for (uint8_t i = 0; i < 4u; ++i)
    {
        if (pin[i] < '0' || pin[i] > '9') return false;
        ascii4[i] = (uint8_t)pin[i];
    }
    return true;
}

static bool read_press_kind(JsonObject params, uint8_t *out)
{
    const char *press_kind = params["pressKind"] | "";
    if (!out) return false;
    if (strcmp(press_kind, "short") == 0)
    {
        *out = STM_PRESS_KIND_SHORT;
        return true;
    }
    if (strcmp(press_kind, "long") == 0)
    {
        *out = STM_PRESS_KIND_LONG;
        return true;
    }
    return false;
}

static bool read_screen_page_direction(JsonObject params, uint8_t *out)
{
    const char *direction = params["direction"] | "";
    if (!out) return false;
    if (strcmp(direction, "up") == 0)
    {
        *out = STM_SCREEN_PAGE_DIR_UP;
        return true;
    }
    if (strcmp(direction, "down") == 0)
    {
        *out = STM_SCREEN_PAGE_DIR_DOWN;
        return true;
    }
    return false;
}

static bool is_manual_status_stm_command(uint8_t cmd)
{
    switch (cmd)
    {
    case STM_CMD_MENU_ITEM_CLICK:
    case STM_CMD_TRIGGER_ENCODER_BUTTON:
    case STM_CMD_TRIGGER_USER_BUTTON:
    case STM_CMD_REQUEST_SCREEN_PAGE:
    case STM_CMD_TRIGGER_ENCODER_ROTATE_LEFT:
    case STM_CMD_TRIGGER_ENCODER_ROTATE_RIGHT:
    case STM_CMD_AUTH_REMOTE_RESULT:
    case STM_CMD_SET_I2C_POLICY:
    case STM_CMD_GET_I2C_POLICY:
    case STM_CMD_SET_MPU_STREAM:
    case STM_CMD_STOP_MPU_STREAM:
        return true;

    default:
        return false;
    }
}

static bool should_refresh_screen_after_status(uint8_t cmd, uint8_t status)
{
    if (status != 0u) return false;

    switch (cmd)
    {
    case STM_CMD_MENU_ITEM_CLICK:
    case STM_CMD_TRIGGER_ENCODER_BUTTON:
    case STM_CMD_TRIGGER_USER_BUTTON:
    case STM_CMD_REQUEST_SCREEN_PAGE:
    case STM_CMD_TRIGGER_ENCODER_ROTATE_LEFT:
    case STM_CMD_TRIGGER_ENCODER_ROTATE_RIGHT:
    case STM_CMD_AUTH_REMOTE_RESULT:
        return true;

    default:
        return false;
    }
}

static void schedule_screen_refresh(uint8_t reason_cmd, const char *request_id)
{
    memset(&s_bridge.auto_screen_refresh, 0, sizeof(s_bridge.auto_screen_refresh));
    s_bridge.auto_screen_refresh.active = 1;
    s_bridge.auto_screen_refresh.reason_cmd = reason_cmd;
    s_bridge.auto_screen_refresh.due_ms = s_bridge.platform.now_ms() + 40u;
    copy_cstr(s_bridge.auto_screen_refresh.request_id, sizeof(s_bridge.auto_screen_refresh.request_id), request_id);
}

static bool resolve_legacy_stm_hex_command(const char *command, uint8_t *stm_cmd, uint8_t *payload_len)
{
    if (!command || !stm_cmd || !payload_len) return false;

    if (strcmp(command, "0x53") == 0)
    {
        *stm_cmd = STM_CMD_MENU_ITEM_CLICK;
        *payload_len = 5u;
        return true;
    }
    if (strcmp(command, "0x54") == 0)
    {
        *stm_cmd = STM_CMD_TRIGGER_ENCODER_BUTTON;
        *payload_len = 5u;
        return true;
    }
    if (strcmp(command, "0x55") == 0)
    {
        *stm_cmd = STM_CMD_TRIGGER_USER_BUTTON;
        *payload_len = 5u;
        return true;
    }
    if (strcmp(command, "0x56") == 0)
    {
        *stm_cmd = STM_CMD_REQUEST_SCREEN_PAGE;
        *payload_len = 5u;
        return true;
    }
    if (strcmp(command, "0x57") == 0)
    {
        *stm_cmd = STM_CMD_TRIGGER_ENCODER_ROTATE_LEFT;
        *payload_len = 4u;
        return true;
    }
    if (strcmp(command, "0x58") == 0)
    {
        *stm_cmd = STM_CMD_TRIGGER_ENCODER_ROTATE_RIGHT;
        *payload_len = 4u;
        return true;
    }
    if (strcmp(command, "0x5B") == 0)
    {
        *stm_cmd = STM_CMD_GET_CAR_MODE;
        *payload_len = 0u;
        return true;
    }
    if (strcmp(command, "0x5E") == 0)
    {
        *stm_cmd = STM_CMD_SET_I2C_POLICY;
        *payload_len = 1u;
        return true;
    }
    if (strcmp(command, "0x5F") == 0)
    {
        *stm_cmd = STM_CMD_GET_I2C_POLICY;
        *payload_len = 0u;
        return true;
    }
    if (strcmp(command, "0x60") == 0)
    {
        *stm_cmd = STM_CMD_GET_MPU_SNAPSHOT;
        *payload_len = 0u;
        return true;
    }
    return false;
}

static bool extract_legacy_stm_payload(JsonObject root_payload, uint8_t *out, uint8_t expected_len, const char **message)
{
    if (!out)
    {
        if (message) *message = "payload buffer is invalid";
        return false;
    }

    if (expected_len == 0)
    {
        return true;
    }

    if (!root_payload["payload"].is<JsonArray>())
    {
        if (message) *message = "payload array is required";
        return false;
    }

    JsonArray payload = root_payload["payload"].as<JsonArray>();
    if (payload.size() != expected_len)
    {
        if (message) *message = "payload length is invalid";
        return false;
    }

    for (uint8_t i = 0; i < expected_len; ++i)
    {
        JsonVariant value = payload[i];
        if (!value.is<int>() && !value.is<unsigned int>())
        {
            if (message) *message = "payload bytes must be integers";
            return false;
        }

        int byte_value = value.as<int>();
        if (byte_value < 0 || byte_value > 255)
        {
            if (message) *message = "payload bytes must be in range 0..255";
            return false;
        }

        out[i] = (uint8_t)byte_value;
    }

    return true;
}

static void add_scan_network_fields(JsonObject obj, const UNER_WifiScanCacheEntry &entry, uint8_t index = 0xFFu)
{
    char bssid[18];
    snprintf(bssid, sizeof(bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
             entry.bssid[0], entry.bssid[1], entry.bssid[2],
             entry.bssid[3], entry.bssid[4], entry.bssid[5]);
    if (index != 0xFFu) obj["index"] = index;
    obj["ssid"] = String(entry.ssid);
    obj["bssid"] = String(bssid);
    obj["signalStrength"] = entry.signal_strength;
    obj["encryptionType"] = entry.encryption_type;
    obj["channel"] = entry.channel;
    obj["rssi"] = entry.signal_strength;
    obj["security"] = entry.encryption_type;
    JsonArray ssid_bytes = obj.createNestedArray("ssidBytes");
    for (uint8_t i = 0; entry.ssid[i] != '\0' && i < SSID_MAX_LEN; ++i)
    {
        ssid_bytes.add((uint8_t)entry.ssid[i]);
    }
}

static void add_scan_network_summary_fields(JsonObject obj,
                                            const UNER_WifiScanCacheEntry &entry,
                                            uint8_t index)
{
    char bssid[18];
    snprintf(bssid, sizeof(bssid), "%02X:%02X:%02X:%02X:%02X:%02X",
             entry.bssid[0], entry.bssid[1], entry.bssid[2],
             entry.bssid[3], entry.bssid[4], entry.bssid[5]);
    obj["index"] = index;
    obj["ssid"] = String(entry.ssid);
    obj["bssid"] = String(bssid);
    obj["signalStrength"] = entry.signal_strength;
    obj["encryptionType"] = entry.encryption_type;
    obj["channel"] = entry.channel;
}

static bool deliver_pending_json_scan_result(uint32_t client_id,
                                             uint8_t api_v1,
                                             bool resumed_after_disconnect)
{
    if (!s_bridge.json_scan_result_pending) return false;

    uint32_t now = s_bridge.platform.now_ms();
    if (deadline_reached(now, s_bridge.json_scan_result_expires_ms))
    {
        clear_json_scan_result();
        return false;
    }

    AsyncWebSocketClient *client = client_by_id(client_id);
    if (!client || client->queueIsFull()) return false;

    DynamicJsonDocument doc(3072);
    JsonObject data;
    if (api_v1)
    {
        doc["api"] = 1;
        doc["type"] = "event";
        doc["event"] = "wifi.scan.results";
        data = doc.createNestedObject("data");
        data["origin"] = "esp";
        data["requestId"] = s_bridge.json_scan_result_request_id;
    }
    else
    {
        data = make_event_base(doc, "wifi.scan.results", "esp",
                               s_bridge.json_scan_result_request_id);
    }

    data["timedOut"] = s_bridge.json_scan_result_timed_out != 0u;
    data["resumedAfterDisconnect"] = resumed_after_disconnect;
    JsonArray networks = data.createNestedArray("networks");
    uint8_t cached_count = WifiScanSnapshot_Count(&s_bridge.web_scan_cache);
    uint8_t returned_count = cached_count > JSON_SCAN_MAX_RETURNED_NETWORKS
                                 ? JSON_SCAN_MAX_RETURNED_NETWORKS
                                 : cached_count;
    for (uint8_t i = 0; i < returned_count; ++i)
    {
        UNER_WifiScanCacheEntry entry;
        if (!WifiScanSnapshot_Get(&s_bridge.web_scan_cache, i, &entry)) continue;
        JsonObject net = networks.createNestedObject();
        add_scan_network_summary_fields(net, entry, i);
    }
    data["status"] = s_bridge.json_scan_result_timed_out
                         ? "timeout"
                         : (s_bridge.json_scan_result_failed ? "failed" : "ok");
    data["count"] = s_bridge.json_scan_result_count;
    data["returnedCount"] = returned_count;
    data["truncated"] = cached_count > returned_count;

    if (doc.overflowed())
    {
        StaticJsonDocument<512> fallback;
        JsonObject fallback_data;
        if (api_v1)
        {
            fallback["api"] = 1;
            fallback["type"] = "event";
            fallback["event"] = "wifi.scan.results";
            fallback_data = fallback.createNestedObject("data");
            fallback_data["origin"] = "esp";
            fallback_data["requestId"] = s_bridge.json_scan_result_request_id;
        }
        else
        {
            fallback_data = make_event_base(fallback, "wifi.scan.results", "esp",
                                            s_bridge.json_scan_result_request_id);
        }
        fallback_data["status"] = "low_memory";
        fallback_data["count"] = s_bridge.json_scan_result_count;
        fallback_data["returnedCount"] = 0;
        fallback_data["truncated"] = s_bridge.json_scan_result_count > 0u;
        fallback_data["timedOut"] = s_bridge.json_scan_result_timed_out != 0u;
        fallback_data["resumedAfterDisconnect"] = resumed_after_disconnect;
        fallback_data.createNestedArray("networks");
        send_doc_to_client(client_id, fallback);
        clear_json_scan_result();
        return true;
    }

    send_doc_to_client(client_id, doc);
    clear_json_scan_result();
    return true;
}

static uint8_t append_wifi_credentials_web_result(uint8_t status, const char *ssid, WifiIpv4 ip, uint8_t *out, uint8_t max_len)
{
    uint8_t ssid_len = bounded_strlen_u8(ssid, SSID_MAX_LEN);
    uint8_t ip_len = status == 0 ? 4u : 0u;
    if (!out || (uint16_t)(2u + ssid_len + ip_len) > max_len) return 0;
    uint8_t i = 0;
    out[i++] = status;
    out[i++] = ssid_len;
    for (uint8_t k = 0; k < ssid_len; ++k) out[i++] = (uint8_t)ssid[k];
    if (status == 0)
    {
        out[i++] = ip[0];
        out[i++] = ip[1];
        out[i++] = ip[2];
        out[i++] = ip[3];
    }
    return i;
}

static JsonObject make_wifi_credentials_event(JsonDocument &doc, const char *event, const char *ssid)
{
    const char *safe_ssid = ssid ? ssid : "";
    doc["type"] = "device.event";
    doc["event"] = event;
    doc["ssid"] = safe_ssid;
    JsonObject payload = doc.createNestedObject("payload");
    payload["event"] = event;
    payload["origin"] = "esp";
    JsonObject data = payload.createNestedObject("data");
    data["ssid"] = safe_ssid;
    return data;
}

static void emit_wifi_credentials_requested(const char *ssid)
{
    StaticJsonDocument<512> doc;
    make_wifi_credentials_event(doc, "wifi.credentials.requested", ssid);
    broadcast_doc(doc);
}

static void emit_wifi_credentials_result(const char *ssid, const char *status, const char *ip, const char *reason)
{
    StaticJsonDocument<640> doc;
    JsonObject data = make_wifi_credentials_event(doc, "wifi.credentials.result", ssid);
    doc["status"] = status ? status : "";
    data["status"] = status ? status : "";
    data["sessionOnly"] = s_bridge.wifi_credentials_web.session_only != 0u;
    data["persisted"] = status && strcmp(status, "success") == 0 && !s_bridge.wifi_credentials_web.session_only;
    if (ip && ip[0])
    {
        doc["ip"] = ip;
        data["ip"] = ip;
    }
    if (reason && reason[0])
    {
        doc["reason"] = reason;
        data["reason"] = reason;
    }
    broadcast_doc(doc);
}

static void emit_ap_fallback_ready(const char *reason)
{
    StaticJsonDocument<896> doc;
    JsonObject data = make_event_base(doc, "wifi.ap.ready", "esp", nullptr);
    add_wifi_info(data, false);
    data["fallback"] = true;
    if (reason && reason[0]) data["reason"] = reason;
    broadcast_doc(doc);
}

static void send_wifi_credentials_result_to_stm(uint8_t status, const char *ssid, WifiIpv4 ip)
{
    uint8_t payload[2 + SSID_MAX_LEN + 4];
    uint8_t len = append_wifi_credentials_web_result(status, ssid, ip, payload, sizeof(payload));
    if (!s_bridge.app || len == 0)
    {
#if UNER_BRIDGE_DEBUG
        Serial.printf("[WS][WIFI_CREDENTIALS] STM result skip status=%u app=%u len=%u\n",
                      status, s_bridge.app ? 1u : 0u, len);
#endif
        return;
    }
    UNER_Status st = s_bridge.backend.send_peer(s_bridge.backend.context, UNER_CMD_WIFI_CREDENTIALS_WEB_RESULT, payload, len);
#if UNER_BRIDGE_DEBUG
    Serial.printf("[WS][WIFI_CREDENTIALS] STM result cmd=0x%02X status=%u ssid='%s' len=%u send_status=%d\n",
                  UNER_CMD_WIFI_CREDENTIALS_WEB_RESULT, status, ssid ? ssid : "", len, (int)st);
#else
    (void)st;
#endif
}

static void restore_previous_sta_connection(const PendingWifiCredentialsWeb &transaction)
{
    const bool keep_ap = (transaction.ap_provisioning != 0u || transaction.session_only) &&
                         (s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA);
    (void)s_bridge.wifi.DisconnectSTA(false, false);
    (void)s_bridge.wifi.ApplySTAConfig(transaction.previous_config);
    if (transaction.previous_config.staSsid[0] != '\0')
    {
        (void)s_bridge.wifi.ConnectSTA(transaction.previous_config.staSsid,
                                   transaction.previous_config.staPass,
                                   keep_ap, true);
    }
    s_bridge.wifi.SetSTAReconnectEnabled(transaction.previous_auto_reconnect != 0u);
}

static void finish_wifi_credentials_flow(uint8_t stm_status, const char *web_status,
                                         const char *reason, WifiIpv4 ip,
                                         bool restore_previous)
{
    char ssid[SSID_MAX_LEN + 1];
    uint8_t notify_stm = s_bridge.wifi_credentials_web.notify_stm;
    const bool exit_ap_after_success =
        stm_status == 0u && s_bridge.wifi_credentials_web.ap_provisioning != 0u;
    const bool report_ap_fallback =
        stm_status != 0u && s_bridge.wifi_credentials_web.ap_provisioning != 0u;
    copy_cstr(ssid, sizeof(ssid), s_bridge.wifi_credentials_web.candidate_config.staSsid);
    String ip_text = stm_status == 0 ? ip_to_string(ip) : String("");
    if (restore_previous) restore_previous_sta_connection(s_bridge.wifi_credentials_web);
    else s_bridge.wifi.SetSTAReconnectEnabled(s_bridge.wifi_credentials_web.previous_auto_reconnect != 0u);
    (void)s_bridge.wifi.ConsumeAPFallbackReady();
    emit_wifi_credentials_result(ssid, web_status, ip_text.c_str(), reason);
    if (notify_stm)
    {
        send_wifi_credentials_result_to_stm(stm_status, ssid, ip);
    }
    if (report_ap_fallback &&
        (s_bridge.platform.radio_mode() == WIFI_RADIO_AP || s_bridge.platform.radio_mode() == WIFI_RADIO_AP_STA))
    {
        emit_ap_fallback_ready(reason ? reason : web_status);
        s_bridge.backend.notify_ap_fallback(s_bridge.backend.context);
    }
    if (exit_ap_after_success)
    {
        s_bridge.ap_provisioning_exit.active = 1u;
        s_bridge.ap_provisioning_exit.stop_at_ms = s_bridge.platform.now_ms() + AP_PROVISIONING_EXIT_DELAY_MS;
    }
    memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
}

void WSJsonBridge_RequestWifiCredentials(const char *ssid)
{
    // En AP-only la Web ya esta dentro del canal de aprovisionamiento. Una
    // solicitud automatica del dashboard F4 no debe abrir un modal global ni
    // competir con el formulario explicito de seleccion de red.
    if (ap_provisioning_active() || s_bridge.debug_wifi.active) return;

    memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
    s_bridge.wifi_credentials_web.requested = 1;
    s_bridge.wifi_credentials_web.notify_stm = 1;
    copy_cstr(s_bridge.wifi_credentials_web.candidate_config.staSsid,
              sizeof(s_bridge.wifi_credentials_web.candidate_config.staSsid), ssid);
#if UNER_BRIDGE_DEBUG
    Serial.printf("[WS][WIFI_CREDENTIALS] requested ssid='%s'\n",
                  s_bridge.wifi_credentials_web.candidate_config.staSsid);
#endif
    emit_wifi_credentials_requested(s_bridge.wifi_credentials_web.candidate_config.staSsid);
}

uint8_t WSJsonBridge_GetWifiCredentialsState(uint8_t *flags,
                                             char *sta_ssid,
                                             size_t sta_ssid_capacity,
                                             char *ap_ssid,
                                             size_t ap_ssid_capacity)
{
    if (!flags || !sta_ssid || sta_ssid_capacity == 0u ||
        !ap_ssid || ap_ssid_capacity == 0u) return 1u;

    const AppConfig config = current_config_copy();
    uint8_t value = 0u;
    if (config.staSsid[0] != '\0') value |= 0x01u;
    if (config.apSsid[0] != '\0') value |= 0x02u;
    if (s_bridge.wifi_credentials_web.requested) value |= 0x04u;
    *flags = value;
    copy_cstr(sta_ssid, sta_ssid_capacity,
              s_bridge.wifi_credentials_web.requested
                  ? s_bridge.wifi_credentials_web.candidate_config.staSsid
                  : config.staSsid);
    copy_cstr(ap_ssid, ap_ssid_capacity, config.apSsid);
    return 0u;
}

uint8_t WSJsonBridge_SubmitWifiCredentials(const char *ssid,
                                           const char *password,
                                           bool reuse_password,
                                           const char *ip,
                                           const char *gateway,
                                           const char *subnet,
                                           const char *bssid)
{
    const AppConfig previous = current_config_copy();
    const char *safe_ssid = ssid ? ssid : "";
    const char *safe_password = password ? password : "";
    const uint8_t ssid_len = bounded_strlen_u8(safe_ssid, SSID_MAX_LEN + 1u);
    if (ssid_len == 0u || ssid_len > SSID_MAX_LEN) return 1u;
    if (s_bridge.wifi_credentials_web.requested && s_bridge.wifi_credentials_web.connecting) return 2u;
    if (s_bridge.wifi_credentials_web.requested &&
        strcmp(safe_ssid, s_bridge.wifi_credentials_web.candidate_config.staSsid) != 0) return 3u;
    if (reuse_password && strcmp(safe_ssid, previous.staSsid) != 0) return 3u;

    const char *effective_password = reuse_password ? previous.staPass : safe_password;
    if (bounded_strlen_u8(effective_password, PASS_MAX_LEN + 1u) > PASS_MAX_LEN) return 1u;

    const char *effective_ip = (ip && ip[0]) ? ip : previous.staIp;
    const char *effective_gateway = (gateway && gateway[0]) ? gateway : previous.staGateway;
    const char *effective_subnet = (subnet && subnet[0]) ? subnet : previous.staSubnet;
    WifiIpv4 fixed_ip;
    WifiIpv4 gateway_ip;
    WifiIpv4 subnet_ip;
    if (!parse_ipv4(effective_ip, fixed_ip) ||
        !parse_ipv4(effective_gateway, gateway_ip) ||
        !parse_ipv4(effective_subnet, subnet_ip) ||
        (fixed_ip != WifiIpv4(0, 0, 0, 0) &&
         (gateway_ip == WifiIpv4(0, 0, 0, 0) || subnet_ip == WifiIpv4(0, 0, 0, 0)))) {
        return 1u;
    }

    AppConfig candidate = previous;
    copy_cstr(candidate.staSsid, sizeof(candidate.staSsid), safe_ssid);
    copy_cstr(candidate.staPass, sizeof(candidate.staPass), effective_password);
    copy_cstr(candidate.staIp, sizeof(candidate.staIp), effective_ip);
    copy_cstr(candidate.staGateway, sizeof(candidate.staGateway), effective_gateway);
    copy_cstr(candidate.staSubnet, sizeof(candidate.staSubnet), effective_subnet);
    const bool provisioning = ap_provisioning_active();
    if (provisioning) candidate.nextBootMode = APP_BOOT_MODE_NORMAL;
    if (!s_bridge.wifi.ApplySTAConfig(candidate)) {
        (void)s_bridge.wifi.ApplySTAConfig(previous);
        return 4u;
    }

    memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
    s_bridge.wifi_credentials_web.requested = 1u;
    s_bridge.wifi_credentials_web.connecting = 1u;
    s_bridge.wifi_credentials_web.notify_stm = 1u;
    s_bridge.wifi_credentials_web.previous_auto_reconnect =
        s_bridge.wifi.IsSTAReconnectEnabled() ? 1u : 0u;
    s_bridge.wifi_credentials_web.ap_provisioning = provisioning ? 1u : 0u;
    s_bridge.wifi_credentials_web.previous_config = previous;
    s_bridge.wifi_credentials_web.candidate_config = candidate;
    copy_cstr(s_bridge.wifi_credentials_web.request_id,
              sizeof(s_bridge.wifi_credentials_web.request_id), "qt-usb");
    if (bssid && bssid[0]) {
        s_bridge.wifi_credentials_web.has_bssid = parse_bssid(bssid, s_bridge.wifi_credentials_web.bssid) ? 1u : 0u;
        if (!s_bridge.wifi_credentials_web.has_bssid) {
            (void)s_bridge.wifi.ApplySTAConfig(previous);
            memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
            return 1u;
        }
    }
    s_bridge.wifi_credentials_web.started_ms = s_bridge.platform.now_ms();
    s_bridge.wifi_credentials_web.deadline_ms =
        s_bridge.wifi_credentials_web.started_ms + WIFI_CREDENTIALS_CONNECT_TIMEOUT_MS;

    s_bridge.wifi.SetSTAReconnectEnabled(false);
    const WifiLinkStatus begin_status = s_bridge.wifi.ConnectSTA(
        candidate.staSsid, candidate.staPass, provisioning, true,
        s_bridge.wifi_credentials_web.has_bssid ? s_bridge.wifi_credentials_web.bssid : nullptr);
    if (begin_status == WIFI_LINK_CONNECT_FAILED) {
        finish_wifi_credentials_flow(1u, "failed", "auth_failed",
                                     WifiIpv4(0, 0, 0, 0), true);
        return 5u;
    }
    return 0u;
}

uint8_t WSJsonBridge_CancelWifiCredentials(const char *ssid)
{
    const char *safe_ssid = ssid ? ssid : "";
    if (!s_bridge.wifi_credentials_web.requested) return 6u;
    if (safe_ssid[0] == '\0' ||
        strcmp(safe_ssid, s_bridge.wifi_credentials_web.candidate_config.staSsid) != 0) return 3u;
    finish_wifi_credentials_flow(3u, "cancelled", nullptr,
                                 WifiIpv4(0, 0, 0, 0), true);
    return 0u;
}

