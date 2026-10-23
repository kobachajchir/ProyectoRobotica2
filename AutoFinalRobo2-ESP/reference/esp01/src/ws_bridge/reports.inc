// Private bridge implementation; included once by ws_json_bridge.cpp.
static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static void add_mpu_snapshot_fields(JsonObject data, const uint8_t *p)
{
    F4_MpuSnapshot sample;
    if (F4_DecodeMpuSnapshot(p, 42u, &sample) != F4_DECODE_OK) return;
    data["status"] = sample.status;
    data["flags"] = sample.flags;
    data["sampleValid"] = (sample.flags & F4_MPU_FLAG_SAMPLE_VALID) != 0u;
    data["calibrated"] = (sample.flags & F4_MPU_FLAG_CALIBRATED) != 0u;
    data["magValid"] = (sample.flags & F4_MPU_FLAG_MAG_VALID) != 0u;
    data["stationary"] = (sample.flags & F4_MPU_FLAG_STATIONARY) != 0u;
    data["calibrating"] = (sample.flags & F4_MPU_FLAG_CALIBRATING) != 0u;
    data["sensorError"] = (sample.flags & F4_MPU_FLAG_ERROR) != 0u;
    data["sampleSeq"] = sample.sample_sequence;
    data["sampleDtUs"] = sample.sample_dt_us;
    data["roll"] = sample.roll_deg;
    data["pitch"] = sample.pitch_deg;
    data["yaw"] = sample.yaw_magnetic_deg;
    JsonObject accel = data.createNestedObject("accel");
    accel["x"] = sample.accel_g[0];
    accel["y"] = sample.accel_g[1];
    accel["z"] = sample.accel_g[2];
    JsonObject gyro = data.createNestedObject("gyro");
    gyro["x"] = sample.gyro_deg_s[0];
    gyro["y"] = sample.gyro_deg_s[1];
    gyro["z"] = sample.gyro_deg_s[2];
}

static bool add_ir_snapshot_fields(JsonObject data, const uint8_t *p, size_t len)
{
    F4_IrSnapshot sample;
    if (F4_DecodeIrSnapshot(p, len, &sample) != F4_DECODE_OK) return false;
    data["status"] = sample.status;
    data["flags"] = sample.flags;
    data["sampleSeq"] = sample.sample_sequence;
    data["periodMs"] = sample.stream_period_ms;
    data["tickMs"] = sample.hal_tick_ms;
    JsonArray raw = data.createNestedArray("raw");
    JsonArray normalized = data.createNestedArray("normalized");
    for (uint8_t i = 0; i < 8u; ++i)
    {
        raw.add(sample.raw_adc[i]);
        normalized.add(sample.normalized[i]);
    }
    data["linePattern"] = sample.line_pattern;
    data["routeAlignment"] = sample.route_alignment;
    data["confidence"] = sample.confidence_percent;
    data["ambiguous"] = sample.ambiguous != 0u;
    data["lineWidthMm"] = sample.line_width_mm;
    data["lateralErrorMm"] = sample.lateral_error_mm;
    data["normalizedLateralError"] = sample.normalized_lateral_error;
    return true;
}

static void add_ir_emitters_fields(JsonObject data, const F4_IrEmitters &state)
{
    data["objects"] = state.objects != 0u;
    data["line"] = state.line != 0u;
    data["lights"] = state.lights != 0u;
}

static void emit_ir_emitters_changed(const UNER_Packet *pkt)
{
    F4_IrEmitters state;
    if (!pkt || pkt->src != UNER_NODE_MCU ||
        F4_DecodeIrEmitters(pkt->payload, pkt->len, &state) != F4_DECODE_OK) return;
    StaticJsonDocument<384> doc;
    JsonObject data = make_event_base(doc, "stm.irEmitters.changed", "stm", nullptr);
    add_ir_emitters_fields(data, state);
    broadcast_doc_stm_authorized(doc);
}

static const char *boot_handoff_name(uint8_t handoff)
{
    switch (handoff)
    {
    case 0u: return "none";
    case 1u: return "application";
    case 2u: return "rom";
    case 3u: return "recovery";
    default: return "unknown";
    }
}

static const char *boot_profile_name(uint8_t profile)
{
    return profile == 0u ? "nrf24" : (profile == 1u ? "beeper" : "unknown");
}

static bool add_boot_report_fields(JsonObject data, const uint8_t *payload, size_t len)
{
    F4_BootReport report;
    if (F4_DecodeBootReport(payload, len, &report) != F4_DECODE_OK) return false;
    data["schema"] = report.schema;
    data["status"] = report.status;
    data["mailboxValid"] = report.status == 0u;
    data["handoff"] = report.handoff;
    data["handoffName"] = boot_handoff_name(report.handoff);
    data["resetFlags"] = report.reset_flags;
    data["appValidationFlags"] = report.app_validation_flags;
    data["extensionProfileId"] = report.extension_profile;
    data["extensionProfile"] = boot_profile_name(report.extension_profile);
    data["bootloaderVersion"] = report.bootloader_version;
    data["appVersion"] = report.app_version;
    data["appSize"] = report.app_size;
    data["appCrc32"] = report.app_crc32;
    return true;
}

static void emit_mpu_snapshot_event(const UNER_Packet *pkt, const char *event_name, const char *request_id)
{
    if (!pkt || pkt->len != 42) return;
    StaticJsonDocument<1024> legacy_doc;
    JsonObject legacy_data = make_event_base(legacy_doc, event_name, "stm", request_id);
    add_mpu_snapshot_fields(legacy_data, pkt->payload);
    StaticJsonDocument<1024> v1_doc;
    v1_doc["api"] = 1; v1_doc["type"] = "event"; v1_doc["event"] = "mpuSample";
    JsonObject v1_data = v1_doc.createNestedObject("data");
    add_mpu_snapshot_fields(v1_data, pkt->payload);
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used) continue;
        WsmSession *session = wsm_find(&s_bridge.sessions, s_bridge.clients[i].id, s_bridge.clients[i].generation);
        uint8_t authenticated = session && session->stm_authenticated;
        if (!sb_should_fanout(&s_bridge.streams, s_bridge.clients[i].id, s_bridge.clients[i].generation, SB_STREAM_MPU, authenticated)) continue;
        if (wsm_touch_stm_auth(&s_bridge.sessions, s_bridge.clients[i].id,
                               s_bridge.clients[i].generation, s_bridge.platform.now_ms()) != WSM_OK)
            continue;
        send_doc_to_client(s_bridge.clients[i].id, s_bridge.clients[i].preferred_api_v1 ? (JsonDocument &)v1_doc : (JsonDocument &)legacy_doc);
    }
}

static void emit_ir_snapshot_event(const UNER_Packet *pkt, const char *event_name)
{
    if (!pkt || pkt->len != 56u) return;
    DynamicJsonDocument legacy_doc(1536);
    JsonObject legacy_data = make_event_base(legacy_doc, event_name, "stm", nullptr);
    if (!add_ir_snapshot_fields(legacy_data, pkt->payload, pkt->len)) return;
    DynamicJsonDocument v1_doc(1536);
    v1_doc["api"] = 1; v1_doc["type"] = "event"; v1_doc["event"] = "irSample";
    JsonObject v1_data = v1_doc.createNestedObject("data");
    if (!add_ir_snapshot_fields(v1_data, pkt->payload, pkt->len)) return;
    for (uint8_t i = 0; i < WS_CLIENT_MAX; ++i)
    {
        if (!s_bridge.clients[i].used) continue;
        WsmSession *session = wsm_find(&s_bridge.sessions, s_bridge.clients[i].id, s_bridge.clients[i].generation);
        uint8_t authenticated = session && session->stm_authenticated;
        if (!sb_should_fanout(&s_bridge.streams, s_bridge.clients[i].id, s_bridge.clients[i].generation, SB_STREAM_IR, authenticated)) continue;
        if (wsm_touch_stm_auth(&s_bridge.sessions, s_bridge.clients[i].id,
                               s_bridge.clients[i].generation, s_bridge.platform.now_ms()) != WSM_OK)
            continue;
        send_doc_to_client(s_bridge.clients[i].id, s_bridge.clients[i].preferred_api_v1 ? (JsonDocument &)v1_doc : (JsonDocument &)legacy_doc);
    }
}

