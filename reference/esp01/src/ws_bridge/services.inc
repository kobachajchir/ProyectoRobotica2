// Private bridge implementation; included once by ws_json_bridge.cpp.
static void service_json_scan()
{
    service_json_scan_driver_cleanup();
    uint32_t now = s_bridge.platform.now_ms();

    if (s_bridge.json_scan_result_pending &&
        deadline_reached(now, s_bridge.json_scan_result_expires_ms))
    {
        clear_json_scan_result();
    }
    if (!s_bridge.json_scan_active) return;

    WsClientState *origin = find_client(s_bridge.json_scan_client_id);
    const WsmSession *origin_session =
        origin && origin->generation == s_bridge.json_scan_generation
            ? wsm_find_const(&s_bridge.sessions, origin->id, origin->generation)
            : nullptr;
    bool origin_ready = origin_session &&
        (origin_session->stm_authenticated || origin_session->esp_debug_wifi || ap_provisioning_active());
    bool waiting_for_reconnect = s_bridge.json_scan_client_id == 0u &&
        s_bridge.json_scan_origin_ip_key != 0u &&
        !deadline_reached(now, s_bridge.json_scan_reconnect_until_ms);
    if (!origin_ready && !waiting_for_reconnect)
    {
        finish_json_scan();
        return;
    }
    int n = s_bridge.wifi.ScanComplete();
    bool timed_out = (uint32_t)(s_bridge.platform.now_ms() - s_bridge.json_scan_started_ms) >= JSON_SCAN_TIMEOUT_MS;
    if ((uint32_t)(now - s_bridge.json_scan_last_log_ms) >= 500u)
    {
        WS_SCAN_LOG("[WS][SCAN] poll scanComplete=%d active=%u elapsed=%lu timeout=%u client=%lu\n",
                    n, s_bridge.json_scan_active, (unsigned long)(now - s_bridge.json_scan_started_ms),
                    timed_out ? 1u : 0u, (unsigned long)s_bridge.json_scan_client_id);
        s_bridge.json_scan_last_log_ms = now;
    }
    if (n == WIFI_DRIVER_SCAN_RUNNING && !timed_out) return;

    uint8_t completed_count = (uint8_t)((n > 255) ? 255 : (n < 0 ? 0 : n));
    WS_SCAN_LOG("[WS][SCAN] completed scanComplete=%d timedOut=%u count=%u request='%s'\n",
                n, timed_out ? 1u : 0u, completed_count, s_bridge.json_scan_request_id);
    // El driver es compartido, pero su snapshot no. El cache STM debe seguir
    // disponible para GET_SCAN_RESULTS/GET_WIFI_DETAIL aunque una busqueda Web
    // se ejecute despues de liberar el arbiter.
    s_bridge.backend.scan_snapshot(s_bridge.backend.context, &s_bridge.web_scan_cache, n);

    // Copiar primero el snapshot y liberar los resultados nativos antes de
    // construir/serializar JSON. El driver y ArduinoJson compiten por el heap
    // reducido de la ESP-01, especialmente cuando hay muchas redes visibles.
    const uint32_t completed_client_id = origin_ready ? origin->id : 0u;
    const uint8_t completed_api_v1 = origin_ready
        ? origin->preferred_api_v1
        : s_bridge.json_scan_origin_api_v1;
    s_bridge.json_scan_result_pending = 1u;
    s_bridge.json_scan_result_timed_out = timed_out ? 1u : 0u;
    s_bridge.json_scan_result_failed = (!timed_out && n < 0) ? 1u : 0u;
    s_bridge.json_scan_result_count = completed_count;
    s_bridge.json_scan_result_api_v1 = completed_api_v1;
    s_bridge.json_scan_result_ip_key = s_bridge.json_scan_origin_ip_key;
    s_bridge.json_scan_result_expires_ms = s_bridge.json_scan_reconnect_until_ms != 0u
        ? s_bridge.json_scan_reconnect_until_ms
        : now + JSON_SCAN_RECONNECT_GRACE_MS;
    copy_cstr(s_bridge.json_scan_result_request_id,
              sizeof(s_bridge.json_scan_result_request_id),
              s_bridge.json_scan_request_id);
    finish_json_scan();

    if (completed_client_id != 0u)
    {
        (void)deliver_pending_json_scan_result(completed_client_id,
                                               completed_api_v1,
                                               false);
    }
}

