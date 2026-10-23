// Private bridge implementation; included once by ws_json_bridge.cpp.
static bool require_params_object(JsonObject payload, JsonObject &params)
{
    if (!payload["params"].is<JsonObject>()) return false;
    params = payload["params"].as<JsonObject>();
    return true;
}

static bool validate_optional_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    (void)message;
    if (root_payload["params"].is<JsonObject>()) params = root_payload["params"].as<JsonObject>();
    return true;
}

static bool validate_required_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (require_params_object(root_payload, params)) return true;
    if (message) *message = "params object is required";
    return false;
}

static bool validate_reboot_esp_params(JsonObject root_payload, JsonObject &params,
                                       const char **message)
{
    if (!require_params_object(root_payload, params) ||
        !params["mode"].is<const char *>())
    {
        if (message) *message = "mode is required";
        return false;
    }
    for (JsonPair pair : params)
    {
        if (strcmp(pair.key().c_str(), "mode") != 0)
        {
            if (message) *message = "Only mode is accepted";
            return false;
        }
    }
    const char *mode = params["mode"].as<const char *>();
    if (strcmp(mode, "normal") != 0 && strcmp(mode, "ap") != 0)
    {
        if (message) *message = "mode must be normal or ap";
        return false;
    }
    return true;
}

static bool validate_empty_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (params.size() != 0u)
    {
        if (message) *message = "No arguments are accepted";
        return false;
    }
    return true;
}

static bool validate_wifi_credentials_submit_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (root_payload["params"].is<JsonObject>())
    {
        params = root_payload["params"].as<JsonObject>();
    }
    else
    {
        params = root_payload;
    }
    if (!params["ssid"].is<const char *>() || !params["password"].is<const char *>())
    {
        if (message) *message = "ssid and password are required";
        return false;
    }
    for (JsonPair pair : params)
    {
        const char *key = pair.key().c_str();
        if (strcmp(key, "ssid") != 0 && strcmp(key, "password") != 0 &&
            strcmp(key, "bssid") != 0 && strcmp(key, "ip") != 0 &&
            strcmp(key, "gateway") != 0 && strcmp(key, "subnet") != 0 &&
            strcmp(key, "reusePassword") != 0)
        {
            if (message) *message = "Only ssid, password, bssid, ip, gateway and subnet are accepted";
            return false;
        }
    }
    if (!params["reusePassword"].isNull() && !params["reusePassword"].is<bool>())
    {
        if (message) *message = "reusePassword must be boolean";
        return false;
    }
    if (!params["bssid"].isNull())
    {
        uint8_t parsed[6];
        if (!params["bssid"].is<const char *>() ||
            !parse_bssid(params["bssid"].as<const char *>(), parsed))
        {
            if (message) *message = "bssid must use AA:BB:CC:DD:EE:FF format";
            return false;
        }
    }
    const char *ipv4_keys[] = {"ip", "gateway", "subnet"};
    for (uint8_t i = 0u; i < 3u; ++i)
    {
        if (!params[ipv4_keys[i]].isNull() && !params[ipv4_keys[i]].is<const char *>())
        {
            if (message) *message = "ip, gateway and subnet must be IPv4 strings";
            return false;
        }
    }
    return true;
}

static bool validate_ap_credentials_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    for (JsonPair pair : params)
    {
        const char *key = pair.key().c_str();
        if (strcmp(key, "ssid") != 0 && strcmp(key, "password") != 0 &&
            strcmp(key, "ip") != 0 && strcmp(key, "reusePassword") != 0)
        {
            if (message) *message = "Only ssid, password and ip are accepted";
            return false;
        }
    }
    if (!params["ssid"].is<const char *>() || !params["password"].is<const char *>())
    {
        if (message) *message = "ssid and password are required";
        return false;
    }
    if (!params["ip"].isNull() && !params["ip"].is<const char *>())
    {
        if (message) *message = "ip must be an IPv4 string";
        return false;
    }
    if (!params["reusePassword"].isNull() && !params["reusePassword"].is<bool>())
    {
        if (message) *message = "reusePassword must be boolean";
        return false;
    }
    return true;
}

static bool validate_wifi_credentials_cancel_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (root_payload["params"].is<JsonObject>())
    {
        params = root_payload["params"].as<JsonObject>();
    }
    else
    {
        params = root_payload;
    }
    if (!params["ssid"].is<const char *>())
    {
        if (message) *message = "ssid is required";
        return false;
    }
    return true;
}

static bool validate_pin_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint8_t ascii4[4];

    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!read_pin_ascii4(params, ascii4))
    {
        if (message) *message = "pin must be a 4-digit string";
        return false;
    }
    return true;
}

static bool validate_auth_screen_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t screen_code = 0;
    if (!validate_pin_params(root_payload, params, message)) return false;
    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        if (message) *message = "screenCode must be uint32 or hex string";
        return false;
    }
    return true;
}

