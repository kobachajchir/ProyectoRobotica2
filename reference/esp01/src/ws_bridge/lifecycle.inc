// Private bridge implementation; included once by ws_json_bridge.cpp.
void WSJsonBridge_InitWithPorts(AsyncWebSocket *socket,
    const BridgeBackendOps &backend, const BridgeWifiPort &wifi,
    const BridgePlatformOps &platform)
{
    s_bridge.ws = socket;
    s_bridge.app = backend.context;
    s_bridge.backend = backend;
    s_bridge.wifi = wifi;
    s_bridge.platform = platform;
    if (s_bridge.app) s_bridge.backend.intercept_packets(s_bridge.backend.context, WSJsonBridge_OnUNERPacket, nullptr);
    wsm_init(&s_bridge.sessions, 60000u, 5u);
    sb_init(&s_bridge.streams);
    srw_init(&s_bridge.stream_waiters);
    utm_init(&s_bridge.transactions, transaction_send, nullptr, transaction_complete, nullptr);
    memset(s_bridge.logout_enqueued, 0, sizeof(s_bridge.logout_enqueued));
    memset(s_bridge.logout_retry_at_ms, 0, sizeof(s_bridge.logout_retry_at_ms));
    memset(s_bridge.cleanup_release_at_ms, 0, sizeof(s_bridge.cleanup_release_at_ms));
    memset(&s_bridge.wifi_credentials_web, 0, sizeof(s_bridge.wifi_credentials_web));
    memset(&s_bridge.ap_credentials_web, 0, sizeof(s_bridge.ap_credentials_web));
    memset(&s_bridge.ap_credentials_result, 0, sizeof(s_bridge.ap_credentials_result));
    memset(&s_bridge.ap_provisioning_exit, 0, sizeof(s_bridge.ap_provisioning_exit));
    memset(&s_bridge.ap_client_disconnect, 0, sizeof(s_bridge.ap_client_disconnect));
    oled_canvas_reset();
    s_bridge.json_scan_active = 0u;
    s_bridge.json_scan_cleanup_pending = 0u;
    s_bridge.json_scan_previous_auto_reconnect = 0u;
    s_bridge.json_scan_client_id = 0u;
    s_bridge.json_scan_generation = 0u;
    s_bridge.json_scan_origin_ip_key = 0u;
    s_bridge.json_scan_reconnect_until_ms = 0u;
    s_bridge.json_scan_origin_api_v1 = 0u;
    s_bridge.json_scan_request_id[0] = '\0';
    clear_json_scan_result();
    WifiScanSnapshot_Init(&s_bridge.web_scan_cache);
    s_bridge.last_f4_alive_ms = 0u;
    s_bridge.f4_offline_since_ms = 0u;
    s_bridge.f4_alive_seen = 0u;
    s_bridge.f4_backend_online = 0u;
    s_bridge.f4_alive_period_ms = F4_WEB_ALIVE_DEFAULT_PERIOD_MS;
    s_bridge.last_wifi_mode = 0xFF;
    s_bridge.last_sta_status = 0xFF;
    s_bridge.last_ap_clients = 0xFF;
}