static void service_wifi_credentials_flow()
{
    if (!s_bridge.wifi_credentials_web.requested || !s_bridge.wifi_credentials_web.connecting) return;

    uint8_t sta = (uint8_t)s_bridge.platform.radio_status();
    if (sta == WIFI_LINK_CONNECTED)
    {
        String connected_ssid = s_bridge.platform.sta_ssid();
        if (!connected_ssid.equals(s_bridge.wifi_credentials_web.candidate_config.staSsid))
        {
            return;
        }
        WifiIpv4 ip = s_bridge.platform.sta_ip();
        if (!valid_sta_ip(ip)) return;
#if UNER_BRIDGE_DEBUG
        Serial.printf("[WS][WIFI_CREDENTIALS] connected ssid='%s' ip=%u.%u.%u.%u\n",
                      s_bridge.wifi_credentials_web.candidate_config.staSsid,
                      ip[0], ip[1], ip[2], ip[3]);
#endif
        // The STA candidate predates an asynchronous F4 preference update.
        // Commit credentials without rolling back the latest fallback policy.
        const AppConfig *current = s_bridge.backend.config(s_bridge.backend.context);
        if (current) s_bridge.wifi_credentials_web.candidate_config.apFallbackEnabled = current->apFallbackEnabled;
        if (!s_bridge.wifi_credentials_web.session_only && !s_bridge.backend.save_config(s_bridge.backend.context, s_bridge.wifi_credentials_web.candidate_config))
        {
            finish_wifi_credentials_flow(1, "failed", "save_failed", ip, true);
            return;
        }
        if (s_bridge.wifi_credentials_web.session_only) s_bridge.debug_wifi.config = s_bridge.wifi_credentials_web.candidate_config;
        finish_wifi_credentials_flow(0, "success", nullptr, ip, false);
        return;
    }

    if (sta == WIFI_LINK_CONNECT_FAILED)
    {
#if UNER_BRIDGE_DEBUG
        Serial.printf("[WS][WIFI_CREDENTIALS] auth failed ssid='%s' wl_status=%u\n",
                      s_bridge.wifi_credentials_web.candidate_config.staSsid, sta);
#endif
        finish_wifi_credentials_flow(1, "failed", "auth_failed", WifiIpv4(0, 0, 0, 0), true);
        return;
    }

    if ((uint32_t)(s_bridge.platform.now_ms() - s_bridge.wifi_credentials_web.deadline_ms) < 0x80000000UL)
    {
#if UNER_BRIDGE_DEBUG
        Serial.printf("[WS][WIFI_CREDENTIALS] timeout ssid='%s' wl_status=%u\n",
                      s_bridge.wifi_credentials_web.candidate_config.staSsid, sta);
#endif
        finish_wifi_credentials_flow(2, "timeout", nullptr, WifiIpv4(0, 0, 0, 0), true);
    }
}

static void service_ap_provisioning_exit()
{
    if (!s_bridge.ap_provisioning_exit.active ||
        (int32_t)(s_bridge.platform.now_ms() - s_bridge.ap_provisioning_exit.stop_at_ms) < 0)
        return;

    s_bridge.ap_provisioning_exit.active = 0u;
    if (s_bridge.platform.radio_status() == WIFI_LINK_CONNECTED)
        (void)s_bridge.wifi.StopAP(false);
}