static void emit_telemetry_data(const UNER_Packet *pkt)
{
    if (!pkt || pkt->len != 17) return;
    StaticJsonDocument<768> doc;
    JsonObject data = make_event_base(doc, "telemetry.data", "stm", nullptr);
    data["schema"] = pkt->payload[0];
    data["seq"] = (uint16_t)pkt->payload[1] | ((uint16_t)pkt->payload[2] << 8);
    JsonObject acc = data.createNestedObject("acc");
    acc["x"] = read_i16_le(&pkt->payload[3]);
    acc["y"] = read_i16_le(&pkt->payload[5]);
    acc["z"] = read_i16_le(&pkt->payload[7]);
    JsonObject gyro = data.createNestedObject("gyro");
    gyro["x"] = read_i16_le(&pkt->payload[9]);
    gyro["y"] = read_i16_le(&pkt->payload[11]);
    gyro["z"] = read_i16_le(&pkt->payload[13]);
    data["tempRaw"] = read_i16_le(&pkt->payload[15]);
    broadcast_doc(doc);
}

static bool add_screen_report(JsonObject data, const UNER_Packet *pkt)
{
    if (!pkt || pkt->len < 5) return false;

    uint32_t screen_code = (uint32_t)pkt->payload[0] |
                           ((uint32_t)pkt->payload[1] << 8) |
                           ((uint32_t)pkt->payload[2] << 16) |
                           ((uint32_t)pkt->payload[3] << 24);
    uint8_t selected_index = 0;
    uint8_t item_count = 0;
    uint8_t source = 0;

    if (pkt->cmd == STM_EVT_MENU_SELECTION_CHANGED && pkt->len >= 7)
    {
        selected_index = pkt->payload[4];
        item_count = pkt->payload[5];
        source = pkt->payload[6];
    }
    else if (pkt->cmd == STM_EVT_MENU_SELECTION_CHANGED && pkt->len >= 6)
    {
        item_count = pkt->payload[4];
        source = pkt->payload[5];
    }
    else
    {
        source = pkt->payload[4];
        if (pkt->cmd == UNER_CMD_GET_CURRENT_SCREEN && source == 1u && pkt->len >= 7u)
        {
            selected_index = pkt->payload[5];
            item_count = pkt->payload[6];
        }
    }
    if (pkt->cmd != STM_EVT_MENU_SELECTION_CHANGED && source == 3u && pkt->len >= 9u)
    {
        data["notificationActive"] = true;
        data["remainingMs"] = read_u16_le(&pkt->payload[5]);
        data["totalMs"] = read_u16_le(&pkt->payload[7]);
        if (pkt->len >= 10u && pkt->payload[9] != 0u)
            data["clientNumber"] = pkt->payload[9];
    }
    if (pkt->cmd == UNER_CMD_GET_CURRENT_SCREEN &&
        screen_code == STM_SCREEN_CODE_DASHBOARD && source == 2u && pkt->len >= 13u)
    {
        const uint8_t flags = pkt->payload[5];
        const uint8_t backend = pkt->payload[6];
        const uint8_t car_mode = pkt->payload[7];
        uint8_t ssid_len = pkt->payload[12];
        const bool esp_present = (flags & (1u << 0)) != 0u;
        const bool sta_active = esp_present && (flags & (1u << 1)) != 0u;
        const bool ap_active = esp_present && !sta_active && (flags & (1u << 2)) != 0u;
        const bool ip_valid = pkt->payload[8] != 0u || pkt->payload[9] != 0u ||
                              pkt->payload[10] != 0u || pkt->payload[11] != 0u;
        char ssid[33];
        char ip[16];

        if (ssid_len > 32u) ssid_len = 32u;
        if (ssid_len > (uint8_t)(pkt->len - 13u)) ssid_len = (uint8_t)(pkt->len - 13u);

        memcpy(ssid, &pkt->payload[13], ssid_len);
        ssid[ssid_len] = '\0';
        if (ip_valid)
        {
            snprintf(ip, sizeof(ip), "%u.%u.%u.%u",
                     pkt->payload[8], pkt->payload[9], pkt->payload[10], pkt->payload[11]);
        }
        else
        {
            strcpy(ip, "Sin IP");
        }

        data["espPresent"] = esp_present;
        data["connection"] = sta_active ? "sta" : (ap_active ? "ap" : "none");
        data["ssid"] = ssid;
        data["ipAddress"] = ip;
        data["backend"] = backend == 2u ? "WEB" : (backend == 1u ? "AT" : "No ESP");
        data["carMode"] = car_mode;
        data["carModeName"] = car_mode_name(car_mode);
        data["usbActive"] = (flags & (1u << 3)) != 0u;
        data["rfActive"] = (flags & (1u << 4)) != 0u;
        data["apActive"] = (flags & (1u << 2)) != 0u;
        data["staActive"] = (flags & (1u << 1)) != 0u;
    }

    char screen_hex[11];
    if ((screen_code & 0xFF000000UL) == 0)
    {
        snprintf(screen_hex, sizeof(screen_hex), "0x%06lX", (unsigned long)screen_code);
    }
    else
    {
        snprintf(screen_hex, sizeof(screen_hex), "0x%08lX", (unsigned long)screen_code);
    }

    data["cmd"] = pkt->cmd;
    data["src"] = pkt->src;
    data["dst"] = pkt->dst;
    data["len"] = pkt->len;
    data["screen_code"] = screen_code;
    data["screenCode"] = screen_code;
    data["screenCodeHex"] = screen_hex;
    data["menu"] = (uint8_t)((screen_code >> 16) & 0xFFu);
    data["submenu"] = (uint8_t)((screen_code >> 8) & 0xFFu);
    data["page"] = (uint8_t)(screen_code & 0xFFu);
    data["selectedIndex"] = selected_index;
    data["itemCount"] = item_count;
    data["hasMenuItems"] = item_count > 0;
    data["source"] = source;
    data["sourceName"] = screen_source_name(source);
    JsonArray bytes = data.createNestedArray("payload");
    for (uint8_t i = 0; i < pkt->len; ++i) bytes.add(pkt->payload[i]);
    return true;
}

static void emit_screen_report_event(const UNER_Packet *pkt, const char *event_name, const char *request_id)
{
    debug_log_ws_uner_packet(event_name ? event_name : "screen.report", pkt);
    StaticJsonDocument<896> doc;
    JsonObject data = make_event_base(doc, event_name, "stm", request_id);
    if (!add_screen_report(data, pkt)) return;
    s_bridge.auto_screen_refresh.active = 0;
    debug_log_ws_send("broadcast", "device.event", event_name, pkt);
    broadcast_doc_stm_authorized(doc);
}

static bool is_screen_report_payload(const UNER_Packet *pkt)
{
    return pkt && pkt->len >= 5;
}

static bool is_screen_changed_event(const UNER_Packet *pkt)
{
    return pkt &&
           (pkt->cmd == UNER_EVT_SCREEN_CHANGED || pkt->cmd == STM_EVT_MENU_SELECTION_CHANGED) &&
           is_screen_report_payload(pkt);
}

static const char *extension_profile_name(uint8_t profile_id)
{
    switch (profile_id)
    {
    case F4_EXTENSION_PROFILE_NRF24: return "nrf24";
    case F4_EXTENSION_PROFILE_BEEPER: return "beeper";
    default: return "unknown";
    }
}

static bool is_extension_profile_selection_command(uint8_t command_id)
{
    return command_id == UNER_CMD_SET_EXTENSION_PROFILE ||
           command_id == UNER_CMD_REBOOT_INTO_PROFILE;
}