static bool validate_auth_change_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint8_t current_pin[4];
    uint8_t new_pin[4];
    if (!require_params_object(root_payload, params) ||
        !read_pin_key_ascii4(params, "currentPin", current_pin) ||
        !read_pin_key_ascii4(params, "newPin", new_pin))
    {
        if (message) *message = "currentPin and newPin must be 4-digit strings";
        return false;
    }
    return true;
}

static bool validate_stm_screen_code_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t screen_code = 0;

    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!read_json_u32(params, "screenCode", &screen_code))
    {
        if (message) *message = "screenCode must be uint32 or hex string";
        return false;
    }
    return true;
}

static bool validate_stm_press_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint8_t press_kind = 0;

    if (!validate_stm_screen_code_params(root_payload, params, message)) return false;
    if (!read_press_kind(params, &press_kind))
    {
        if (message) *message = "pressKind must be short or long";
        return false;
    }
    return true;
}

static bool validate_stm_menu_click_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t item = 0;

    if (!validate_stm_screen_code_params(root_payload, params, message)) return false;
    if (!read_json_u32(params, "item", &item) || item < 1u || item > 3u)
    {
        if (message) *message = "item must be 1, 2 or 3";
        return false;
    }
    return true;
}

static bool validate_stm_screen_page_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint8_t direction = 0;

    if (!validate_stm_screen_code_params(root_payload, params, message)) return false;
    if (!read_screen_page_direction(params, &direction))
    {
        if (message) *message = "direction must be up or down";
        return false;
    }
    return true;
}

static bool validate_stm_auth_remote_result_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t result_code = 0;
    uint32_t attempts_left = 0;

    if (!validate_stm_screen_code_params(root_payload, params, message)) return false;
    if (!read_json_u32(params, "resultCode", &result_code) || result_code > 2u)
    {
        if (message) *message = "resultCode must be 0, 1 or 2";
        return false;
    }
    if (!read_json_u32(params, "attemptsLeft", &attempts_left) || attempts_left > 255u)
    {
        if (message) *message = "attemptsLeft must be 0..255";
        return false;
    }
    return true;
}

static bool validate_motion_switch_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params) || params.size() != 1u ||
        (!params["enabled"].is<bool>() && !params["disableBalance"].is<bool>())) {
        if (message) *message = "enabled or disableBalance must be boolean";
        return false;
    }
    return true;
}

static bool validate_motion_percent_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params) || params.size() != 3u) {
        if (message) *message = "Two percentages and durationMs are required";
        return false;
    }
    const bool test = !params["leftPercent"].isNull();
    const char *a = test ? "leftPercent" : "forwardPercent";
    const char *b = test ? "rightPercent" : "turnPercent";
    uint32_t duration = 0u;
    if (!params[a].is<float>() || !params[b].is<float>() ||
        !isfinite(params[a].as<float>()) || !isfinite(params[b].as<float>()) ||
        params[a].as<float>() < -100.0f || params[a].as<float>() > 100.0f ||
        params[b].as<float>() < -100.0f || params[b].as<float>() > 100.0f ||
        !read_json_u32(params, "durationMs", &duration) || duration < 50u || duration > 2000u) {
        if (message) *message = "Percentages must be finite -100..100; durationMs 50..2000";
        return false;
    }
    return true;
}

static bool validate_stm_ir_emitters_params(JsonObject root_payload, JsonObject &params,
                                           const char **message)
{
    if (!require_params_object(root_payload, params) || params.size() != 3u ||
        !params["objects"].is<bool>() || !params["line"].is<bool>() || !params["lights"].is<bool>())
    {
        if (message) *message = "objects, line and lights must be three booleans";
        return false;
    }
    return true;
}

static bool validate_stm_extension_profile_params(JsonObject root_payload,
                                                  JsonObject &params,
                                                  const char **message)
{
    uint32_t profile_id = 0u;
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!read_json_u32(params, "profileId", &profile_id) ||
        !F4_ExtensionProfile_IsValid((uint8_t)profile_id) ||
        profile_id > UINT8_MAX)
    {
        if (message) *message = "profileId must be 0 (NRF24) or 1 (Beeper)";
        return false;
    }
    return true;
}

static bool validate_stm_i2c_policy_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t mode = 0;

    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!read_json_u32(params, "mode", &mode) || mode < 1u || mode > 3u)
    {
        if (message) *message = "mode must be 1, 2 or 3";
        return false;
    }
    return true;
}

static bool validate_stm_car_mode_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t mode = 0;
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!read_json_u32(params, "mode", &mode) || mode > CAR_MODE_MAX)
    {
        if (message) *message = "mode must be 0 (IDLE), 1 (FOLLOW), 2 (TEST) or 3 (GOTO)";
        return false;
    }
    return true;
}

static bool validate_stm_mpu_stream_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t period_ms = 0;

    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    if (!params["enable"].is<bool>())
    {
        if (message) *message = "enable must be boolean";
        return false;
    }
    if (!params["periodMs"].isNull() &&
        (!read_json_u32(params, "periodMs", &period_ms) || period_ms > 65535u))
    {
        if (message) *message = "periodMs must be 0..65535";
        return false;
    }
    return true;
}

