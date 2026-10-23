// Private bridge implementation; included once by ws_json_bridge.cpp.
static void dispatch_stm_screen_code_command(uint32_t client_id, const char *request_id, const char *command,
                                             JsonObject params, uint8_t stm_cmd)
{
    uint32_t screen_code = 0;
    uint8_t payload[4];

    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "screenCode must be uint32 or hex string");
        return;
    }

    write_u32_le(screen_code, payload);
    (void)start_stm_request(client_id, request_id, command, stm_cmd, stm_cmd, payload, sizeof(payload), 0);
}

static void dispatch_stm_press_command(uint32_t client_id, const char *request_id, const char *command,
                                       JsonObject params, uint8_t stm_cmd)
{
    uint32_t screen_code = 0;
    uint8_t press_kind = 0;
    uint8_t payload[5];

    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "screenCode must be uint32 or hex string");
        return;
    }
    if (!read_press_kind(params, &press_kind))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "pressKind must be short or long");
        return;
    }

    write_u32_le(screen_code, payload);
    payload[4] = press_kind;
    (void)start_stm_request(client_id, request_id, command, stm_cmd, stm_cmd, payload, sizeof(payload), 0);
}

static void dispatch_stm_menu_click(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t screen_code = 0;
    uint32_t item = 0;
    uint8_t payload[5];

    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "screenCode must be uint32 or hex string");
        return;
    }
    if (!read_json_u32(params, "item", &item) || item < 1u || item > 3u)
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "item must be 1, 2 or 3");
        return;
    }

    write_u32_le(screen_code, payload);
    payload[4] = (uint8_t)item;
    (void)start_stm_request(client_id, request_id, command, STM_CMD_MENU_ITEM_CLICK, STM_CMD_MENU_ITEM_CLICK, payload, sizeof(payload), 0);
}

static void dispatch_stm_screen_page(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t screen_code = 0;
    uint8_t direction = 0;
    uint8_t payload[5];

    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "screenCode must be uint32 or hex string");
        return;
    }
    if (!read_screen_page_direction(params, &direction))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "direction must be up or down");
        return;
    }

    write_u32_le(screen_code, payload);
    payload[4] = direction;
    (void)start_stm_request(client_id, request_id, command, STM_CMD_REQUEST_SCREEN_PAGE, STM_CMD_REQUEST_SCREEN_PAGE, payload, sizeof(payload), 0);
}

static void dispatch_stm_empty_command(uint32_t client_id, const char *request_id, const char *command, uint8_t stm_cmd)
{
    (void)start_stm_request(client_id, request_id, command, stm_cmd, stm_cmd, nullptr, 0, 0);
}

static void dispatch_stm_auth_remote_result(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t screen_code = 0;
    uint32_t result_code = 0;
    uint32_t attempts_left = 0;
    uint8_t payload[6];

    if (!read_json_u32(params, "screenCode", &screen_code) ||
        !read_json_u32(params, "resultCode", &result_code) ||
        !read_json_u32(params, "attemptsLeft", &attempts_left))
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "screenCode, resultCode and attemptsLeft are required");
        return;
    }

    write_u32_le(screen_code, payload);
    payload[4] = (uint8_t)result_code;
    payload[5] = (uint8_t)attempts_left;
    (void)start_stm_request(client_id, request_id, command, STM_CMD_AUTH_REMOTE_RESULT, STM_CMD_AUTH_REMOTE_RESULT, payload, sizeof(payload), 0);
}

static void dispatch_stm_i2c_set_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t mode = 0;
    uint8_t payload[1];

    if (!read_json_u32(params, "mode", &mode) || mode < 1u || mode > 3u)
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "mode must be 1, 2 or 3");
        return;
    }

    payload[0] = (uint8_t)mode;
    (void)start_stm_request(client_id, request_id, command, STM_CMD_SET_I2C_POLICY, STM_CMD_SET_I2C_POLICY, payload, sizeof(payload), 0);
}

static void dispatch_stm_extension_profile_get(uint32_t client_id,
                                               const char *request_id,
                                               const char *command)
{
    F4_ExtensionProfileRequest request;
    if (!F4_ExtensionProfile_BuildGetRequest(&request)) return;
    (void)start_stm_request(client_id, request_id, command,
                            request.command_id, request.command_id,
                            nullptr, request.payload_len, 0u);
}