static float line_read_f32(const uint8_t *p) {
    uint32_t bits=read_u32_le(p); float value; memcpy(&value,&bits,sizeof(value)); return value;
}
static bool is_line_command(uint8_t cmd) { return cmd>=0x1Du && cmd<=0x1Fu; }
static void add_line_fields(JsonObject data,const uint8_t *p)
{
    uint8_t flags=p[8];
    data["status"]=p[0]; data["version"]=p[1]; data["calibrationState"]=p[2];
    data["target"]=p[3]; data["phase"]=p[4]; data["progress"]=p[5];
    data["calibrationReason"]=p[6]; data["followReason"]=p[7];
    data["whiteValid"]=(flags&1u)!=0; data["trackValid"]=(flags&2u)!=0;
    data["balanceSettled"]=(flags&4u)!=0; data["following"]=(flags&8u)!=0;
    data["motionArmed"]=(flags&16u)!=0; data["balanceActive"]=(flags&32u)!=0;
    data["safetyStop"]=(flags&64u)!=0; data["routeValid"]=(flags&128u)!=0;
    data["carMode"]=p[9]; data["pattern"]=p[10]; data["confidence"]=p[11];
    data["sessionId"]=read_u32_le(p+12); data["whiteRevision"]=read_u32_le(p+16);
    data["trackWhiteRevision"]=read_u32_le(p+20); data["usefulScans"]=read_u32_le(p+24);
    data["neutralPitchDeg"]=line_read_f32(p+36);
    data["referenceRollDeg"]=line_read_f32(p+40); data["referenceYawDeg"]=line_read_f32(p+44);
    for (unsigned t=0;t<2;++t) {
        JsonObject profile=data.createNestedObject(t?"track":"white");
        profile["usefulScans"]=read_u32_le(p+28+t*4u);
        profile["pitchDeg"]=line_read_f32(p+144+t*12u);
        profile["rollDeg"]=line_read_f32(p+148+t*12u);
        profile["yawDeg"]=line_read_f32(p+152+t*12u);
        JsonArray means=profile.createNestedArray("means"); JsonArray noise=profile.createNestedArray("noise");
        for(unsigned phase=0;phase<4;++phase) {
            JsonArray m=means.createNestedArray(), n=noise.createNestedArray();
            for(unsigned c=0;c<3;++c) {
                unsigned pos=t*48u+phase*6u+c*2u;
                m.add(read_u16_le(p+48u+pos)); n.add(read_u16_le(p+72u+pos));
            }
        }
    }
}

static bool f4_payload_has_status_byte(uint8_t command_id)
{
    return (command_id >= UNER_CMD_GET_MOTION_STATE && command_id <= UNER_CMD_SET_MOTOR_TEST) ||
           command_id == UNER_CMD_GET_PREFERENCES ||
           command_id == UNER_CMD_AUTH_VALIDATE_PIN ||
           command_id == STM_CMD_AUTH_REMOTE_RESULT ||
           command_id == STM_CMD_SET_I2C_POLICY ||
           command_id == STM_CMD_GET_I2C_POLICY ||
           command_id == STM_CMD_SET_CAR_MODE ||
           command_id == STM_CMD_GET_MPU_SNAPSHOT ||
           command_id == STM_CMD_SET_MPU_STREAM ||
           command_id == STM_CMD_STOP_MPU_STREAM ||
           (command_id >= UNER_CMD_SET_PREFERENCES &&
            command_id <= UNER_CMD_STOP_IR_STREAM);
}

static void complete_response_from_screen_report(const UtmRequest *request,
                                                 const UNER_Packet *pkt)
{
    if (!request) return;
    StaticJsonDocument<896> doc;
    JsonObject data = make_response_base_for_api(doc, request->api_v1,
                                                 request->request_id, "stm", request->command_name,
                                                 true, "OK", "Current screen read");
    if (!add_screen_report(data, pkt))
    {
        send_error_for_api(request->client_id, request->api_v1,
                           request->request_id, "stm", request->command_name,
                           "ERR_UNER_ARGS", "STM current screen payload must include screenCode and source");
        return;
    }
    send_doc_to_client(request->client_id, doc);
    emit_screen_report_event(pkt, "screen.current", request->request_id);
}

static void complete_response_from_manual_status(const UtmRequest *request,
                                                const UNER_Packet *pkt)
{
    if (!request) return;
    uint8_t status = pkt && pkt->len >= 1 ? pkt->payload[0] : 1u;
    uint8_t cmd = pkt ? pkt->cmd : 0u;
    bool refresh_screen = should_refresh_screen_after_status(cmd, status);
    bool ok = status == 0u;
    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base_for_api(doc, request->api_v1,
                                                 request->request_id, "stm", request->command_name,
                                                 ok, ok ? "OK" : "ERR_STM_REJECTED",
                                                 ok ? "STM command accepted" : "STM command rejected");
    data["stmCode"] = status;
    data["status"] = status;
    if (cmd == UNER_CMD_REBOOT_INTO_PROFILE)
    {
        uint8_t requested_profile =
            request->payload_len == 1u ? request->payload[0] : 0xFFu;
        data["success"] = ok;
        data["rebooting"] = ok;
        if (F4_ExtensionProfile_IsValid(requested_profile))
        {
            data["profileId"] = requested_profile;
            data["profile"] = extension_profile_name(requested_profile);
        }
        if (ok) data["reconnectDelayMs"] = 3000u;
    }
    if (refresh_screen) data["screenRefreshScheduled"] = true;
    send_doc_to_client(request->client_id, doc);
    if (refresh_screen) schedule_screen_refresh(cmd, request->request_id);
}