bool WSJsonBridge_OnConnect(AsyncWebSocketClient *client, const char *resume_token)
{
    if (!client) return false;
    WsClientState *state = ensure_client(client->id());
    WsmSession *session = nullptr;
    bool resumed = false;
    uint32_t now = s_bridge.platform.now_ms();
    uint32_t ip_key = (uint32_t)s_bridge.platform.client_ip(client);
    bool resumed_json_scan = false;
    WsmResult session_result = state
        ? wsm_resume_by_token(&s_bridge.sessions, client->id(), resume_token, now, &session)
        : WSM_INVALID;
    if (session_result == WSM_OK && session)
    {
        resumed = true;
        session->ip_key = ip_key;
        if (session->node >= WSM_NODE_MIN && session->node <= WSM_NODE_MAX)
            clear_cleanup_tracking((uint8_t)(session->node - WSM_NODE_MIN));
    }
    else if (state)
    {
        session_result = wsm_open(&s_bridge.sessions, client->id(), ip_key, &session);
    }
    if (!state || session_result != WSM_OK || !session)
    {
        if (state) memset(state, 0, sizeof(*state));
        client->text("{\"api\":1,\"type\":\"error\",\"code\":\"busy\",\"message\":\"No WebSocket session node is available\"}");
        client->close();
        return false;
    }
    if (state)
    {
        state->generation = session->generation;
        state->preferred_api_v1 = 1u;
        state->rx_len = 0;
        state->dropping = 0;
    }
    if (!resumed)
    {
        // A fresh socket cannot inherit PIN authority merely by sharing an IP.
        // Without a platform entropy provider, reconnect requires PIN again.
        if (s_bridge.platform.random_u32)
        {
            for (uint8_t word = 0u; word < 4u; ++word)
                snprintf(&session->resume_token[word * 8u], 9u, "%08lx",
                         (unsigned long)s_bridge.platform.random_u32());
        }
    }
    client->setCloseClientOnQueueFull(false);
    client->keepAlivePeriod(15u);
    StaticJsonDocument<1280> doc;
    doc["api"] = 1;
    doc["type"] = "event";
    doc["event"] = "hello";
    JsonObject data = doc.createNestedObject("data");
    data["clientId"] = client->id();
    data["clientIp"] = ip_to_string(s_bridge.platform.client_ip(client));
    data["apiVersion"] = 1;
    data["espVersion"] = ESP_FIRMWARE_VERSION;
    data["f4ContractCutoff"] = "2026-07-25";
    data["sessionGeneration"] = session->generation;
    char node_hex[5]; snprintf(node_hex, sizeof(node_hex), "0x%02X", session->node);
    data["assignedNode"] = node_hex;
    data["maxClients"] = WS_MAX_SESSIONS;
    if (session->resume_token[0]) data["resumeToken"] = session->resume_token;
    JsonObject features = data.createNestedObject("features");
    features["typedF4Commands"] = true;
    features["motionControlV1"] = true;
    features["independentSessions"] = true;
    features["sharedMpuStream"] = true;
    features["sharedIrStream"] = true;
    features["pinAuthentication"] = true;
    features["apClientManagement"] = true;
    features["qtAliveForwarding"] = true;
    F4_CommandSpec oled_canvas_spec;
    features["oledCanvas"] = F4_CommandRegistry_Lookup(OLED_CANVAS_COMMAND_ID,
                                                        &oled_canvas_spec) &&
                             F4_CommandRegistry_IsWireAllowed(&oled_canvas_spec);
    F4_CommandSpec reset_spec;
    features["rebootStm"] = F4_CommandRegistry_Lookup(STM_CMD_RESET_MCU, &reset_spec) &&
                            F4_CommandRegistry_IsWireAllowed(&reset_spec);
    F4_CommandSpec extension_profile_spec;
    features["extensionProfiles"] =
        F4_CommandRegistry_Lookup(UNER_CMD_REBOOT_INTO_PROFILE,
                                  &extension_profile_spec) &&
        F4_CommandRegistry_IsWireAllowed(&extension_profile_spec);
    F4_CommandSpec boot_report_spec;
    features["bootReport"] =
        F4_CommandRegistry_Lookup(UNER_CMD_GET_BOOT_REPORT,
                                  &boot_report_spec) &&
        F4_CommandRegistry_IsWireAllowed(&boot_report_spec);
    features["diagnosticRaw"] = ENABLE_DIAGNOSTIC_RAW_UNER != 0;
    features["pageReady"] = s_bridge.backend.web_page_ready != nullptr;
    features["tls"] = false;
    JsonObject backend = data.createNestedObject("backend");
    backend["uart"] = s_bridge.app ? "ready" : "unavailable";
    backend["f4Alive"] = f4_alive_now();
    backend["wifi"] = "ready";
    backend["wifiState"] = (uint8_t)s_bridge.wifi.GetState();
    backend["filesystem"] = s_bridge.backend.storage_ready(s_bridge.backend.context) ? "ready" : "unavailable";
    backend["spaAssets"] = s_bridge.backend.storage_ready(s_bridge.backend.context) && s_bridge.platform.asset_exists("/dist/index.html.gz");
    backend["modelAssets"] = s_bridge.backend.storage_ready(s_bridge.backend.context) &&
                             s_bridge.platform.asset_exists("/dist/models/auto_micro.glb");
    data["sessionResumed"] = resumed;
    data["authenticated"] = session->stm_authenticated != 0u;
    data["wifiScanPending"] =
        (s_bridge.json_scan_active && s_bridge.json_scan_client_id == 0u &&
         s_bridge.json_scan_origin_ip_key == ip_key &&
         !deadline_reached(now, s_bridge.json_scan_reconnect_until_ms)) ||
        (s_bridge.json_scan_result_pending && s_bridge.json_scan_result_ip_key == ip_key &&
         !deadline_reached(now, s_bridge.json_scan_result_expires_ms));
    send_doc_to_client(client->id(), doc);

    if (s_bridge.json_scan_active && s_bridge.json_scan_client_id == 0u &&
        s_bridge.json_scan_origin_ip_key == ip_key &&
        !deadline_reached(now, s_bridge.json_scan_reconnect_until_ms))
    {
        s_bridge.json_scan_client_id = client->id();
        s_bridge.json_scan_generation = session->generation;
        s_bridge.json_scan_origin_api_v1 = state->preferred_api_v1;
        s_bridge.json_scan_reconnect_until_ms = 0u;
        resumed_json_scan = true;
    }

    if (s_bridge.json_scan_result_pending && s_bridge.json_scan_result_ip_key == ip_key)
    {
        (void)deliver_pending_json_scan_result(client->id(),
                                               state->preferred_api_v1,
                                               true);
    }
    else if (resumed_json_scan)
    {
        StaticJsonDocument<320> resumed_doc;
        resumed_doc["api"] = 1;
        resumed_doc["type"] = "event";
        resumed_doc["event"] = "wifi.scan.resumed";
        JsonObject resumed_data = resumed_doc.createNestedObject("data");
        resumed_data["origin"] = "esp";
        resumed_data["requestId"] = s_bridge.json_scan_request_id;
        resumed_data["elapsedMs"] = (uint32_t)(now - s_bridge.json_scan_started_ms);
        send_doc_to_client(client->id(), resumed_doc);
    }
    return true;
}