static bool validate_stm_preferences_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    uint32_t field_mask = 0;
    if (!require_params_object(root_payload, params) || !read_json_u32(params, "fieldMask", &field_mask) ||
        !params["values"].is<JsonArray>())
    {
        if (message) *message = "fieldMask and values byte array are required";
        return false;
    }
    JsonArray values = params["values"].as<JsonArray>();
    if (values.size() > 60u)
    {
        if (message) *message = "values is too large";
        return false;
    }
    for (JsonVariant value : values)
    {
        if (!value.is<int>() || value.as<int>() < 0 || value.as<int>() > 255)
        {
            if (message) *message = "values must contain bytes";
            return false;
        }
    }
    return field_mask != 0u;
}

static bool validate_stm_pid_params(JsonObject root_payload, JsonObject &params, const char **message)
{
    if (!require_params_object(root_payload, params))
    {
        if (message) *message = "params object is required";
        return false;
    }
    uint8_t present = 0u;
    const char *keys[] = {"kp", "ki", "kd"};
    for (uint8_t i = 0; i < 3u; ++i)
    {
        if (params[keys[i]].isNull()) continue;
        float value = params[keys[i]].as<float>();
        if (!isfinite(value) || value < 0.0f || value > 1000.0f)
        {
            if (message) *message = "PID values must be finite and in range 0..1000";
            return false;
        }
        present++;
    }
    if (present == 0u && message) *message = "At least one of kp, ki or kd is required";
    return present > 0u;
}

static bool oled_canvas_only_keys(JsonObject params,
                                  const char *const *allowed,
                                  size_t allowed_count,
                                  const char **message)
{
    for (JsonPair pair : params)
    {
        bool found = false;
        for (size_t i = 0u; i < allowed_count; ++i)
        {
            if (strcmp(pair.key().c_str(), allowed[i]) == 0)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            if (message) *message = "OLED Canvas params contain an unknown field";
            return false;
        }
    }
    return true;
}

static bool validate_oled_canvas_begin_params(JsonObject root_payload,
                                              JsonObject &params,
                                              const char **message)
{
    static const char *const allowed[] = {
        "width", "height", "format", "totalBytes", "crc32"
    };
    uint32_t width = 0u;
    uint32_t height = 0u;
    uint32_t total_bytes = 0u;
    uint32_t crc32 = 0u;
    if (!require_params_object(root_payload, params) ||
        !read_json_u32(params, "width", &width) ||
        !read_json_u32(params, "height", &height) ||
        !read_json_u32(params, "totalBytes", &total_bytes) ||
        !params["format"].is<const char *>() ||
        !params["crc32"].is<const char *>() ||
        !OledCanvas_ParseCrc32Hex(params["crc32"].as<const char *>(), &crc32) ||
        width != OLED_CANVAS_WIDTH || height != OLED_CANVAS_HEIGHT ||
        total_bytes != OLED_CANVAS_BYTES ||
        strcmp(params["format"].as<const char *>(), "ssd1306-page-lsb") != 0)
    {
        if (message) *message = "width=128, height=64, totalBytes=1024, format and uppercase crc32 are required";
        return false;
    }
    return oled_canvas_only_keys(params, allowed,
                                 sizeof(allowed) / sizeof(allowed[0]), message);
}

static bool validate_oled_canvas_chunk_params(JsonObject root_payload,
                                              JsonObject &params,
                                              const char **message)
{
    static const char *const allowed[] = {"transferId", "offset", "dataBase64"};
    uint32_t transfer_id = 0u;
    uint32_t offset = 0u;
    const char *encoded;
    if (!require_params_object(root_payload, params) ||
        !read_json_u32(params, "transferId", &transfer_id) ||
        transfer_id == 0u || transfer_id > 0xFFFFu ||
        !read_json_u32(params, "offset", &offset) || offset > 0xFFFFu ||
        !params["dataBase64"].is<const char *>())
    {
        if (message) *message = "transferId, offset and dataBase64 are required";
        return false;
    }
    encoded = params["dataBase64"].as<const char *>();
    if (!encoded || strlen(encoded) != 684u)
    {
        if (message) *message = "dataBase64 must encode exactly 512 bytes";
        return false;
    }
    return oled_canvas_only_keys(params, allowed,
                                 sizeof(allowed) / sizeof(allowed[0]), message);
}

static bool validate_oled_canvas_transfer_params(JsonObject root_payload,
                                                 JsonObject &params,
                                                 const char **message)
{
    static const char *const allowed[] = {"transferId"};
    uint32_t transfer_id = 0u;
    if (!require_params_object(root_payload, params) ||
        !read_json_u32(params, "transferId", &transfer_id) ||
        transfer_id == 0u || transfer_id > 0xFFFFu)
    {
        if (message) *message = "transferId must be uint16 and non-zero";
        return false;
    }
    return oled_canvas_only_keys(params, allowed,
                                 sizeof(allowed) / sizeof(allowed[0]), message);
}