static void complete_response_from_payload(const UtmRequest *request,
                                          const UNER_Packet *pkt)
{
    if (!request || !pkt) return;

    bool extension_get = pkt->cmd == UNER_CMD_GET_EXTENSION_PROFILE;
    bool extension_select = is_extension_profile_selection_command(pkt->cmd);
    bool extension_success = false;
    uint8_t extension_profile = 0xFFu;
    bool extension_payload_valid =
        extension_get
            ? F4_ExtensionProfile_DecodeCurrent(pkt->payload, pkt->len,
                                                &extension_profile)
            : (extension_select
                   ? F4_ExtensionProfile_DecodeResult(pkt->payload, pkt->len,
                                                      &extension_success)
                   : true);
    F4_IrEmitters ir_state = {};
    uint8_t ir_status = 0u;
    const bool ir_get = pkt->cmd == UNER_CMD_GET_IR_EMITTERS;
    const bool ir_set = pkt->cmd == UNER_CMD_SET_IR_EMITTERS;
    const bool ir_payload_valid = ir_get
        ? F4_DecodeIrEmitters(pkt->payload, pkt->len, &ir_state) == F4_DECODE_OK
        : (!ir_set || F4_DecodeIrEmittersResult(pkt->payload, pkt->len,
                                               &ir_status, &ir_state) == F4_DECODE_OK);
    extension_payload_valid = extension_payload_valid && ir_payload_valid;
    const bool fallback_get = pkt->cmd == UNER_CMD_GET_AP_FALLBACK;
    const bool fallback_valid = !fallback_get || (pkt->len == 3u && pkt->payload != nullptr &&
        pkt->payload[0] == 0u && pkt->payload[1] <= 1u && pkt->payload[2] <= 3u);
    extension_payload_valid = extension_payload_valid && fallback_valid;
    const bool line_command=is_line_command(pkt->cmd);
    const bool line_valid=!line_command || (pkt->len==168u && pkt->payload && pkt->payload[1]==1u && pkt->payload[0]<=1u);
    extension_payload_valid=extension_payload_valid && line_valid;
    bool status_payload = f4_payload_has_status_byte(pkt->cmd) || ir_set;
    bool ok = (extension_get || extension_select)
                  ? extension_payload_valid && (!extension_select || extension_success)
                  : (!status_payload || pkt->len == 0 || pkt->payload[0] == 0u);
    ok = ok && ir_payload_valid && fallback_valid && line_valid;
    const char *code = ok ? "OK"
                          : (extension_payload_valid ? "ERR_STM_REJECTED"
                                                     : "ERR_STM_RESPONSE");
    const char *message = ok ? "STM command answered"
                             : (extension_payload_valid ? "STM command rejected"
                                                        : "STM response payload is invalid");

    DynamicJsonDocument doc(3072);
    JsonObject data = make_response_base_for_api(doc, request->api_v1,
                                                 request->request_id, "stm", request->command_name,
                                                 ok, code, message);
    data["cmd"] = pkt->cmd;
    data["len"] = pkt->len;

    if (line_command && line_valid) {
        add_line_fields(data,pkt->payload);
    }
    else if (fallback_get && fallback_valid) {
        data["status"] = 0u;
        data["enabled"] = pkt->payload[1] != 0u;
        data["syncState"] = pkt->payload[2];
    }
    else if ((ir_get || ir_set) && ir_payload_valid)
    {
        add_ir_emitters_fields(data, ir_state);
        if (ir_set) data["status"] = ir_status;
    }
    else if (extension_get && extension_payload_valid)
    {
        data["status"] = 0u;
        data["profileId"] = extension_profile;
        data["profile"] = extension_profile_name(extension_profile);
        data["active"] = true;
        data["persisted"] = true;
    }
    else if (extension_select)
    {
        uint8_t requested_profile =
            request->payload_len == 1u ? request->payload[0] : 0xFFu;
        data["status"] = pkt->len == 1u ? pkt->payload[0] : 0u;
        data["success"] = extension_payload_valid && extension_success;
        if (F4_ExtensionProfile_IsValid(requested_profile))
        {
            data["profileId"] = requested_profile;
            data["profile"] = extension_profile_name(requested_profile);
        }
        data["rebootRequired"] =
            pkt->cmd == UNER_CMD_SET_EXTENSION_PROFILE && ok;
        data["rebooting"] =
            pkt->cmd == UNER_CMD_REBOOT_INTO_PROFILE && ok;
        if (pkt->cmd == UNER_CMD_REBOOT_INTO_PROFILE && ok)
            data["reconnectDelayMs"] = 3000u;
    }
    else if (pkt->cmd == UNER_CMD_GET_PREFERENCES && pkt->len >= 2u)
    {
        data["status"] = pkt->payload[0];
        data["mode"] = pkt->payload[1];
        if (pkt->payload[1] == 0u)
        {
            F4_PreferencesSnapshot snapshot;
            if (F4_DecodePreferencesSnapshot(pkt->payload, pkt->len,
                                             &snapshot) == F4_DECODE_OK)
            {
                data["descriptorCount"] = snapshot.descriptor_count;
                data["fieldMask"] = snapshot.field_mask;
                data["encoderInverted"] = snapshot.encoder_inverted != 0u;
                data["screenLockEnabled"] = snapshot.screen_lock_enabled != 0u;
                data["screenLockTimeoutSec"] = snapshot.screen_lock_timeout_sec;
                data["leftMinPwm"] = snapshot.left_min_pwm;
                data["rightMinPwm"] = snapshot.right_min_pwm;
                data["systemActionTimeoutSec"] = snapshot.system_action_timeout_sec;
                data["usbAlivePeriodMs"] = snapshot.usb_alive_period_ms;
                data["oledRotationDegrees"] = snapshot.oled_rotation_degrees;
                data["oledAutoRotationEnabled"] = snapshot.oled_auto_rotation_enabled != 0u;
                data["unerRouterForwardingEnabled"] = snapshot.uner_router_forwarding_enabled != 0u;
                data["blackLineWidthMm"] = snapshot.black_line_width_mm;
                data["mpuCalibrationValid"] = snapshot.mpu_calibration_valid != 0u;
                if (snapshot.has_pin_lock_enabled)
                    data["pinLockEnabled"] = snapshot.pin_lock_enabled != 0u;
                if (snapshot.has_web_alive_period_ms)
                {
                    s_bridge.f4_alive_period_ms = normalize_f4_alive_period(
                        snapshot.web_alive_period_ms);
                    data["webAlivePeriodMs"] = s_bridge.f4_alive_period_ms;
                }
                data["recordSize"] = snapshot.record_size;
                data["recordCapacity"] = snapshot.record_capacity;
            }
        }
        else if (pkt->payload[1] == 1u && pkt->len >= 18u)
        {
            data["index"] = pkt->payload[2];
            data["descriptorCount"] = pkt->payload[3];
            data["fieldMask"] = read_u32_le(&pkt->payload[4]);
            data["valueType"] = pkt->payload[8];
            data["min"] = read_u16_le(&pkt->payload[9]);
            data["max"] = read_u16_le(&pkt->payload[11]);
            data["step"] = read_u16_le(&pkt->payload[13]);
            data["default"] = read_u16_le(&pkt->payload[15]);
            uint8_t key_len = pkt->payload[17];
            if ((uint16_t)(18u + key_len) == pkt->len)
            {
                char key[65];
                uint8_t copy_len = key_len > 64u ? 64u : key_len;
                memcpy(key, &pkt->payload[18], copy_len); key[copy_len] = '\0';
                data["key"] = key;
            }
        }
    }
    else if (pkt->cmd == STM_CMD_GET_MPU_SNAPSHOT && pkt->len == 42)
    {
        add_mpu_snapshot_fields(data, pkt->payload);
    }
    else if (pkt->cmd == STM_CMD_SET_MPU_STREAM && pkt->len == 4u)
    {
        F4_StreamStateResponse response;
        if (F4_DecodeStreamStateResponse(pkt->payload, pkt->len, &response) == F4_DECODE_OK)
        {
            data["status"] = response.status;
            data["active"] = response.active != 0u;
            data["periodMs"] = response.period_ms;
        }
    }
    else if (pkt->cmd == STM_CMD_STOP_MPU_STREAM && pkt->len == 1u)
    {
        data["status"] = pkt->payload[0];
        data["active"] = false;
    }
    else if ((pkt->cmd == STM_CMD_SET_I2C_POLICY || pkt->cmd == STM_CMD_GET_I2C_POLICY) && pkt->len >= 2)
    {
        data["status"] = pkt->payload[0];
        data["currentMode"] = pkt->payload[1];
    }
    else if (pkt->cmd == STM_CMD_GET_CAR_MODE && pkt->len >= 1)
    {
        data["mode"] = pkt->payload[0];
    }
    else if (pkt->cmd == STM_CMD_SET_CAR_MODE && pkt->len >= 2)
    {
        data["status"] = pkt->payload[0];
        data["mode"] = pkt->payload[1];
    }
    else if (pkt->cmd == UNER_CMD_AUTH_VALIDATE_PIN && pkt->len == 9u)
    {
        data["status"] = pkt->payload[0];
        data["authRequestId"] = pkt->payload[1];
        data["scope"] = pkt->payload[2];
        data["granted"] = pkt->payload[3] != 0u;
        uint32_t ttl = (uint32_t)pkt->payload[4] | ((uint32_t)pkt->payload[5] << 8) |
                       ((uint32_t)pkt->payload[6] << 16) | ((uint32_t)pkt->payload[7] << 24);
        data["ttlMs"] = ttl;
        data["attemptsLeft"] = pkt->payload[8];
        data["authSource"] = "stm32";
    }
    else if (pkt->cmd == STM_CMD_AUTH_REMOTE_RESULT && pkt->len == 6u)
    {
        uint32_t ttl = (uint32_t)pkt->payload[2] | ((uint32_t)pkt->payload[3] << 8) |
                       ((uint32_t)pkt->payload[4] << 16) | ((uint32_t)pkt->payload[5] << 24);
        data["status"] = pkt->payload[0];
        data["authenticated"] = pkt->payload[1] != 0u;
        data["ttlMs"] = ttl;
        data["authSource"] = "stm32";
    }
    else if (pkt->cmd == 0x63u && pkt->len == 5u)
    {
        F4_SetPreferencesResponse response;
        if (F4_DecodeSetPreferencesResponse(pkt->payload, pkt->len, &response) == F4_DECODE_OK)
        {
            data["status"] = response.status;
            data["changedMask"] = response.changed_mask;
        }
    }

    else if (pkt->cmd>=0x23u && pkt->cmd<=0x2Bu && pkt->len==128u && pkt->payload[1]==1u) {
        const uint8_t *p=pkt->payload;
        const char *bytes[]={"status","version","kind","state","reason","carMode","flags","obstacleMask"};
        for(unsigned n=0;n<8;++n)data[bytes[n]]=p[n];
        const char *flags[]={"mpuReady","mpuCalibrated","motorCalibrated","irCalibrated","balanceActive","balanceSettled","safetyStop","lineCalibrated"};
        for(unsigned n=0;n<8;++n)data[flags[n]]=(p[6]&(1u<<n))!=0;
        data["missionId"]=read_u32_le(p+8);data["elapsedMs"]=read_u32_le(p+12);
        const char *values[]={"xCm","yCm","goalXCm","goalYCm","yawDeg","speedCmS","percent"};
        for(unsigned n=0;n<7;++n)data[values[n]]=line_read_f32(p+16+n*4);
        data["rampMs"]=read_u32_le(p+44);data["measurementMs"]=read_u32_le(p+48);data["pwmHz"]=line_read_f32(p+52);
        const char *metrics[]={"leftDutyMean","rightDutyMean","leftDutyMax","rightDutyMax","leftDutySeconds","rightDutySeconds","leftPwmCycles","rightPwmCycles"};
        for(unsigned n=0;n<8;++n)data[metrics[n]]=line_read_f32(p+56+n*4);
        const char *times[]={"leftForwardMs","leftReverseMs","rightForwardMs","rightReverseMs"};
        for(unsigned n=0;n<4;++n)data[times[n]]=read_u32_le(p+88+n*4);
        data["pointCount"]=p[104];data["phase"]=p[105];data["persisted"]=p[106]!=0;data["resultValid"]=p[107]!=0;
        data["referenceCm"]=line_read_f32(p+108);data["yawErrorMax"]=line_read_f32(p+112);data["setupMs"]=read_u32_le(p+116);
        data["objectsEnabled"]=p[120]!=0;data["lightsEnabled"]=p[121]!=0;data["objectsFresh"]=p[122]!=0;data["lineFresh"]=p[123]!=0;data["revision"]=read_u32_le(p+124);
        data["positionEstimated"]=true;
    }
    else if ((pkt->cmd==0x29u||pkt->cmd==0x2Au)&&pkt->len==88u && pkt->payload[1]==1u) {
        const uint8_t *p=pkt->payload;data["status"]=p[0];data["version"]=p[1];data["pointCount"]=p[2];data["irCalibrated"]=p[3]!=0;data["revision"]=read_u32_le(p+4);
        JsonArray points=data.createNestedArray("points");
        for(unsigned n=0;n<p[2] && n<5;++n){JsonObject v=points.createNestedObject();v["percent"]=line_read_f32(p+8+n*16);v["speedCmS"]=line_read_f32(p+12+n*16);v["rampMs"]=read_u32_le(p+16+n*16);v["runs"]=read_u32_le(p+20+n*16);}
    }

    else if (pkt->cmd >= UNER_CMD_GET_MOTION_STATE && pkt->cmd <= UNER_CMD_SET_MOTOR_TEST && pkt->len == 8u) {
        data["status"] = pkt->payload[0]; data["motionArmed"] = pkt->payload[1] != 0;
        data["balanceActive"] = pkt->payload[2] != 0; data["commandActive"] = pkt->payload[3] != 0;
        data["safetyStop"] = pkt->payload[4] != 0; data["rejectReason"] = pkt->payload[5];
        data["carMode"] = pkt->payload[6]; data["motorTestActive"] = pkt->payload[7] != 0;
    }
    else if (pkt->cmd == 0x64u && pkt->len == 92u)
    {
        F4_TelemetrySnapshot snapshot;
        if (F4_DecodeTelemetrySnapshot(pkt->payload, pkt->len, &snapshot) == F4_DECODE_OK)
        {
            data["status"] = snapshot.status;
            data["backend"] = snapshot.backend;
            data["wifiMode"] = snapshot.wifi_mode;
            data["bridgeConnected"] = snapshot.bridge_connected != 0u;
            data["capabilityMask"] = snapshot.capability_mask;
            data["i2cPolicy"] = snapshot.i2c_policy;
            data["motionArmed"] = snapshot.motion_armed != 0u;
            data["balanceActive"] = snapshot.balance_active != 0u;
            data["commandActive"] = snapshot.command_active != 0u;
            data["safetyStop"] = snapshot.safety_stop != 0u;
            data["pitchMeasuredDeg"] = snapshot.pitch_measured_deg;
            data["pitchSetpointDeg"] = snapshot.pitch_setpoint_deg;
            data["balanceOutputPercent"] = snapshot.balance_output_percent;
            data["leftOutputPercent"] = snapshot.left_output_percent;
            data["rightOutputPercent"] = snapshot.right_output_percent;
            JsonObject pid = data.createNestedObject("pid");
            pid["kp"] = snapshot.kp; pid["ki"] = snapshot.ki; pid["kd"] = snapshot.kd;
            JsonObject attitude = data.createNestedObject("attitude");
            attitude["roll"] = snapshot.roll_deg; attitude["pitch"] = snapshot.pitch_deg; attitude["yaw"] = snapshot.yaw_magnetic_deg;
            data["mpuStreamActive"] = snapshot.mpu_stream_active != 0u;
            data["mpuStreamPeriodMs"] = snapshot.mpu_stream_period_ms;
            data["leftMinPwm"] = snapshot.left_min_pwm;
            data["rightMinPwm"] = snapshot.right_min_pwm;
            JsonArray raw = data.createNestedArray("rawAdc");
            for (uint8_t i = 0; i < 8u; ++i) raw.add(snapshot.raw_adc[i]);
            data["pidErrorDeg"] = snapshot.pid_error_deg;
            data["pidIntegralDegS"] = snapshot.pid_integral_deg_s;
            data["pidDerivativeDegS"] = snapshot.pid_derivative_deg_s;
        }
    }
    else if (pkt->cmd == 0x65u && pkt->len == 13u)
    {
        F4_BalancePidResponse response;
        if (F4_DecodeBalancePidResponse(pkt->payload, pkt->len, &response) == F4_DECODE_OK)
        {
            data["status"] = response.status; data["kp"] = response.kp; data["ki"] = response.ki; data["kd"] = response.kd;
        }
    }
    else if (pkt->cmd == 0x66u && pkt->len == 14u)
    {
        F4_SetBalancePidResponse response;
        if (F4_DecodeSetBalancePidResponse(pkt->payload, pkt->len, &response) == F4_DECODE_OK)
        {
            data["status"] = response.status; data["changedMask"] = response.changed_mask;
            data["kp"] = response.kp; data["ki"] = response.ki; data["kd"] = response.kd;
        }
    }
    else if (pkt->cmd == 0x67u && pkt->len == 64u)
    {
        F4_ControlSnapshot snapshot;
        if (F4_DecodeControlSnapshot(pkt->payload, pkt->len, &snapshot) == F4_DECODE_OK)
        {
            data["status"] = snapshot.status; data["carMode"] = snapshot.car_mode;
            data["carModeName"] = car_mode_name(snapshot.car_mode);
            data["lineFollowState"] = snapshot.line_follow_fsm; data["linePattern"] = snapshot.stable_line_pattern;
            data["lineConfidence"] = snapshot.line_confidence_percent; data["lineAmbiguous"] = snapshot.line_ambiguous != 0u;
            data["lineRejectReason"] = snapshot.line_reject_reason; data["lineWidthMm"] = snapshot.line_width_mm;
            data["lateralErrorMm"] = snapshot.lateral_error_mm; data["normalizedLateralError"] = snapshot.normalized_lateral_error;
            data["linearAccelerationMps2"] = snapshot.linear_acceleration_m_s2; data["estimatedVelocityMps"] = snapshot.estimated_velocity_m_s;
            data["yawRateDegS"] = snapshot.yaw_rate_deg_s; data["estimatorConfidence"] = snapshot.estimator_confidence_percent;
            data["estimatorFlags"] = snapshot.estimator_flags; data["commonMotionOutputPercent"] = snapshot.common_motion_output_percent;
            data["differentialMotionOutputPercent"] = snapshot.differential_motion_output_percent; data["turnLimitPercent"] = snapshot.turn_limit_percent;
            data["saturationLimited"] = snapshot.saturation_limited != 0u; data["usbActive"] = snapshot.usb_active != 0u;
            data["usbAlivePeriodMs"] = snapshot.usb_alive_period_ms; data["usbAliveAcceptedCount"] = snapshot.usb_alive_accepted_count;
            data["usbRxOverflowCount"] = snapshot.usb_rx_overflow_count; data["usbTxQueueFullCount"] = snapshot.usb_tx_queue_full_count;
            data["usbQueuedTxCount"] = snapshot.usb_queued_tx_count; data["usbTxDropCount"] = snapshot.usb_tx_drop_count;
        }
    }
    else if (pkt->cmd == 0x68u)
    {
        F4_FirmwareInfo info;
        if (F4_DecodeFirmwareInfo(pkt->payload, pkt->len, &info) == F4_DECODE_OK)
        {
            data["status"] = info.status; data["name"] = info.name; data["version"] = info.version; data["target"] = info.target;
        }
    }
    else if (pkt->cmd == 0x69u)
    {
        F4_BuildInfo info;
        if (F4_DecodeBuildInfo(pkt->payload, pkt->len, &info) == F4_DECODE_OK)
        {
            data["status"] = info.status; data["date"] = info.date; data["time"] = info.time;
            data["profile"] = info.profile; data["compiler"] = info.compiler;
        }
    }
    else if (pkt->cmd == UNER_CMD_GET_BOOT_REPORT && pkt->len == F4_BOOT_REPORT_PAYLOAD_SIZE)
    {
        (void)add_boot_report_fields(data, pkt->payload, pkt->len);
    }
    else if (pkt->cmd == 0x6Au && pkt->len == 56u)
    {
        (void)add_ir_snapshot_fields(data, pkt->payload, pkt->len);
    }
    else if (pkt->cmd == 0x6Bu && pkt->len == 4u)
    {
        F4_StreamStateResponse response;
        if (F4_DecodeStreamStateResponse(pkt->payload, pkt->len, &response) == F4_DECODE_OK)
        {
            data["status"] = response.status; data["active"] = response.active != 0u; data["periodMs"] = response.period_ms;
        }
    }
    else if (pkt->cmd == 0x6Cu && pkt->len == 1u)
    {
        data["status"] = pkt->payload[0]; data["active"] = false;
    }
    else if (pkt->cmd == 0x77u)
    {
        F4_GetApConfigResponse ap;
        if (F4_DecodeGetApConfigResponse(pkt->payload, pkt->len, &ap) == F4_DECODE_OK)
        {
            data["status"] = ap.status;
            data["applyState"] = ap.apply_state;
            data["applyStateName"] = ap_apply_state_name(ap.apply_state);
            data["ssid"] = ap.ssid;
            data["protected"] = ap.ap_protected != 0u;
            data["passwordSet"] = ap.password_set != 0u;
            char ip_str[16];
            snprintf(ip_str, sizeof(ip_str), "%u.%u.%u.%u", ap.ip[0], ap.ip[1], ap.ip[2], ap.ip[3]);
            data["ip"] = ip_str;
        }
    }
    else if (pkt->cmd == 0x78u && pkt->len == 3u)
    {
        F4_SetApConfigResponse ap;
        if (F4_DecodeSetApConfigResponse(pkt->payload, pkt->len, &ap) == F4_DECODE_OK)
        {
            data["status"] = ap.status;
            data["changedMask"] = ap.changed_mask;
            data["applyState"] = ap.apply_state;
            data["applyStateName"] = ap_apply_state_name(ap.apply_state);
            WifiRadioMode wm = s_bridge.wifi.GetMode();
            data["apActive"] = (wm == WIFI_RADIO_AP || wm == WIFI_RADIO_AP_STA);
            data["connectionWillDrop"] = (wm == WIFI_RADIO_AP || wm == WIFI_RADIO_AP_STA) &&
                                         (ap.changed_mask & (AP_FIELD_SSID | AP_FIELD_PASSWORD | AP_FIELD_PROTECTED)) != 0u;
        }
    }
    else
    {
        JsonArray bytes = data.createNestedArray("payload");
        for (uint8_t i = 0; i < pkt->len; ++i) bytes.add(pkt->payload[i]);
    }

    send_doc_to_client(request->client_id, doc);
}