static void publish_ap_credentials_result(const PendingApCredentialsWeb &transaction,
                                          bool success, const char *reason)
{
    memset(&s_bridge.ap_credentials_result, 0, sizeof(s_bridge.ap_credentials_result));
    WifiScanSnapshot_Init(&s_bridge.web_scan_cache);
    s_bridge.ap_credentials_result.available = 1u;
    s_bridge.ap_credentials_result.success = success ? 1u : 0u;
    s_bridge.ap_credentials_result.completed_ms = s_bridge.platform.now_ms();
    copy_cstr(s_bridge.ap_credentials_result.request_id,
              sizeof(s_bridge.ap_credentials_result.request_id), transaction.request_id);
    copy_cstr(s_bridge.ap_credentials_result.ssid,
              sizeof(s_bridge.ap_credentials_result.ssid), transaction.candidate_config.apSsid);
    copy_cstr(s_bridge.ap_credentials_result.status,
              sizeof(s_bridge.ap_credentials_result.status), success ? "success" : "failed");
    copy_cstr(s_bridge.ap_credentials_result.reason,
              sizeof(s_bridge.ap_credentials_result.reason), reason ? reason : "");
    String configured_ip = ip_to_string(transaction.candidate_ip);
    copy_cstr(s_bridge.ap_credentials_result.configured_ip,
              sizeof(s_bridge.ap_credentials_result.configured_ip), configured_ip.c_str());

    StaticJsonDocument<640> doc;
    JsonObject data = make_event_base(doc, "wifi.ap.credentials.result", "esp",
                                      transaction.request_id);
    data["success"] = success;
    data["sessionOnly"] = transaction.session_only != 0u;
    data["persisted"] = success && !transaction.session_only;
    data["status"] = success ? "success" : "failed";
    data["ssid"] = transaction.candidate_config.apSsid;
    data["configuredIp"] = configured_ip;
    if (reason && reason[0]) data["reason"] = reason;
    broadcast_doc(doc);
}

static void restore_previous_ap(const PendingApCredentialsWeb &transaction)
{
    (void)s_bridge.wifi.ApplyAPConfig(transaction.previous_config);
    (void)s_bridge.wifi.StartAP(transaction.previous_config.apSsid,
                            transaction.previous_config.apPass,
                            transaction.keep_sta != 0u);
}

static void service_ap_credentials_flow()
{
    if (!s_bridge.ap_credentials_web.session_only || !s_bridge.debug_wifi.active) return;
    if (!s_bridge.ap_credentials_web.active ||
        (int32_t)(s_bridge.platform.now_ms() - s_bridge.ap_credentials_web.apply_at_ms) < 0)
        return;

    PendingApCredentialsWeb transaction = s_bridge.ap_credentials_web;
    bool configured = !transaction.apply_radio || s_bridge.wifi.ApplyAPConfig(transaction.candidate_config);
    bool started = !transaction.apply_radio || (configured &&
                   s_bridge.wifi.StartAP(transaction.candidate_config.apSsid,
                                     transaction.candidate_config.apPass,
                                     transaction.keep_sta != 0u, 1u, false, 4u, transaction.candidate_config.apProtected != 0u));
    WifiRadioMode applied_mode = s_bridge.wifi.GetMode();
    bool active = !transaction.apply_radio || (started && (applied_mode == WIFI_RADIO_AP || applied_mode == WIFI_RADIO_AP_STA));

    if (!active)
    {
        restore_previous_ap(transaction);
        publish_ap_credentials_result(transaction, false, "start_failed");
    }
    else if (!transaction.session_only && !s_bridge.backend.save_config(s_bridge.backend.context, transaction.candidate_config))
    {
        restore_previous_ap(transaction);
        publish_ap_credentials_result(transaction, false, "save_failed");
    }
    else
    {
        if (transaction.session_only) s_bridge.debug_wifi.config = transaction.candidate_config;
        publish_ap_credentials_result(transaction, true, nullptr);
    }

    memset(&transaction, 0, sizeof(transaction));
    memset(&s_bridge.ap_credentials_web, 0, sizeof(s_bridge.ap_credentials_web));
}