bool WSJsonBridge_OnDisconnect(uint32_t client_id)
{
    WsClientState *state = find_client(client_id);
    bool had_session = state != nullptr;
    if (state)
    {
        uint32_t generation = state->generation;
        uint32_t now = s_bridge.platform.now_ms();
        oled_canvas_on_disconnect(client_id, generation);
        if (s_bridge.json_scan_active && s_bridge.json_scan_client_id == client_id &&
            s_bridge.json_scan_generation == generation)
        {
            // Un scan puede sacar temporalmente a la radio del canal del AP.
            // Se conserva la operacion y se reasocia al WebSocket que vuelva
            // desde la misma IP dentro de la gracia de reconexion.
            s_bridge.json_scan_client_id = 0u;
            s_bridge.json_scan_generation = 0u;
            s_bridge.json_scan_reconnect_until_ms = now + JSON_SCAN_RECONNECT_GRACE_MS;
        }
        (void)srw_remove_client(&s_bridge.stream_waiters, client_id, generation);
        (void)sb_remove_client(&s_bridge.streams, client_id, generation);
        utm_disconnect_client(&s_bridge.transactions, client_id, generation, now);
        (void)wsm_suspend_for_reconnect(&s_bridge.sessions, client_id, generation,
                                        now, WS_RECONNECT_GRACE_MS);
        WsmSession *session = wsm_find(&s_bridge.sessions, client_id, generation);
        if (session && session->node >= WSM_NODE_MIN && session->node <= WSM_NODE_MAX)
        {
            uint8_t index = (uint8_t)(session->node - WSM_NODE_MIN);
            s_bridge.logout_enqueued[index] = 0u;
            s_bridge.logout_retry_at_ms[index] = now + WS_RECONNECT_GRACE_MS;
            s_bridge.cleanup_release_at_ms[index] = now + STM_CLEANUP_QUARANTINE_MS;
        }
    }
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (s_bridge.clients[i].used && s_bridge.clients[i].id == client_id)
        {
            memset(&s_bridge.clients[i], 0, sizeof(s_bridge.clients[i]));
            break;
        }
    }
    if (s_bridge.telem_stop.active && s_bridge.telem_stop.client_id == client_id) s_bridge.telem_stop.active = 0;
    return had_session;
}