static UtmSendResult transaction_send(void *ctx, const UtmRequest *request)
{
    (void)ctx;
    if (!s_bridge.app || !request || request->payload_len > UNER_MAX_PAYLOAD) return UTM_SEND_ERROR;
    UNER_Status status = s_bridge.backend.send_from_node(s_bridge.backend.context, request->node, request->dst_node,
                                                   request->command_id, request->payload,
                                                   (uint8_t)request->payload_len);
    if (status == UNER_ERR_BUSY) return UTM_SEND_BUSY;
    if (status != UNER_OK) return UTM_SEND_ERROR;
    return UTM_SEND_OK;
}

static void clear_stm_session_and_streams(WsmSession *session)
{
    if (!session) return;
    (void)srw_remove_client(&s_bridge.stream_waiters, session->client_id, session->generation);
    (void)sb_remove_client(&s_bridge.streams, session->client_id, session->generation);
    (void)wsm_set_stm_auth(&s_bridge.sessions, session->client_id, session->generation, 0u, 0u, s_bridge.platform.now_ms());
}

static void invalidate_stm_session_and_streams(WsmSession *session, const char *reason)
{
    if (!session) return;
    if (session->state == WSM_SLOT_ACTIVE && session->stm_authenticated)
        emit_auth_session_invalidated(session->client_id, reason);
    clear_stm_session_and_streams(session);
}