static void service_wifi_events()
{
    uint8_t mode = (uint8_t)s_bridge.platform.radio_mode();
    uint8_t sta = (uint8_t)s_bridge.platform.radio_status();
    uint8_t ap_clients = s_bridge.platform.ap_clients();
    if (s_bridge.last_wifi_mode == 0xFF)
    {
        s_bridge.last_wifi_mode = mode;
        s_bridge.last_sta_status = sta;
        s_bridge.last_ap_clients = ap_clients;
        return;
    }
    if (mode != s_bridge.last_wifi_mode)
    {
        StaticJsonDocument<896> doc;
        JsonObject data = make_event_base(doc, "wifi.mode.changed", "esp", nullptr);
        add_wifi_info(data, true);
        broadcast_doc(doc);
        s_bridge.last_wifi_mode = mode;
    }
    if (sta != s_bridge.last_sta_status)
    {
        StaticJsonDocument<896> doc;
        JsonObject data = make_event_base(doc, sta == WIFI_LINK_CONNECTED ? "wifi.sta.connected" : "wifi.sta.statusChanged", "esp", nullptr);
        add_wifi_info(data, true);
        broadcast_doc(doc);
        s_bridge.last_sta_status = sta;
    }
    if (ap_clients != s_bridge.last_ap_clients)
    {
        StaticJsonDocument<384> doc;
        JsonObject data = make_event_base(doc, ap_clients > s_bridge.last_ap_clients ? "wifi.ap.clientJoined" : "wifi.ap.clientLeft", "esp", nullptr);
        data["apClients"] = ap_clients;
        broadcast_doc(doc);
        s_bridge.last_ap_clients = ap_clients;
    }
}

static void service_ap_fallback_ready()
{
    // La transaccion Web publica un motivo mas preciso al finalizar. Para
    // intentos iniciados por UART/F4, el latch de WifiUtils garantiza que un
    // AP que nunca se detuvo igual se republica tras el timeout STA.
    if (s_bridge.wifi_credentials_web.requested && s_bridge.wifi_credentials_web.connecting)
        return;
    if (!s_bridge.wifi.ConsumeAPFallbackReady()) return;

    emit_ap_fallback_ready("sta_unavailable");
    if (!s_bridge.debug_wifi.active) s_bridge.backend.notify_ap_fallback(s_bridge.backend.context);
}

static void service_ap_client_disconnect()
{
    if (!s_bridge.ap_client_disconnect.active ||
        (int32_t)(s_bridge.platform.now_ms() - s_bridge.ap_client_disconnect.execute_at_ms) < 0)
        return;

    uint8_t mac[6];
    memcpy(mac, s_bridge.ap_client_disconnect.mac, sizeof(mac));
    memset(&s_bridge.ap_client_disconnect, 0, sizeof(s_bridge.ap_client_disconnect));
    (void)s_bridge.platform.station_deauth(mac);
}

static void service_auto_screen_refresh()
{
    if (!s_bridge.auto_screen_refresh.active) return;
    if ((uint32_t)(s_bridge.platform.now_ms() - s_bridge.auto_screen_refresh.due_ms) >= 0x80000000UL) return;
    // La Web recibe 0x95/0x96 espontaneos. No se inyecta aqui un segundo
    // request desde el nodo de control porque toda operacion MCU-bound debe
    // pasar por la cola y por la sesion Web que la origino.
    s_bridge.auto_screen_refresh.active = 0;
}

static void service_stm_pending()
{
    if (s_bridge.telem_stop.active && (uint32_t)(s_bridge.platform.now_ms() - s_bridge.telem_stop.deadline_ms) < 0x80000000UL)
    {
        WsClientState *state = find_client(s_bridge.telem_stop.client_id);
        if (state) (void)sb_unsubscribe(&s_bridge.streams, state->id, state->generation, SB_STREAM_MPU);
        s_bridge.telem_stop.active = 0;
    }
}