static void dispatch_stm_extension_profile_select(uint32_t client_id,
                                                  const char *request_id,
                                                  const char *command,
                                                  JsonObject params,
                                                  bool reboot)
{
    uint32_t profile_id = 0u;
    F4_ExtensionProfileRequest request;
    if (!read_json_u32(params, "profileId", &profile_id) ||
        profile_id > UINT8_MAX)
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS",
                   "profileId must be 0 (NRF24) or 1 (Beeper)");
        return;
    }
    bool built = reboot
                     ? F4_ExtensionProfile_BuildRebootRequest((uint8_t)profile_id, &request)
                     : F4_ExtensionProfile_BuildSetRequest((uint8_t)profile_id, &request);
    if (!built)
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS",
                   "profileId must be 0 (NRF24) or 1 (Beeper)");
        return;
    }
    (void)start_stm_request(client_id, request_id, command,
                            request.command_id, request.command_id,
                            request.payload, request.payload_len, 0u);
}

static void cmd_stm_get_ap_fallback(uint32_t client_id, const char *request_id,
                                    const char *command, JsonObject params)
{
    (void)params;
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_GET_AP_FALLBACK,
                          UNER_CMD_GET_AP_FALLBACK, nullptr, 0u, 0u);
}

static bool validate_ap_fallback_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    (void)root_payload;
    if (params.size() != 1u || !params["enabled"].is<bool>()) {
        *message = "enabled must be boolean"; return false;
    }
    return true;
}

static void cmd_stm_set_ap_fallback(uint32_t client_id, const char *request_id,
                                    const char *command, JsonObject params)
{
    const uint8_t payload[5u] = {0u, 0u, 0u, 1u, (uint8_t)(params["enabled"].as<bool>() ? 1u : 0u)};
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_SET_PREFERENCES,
                          UNER_CMD_SET_PREFERENCES, payload, sizeof(payload), 0u);
}

static void cmd_stm_get_ir_emitters(uint32_t client_id, const char *request_id,
                                    const char *command, JsonObject params)
{
    (void)params;
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_GET_IR_EMITTERS,
                            UNER_CMD_GET_IR_EMITTERS, nullptr, 0u, 0u);
}

static void cmd_stm_set_ir_emitters(uint32_t client_id, const char *request_id,
                                    const char *command, JsonObject params)
{
    uint8_t payload[3] = { (uint8_t)params["objects"].as<bool>(),
                           (uint8_t)params["line"].as<bool>(),
                           (uint8_t)params["lights"].as<bool>() };
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_SET_IR_EMITTERS,
                            UNER_CMD_SET_IR_EMITTERS, payload, sizeof(payload), 0u);
}

static const char *car_mode_name(uint8_t mode)
{
    switch (mode) {
    case CAR_MODE_IDLE:   return "IDLE";
    case CAR_MODE_FOLLOW: return "FOLLOW";
    case CAR_MODE_TEST:   return "TEST";
    case CAR_MODE_GOTO:   return "GOTO";
    default:              return "UNKNOWN";
    }
}

static const char *ap_apply_state_name(uint8_t state)
{
    switch (state) {
    case AP_APPLY_STATE_SYNCED:   return "SYNCED";
    case AP_APPLY_STATE_PENDING:  return "PENDING";
    case AP_APPLY_STATE_APPLYING: return "APPLYING";
    case AP_APPLY_STATE_FAILED:   return "FAILED";
    default:                      return "UNKNOWN";
    }
}

static void dispatch_stm_ap_config_get(uint32_t client_id, const char *request_id,
                                       const char *command, JsonObject params)
{
    (void)params;
    (void)start_stm_request(client_id, request_id, command,
                            0x77u, 0x77u, nullptr, 0u, 0);
}