static void revoke_all_stm_sessions_and_streams(const char *reason)
{
    for (uint8_t i = 0u; i < WSM_MAX_SESSIONS; ++i)
    {
        WsmSession *session = &s_bridge.sessions.sessions[i];
        if (session->state == WSM_SLOT_ACTIVE)
            invalidate_stm_session_and_streams(session, reason);
    }
    s_bridge.telem_stop.active = 0u;
}

static void transaction_complete(void *ctx, const UtmRequest *request,
                                 UtmCompletionStatus status, const UtmFrame *frame)
{
    (void)ctx;
    if (!request) return;

    if (request->internal_kind == UTM_INTERNAL_OLED_CANVAS)
    {
        oled_canvas_transaction_complete(status, frame);
        return;
    }

    if (request->internal_kind == UTM_INTERNAL_STREAM_MPU ||
        request->internal_kind == UTM_INTERNAL_STREAM_IR ||
        request->internal_kind == UTM_INTERNAL_STREAM_RECONCILE)
    {
        // Reconstruir exactamente la accion que genero esta transaccion. No
        // usar `state->pending_action`: mientras UART estaba en vuelo pudo
        // cambiar la demanda y ese slot ya puede describir otra accion.
        SbAction action = {};
        action.stream = request->command_id == 0x6Bu || request->command_id == 0x6Cu
                            ? SB_STREAM_IR : SB_STREAM_MPU;
        action.type = request->command_id == 0x61u || request->command_id == 0x6Bu
                          ? SB_ACTION_SET : SB_ACTION_STOP;
        action.endpoint.client_id = request->client_id;
        action.endpoint.generation = request->generation;
        action.endpoint.node = request->node;
        action.reconcile = request->internal_kind == UTM_INTERNAL_STREAM_RECONCILE ? 1u : 0u;
        if (action.type == SB_ACTION_SET && request->payload_len == 3u)
            action.period_ms = read_u16_le(&request->payload[1]);

        uint8_t ok = 0u;
        SrwCompletionStatus waiter_status = SRW_COMPLETED_UART_ERROR;
        if (status == UTM_COMPLETED_TIMEOUT)
            waiter_status = SRW_COMPLETED_TIMEOUT;
        else if (status == UTM_COMPLETED_F4_NACK)
            waiter_status = SRW_COMPLETED_F4_NACK;
        else if (status == UTM_COMPLETED_RESPONSE && frame && frame->payload)
        {
            uint16_t expected_len = action.type == SB_ACTION_SET ? 4u : 1u;
            if (frame->payload_len == expected_len)
            {
                if (frame->payload[0] != 0u)
                {
                    waiter_status = SRW_COMPLETED_F4_NACK;
                }
                else if (action.type == SB_ACTION_STOP)
                {
                    ok = 1u;
                }
                else
                {
                    uint16_t applied_period = read_u16_le(&frame->payload[2]);
                    /* SET solo converge cuando F4 confirma activo y el mismo
                     * periodo que el broker agrego. Un status=0 con otra
                     * configuracion no se publica como exito Web. */
                    ok = frame->payload[1] != 0u && applied_period == action.period_ms;
                }
            }
        }

        uint8_t session_revoked = status == UTM_COMPLETED_F4_NACK && frame &&
                                  frame->payload && frame->payload_len >= 2u &&
                                  frame->payload[1] == 7u;
        SbResult completion_result = sb_complete_action(&s_bridge.streams, &action, ok);
        if (completion_result == SB_STALE_ACTION)
        {
            if (session_revoked)
            {
                WsmSession *session = wsm_find(&s_bridge.sessions, request->client_id, request->generation);
                if (session) invalidate_stm_session_and_streams(session, "stm_auth_rejected");
                sb_backend_reconnected(&s_bridge.streams);
            }
            return;
        }
        if (ok)
            resolve_stream_waiters_if_converged(action.stream);
        else
            complete_stream_waiters(action.stream, waiter_status);
        if (session_revoked)
        {
            WsmSession *session = wsm_find(&s_bridge.sessions, request->client_id, request->generation);
            if (session) invalidate_stm_session_and_streams(session, "stm_auth_rejected");
            sb_backend_reconnected(&s_bridge.streams);
        }
        return;
    }

    if (request->internal_kind == UTM_INTERNAL_AUTH_CLEANUP)
    {
        WsmSession *session = wsm_find_by_node(&s_bridge.sessions, request->node);
        if (session && session->generation == request->generation)
        {
            uint8_t index = (uint8_t)(session->node - WSM_NODE_MIN);
            s_bridge.logout_enqueued[index] = 0u;
            uint32_t now = s_bridge.platform.now_ms();
            bool logout_confirmed = status == UTM_COMPLETED_RESPONSE && frame &&
                                    frame->payload_len == 6u && frame->payload &&
                                    frame->payload[0] == 0u && frame->payload[1] == 0u;
            bool quarantine_expired = s_bridge.cleanup_release_at_ms[index] != 0u &&
                                      (int32_t)(now - s_bridge.cleanup_release_at_ms[index]) >= 0;
            if (logout_confirmed || quarantine_expired)
            {
                uint32_t client_id = session->client_id;
                uint32_t generation = session->generation;
                if (wsm_release(&s_bridge.sessions, client_id, generation) == WSM_OK)
                    clear_cleanup_tracking(index);
            }
            else
            {
                // Logout es idempotente. Se reintenta de forma explicita y
                // acotada por la cuarentena; nunca se reutiliza el nodo solo
                // porque UART haya dado timeout/NACK/error.
                s_bridge.logout_retry_at_ms[index] = now + STM_LOGOUT_RETRY_MS;
            }
        }
        return;
    }

    WsmSession *session = wsm_find(&s_bridge.sessions, request->client_id, request->generation);
    if (!session || session->state != WSM_SLOT_ACTIVE) return;

    if (status == UTM_COMPLETED_TIMEOUT)
    {
        send_error_for_api(request->client_id, request->api_v1,
                           request->request_id, "stm", request->command_name,
                           "timeout", "STM32 response deadline expired");
        return;
    }
    if (status == UTM_COMPLETED_UART_ERROR)
    {
        send_error_for_api(request->client_id, request->api_v1,
                           request->request_id, "stm", request->command_name,
                           "uart_error", "UART frame could not be queued");
        return;
    }
    if (status == UTM_COMPLETED_CANCELED) return;
    if (status == UTM_COMPLETED_F4_NACK)
    {
        uint8_t nack_status = frame && frame->payload_len >= 2u ? frame->payload[1] : 0xFFu;
        if (nack_status == 7u)
        {
            invalidate_stm_session_and_streams(session, "stm_auth_rejected");
            if (stream_broker_has_runtime_state()) sb_backend_reconnected(&s_bridge.streams);
        }
        send_error_for_api(request->client_id, request->api_v1,
                           request->request_id, "stm", request->command_name,
                           "f4_nack", "STM32 rejected the command");
        return;
    }
    if (!frame) return;

    if (request->command_id == STM_CMD_RESET_MCU && status == UTM_COMPLETED_ACK)
    {
        if (frame->payload_len != 2u || frame->payload[0] != STM_CMD_RESET_MCU ||
            frame->payload[1] != 0u)
        {
            send_error_for_api(request->client_id, request->api_v1,
                               request->request_id, "stm", request->command_name,
                               "f4_nack", "STM32 rejected the reboot command");
            return;
        }
        uint8_t ack_status = 0u;
        UNER_Packet packet = {};
        packet.src = frame->src;
        packet.dst = frame->dst;
        packet.cmd = STM_CMD_RESET_MCU;
        packet.len = 1u;
        packet.payload = &ack_status;
        complete_response_from_manual_status(request, &packet);

        // The ACK definitively resolves this request. Commands queued behind a
        // reboot must not leak into the next F4 boot/session generation.
        utm_cancel_all(&s_bridge.transactions);
        revoke_all_stm_sessions_and_streams("stm_reset");
        s_bridge.f4_backend_online = 0u;
        s_bridge.f4_alive_seen = 0u;
        s_bridge.last_f4_alive_ms = 0u;
        s_bridge.f4_offline_since_ms = 0u;
        return;
    }

    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command(request->command_id, spec_storage);
    if (status == UTM_COMPLETED_RESPONSE &&
        (!spec || !F4_CommandRegistry_IsResponseLengthValid(spec, frame->payload_len)))
    {
        send_error_for_api(request->client_id, request->api_v1,
                           request->request_id, "stm", request->command_name,
                           "uart_error", "STM32 response length violates the known contract");
        return;
    }
    bool accepted_by_f4 = true;
    if (status == UTM_COMPLETED_ACK)
        accepted_by_f4 = frame->payload_len >= 2u && frame->payload[1] == 0u;
    else if (status == UTM_COMPLETED_RESPONSE &&
             is_extension_profile_selection_command(request->command_id))
    {
        bool extension_success = false;
        accepted_by_f4 = F4_ExtensionProfile_DecodeResult(
            frame->payload, (uint8_t)frame->payload_len, &extension_success) &&
                         extension_success;
    }
    else if (status == UTM_COMPLETED_RESPONSE && f4_payload_has_status_byte(request->command_id))
        accepted_by_f4 = frame->payload_len >= 1u && frame->payload[0] == 0u;
    if (accepted_by_f4 && spec && spec->privilege == F4_PRIVILEGE_STM_SESSION &&
        session->stm_authenticated)
        (void)wsm_set_stm_auth(&s_bridge.sessions, session->client_id, session->generation, 1u, 60000u, s_bridge.platform.now_ms());

    if (accepted_by_f4 && request->command_id == UNER_CMD_SET_PREFERENCES &&
        request->payload_len >= 6u &&
        (read_u32_le(request->payload) & STM_PREF_FIELD_WEB_ALIVE_PERIOD) != 0u)
    {
        /* webAlivePeriodMs es el ultimo descriptor y, por contrato, su U16
         * queda al final aun cuando el patch incluya otros campos. Solo se
         * adopta despues de una response F4 exitosa. */
        s_bridge.f4_alive_period_ms = normalize_f4_alive_period(
            read_u16_le(&request->payload[request->payload_len - 2u]));
    }

    if (request->command_id == UNER_CMD_AUTH_VALIDATE_PIN && frame->payload_len == 9u)
    {
        if (request->payload_len < 2u ||
            frame->payload[1] != request->payload[0] ||
            frame->payload[2] != request->payload[1])
        {
            send_error_for_api(request->client_id, request->api_v1,
                               request->request_id, "esp", request->command_name,
                               "uart_error", "Authentication correlation mismatch");
            return;
        }
        uint8_t granted = frame->payload[0] == 0u && frame->payload[3] != 0u;
        uint8_t scope = frame->payload[2];
        uint32_t ttl = (uint32_t)frame->payload[4] | ((uint32_t)frame->payload[5] << 8) |
                       ((uint32_t)frame->payload[6] << 16) | ((uint32_t)frame->payload[7] << 24);
        if (scope == STM_AUTH_SCOPE_LOGIN)
        {
            if (granted)
                (void)wsm_set_stm_auth(&s_bridge.sessions, session->client_id,
                                       session->generation, 1u, ttl, s_bridge.platform.now_ms());
            else
                invalidate_stm_session_and_streams(session, "stm_auth_rejected");
        }
        else if (scope == STM_AUTH_SCOPE_CHANGE && granted)
        {
            // Cambiar el PIN invalida toda sesion remota. Ningun scope distinto
            // de login puede conceder stm_authenticated.
            for (uint8_t i = 0; i < WSM_MAX_SESSIONS; ++i)
            {
                WsmSession *other = &s_bridge.sessions.sessions[i];
                if (other->state == WSM_SLOT_ACTIVE)
                    invalidate_stm_session_and_streams(other, "pin_changed");
            }
        }
    }
    else if (request->command_id == STM_CMD_AUTH_REMOTE_RESULT && frame->payload_len == 6u)
    {
        // Query may refresh our own login, never adopt a stale F4 node grant
        // from another socket (including peers on the same IP).
        uint8_t authenticated = frame->payload[0] == 0u && frame->payload[1] != 0u &&
            wsm_touch_stm_auth(&s_bridge.sessions, session->client_id,
                               session->generation, s_bridge.platform.now_ms()) == WSM_OK;
        uint32_t ttl = (uint32_t)frame->payload[2] | ((uint32_t)frame->payload[3] << 8) |
                       ((uint32_t)frame->payload[4] << 16) | ((uint32_t)frame->payload[5] << 24);
        if (authenticated)
            (void)wsm_set_stm_auth(&s_bridge.sessions, session->client_id, session->generation, 1u, ttl, s_bridge.platform.now_ms());
        else
            invalidate_stm_session_and_streams(session, "stm_auth_rejected");
    }

    uint8_t payload_copy[UNER_MAX_PAYLOAD];
    uint8_t payload_len = (uint8_t)frame->payload_len;
    if (payload_len > 0u && frame->payload) memcpy(payload_copy, frame->payload, payload_len);
    if (request->command_id == STM_CMD_AUTH_REMOTE_RESULT && payload_len == 6u &&
        !session->stm_authenticated)
    {
        payload_copy[1] = 0u;
        memset(&payload_copy[2], 0, 4u);
    }
    UNER_Packet packet = {};
    packet.src = frame->src; packet.dst = frame->dst; packet.cmd = frame->command_id;
    packet.len = payload_len; packet.payload = payload_copy;

    if (status == UTM_COMPLETED_ACK)
    {
        uint8_t ack_status = frame->payload_len >= 2u ? frame->payload[1] : 0u;
        packet.cmd = request->command_id; packet.len = 1u; packet.payload = &ack_status;
        complete_response_from_manual_status(request, &packet);
    }
    else if (request->command_id == UNER_CMD_GET_CURRENT_SCREEN)
        complete_response_from_screen_report(request, &packet);
    else
    {
        complete_response_from_payload(request, &packet);
    }
    if (request->command_id == UNER_CMD_REBOOT_INTO_PROFILE &&
        accepted_by_f4)
    {
        // The response is sent before F4 resets. Cancel stale work and force
        // the same re-authentication path used by an explicit reset.
        utm_cancel_all(&s_bridge.transactions);
        revoke_all_stm_sessions_and_streams("stm_profile_reset");
        s_bridge.f4_backend_online = 0u;
        s_bridge.f4_alive_seen = 0u;
        s_bridge.last_f4_alive_ms = 0u;
        s_bridge.f4_offline_since_ms = 0u;
    }
}