static UtmEnqueueResult enqueue_internal(uint8_t node, uint32_t client_id, uint32_t generation,
                                         uint8_t command_id, const uint8_t *payload, uint8_t payload_len,
                                         uint8_t internal_kind, const char *name)
{
    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command(command_id, spec_storage);
    if (!spec || !F4_CommandRegistry_IsWireAllowed(spec) ||
        !F4_CommandRegistry_IsRequestLengthValid(spec, payload_len)) return UTM_ENQUEUE_INVALID;
    UtmRequest request;
    utm_request_init(&request);
    request.client_id = client_id;
    request.generation = generation;
    request.node = node;
    request.dst_node = UNER_NODE_MCU;
    request.command_id = command_id;
    request.owner = transaction_owner(spec->owner);
    request.response_policy = transaction_response_policy(spec->response_policy);
    request.payload_len = payload_len;
    if (payload_len > 0u && payload) memcpy(request.payload, payload, payload_len);
    copy_cstr(request.command_name, sizeof(request.command_name), name);
    request.internal_kind = internal_kind;
    request.timeout_ms = spec->timeout_ms ? spec->timeout_ms : 100u;
    return utm_enqueue(&s_bridge.transactions, &request, s_bridge.platform.now_ms());
}

static void service_stream_broker()
{
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_IDLE &&
        s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_UPLOADING) return;
    for (uint8_t stream_value = 0u; stream_value < SB_STREAM_COUNT; ++stream_value)
    {
        SbStream stream = (SbStream)stream_value;
        resolve_stream_waiters_if_converged(stream);
        SbAction action = sb_take_action(&s_bridge.streams, stream);
        if (action.type == SB_ACTION_NONE) continue;
        uint8_t payload[3];
        uint8_t payload_len = 0u;
        uint8_t command_id;
        if (action.type == SB_ACTION_SET)
        {
            command_id = stream == SB_STREAM_MPU ? 0x61u : 0x6Bu;
            payload[0] = 1u;
            payload[1] = (uint8_t)(action.period_ms & 0xFFu);
            payload[2] = (uint8_t)(action.period_ms >> 8);
            payload_len = 3u;
        }
        else
        {
            command_id = stream == SB_STREAM_MPU ? 0x62u : 0x6Cu;
        }
        uint8_t internal_kind = stream == SB_STREAM_MPU ? UTM_INTERNAL_STREAM_MPU : UTM_INTERNAL_STREAM_IR;
        if (action.reconcile) internal_kind = UTM_INTERNAL_STREAM_RECONCILE;
        UtmEnqueueResult result = enqueue_internal(action.endpoint.node,
                                                   action.endpoint.client_id,
                                                   action.endpoint.generation,
                                                   command_id, payload, payload_len,
                                                   internal_kind,
                                                   stream == SB_STREAM_MPU ? "stream.mpu.reconcile" : "stream.ir.reconcile");
        if (result != UTM_ENQUEUE_OK)
        {
            SbResult completion_result = sb_complete_action(&s_bridge.streams, &action, 0u);
            if (completion_result != SB_STALE_ACTION)
            {
                SrwCompletionStatus waiter_status =
                    (result == UTM_ENQUEUE_QUEUE_FULL ||
                     result == UTM_ENQUEUE_CLIENT_LIMIT)
                        ? SRW_COMPLETED_BUSY : SRW_COMPLETED_UART_ERROR;
                complete_stream_waiters(stream, waiter_status);
            }
        }
    }
}