static void dispatch_stm_ap_config_set(uint32_t client_id, const char *request_id,
                                       const char *command, JsonObject params)
{
    uint8_t mask = 0u;
    uint8_t payload[128];
    uint8_t pos = 1u; /* byte 0 reserved for mask */

    /* SSID */
    if (!params["ssid"].isNull()) {
        const char *ssid = params["ssid"] | "";
        uint8_t slen = bounded_strlen_u8(ssid, SSID_MAX_LEN + 1);
        if (slen == 0u || slen > SSID_MAX_LEN) {
            send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "ssid must be 1..32 printable chars");
            return;
        }
        mask |= AP_FIELD_SSID;
        payload[pos++] = slen;
        memcpy(&payload[pos], ssid, slen);
        pos += slen;
    }

    /* Password */
    if (!params["password"].isNull()) {
        const char *pwd = params["password"] | "";
        uint8_t plen = bounded_strlen_u8(pwd, PASS_MAX_LEN + 1);
        if (plen > PASS_MAX_LEN || (plen > 0u && plen < 8u)) {
            send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "password must be empty or 8..63 chars");
            return;
        }
        mask |= AP_FIELD_PASSWORD;
        payload[pos++] = plen;
        if (plen > 0u) { memcpy(&payload[pos], pwd, plen); pos += plen; }
    }

    /* Protected */
    if (!params["protected"].isNull()) {
        bool prot = params["protected"] | false;
        mask |= AP_FIELD_PROTECTED;
        payload[pos++] = prot ? 1u : 0u;
    }

    /* IPv4 */
    if (!params["ip"].isNull()) {
        const char *ip_text = params["ip"] | "0.0.0.0";
        WifiIpv4 ip;
        if (!parse_ipv4(ip_text, ip)) {
            send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "ip must be a valid IPv4 address");
            return;
        }
        mask |= AP_FIELD_IPV4;
        payload[pos++] = ip[0]; payload[pos++] = ip[1];
        payload[pos++] = ip[2]; payload[pos++] = ip[3];
    }

    if (mask == 0u) {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS",
                   "At least one field (ssid, password, protected, ip) is required");
        return;
    }
    payload[0] = mask;
    (void)start_stm_request(client_id, request_id, command,
                            0x78u, 0x78u, payload, pos, 0);
}

static bool validate_stm_ap_config_set_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params)) {
        if (message) *message = "params object is required";
        return false;
    }
    if (params["ssid"].isNull() && params["password"].isNull() &&
        params["protected"].isNull() && params["ip"].isNull()) {
        if (message) *message = "at least one of ssid, password, protected, ip is required";
        return false;
    }
    return true;
}

static void dispatch_stm_set_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t mode = 0;
    uint8_t payload[1];
    if (!read_json_u32(params, "mode", &mode) || mode > CAR_MODE_MAX)
    {
        send_error(client_id, request_id, "stm", command, "ERR_BAD_PARAMS", "mode must be 0 (IDLE), 1 (FOLLOW), 2 (TEST) or 3 (GOTO)");
        return;
    }
    payload[0] = (uint8_t)mode;
    (void)start_stm_request(client_id, request_id, command, STM_CMD_SET_CAR_MODE, STM_CMD_SET_CAR_MODE, payload, sizeof(payload), 0);
}

static void dispatch_stm_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    bool enable = params["enable"] | false;
    uint32_t period_ms = 100u;

    if (!params["periodMs"].isNull())
    {
        (void)read_json_u32(params, "periodMs", &period_ms);
    }
    if (period_ms > 65535u) period_ms = 65535u;

    (void)dispatch_stream_subscription(client_id, request_id, command,
                                       SB_STREAM_MPU, enable, (uint16_t)period_ms);
}

static void dispatch_stm_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    bool enable = params["enable"] | false;
    uint32_t period_ms = 100u;
    if (!params["periodMs"].isNull()) (void)read_json_u32(params, "periodMs", &period_ms);
    if (period_ms > 65535u) period_ms = 65535u;
    (void)dispatch_stream_subscription(client_id, request_id, command,
                                       SB_STREAM_IR, enable, (uint16_t)period_ms);
}

static void dispatch_stm_set_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint32_t mask = 0u;
    (void)read_json_u32(params, "fieldMask", &mask);
    JsonArray values = params["values"].as<JsonArray>();
    uint8_t payload[64];
    write_u32_le(mask, payload);
    uint8_t length = 4u;
    for (JsonVariant value : values) payload[length++] = (uint8_t)value.as<int>();
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_SET_PREFERENCES,
                            UNER_CMD_SET_PREFERENCES, payload, length, 0u);
}

static void write_f32_le(float value, uint8_t out[4])
{
    uint32_t bits = 0u;
    memcpy(&bits, &value, sizeof(bits));
    write_u32_le(bits, out);
}

static void dispatch_stm_set_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint8_t payload[13];
    uint8_t length = 1u;
    uint8_t mask = 0u;
    const char *keys[] = {"kp", "ki", "kd"};
    for (uint8_t i = 0; i < 3u; ++i)
    {
        if (params[keys[i]].isNull()) continue;
        mask = (uint8_t)(mask | (uint8_t)(1u << i));
        write_f32_le(params[keys[i]].as<float>(), &payload[length]);
        length = (uint8_t)(length + 4u);
    }
    payload[0] = mask;
    (void)start_stm_request(client_id, request_id, command, UNER_CMD_SET_BALANCE_PID,
                            UNER_CMD_SET_BALANCE_PID, payload, length, 0u);
}