void WSJsonBridge_OnData(AsyncWebSocketClient *client, AwsFrameInfo *info, uint8_t *data, size_t len)
{
    if (!client || !info) return;
    if (info->message_opcode != WS_TEXT)
    {
        emit_bridge_error(client->id(), "ERR_UNKNOWN_TYPE", "Only WS_TEXT JSON is accepted on /ws");
        return;
    }
    WsClientState *state = ensure_client(client->id());
    if (!state)
    {
        emit_bridge_error(client->id(), "ERR_INTERNAL", "No client slot available");
        return;
    }
    // `index` vuelve a cero al comenzar cada frame de un mensaje WS
    // fragmentado. Solo `num==0 && index==0` identifica un mensaje nuevo.
    if (info->num == 0u && info->index == 0u)
    {
        state->rx_len = 0;
        state->dropping = 0;
        if (info->len > WS_JSON_MAX)
        {
            state->dropping = 1;
            emit_bridge_error(client->id(), "ERR_JSON_TOO_LARGE", "JSON message exceeds 1024 bytes");
        }
    }
    if (state->dropping)
    {
        if (info->final && (info->index + len) == info->len)
        {
            state->dropping = 0;
            state->rx_len = 0;
        }
        return;
    }
    if (state->rx_len + len > WS_JSON_MAX)
    {
        state->dropping = 1;
        state->rx_len = 0;
        emit_bridge_error(client->id(), "ERR_JSON_TOO_LARGE", "JSON message exceeds 1024 bytes");
        return;
    }
    memcpy(&state->rx[state->rx_len], data, len);
    state->rx_len += len;
    if (!info->final || (info->index + len) != info->len) return;
    state->rx[state->rx_len] = '\0';
    process_json_message(client->id(), state->rx);
    state->rx_len = 0;
}

void WSJsonBridge_Loop()
{
    service_debug_wifi_scope();
    service_json_scan();
    service_wifi_credentials_flow();
    service_ap_provisioning_exit();
    service_ap_credentials_flow();
    service_ap_client_disconnect();
    service_wifi_events();
    service_ap_fallback_ready();
    service_auto_screen_refresh();
    service_stm_pending();
    utm_service(&s_bridge.transactions, s_bridge.platform.now_ms());
    service_oled_canvas();
    service_f4_alive_timeout();
    service_session_expiry();
    service_stream_broker();
    service_stream_waiter_timeouts();
    service_session_cleanup();
}