static void emit_generic_stm_event(const UNER_Packet *pkt)
{
    debug_log_ws_uner_packet("stm.event", pkt);
    StaticJsonDocument<640> doc;
    JsonObject data = make_event_base(doc, "stm.event", "stm", nullptr);
    data["cmd"] = pkt->cmd;
    data["src"] = pkt->src;
    data["dst"] = pkt->dst;
    data["len"] = pkt->len;
    JsonArray bytes = data.createNestedArray("payload");
    for (uint8_t i = 0; i < pkt->len; ++i) bytes.add(pkt->payload[i]);
    debug_log_ws_send("broadcast", "device.event", "stm.event", pkt);
    broadcast_doc_stm_authorized(doc);
}

static void emit_boot_report_event(const UNER_Packet *pkt)
{
    if (!pkt || pkt->len != F4_BOOT_REPORT_PAYLOAD_SIZE) return;
    StaticJsonDocument<768> doc;
    JsonObject data = make_event_base(doc, "bootReport", "stm", nullptr);
    data["cmd"] = pkt->cmd;
    if (!add_boot_report_fields(data, pkt->payload, pkt->len)) return;
    broadcast_doc_stm_authorized(doc);
}

static bool validate_ap_client_disconnect_params(JsonObject root_payload,
                                                 JsonObject &params,
                                                 const char **message)
{
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    const char *mac = params["mac"] | "";
    if (strlen(mac) != 17u)
    {
        if (message) *message = "mac must use AA:BB:CC:DD:EE:FF";
        return false;
    }
    for (JsonPair pair : params)
    {
        if (strcmp(pair.key().c_str(), "mac") != 0)
        {
            if (message) *message = "only mac is accepted";
            return false;
        }
    }
    return true;
}