static void service_session_cleanup()
{
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_IDLE &&
        s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_UPLOADING) return;
    uint32_t now = s_bridge.platform.now_ms();
    for (uint8_t i = 0; i < WSM_MAX_SESSIONS; ++i)
    {
        WsmSession *session = &s_bridge.sessions.sessions[i];
        if (session->state != WSM_SLOT_CLEANUP) continue;
        if (session->resume_until_ms != 0u &&
            (int32_t)(now - session->resume_until_ms) < 0)
            continue;
        session->resume_until_ms = 0u;
        if (session->esp_debug_wifi || session->esp_debug_used)
        {
            const uint32_t client_id = session->client_id;
            const uint32_t generation = session->generation;
            if (wsm_release(&s_bridge.sessions, client_id, generation) == WSM_OK)
                clear_cleanup_tracking(i);
            continue;
        }
        if (s_bridge.cleanup_release_at_ms[i] == 0u)
            s_bridge.cleanup_release_at_ms[i] = now + STM_CLEANUP_QUARANTINE_MS;
        if (!s_bridge.logout_enqueued[i] &&
            (int32_t)(now - s_bridge.cleanup_release_at_ms[i]) >= 0)
        {
            uint32_t client_id = session->client_id;
            uint32_t generation = session->generation;
            if (wsm_release(&s_bridge.sessions, client_id, generation) == WSM_OK)
                clear_cleanup_tracking(i);
            continue;
        }
        if (s_bridge.logout_enqueued[i] ||
            (s_bridge.logout_retry_at_ms[i] != 0u &&
             (int32_t)(now - s_bridge.logout_retry_at_ms[i]) < 0)) continue;
        uint8_t action = 0u;
        UtmEnqueueResult result = enqueue_internal(session->node, session->client_id, session->generation,
                                                   STM_CMD_AUTH_REMOTE_RESULT, &action, 1u,
                                                   UTM_INTERNAL_AUTH_CLEANUP, "auth.disconnect.logout");
        if (result == UTM_ENQUEUE_OK)
        {
            s_bridge.logout_enqueued[i] = 1u;
        }
        else
        {
            s_bridge.logout_retry_at_ms[i] = now + STM_LOGOUT_RETRY_MS;
        }
    }
}

static void service_session_expiry()
{
    uint32_t now = s_bridge.platform.now_ms();
    for (uint8_t i = 0; i < WSM_MAX_SESSIONS; ++i)
    {
        WsmSession *session = &s_bridge.sessions.sessions[i];
        if (session->state == WSM_SLOT_ACTIVE && session->stm_authenticated &&
            (int32_t)(now - session->stm_expires_at_ms) >= 0)
        {
            emit_auth_session_invalidated(session->client_id, "ttl_expired");
            if (s_bridge.json_scan_active &&
                s_bridge.json_scan_client_id == session->client_id &&
                s_bridge.json_scan_generation == session->generation)
            {
                finish_json_scan();
            }
            (void)srw_remove_client(&s_bridge.stream_waiters, session->client_id, session->generation);
            (void)sb_remove_client(&s_bridge.streams, session->client_id, session->generation);
        }
    }
    (void)wsm_service(&s_bridge.sessions, now);
}

static void service_f4_alive_timeout()
{
    if (!s_bridge.f4_backend_online) return;
    if (f4_alive_now())
    {
        s_bridge.f4_offline_since_ms = 0u;
        return;
    }

    uint32_t now = s_bridge.platform.now_ms();
    if (s_bridge.f4_offline_since_ms == 0u)
    {
        // Cargar assets, asociar otra estacion al SoftAP o barrer canales
        // puede demorar temporalmente el procesamiento sin que F4 haya
        // perdido sus sesiones. Se confirma una caida sostenida antes de
        // revocar autoridad global; F4 sigue validando cada comando.
        s_bridge.f4_offline_since_ms = now == 0u ? 1u : now;
        return;
    }
    if ((uint32_t)(now - s_bridge.f4_offline_since_ms) < F4_BACKEND_OFFLINE_CONFIRM_MS)
        return;

    bool had_stream_state = stream_broker_has_runtime_state();
    s_bridge.f4_backend_online = 0u;
    s_bridge.f4_offline_since_ms = 0u;
    oled_canvas_backend_lost();
    utm_cancel_all(&s_bridge.transactions);

    // F4 elimina las sesiones Web cuando pierde el backend alive. Reflejarlo
    // localmente evita conservar auth/streams validos solo en RAM del ESP.
    revoke_all_stm_sessions_and_streams("f4_offline");
    if (had_stream_state) sb_backend_reconnected(&s_bridge.streams);
}