uint8_t WSJsonBridge_OnUNERPacket(void *ctx, const UNER_Packet *pkt)
{
    (void)ctx;
    if (!pkt) return 0;
    F4_CommandSpec wire_spec_storage;
    const F4_CommandSpec *wire_spec = lookup_f4_command(pkt->cmd, wire_spec_storage);
    if (!wire_spec || !wire_spec->implemented || wire_spec->owner == F4_COMMAND_OWNER_RESERVED)
    {
        return 1;
    }
    if (wire_spec->owner == F4_COMMAND_OWNER_ESP &&
        wire_spec->response_policy == F4_RESPONSE_EVENT)
    {
        // Los eventos de estado ESP solo salen por UART con src=0x3. Nunca se
        // aceptan de vuelta desde F4 ni se convierten en eventos Web.
        return 1;
    }
    if (pkt->cmd == UNER_CMD_ACK || pkt->cmd == UNER_CMD_NACK)
    {
        if (pkt->len < 1u || pkt->len > 2u) return 1;
    }
    else if (wire_spec->owner == F4_COMMAND_OWNER_F4 ||
             wire_spec->owner == F4_COMMAND_OWNER_DUAL)
    {
        if (!F4_CommandRegistry_IsResponseLengthValid(wire_spec, pkt->len)) return 1;
    }
    else if (!F4_CommandRegistry_IsRequestLengthValid(wire_spec, pkt->len))
    {
        // REBOOT_ESP usa ACK manual: el dispatcher local debe ver tambien las
        // longitudes invalidas para responder NACK en vez de descartarlas.
        if (pkt->cmd == UNER_CMD_REBOOT_ESP) return 0;
        return 1;
    }
    if (pkt->src == UNER_NODE_PC_QT && pkt->dst == UNER_NODE_WEB_APP &&
        pkt->cmd == UNER_CMD_PING && pkt->len == 0u)
    {
        emit_qt_alive_event(pkt);
        return 1;
    }
    if (pkt->src == UNER_NODE_PC_QT)
    {
        // Por ahora el único caso Qt reenviado y expuesto a Web es el alive
        // explícito. No permitir que otros comandos PC_QT lleguen por accidente
        // a los handlers locales del ESP.
        return 1;
    }
    if (pkt->src == UNER_NODE_MCU)
    {
        uint32_t received_at_ms = s_bridge.platform.now_ms();
        mark_f4_backend_activity(received_at_ms);
        if (pkt->cmd == UNER_CMD_PING && (pkt->len == 0u || pkt->len == 2u))
        {
            if (pkt->len == 2u)
                s_bridge.f4_alive_period_ms = normalize_f4_alive_period(
                    read_u16_le(pkt->payload));
            emit_f4_alive_event(received_at_ms);
        }
    }
    debug_log_ws_uner_packet("parse", pkt);
    if (pkt->cmd == UNER_EVT_IR_EMITTERS_CHANGED)
    {
        emit_ir_emitters_changed(pkt);
        return 1;
    }
    if (pkt->cmd == UNER_EVT_APP_GET_MPU_READINGS && pkt->len == 42)
    {
        emit_mpu_snapshot_event(pkt, "mpu.readings", nullptr);
        return 1;
    }
    if (pkt->cmd == UNER_EVT_APP_GET_TCRT_READINGS && pkt->len == 56u)
    {
        emit_ir_snapshot_event(pkt, "irSample");
        return 1;
    }
    if (is_screen_changed_event(pkt))
    {
        if (pkt->len >= 4u)
            oled_canvas_on_screen_code(read_u32_le(pkt->payload));
        if (pkt->cmd == STM_EVT_MENU_SELECTION_CHANGED)
        {
#if UNER_BRIDGE_DEBUG
            uint8_t selected_index = pkt->len >= 7 ? pkt->payload[4] : 0;
            uint8_t item_count = pkt->len >= 7 ? pkt->payload[5] : (pkt->len >= 6 ? pkt->payload[4] : 0);
            uint8_t source = pkt->len >= 7 ? pkt->payload[6] : (pkt->len >= 6 ? pkt->payload[5] : pkt->payload[4]);
            Serial.printf("[WS][MENU_SELECTION 0x96] selectedIndex=%u itemCount=%u source=%u\n",
                          selected_index, item_count, source);
#endif
        }
        emit_screen_report_event(pkt, "screen.changed", nullptr);
        return 1;
    }
    if (pkt->cmd == STM_EVT_CAR_MODE_CHANGED && pkt->len == 1u)
    {
        oled_canvas_on_car_mode(pkt->payload[0]);
        emit_car_mode_changed_event(pkt);
        return 1;
    }
    if (pkt->cmd == STM_EVT_QT_USB_STATE && pkt->len == 1u)
    {
        emit_qt_usb_state_event(pkt);
        return 1;
    }
    if (pkt->cmd == STM_EVT_MCU_BOOT_REPORT &&
        pkt->len == F4_BOOT_REPORT_PAYLOAD_SIZE)
    {
        emit_boot_report_event(pkt);
        return 1;
    }
    const UtmRequest *inflight = utm_inflight(&s_bridge.transactions);
    if (inflight && inflight->internal_kind == UTM_INTERNAL_OLED_CANVAS &&
        pkt->cmd == OLED_CANVAS_COMMAND_ID &&
        pkt->src == inflight->dst_node && pkt->dst == inflight->node &&
        !OledCanvas_F4ResponseMatchesExpected(
            pkt->payload, pkt->len,
            s_bridge.oled_canvas.expected_operation,
            s_bridge.oled_canvas.assembly.transfer_id,
            s_bridge.oled_canvas.expected_next_offset))
    {
        /* A retry may produce a second, delayed 0x6D response.  All Canvas
         * phases share command and route, so UTM must not complete the next
         * phase until operation/id/nextOffset also correlate. */
        return 1;
    }
    if (inflight && pkt->cmd == UNER_CMD_AUTH_VALIDATE_PIN && pkt->len == 9u &&
        inflight->command_id == UNER_CMD_AUTH_VALIDATE_PIN && inflight->payload_len >= 2u &&
        (pkt->payload[1] != inflight->payload[0] ||
         pkt->payload[2] != inflight->payload[1]))
    {
        return 1;
    }
    UtmFrame frame = {pkt->src, pkt->dst, pkt->cmd, pkt->payload, pkt->len};
    UtmFrameResult matched = utm_on_frame(&s_bridge.transactions, &frame, s_bridge.platform.now_ms());
    if (matched == UTM_FRAME_MATCHED_PENDING || matched == UTM_FRAME_COMPLETED ||
        matched == UTM_FRAME_IGNORED_EVENT)
    {
        return 1;
    }
    if (pkt->cmd == UNER_CMD_GET_CURRENT_SCREEN && is_screen_report_payload(pkt))
    {
        emit_screen_report_event(pkt, "screen.current", nullptr);
        return 1;
    }
    if (pkt->cmd >= 0x80 && pkt->cmd <= 0x9F)
    {
        emit_generic_stm_event(pkt);
        return 1;
    }
    return 0;
}

void *WSJsonBridge_GetContext() { return &s_bridge; }