static void emit_car_mode_changed_event(const UNER_Packet *pkt)
{
    if (!pkt || pkt->len != 1u) return;
    const uint8_t mode = pkt->payload[0];
    StaticJsonDocument<384> doc;
    JsonObject data = make_event_base(doc, "carModeChanged", "stm", nullptr);
    data["cmd"] = pkt->cmd;
    data["mode"] = mode;
    data["modeName"] = car_mode_name(mode);
    if (mode == CAR_MODE_GOTO) data["modeStatus"] = "without_target";
    broadcast_doc_stm_authorized(doc);
}

static void emit_qt_usb_state_event(const UNER_Packet *pkt)
{
    if (!pkt || pkt->len != 1u || pkt->payload[0] > 1u) return;
    StaticJsonDocument<384> doc;
    JsonObject data = make_event_base(doc, "qtUsbStateChanged", "stm", nullptr);
    data["cmd"] = pkt->cmd;
    data["connected"] = pkt->payload[0] != 0u;
    data["transport"] = "USB_CDC";
    data["peer"] = "PC_QT";
    broadcast_doc_stm_authorized(doc);
}

static void emit_qt_alive_event(const UNER_Packet *pkt)
{
    if (!pkt || pkt->src != UNER_NODE_PC_QT ||
        pkt->dst != UNER_NODE_WEB_APP || pkt->cmd != UNER_CMD_PING ||
        pkt->len != 0u) return;

    StaticJsonDocument<448> doc;
    JsonObject data = make_event_base(doc, "qtAlive", "qt", nullptr);
    data["cmd"] = pkt->cmd;
    data["src"] = pkt->src;
    data["dst"] = pkt->dst;
    data["route"] = (uint8_t)((pkt->src << 4u) | pkt->dst);
    data["receivedAtMs"] = s_bridge.platform.now_ms();
    data["forwardedBy"] = "F4";
    data["transport"] = "USB_CDC>UART1_ESP";
    broadcast_doc_stm_authorized(doc);
}

