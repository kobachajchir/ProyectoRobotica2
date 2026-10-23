// Private bridge implementation; included once by ws_json_bridge.cpp.
static void cmd_debug_wifi_login(uint32_t client_id, const char *request_id,
                                 const char *command, JsonObject params)
{
    WsmSession *session = session_for_client(client_id);
    if (!session || (!session->esp_debug_wifi && !wsm_allow_auth_attempt(&s_bridge.sessions, client_id,
            session->generation, s_bridge.platform.now_ms())))
    {
        send_error(client_id, request_id, "esp", command, "busy", "Debug login rate limited");
        return;
    }
    if (strcmp(params["pin"] | "", "9999") != 0)
    {
        send_error(client_id, request_id, "esp", command, "invalid_pin", "Invalid debug PIN");
        return;
    }
    if (!debug_wifi_begin(session))
    {
        send_error(client_id, request_id, "esp", command, "busy", "Another session owns Wi-Fi or an operation is pending" );
        return;
    }
    (void)wsm_set_stm_auth(&s_bridge.sessions, client_id, session->generation,
                          0u, 0u, s_bridge.platform.now_ms());
    session->esp_debug_used = 1u;
    session->esp_debug_wifi = 1u;
    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "Local ESP Wi-Fi debug enabled");
    data["debugWifi"] = true;
    data["authenticated"] = false;
    send_doc_to_client(client_id, doc);
}

static void cmd_debug_wifi_logout(uint32_t client_id, const char *request_id,
                                  const char *command, JsonObject params)
{
    (void)params;
    WsmSession *session = session_for_client(client_id);
    if (session) { session->esp_debug_wifi = 0u; if (s_bridge.debug_wifi.owner_node == session->node) debug_wifi_release(); }
    StaticJsonDocument<256> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command,
                                         true, "OK", "Local ESP Wi-Fi debug disabled");
    data["debugWifi"] = false;
    send_doc_to_client(client_id, doc);
}

static void cmd_web_page_ready(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    const uint8_t number = s_bridge.backend.web_page_ready
        ? s_bridge.backend.web_page_ready(s_bridge.backend.context, client_id) : 0u;
    if (!number)
    {
        send_error(client_id, request_id, "esp", command, "busy", "No active page slot");
        return;
    }
    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base(doc, client_id, request_id, "esp", command, true, "OK", "Web page ready");
    data["ready"] = true;
    data["clientNumber"] = number;
    send_doc_to_client(client_id, doc);
}

static void cmd_get_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_get_mode(client_id, request_id, command);
}

static void cmd_get_status(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_get_status(client_id, request_id, command);
}

static void cmd_ap_clients_list(uint32_t client_id, const char *request_id,
                                const char *command, JsonObject params)
{
    (void)params;
    dispatch_ap_clients_list(client_id, request_id, command);
}

static void cmd_ap_client_disconnect(uint32_t client_id, const char *request_id,
                                     const char *command, JsonObject params)
{
    dispatch_ap_client_disconnect(client_id, request_id, command, params);
}

static void cmd_reboot_esp(uint32_t client_id, const char *request_id,
                           const char *command, JsonObject params)
{
    dispatch_reboot_esp(client_id, request_id, command, params);
}

static void cmd_get_scan(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_get_scan(client_id, request_id, command);
}

static void cmd_stop_scan(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_stop_scan(client_id, request_id, command);
}

static void cmd_get_wifi_detail(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_wifi_detail_get(client_id, request_id, command, params);
}

static void cmd_set_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_set_ap(client_id, request_id, command, params);
}

static void cmd_ap_credentials_result_get(uint32_t client_id, const char *request_id,
                                          const char *command, JsonObject params)
{
    (void)params;
    dispatch_ap_credentials_result_get(client_id, request_id, command);
}

static void cmd_connect_wifi(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_connect_wifi(client_id, request_id, command);
}

static void cmd_wifi_credentials_get_current(uint32_t client_id,
                                             const char *request_id,
                                             const char *command,
                                             JsonObject params)
{
    (void)params;
    dispatch_wifi_credentials_get_current(client_id, request_id, command);
}

static void cmd_wifi_credentials_submit(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_wifi_credentials_submit(client_id, request_id, command, params);
}

static void cmd_wifi_credentials_cancel(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_wifi_credentials_cancel(client_id, request_id, command, params);
}

static void cmd_disconnect_wifi(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_disconnect_wifi(client_id, request_id, command);
}

static void cmd_start_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_start_ap(client_id, request_id, command);
}

static void cmd_stop_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_stop_ap(client_id, request_id, command);
}

static void cmd_auto_reconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_auto_reconnect(client_id, request_id, command, params);
}

static void cmd_telemetry_set_rate(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_telemetry_set_rate(client_id, request_id, command, params);
}

static void cmd_get_current_screen(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_get_current_screen(client_id, request_id, command);
}

static void cmd_reboot_stm(uint32_t client_id, const char *request_id,
                           const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_empty_command(client_id, request_id, command, STM_CMD_RESET_MCU);
}

static void cmd_auth_pin_login(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_pin_auth_command(client_id, request_id, command, params, STM_AUTH_SCOPE_LOGIN);
}

static void cmd_auth_pin_validate_screen(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_pin_auth_command(client_id, request_id, command, params, STM_AUTH_SCOPE_SCREEN);
}

static void cmd_auth_pin_change(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_pin_change_command(client_id, request_id, command, params);
}

static void cmd_auth_session_logout(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_auth_session_command(client_id, request_id, command, 0u);
}

static void cmd_auth_session_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_auth_session_command(client_id, request_id, command, 1u);
}

static void cmd_stm_menu_click(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_menu_click(client_id, request_id, command, params);
}

static void cmd_stm_screen_page(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_screen_page(client_id, request_id, command, params);
}

static void cmd_stm_encoder_button(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_press_command(client_id, request_id, command, params, STM_CMD_TRIGGER_ENCODER_BUTTON);
}

static void cmd_stm_user_button(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_press_command(client_id, request_id, command, params, STM_CMD_TRIGGER_USER_BUTTON);
}

static void cmd_stm_rotate_left(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_screen_code_command(client_id, request_id, command, params, STM_CMD_TRIGGER_ENCODER_ROTATE_LEFT);
}

static void cmd_stm_rotate_right(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_screen_code_command(client_id, request_id, command, params, STM_CMD_TRIGGER_ENCODER_ROTATE_RIGHT);
}

static void cmd_stm_auth_remote_result(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_auth_remote_result(client_id, request_id, command, params);
}

static void cmd_stm_get_extension_profile(uint32_t client_id, const char *request_id,
                                          const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_extension_profile_get(client_id, request_id, command);
}

static void cmd_stm_set_extension_profile(uint32_t client_id, const char *request_id,
                                          const char *command, JsonObject params)
{
    dispatch_stm_extension_profile_select(client_id, request_id, command, params, false);
}

static void cmd_stm_reboot_into_profile(uint32_t client_id, const char *request_id,
                                        const char *command, JsonObject params)
{
    dispatch_stm_extension_profile_select(client_id, request_id, command, params, true);
}

static void cmd_stm_ap_config_get(uint32_t client_id, const char *request_id,
                                  const char *command, JsonObject params)
{
    dispatch_stm_ap_config_get(client_id, request_id, command, params);
}

static void cmd_stm_ap_config_set(uint32_t client_id, const char *request_id,
                                  const char *command, JsonObject params)
{
    dispatch_stm_ap_config_set(client_id, request_id, command, params);
}

static void cmd_stm_ap_config_set_alias(uint32_t client_id, const char *request_id,
                                        const char *command, JsonObject params)
{
    /* Legacy alias: translate old params to stm.apConfig.set format.
     * reusePassword=true → omit password field (F4 preserves it).
     * If password is provided but protected is absent, infer protected=true. */
    bool reuse = params["reusePassword"] | false;
    if (reuse) params.remove("password");
    if (!params["password"].isNull() && params["protected"].isNull()) {
        const char *pwd = params["password"] | "";
        if (pwd[0] != '\0') params["protected"] = true;
    }
    dispatch_stm_ap_config_set(client_id, request_id, command, params);
}

static void cmd_stm_ap_config_get_alias(uint32_t client_id, const char *request_id,
                                        const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_ap_config_get(client_id, request_id, command, params);
}

static void cmd_stm_get_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_empty_command(client_id, request_id, command, STM_CMD_GET_CAR_MODE);
}

static void cmd_stm_set_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_set_car_mode(client_id, request_id, command, params);
}

static void cmd_stm_set_i2c_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_i2c_set_policy(client_id, request_id, command, params);
}

static void cmd_stm_get_i2c_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_empty_command(client_id, request_id, command, STM_CMD_GET_I2C_POLICY);
}

static void cmd_stm_get_mpu_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    dispatch_stm_empty_command(client_id, request_id, command, STM_CMD_GET_MPU_SNAPSHOT);
}

static void cmd_stm_set_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_mpu_stream(client_id, request_id, command, params);
}

static void cmd_stm_stop_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    StaticJsonDocument<64> local;
    JsonObject stream = local.to<JsonObject>();
    stream["enable"] = false;
    dispatch_stm_mpu_stream(client_id, request_id, command, stream);
}

static void cmd_stm_set_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_set_preferences(client_id, request_id, command, params);
}

static void cmd_stm_get_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    if (!params["index"].isNull())
    {
        uint32_t index = 0u;
        if (!read_json_u32(params, "index", &index) || index > 255u)
        {
            send_error(client_id, request_id, "stm", command, "invalid_request", "index must be uint8");
            return;
        }
        uint8_t value = (uint8_t)index;
        (void)start_stm_request(client_id, request_id, command, UNER_CMD_GET_PREFERENCES,
                                UNER_CMD_GET_PREFERENCES, &value, 1u, 0u);
        return;
    }
    dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_PREFERENCES);
}

static void cmd_stm_get_telemetry_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_TELEMETRY_SNAPSHOT);
}



static bool validate_auto_params(JsonObject root,JsonObject &params,const char **error) {
 if(!require_params_object(root,params)){*error="args requeridos";return false;}
 *error="Parametros de GOTO/calibracion invalidos";
 if(params.containsKey("payload")){
  if(params.size()!=1 || !params["payload"].is<JsonArray>())return false;
  JsonArray bytes=params["payload"].as<JsonArray>();if(bytes.size()!=88&&bytes.size()!=186)return false;
  for(JsonVariant v:bytes)if(!v.is<unsigned>()||v.as<unsigned>()>255)return false;
  return true;
 }
 if(params.containsKey("xCm"))return params.size()==2 && params["xCm"].is<float>() && params["yCm"].is<float>() &&
  isfinite(params["xCm"].as<float>()) && isfinite(params["yCm"].as<float>());
 uint32_t op,ramp,timeout;
 return params.size()==4 && read_json_u32(params,"operation",&op) && op<=3 &&
  params["percent"].is<float>() && isfinite(params["percent"].as<float>()) &&
  read_json_u32(params,"rampMs",&ramp) && ramp<=5000 &&
  read_json_u32(params,"timeoutMs",&timeout) && timeout>=1000 && timeout<=30000;
}
static void auto_write_float(uint8_t *p,float f) { uint32_t bits;memcpy(&bits,&f,4);write_u32_le(bits,p); }
static void cmd_stm_auto(uint32_t client,const char *request,const char *command,JsonObject params) {
 uint8_t payload[186]={0},length=0,id=0;
 const char *names[]={"stm.goto.get","stm.goto.start","stm.goto.cancel","stm.goto.irCalibration.set","stm.motorCalibration.get","stm.motorCalibration.control","stm.motorCalibration.profile.get","stm.motorCalibration.profile.set","stm.mpu.calibration.request"};
 for(unsigned n=0;n<9;++n)if(strcmp(command,names[n])==0)id=0x23+n;
 if(id==0x24){auto_write_float(payload,params["xCm"].as<float>());auto_write_float(payload+4,params["yCm"].as<float>());length=8;}
 else if(id==0x28){
  payload[0]=params["operation"].as<uint8_t>();auto_write_float(payload+1,params["percent"].as<float>());
  write_u32_le(params["rampMs"].as<uint32_t>(),payload+5);write_u32_le(params["timeoutMs"].as<uint32_t>(),payload+9);length=13;
 }else if(id==0x26||id==0x2A){
  JsonArray bytes=params["payload"].as<JsonArray>();unsigned expected=id==0x26?186:88;
  if(bytes.size()!=expected){send_error(client,request,"stm",command,"invalid_request","Longitud de perfil invalida");return;}
  for(JsonVariant v:bytes)payload[length++]=v.as<uint8_t>();
 }
 (void)start_stm_request(client,request,command,id,id,payload,length,0u);
}

static bool validate_line_calibration_params(JsonObject root_payload, JsonObject &params, const char **error)
{
    uint32_t operation=0, session=0;
    if (!require_params_object(root_payload,params) || params.size()!=2u || !read_json_u32(params,"operation",&operation) || operation>3u ||
        !read_json_u32(params,"sessionId",&session) || (operation<=1u ? session!=0u : session==0u)) {
        *error="operation 0=blanco, 1=pista, 2=OK, 3=cancelar; sessionId requerido"; return false;
    }
    return true;
}
static bool validate_line_follow_params(JsonObject root_payload, JsonObject &params, const char **error)
{
    if (!require_params_object(root_payload,params) || params.size()!=1u || !params["enabled"].is<bool>()) { *error="enabled debe ser booleano"; return false; }
    return true;
}
static void cmd_stm_line(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint8_t payload[5]={0}; uint8_t length=0; uint8_t id=UNER_CMD_GET_LINE_STATE;
    if (strcmp(command,"stm.ir.lineCalibration.control")==0) {
        uint32_t op=0, session=0; read_json_u32(params,"operation",&op); read_json_u32(params,"sessionId",&session);
        id=UNER_CMD_LINE_CALIBRATION_CONTROL; payload[0]=(uint8_t)op; write_u32_le(session,payload+1); length=5;
    } else if (strcmp(command,"stm.lineFollow.setEnabled")==0) {
        id=UNER_CMD_SET_LINE_FOLLOW; payload[0]=params["enabled"].as<bool>()?1:0; length=1;
    }
    (void)start_stm_request(client_id,request_id,command,id,id,payload,length,0u);
}

static void cmd_stm_motion(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    uint8_t payload[10]; uint8_t length = 0u; uint8_t id = UNER_CMD_GET_MOTION_STATE;
    if (strcmp(command, "stm.balance.setEnabled") == 0 || strcmp(command, "stm.motion.stop") == 0) {
        const bool stop = strcmp(command, "stm.motion.stop") == 0;
        id = stop ? UNER_CMD_STOP_MOTION : UNER_CMD_SET_BALANCE_ENABLED;
        const char *key = stop ? "disableBalance" : "enabled";
        if (!params[key].is<bool>()) {
            send_error(client_id, request_id, "stm", command, "invalid_request", "Missing boolean argument"); return;
        }
        payload[0] = params[key].as<bool>() ? 1u : 0u; length = 1u;
    } else if (strcmp(command, "stm.motion.set") == 0 || strcmp(command, "stm.motor.test") == 0) {
        const bool test = strcmp(command, "stm.motor.test") == 0;
        id = test ? UNER_CMD_SET_MOTOR_TEST : UNER_CMD_SET_MOTION;
        const char *a = test ? "leftPercent" : "forwardPercent";
        const char *b = test ? "rightPercent" : "turnPercent";
        if (params[a].isNull() || params[b].isNull()) {
            send_error(client_id, request_id, "stm", command, "invalid_request", "Wrong percentage fields"); return;
        }
        write_f32_le(params[a].as<float>(), payload); write_f32_le(params[b].as<float>(), &payload[4]);
        const uint16_t duration = params["durationMs"].as<uint16_t>();
        payload[8] = (uint8_t)duration; payload[9] = (uint8_t)(duration >> 8); length = 10u;
    }
    (void)start_stm_request(client_id, request_id, command, id, id, payload, length, 0u);
}

static void cmd_stm_get_balance_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_BALANCE_PID);
}

static void cmd_stm_set_balance_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_set_pid(client_id, request_id, command, params);
}

static void cmd_stm_get_control_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_CONTROL_SNAPSHOT);
}

static void cmd_stm_get_firmware_info(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_FIRMWARE_INFO);
}

static void cmd_stm_get_build_info(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_BUILD_INFO);
}

static void cmd_stm_get_boot_report(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_BOOT_REPORT);
}

static void cmd_stm_get_ir_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params; dispatch_stm_empty_command(client_id, request_id, command, UNER_CMD_GET_IR_SNAPSHOT);
}

static void cmd_stm_set_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    dispatch_stm_ir_stream(client_id, request_id, command, params);
}

static void cmd_stm_stop_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)
{
    (void)params;
    StaticJsonDocument<64> local;
    JsonObject stream = local.to<JsonObject>();
    stream["enable"] = false;
    dispatch_stm_ir_stream(client_id, request_id, command, stream);
}

static bool command_requires_missing_stm_contract(const char *command)
{
    return command &&
           (strncmp(command, "stm.motor.", 10) == 0 ||
            strncmp(command, "stm.tcrt.", 9) == 0 ||
            strcmp(command, "stm.car.setMode") == 0);
}

static void reject_missing_stm_contract(uint32_t client_id, const char *request_id, const char *target, const char *command)
{
    send_error(client_id, request_id, target, command, "ERR_UNER_CONTRACT_MISSING",
               "STM feature exists but has no stable UNER payload contract yet");
}

static const BridgeCommandSpec kBridgeCommands[] PROGMEM = {
    {"esp.web.page.ready", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_web_page_ready, "local.web.page.ready"},
    {"stm.mpu.calibration.request", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.goto.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.goto.start", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_auto_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.goto.cancel", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.goto.irCalibration.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_auto_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.motorCalibration.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.motorCalibration.control", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_auto_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.motorCalibration.profile.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.motorCalibration.profile.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_auto_params, cmd_stm_auto, "uner.auto.v1"},
    {"stm.ir.lineCalibration.control", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_line_calibration_params, cmd_stm_line, "uner.line.state.v1"},
    {"stm.ir.lineCalibration.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_line, "uner.line.state.v1"},
    {"stm.lineFollow.setEnabled", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_line_follow_params, cmd_stm_line, "uner.line.state.v1"},
    {"stm.motion.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_motion, "uner.motion.state.v1"},
    {"stm.balance.setEnabled", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_motion_switch_params, cmd_stm_motion, "uner.motion.state.v1"},
    {"stm.motion.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_motion_percent_params, cmd_stm_motion, "uner.motion.state.v1"},
    {"stm.motion.stop", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_motion_switch_params, cmd_stm_motion, "uner.motion.state.v1"},
    {"stm.motor.test", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_motion_percent_params, cmd_stm_motion, "uner.motion.state.v1"},
    {"esp.wifi.getMode", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_get_mode, "local.wifi.mode"},
    {"esp.device.getStatus", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_get_status, "local.device.status"},
    {"rebootEsp", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_reboot_esp_params, cmd_reboot_esp, "local.device.reboot"},
    {"esp.reboot", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_reboot_esp_params, cmd_reboot_esp, "local.device.reboot"},
    {"esp.device.reboot", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_reboot_esp_params, cmd_reboot_esp, "local.device.reboot"},
    {"esp.wifi.debug.ap.set", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_stm_ap_config_set_params, cmd_debug_ap_set, "local.debug.ap"},
    {"esp.wifi.debug.ap.get", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_debug_ap_get, "local.debug.ap"},
    {"esp.wifi.debug.mode.set", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_reboot_esp_params, cmd_debug_mode_set, "local.debug.mode"},
    {"esp.wifi.debug.fallback.get", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_debug_fallback, "local.debug.fallback"},
    {"esp.wifi.debug.fallback.set", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_ap_fallback_params, cmd_debug_fallback, "local.debug.fallback"},
    {"esp.auth.debug.login", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_pin_params, cmd_debug_wifi_login, "local.auth.debug.wifi"},
    {"esp.auth.debug.logout", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_debug_wifi_logout, "local.auth.debug.wifi"},
    {"esp.auth.pin.login", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_pin_params, cmd_auth_pin_login, "uner.auth.remote"},
    {"esp.auth.pin.validateScreen", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_auth_screen_params, cmd_auth_pin_validate_screen, "uner.auth.remote"},
    {"esp.auth.pin.change", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_auth_change_params, cmd_auth_pin_change, "uner.auth.remote"},
    {"esp.auth.session.logout", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_auth_session_logout, "uner.auth.session"},
    {"esp.auth.session.get", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_auth_session_get, "uner.auth.session"},
    {"oledCanvas.begin", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_oled_canvas_begin_params, cmd_oled_canvas_begin, "local.oled.canvas.begin"},
    {"oledCanvas.chunk", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_oled_canvas_chunk_params, cmd_oled_canvas_chunk, "local.oled.canvas.chunk"},
    {"oledCanvas.commit", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_oled_canvas_transfer_params, cmd_oled_canvas_commit, "uner.oled.canvas.rendered"},
    {"oledCanvas.cancel", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_oled_canvas_transfer_params, cmd_oled_canvas_cancel, "uner.oled.canvas.cancel"},
    {"wifi.scan.start", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_get_scan, "local.wifi.scan"},
    {"wifi.scan.stop", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_stop_scan, "local.wifi.scan.stop"},
    {"esp.wifi.getScan", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_get_scan, "local.wifi.scan"},
    {"wifi.detail.get", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_cancel_params, cmd_get_wifi_detail, "local.wifi.detail"},
    {"esp.wifi.getDetail", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_cancel_params, cmd_get_wifi_detail, "local.wifi.detail"},
    {"wifi.ap.credentials.set", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_stm_ap_config_set_params, cmd_stm_ap_config_set_alias, "uner.ap.config"},
    {"esp.wifi.setAp", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_stm_ap_config_set_params, cmd_stm_ap_config_set_alias, "uner.ap.config"},
    {"wifi.ap.credentials.result.get", "esp", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_ap_config_get_alias, "uner.ap.config"},
    {"wifi.ap.clients.list", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_ap_clients_list, "local.wifi.ap.clients"},
    {"wifi.ap.clients.disconnect", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_ap_client_disconnect_params, cmd_ap_client_disconnect, "local.wifi.ap.client.disconnect"},
    {"esp.wifi.connect", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_connect_wifi, "local.wifi.connect"},
    {"wifi.credentials.getCurrent", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_wifi_credentials_get_current, "local.wifi.credentials.current"},
    {"wifi.credentials.current", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_wifi_credentials_get_current, "local.wifi.credentials.current"},
    {"esp.wifi.credentials.getCurrent", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_wifi_credentials_get_current, "local.wifi.credentials.current"},
    {"esp.wifi.credentials.current", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_empty_params, cmd_wifi_credentials_get_current, "local.wifi.credentials.current"},
    {"wifi.credentials.submit", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_submit_params, cmd_wifi_credentials_submit, "local.wifi.credentials.submit"},
    {"esp.wifi.credentials.submit", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_submit_params, cmd_wifi_credentials_submit, "local.wifi.credentials.submit"},
    {"wifi.credentials.cancel", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_cancel_params, cmd_wifi_credentials_cancel, "local.wifi.credentials.cancel"},
    {"esp.wifi.credentials.cancel", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_wifi_credentials_cancel_params, cmd_wifi_credentials_cancel, "local.wifi.credentials.cancel"},
    {"esp.wifi.disconnect", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_disconnect_wifi, "local.wifi.disconnect"},
    {"esp.ap.start", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_start_ap, "local.ap.start"},
    {"esp.ap.stop", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_optional_params, cmd_stop_ap, "local.ap.stop"},
    {"esp.wifi.setAutoReconnect", "esp", BRIDGE_ROUTE_LOCAL_ESP, validate_required_params, cmd_auto_reconnect, "local.wifi.autoreconnect"},
    {"stm.telemetry.setRate", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_required_params, cmd_telemetry_set_rate, "uner.telemetry.ack"},
    {"stm.sensors.telemetry.setRate", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_required_params, cmd_telemetry_set_rate, "uner.telemetry.ack"},
    {"stm.screen.getCurrent", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_get_current_screen, "uner.screen.report"},
    {"stm.getCurrentScreen", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_get_current_screen, "uner.screen.report"},
    {"rebootStm", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_reboot_stm, "uner.ack.reset_mcu"},
    {"resetMcu", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_reboot_stm, "uner.ack.reset_mcu"},
    {"stm.menu.click", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_menu_click_params, cmd_stm_menu_click, "uner.command.status"},
    {"stm.screen.page", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_page_params, cmd_stm_screen_page, "uner.command.status"},
    {"stm.input.encoderButton", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_press_params, cmd_stm_encoder_button, "uner.command.status"},
    {"stm.input.userButton", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_press_params, cmd_stm_user_button, "uner.command.status"},
    {"stm.input.rotateLeft", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_code_params, cmd_stm_rotate_left, "uner.command.status"},
    {"stm.input.rotateRight", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_code_params, cmd_stm_rotate_right, "uner.command.status"},
    {"stm.wifi.apFallback.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_get_ap_fallback, "uner.ap.fallback"},
    {"stm.wifi.apFallback.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_ap_fallback_params, cmd_stm_set_ap_fallback, "uner.ap.fallback"},
    {"stm.irEmitters.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_get_ir_emitters, "uner.ir.emitters"},
    {"stm.irEmitters.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_ir_emitters_params, cmd_stm_set_ir_emitters, "uner.ir.emitters"},
    {"stm.extensionProfile.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_get_extension_profile, "uner.extension.profile"},
    {"stm.extensionProfile.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_extension_profile_params, cmd_stm_set_extension_profile, "uner.extension.profile"},
    {"stm.extensionProfile.reboot", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_extension_profile_params, cmd_stm_reboot_into_profile, "uner.extension.profile"},
    {"stm.apConfig.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_ap_config_get, "uner.ap.config"},
    {"stm.apConfig.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_ap_config_set_params, cmd_stm_ap_config_set, "uner.ap.config"},
    {"stm.carMode.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_car_mode, "uner.command.payload"},
    {"stm.carMode.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_car_mode_params, cmd_stm_set_car_mode, "uner.command.payload"},
    {"stm.i2c.policy.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_i2c_policy_params, cmd_stm_set_i2c_policy, "uner.command.payload"},
    {"stm.i2c.policy.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_i2c_policy, "uner.command.payload"},
    {"stm.mpu.snapshot.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_mpu_snapshot, "uner.mpu.snapshot"},
    {"stm.mpu.stream.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_mpu_stream_params, cmd_stm_set_mpu_stream, "uner.mpu.stream"},
    {"stm.mpu.stream.stop", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_stop_mpu_stream, "uner.mpu.stream"},
    {"stm.preferences.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_preferences, "uner.preferences"},
    {"stm.preferences.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_preferences_params, cmd_stm_set_preferences, "uner.preferences"},
    {"stm.telemetry.snapshot.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_telemetry_snapshot, "uner.telemetry.snapshot"},
    {"stm.balance.pid.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_balance_pid, "uner.balance.pid"},
    {"stm.balance.pid.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_pid_params, cmd_stm_set_balance_pid, "uner.balance.pid"},
    {"stm.control.snapshot.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_control_snapshot, "uner.control.snapshot"},
    {"stm.firmware.info.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_firmware_info, "uner.firmware.info"},
    {"stm.build.info.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_build_info, "uner.build.info"},
    {"stm.boot.report.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_boot_report, "uner.boot.report"},
    {"stm.ir.snapshot.get", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_ir_snapshot, "uner.ir.snapshot"},
    {"stm.ir.stream.set", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_mpu_stream_params, cmd_stm_set_ir_stream, "uner.ir.stream"},
    {"stm.ir.stream.stop", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_stop_ir_stream, "uner.ir.stream"},
    {"getPreferences", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_preferences, "uner.preferences"},
    {"setPreferences", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_preferences_params, cmd_stm_set_preferences, "uner.preferences"},
    {"getCurrentScreen", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_get_current_screen, "uner.screen.report"},
    {"menuItemClick", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_menu_click_params, cmd_stm_menu_click, "uner.command.status"},
    {"triggerEncoderButton", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_press_params, cmd_stm_encoder_button, "uner.command.status"},
    {"triggerUserButton", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_press_params, cmd_stm_user_button, "uner.command.status"},
    {"requestScreenPage", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_page_params, cmd_stm_screen_page, "uner.command.status"},
    {"rotateEncoderLeft", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_code_params, cmd_stm_rotate_left, "uner.command.status"},
    {"rotateEncoderRight", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_screen_code_params, cmd_stm_rotate_right, "uner.command.status"},
    {"getExtensionProfile", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_empty_params, cmd_stm_get_extension_profile, "uner.extension.profile"},
    {"setExtensionProfile", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_extension_profile_params, cmd_stm_set_extension_profile, "uner.extension.profile"},
    {"rebootIntoProfile", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_extension_profile_params, cmd_stm_reboot_into_profile, "uner.extension.profile"},
    {"getCarMode", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_car_mode, "uner.command.payload"},
    {"setCarMode", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_car_mode_params, cmd_stm_set_car_mode, "uner.command.payload"},
    {"setI2cPolicy", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_i2c_policy_params, cmd_stm_set_i2c_policy, "uner.command.payload"},
    {"getI2cPolicy", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_i2c_policy, "uner.command.payload"},
    {"getMpuSnapshot", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_mpu_snapshot, "uner.mpu.snapshot"},
    {"subscribeMpu", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_mpu_stream_params, cmd_stm_set_mpu_stream, "uner.mpu.stream"},
    {"unsubscribeMpu", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_stop_mpu_stream, "uner.mpu.stream"},
    {"getTelemetrySnapshot", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_telemetry_snapshot, "uner.telemetry.snapshot"},
    {"getBalancePid", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_balance_pid, "uner.balance.pid"},
    {"setBalancePid", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_pid_params, cmd_stm_set_balance_pid, "uner.balance.pid"},
    {"getControlSnapshot", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_control_snapshot, "uner.control.snapshot"},
    {"getFirmwareInfo", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_firmware_info, "uner.firmware.info"},
    {"getBuildInfo", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_build_info, "uner.build.info"},
    {"getBootReport", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_boot_report, "uner.boot.report"},
    {"getIrSnapshot", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_get_ir_snapshot, "uner.ir.snapshot"},
    {"subscribeIr", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_stm_mpu_stream_params, cmd_stm_set_ir_stream, "uner.ir.stream"},
    {"unsubscribeIr", "stm", BRIDGE_ROUTE_FORWARD_STM, validate_optional_params, cmd_stm_stop_ir_stream, "uner.ir.stream"},
};

static bool find_bridge_command(const char *command, BridgeCommandSpec &out)
{
    for (uint8_t i = 0; i < (uint8_t)(sizeof(kBridgeCommands) / sizeof(kBridgeCommands[0])); ++i)
    {
        BridgeCommandSpec candidate;
        memcpy_P(&candidate, &kBridgeCommands[i], sizeof(candidate));
        if (strcmp(candidate.json_name, command) == 0)
        {
            out = candidate;
            return true;
        }
    }
    return false;
}

static bool is_debug_wifi_command(const char *command)
{
    BridgeCommandSpec spec;
    if (!find_bridge_command(command, spec)) return false;
    // Only known local Wi-Fi routes; AP configuration aliases forward to F4.
    return (strncmp(command, "wifi.", 5u) == 0 ||
            strncmp(command, "esp.wifi.", 9u) == 0 ||
            strncmp(command, "esp.ap.", 7u) == 0) &&
           spec.route == BRIDGE_ROUTE_LOCAL_ESP;
}

static void dispatch_device_command(uint32_t client_id, JsonDocument &doc)
{
    WsClientState *client_state = ensure_client(client_id);

    JsonObject root_payload = doc["payload"].as<JsonObject>();
    if (root_payload.isNull()) root_payload = doc.as<JsonObject>();
    const char *request_id = root_payload["requestId"] | "";
    const char *target = root_payload["target"] | "";
    const char *command = root_payload["command"] | "";
    bool wifi_credentials_command =
        strcmp(command, "wifi.credentials.submit") == 0 ||
        strcmp(command, "esp.wifi.credentials.submit") == 0 ||
        strcmp(command, "wifi.credentials.cancel") == 0 ||
        strcmp(command, "esp.wifi.credentials.cancel") == 0;
    if (!target[0] && wifi_credentials_command) target = "esp";
    if ((!request_id[0] && !wifi_credentials_command) || !target[0] || !command[0])
    {
        send_error(client_id, request_id, target, command, "ERR_BAD_PARAMS", "requestId, target and command are required");
        return;
    }
    if (strlen(request_id) > 64 || strlen(command) > 64)
    {
        send_error(client_id, request_id, target, command, "ERR_BAD_PARAMS", "requestId/command is too long");
        return;
    }
    if (strcmp(target, "esp") != 0 && strcmp(target, "stm") != 0)
    {
        send_error(client_id, request_id, target, command, "ERR_TARGET_INVALID", "target must be esp or stm");
        return;
    }

    if (strcmp(command, "esp.auth.login") == 0)
    {
        send_error(client_id, request_id, target, command, "unsupported_by_f4",
                   "Username/password authentication was removed; use esp.auth.pin.login");
        return;
    }

    bool allow_unauthenticated =
        strcmp(target, "esp") == 0 &&
        (strcmp(command, "esp.auth.debug.login") == 0 || strcmp(command, "esp.auth.debug.logout") == 0 || strcmp(command, "esp.web.page.ready") == 0 ||
         strcmp(command, "esp.auth.pin.login") == 0 ||
         strcmp(command, "esp.auth.pin.validateScreen") == 0 ||
         strcmp(command, "esp.auth.pin.change") == 0 ||
         strcmp(command, "esp.auth.session.get") == 0 ||
         strcmp(command, "esp.device.getStatus") == 0 ||
         (ap_provisioning_active() && is_ap_provisioning_command(command)));
    WsmSession *authorized_session = client_state
        ? wsm_find(&s_bridge.sessions, client_id, client_state->generation) : nullptr;
    // The ESP has one radio. Never let another endpoint commit RAM-only settings.
    if (s_bridge.debug_wifi.active &&
        !(authorized_session && authorized_session->esp_debug_wifi &&
          authorized_session->node == s_bridge.debug_wifi.owner_node) &&
        is_debug_wifi_command(command))
    {
        send_error(client_id, request_id, target, command, "busy", "Wi-Fi is owned by a debug session");
        return;
    }
    if (strncmp(command, "esp.wifi.debug.", 15u) == 0 &&
        !(authorized_session && authorized_session->esp_debug_wifi &&
          s_bridge.debug_wifi.active && authorized_session->node == s_bridge.debug_wifi.owner_node))
    {
        send_error(client_id, request_id, target, command, "ERR_AUTH_REQUIRED", "A local debug session is required");
        return;
    }
    // Debug is a separate, restrictive scope even if a late F4 reply arrives.
    if (authorized_session && authorized_session->esp_debug_wifi &&
        !(strcmp(target, "esp") == 0 &&
          (is_debug_wifi_command(command) ||
           strcmp(command, "esp.auth.debug.login") == 0 ||
           strcmp(command, "esp.auth.debug.logout") == 0 ||
           strcmp(command, "esp.web.page.ready") == 0 ||
           strcmp(command, "esp.device.getStatus") == 0)))
    {
        send_error(client_id, request_id, target, command, "ERR_AUTH_REQUIRED",
                   "Debug session only permits local ESP Wi-Fi operations");
        return;
    }

    if (!client_state || !authorized_session ||
        (!authorized_session->stm_authenticated && !allow_unauthenticated &&
         !(authorized_session->esp_debug_wifi && strcmp(target, "esp") == 0 && is_debug_wifi_command(command))))
    {
        send_error(client_id, request_id, target, command, "ERR_AUTH_REQUIRED",
                   "A valid FirmwareF4 PIN session is required");
        return;
    }

    if (strcmp(target, "stm") == 0)
    {
        uint8_t stm_cmd = 0;
        uint8_t payload_len = 0;
        if (resolve_legacy_stm_hex_command(command, &stm_cmd, &payload_len))
        {
#if ENABLE_DIAGNOSTIC_RAW_UNER
            s_bridge.legacy_raw_requests++;
            WsmSession *session = session_for_client(client_id);
            if (!session || session->esp_debug_wifi || !wsm_is_authorized(&s_bridge.sessions, client_id, session->generation))
            {
                send_error(client_id, request_id, target, command, "forbidden", "A FirmwareF4 PIN session is required");
                return;
            }
            uint8_t payload[6];
            const char *legacy_error = nullptr;
            if (!extract_legacy_stm_payload(root_payload, payload, payload_len, &legacy_error))
            {
                send_error(client_id, request_id, target, command, "ERR_BAD_PARAMS",
                           legacy_error ? legacy_error : "legacy payload is invalid");
                return;
            }
            (void)start_stm_request(client_id, request_id, command, stm_cmd, stm_cmd, payload, payload_len, 0);
            return;
#else
            send_error(client_id, request_id, target, command, "forbidden",
                       "Raw UNER diagnostics are disabled in this build");
            return;
#endif
        }
    }

    JsonObject params;

    BridgeCommandSpec spec_storage;
    if (!find_bridge_command(command, spec_storage))
    {
        if (command_requires_missing_stm_contract(command))
        {
            reject_missing_stm_contract(client_id, request_id, target, command);
        }
        else
        {
            send_error(client_id, request_id, target, command, "ERR_UNKNOWN_COMMAND", "Unknown command");
        }
        return;
    }
    const BridgeCommandSpec *spec = &spec_storage;

    if (strcmp(target, spec->default_target) != 0)
    {
        send_error(client_id, request_id, target, command, "ERR_TARGET_INVALID", "command target mismatch");
        return;
    }

    const char *validation_message = nullptr;
    if (spec->validate_params && !spec->validate_params(root_payload, params, &validation_message))
    {
        send_error(client_id, request_id, target, command, "ERR_BAD_PARAMS",
                   validation_message ? validation_message : "params are invalid");
        return;
    }

    if (authorized_session->esp_debug_wifi &&
        (strcmp(command, "esp.wifi.connect") == 0 || strcmp(command, "esp.wifi.disconnect") == 0 ||
         strcmp(command, "esp.ap.start") == 0 || strcmp(command, "esp.ap.stop") == 0))
        s_bridge.debug_wifi.radio_dirty = 1u;
    if (authorized_session->esp_debug_wifi &&
        (s_bridge.debug_wifi.mode_pending || s_bridge.ap_credentials_web.active || s_bridge.wifi_credentials_web.connecting) &&
        strcmp(command, "esp.wifi.debug.fallback.get") != 0 &&
        strcmp(command, "esp.wifi.getMode") != 0 &&
        strcmp(command, "wifi.credentials.cancel") != 0 &&
        strcmp(command, "esp.wifi.credentials.cancel") != 0 &&
        strcmp(command, "wifi.credentials.getCurrent") != 0 &&
        strcmp(command, "wifi.credentials.current") != 0 &&
        strcmp(command, "esp.wifi.credentials.getCurrent") != 0 &&
        strcmp(command, "esp.wifi.credentials.current") != 0 &&
        strcmp(command, "wifi.ap.clients.list") != 0 &&
        strcmp(command, "esp.wifi.debug.ap.get") != 0 &&
        strcmp(command, "esp.auth.debug.logout") != 0 &&
        strcmp(command, "esp.auth.debug.login") != 0 &&
        strcmp(command, "esp.device.getStatus") != 0 &&
        strcmp(command, "esp.web.page.ready") != 0)
    {
        send_error(client_id, request_id, target, command, "busy", "Deferred radio operation is applying");
        return;
    }
    spec->execute(client_id, request_id, command, params);
}

static bool canonical_field_allowed(const char *key)
{
    return key && (strcmp(key, "api") == 0 || strcmp(key, "type") == 0 ||
                   strcmp(key, "requestId") == 0 || strcmp(key, "command") == 0 ||
                   strcmp(key, "args") == 0);
}

static const char *canonical_target(const char *command)
{
    if (!command) return "stm";
    if (strncmp(command, "esp.", 4u) == 0 || strncmp(command, "wifi.", 5u) == 0)
        return "esp";
    if (strncmp(command, "oledCanvas.", 11u) == 0) return "esp";
    return "stm";
}

static void dispatch_canonical_raw(uint32_t client_id, const char *request_id,
                                   const char *command, JsonObject args)
{
#if !ENABLE_DIAGNOSTIC_RAW_UNER
    (void)args;
    send_error(client_id, request_id, "stm", command, "forbidden",
               "Raw UNER diagnostics are disabled in this build");
    return;
#else
    WsClientState *state = find_client(client_id);
    WsmSession *session = state ? wsm_find(&s_bridge.sessions, client_id, state->generation) : nullptr;
    if (!session || session->esp_debug_wifi || !wsm_is_authorized(&s_bridge.sessions, client_id, session->generation))
    {
        send_error(client_id, request_id, "stm", command, "forbidden", "A FirmwareF4 PIN session is required");
        return;
    }
    if (!args["src"].isNull() || !args["dst"].isNull() || !args["route"].isNull())
    {
        send_error(client_id, request_id, "stm", command, "invalid_request", "Raw route is assigned by the server");
        return;
    }
    uint32_t command_id = 0u;
    if (!read_json_u32(args, "commandId", &command_id) || command_id > 255u)
    {
        send_error(client_id, request_id, "stm", command, "invalid_request", "commandId must be uint8");
        return;
    }
    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command((uint8_t)command_id, spec_storage);
    if (!spec || !F4_CommandRegistry_IsWireAllowed(spec) || spec->sensitive ||
        command_id == 0x61u || command_id == 0x62u || command_id == 0x6Bu || command_id == 0x6Cu ||
        command_id == OLED_CANVAS_COMMAND_ID ||
        (spec->owner != F4_COMMAND_OWNER_F4 && spec->owner != F4_COMMAND_OWNER_DUAL) ||
        spec->response_policy == F4_RESPONSE_EVENT)
    {
        send_error(client_id, request_id, "stm", command, "unsupported_by_f4", "Raw command is not allowed on the F4 route");
        return;
    }
    uint8_t payload[UNER_MAX_PAYLOAD];
    uint8_t payload_len = 0u;
    if (!args["payload"].isNull())
    {
        if (!args["payload"].is<JsonArray>())
        {
            send_error(client_id, request_id, "stm", command, "invalid_request", "payload must be a byte array");
            return;
        }
        JsonArray bytes = args["payload"].as<JsonArray>();
        if (bytes.size() > UNER_MAX_PAYLOAD)
        {
            send_error(client_id, request_id, "stm", command, "invalid_request", "payload is too large");
            return;
        }
        for (JsonVariant value : bytes)
        {
            if (!value.is<int>() || value.as<int>() < 0 || value.as<int>() > 255)
            {
                send_error(client_id, request_id, "stm", command, "invalid_request", "payload must contain bytes");
                return;
            }
            payload[payload_len++] = (uint8_t)value.as<int>();
        }
    }
    (void)start_stm_request(client_id, request_id, command, (uint8_t)command_id,
                            (uint8_t)command_id, payload, payload_len, 0u);
#endif
}

static void dispatch_api_v1_request(uint32_t client_id, JsonDocument &doc)
{
    s_bridge.api_v1_requests++;
    WsClientState *state = ensure_client(client_id);
    if (state) state->preferred_api_v1 = 1u;
    const char *request_id = doc["requestId"] | "";
    const char *command = doc["command"] | "";

    for (JsonPair pair : doc.as<JsonObject>())
    {
        if (!canonical_field_allowed(pair.key().c_str()))
        {
            send_error(client_id, request_id, "", command, "invalid_request", "Unknown envelope field");
            return;
        }
    }
    if (!doc["api"].is<int>() || doc["api"].as<int>() != 1 ||
        strcmp(doc["type"] | "", "request") != 0 || !request_id[0] || !command[0] ||
        strlen(request_id) > 64u || strlen(command) > 64u || !doc["args"].is<JsonObject>())
    {
        send_error(client_id, request_id, "", command, "invalid_request", "api=1, type=request, requestId, command and args are required");
        return;
    }
    JsonObject args = doc["args"].as<JsonObject>();
    if (strcmp(command, "rawUner") == 0)
    {
        dispatch_canonical_raw(client_id, request_id, command, args);
        return;
    }
    BridgeCommandSpec spec_storage;
    if (!find_bridge_command(command, spec_storage))
    {
        send_error(client_id, request_id, canonical_target(command), command, "invalid_request", "Unknown API v1 command");
        return;
    }
    const BridgeCommandSpec *spec = &spec_storage;

    StaticJsonDocument<1536> adapted;
    adapted["type"] = "device.command";
    JsonObject payload = adapted.createNestedObject("payload");
    payload["requestId"] = request_id;
    // La tabla es la autoridad del destino. Los aliases sin prefijo, como
    // rebootEsp, no deben caer por heuristica en el target STM.
    payload["target"] = spec->default_target;
    payload["command"] = command;
    JsonObject params = payload.createNestedObject("params");
    for (JsonPair pair : args) params[pair.key()] = pair.value();
    if (strcmp(command, "subscribeMpu") == 0 || strcmp(command, "subscribeIr") == 0)
        params["enable"] = true;
    dispatch_device_command(client_id, adapted);
}

static void process_json_message(uint32_t client_id, char *json)
{
    StaticJsonDocument<1536> doc;
    DeserializationError err = deserializeJson(doc, json, strlen(json));
    if (err)
    {
        emit_bridge_error(client_id, "ERR_JSON_INVALID", err.c_str());
        return;
    }
    const char *type = doc["type"] | "";
    if (!type[0])
    {
        emit_bridge_error(client_id, "ERR_BAD_PARAMS", "type is required");
        return;
    }
    if (doc["api"].is<int>() || strcmp(type, "request") == 0)
    {
        dispatch_api_v1_request(client_id, doc);
        return;
    }
    WsClientState *state = find_client(client_id);
    if (state) state->preferred_api_v1 = 0u;
    s_bridge.legacy_envelopes++;
    if (strcmp(type, "device.command") == 0)
    {
        dispatch_device_command(client_id, doc);
        return;
    }
    emit_bridge_error(client_id, "ERR_UNKNOWN_TYPE", "Only API v1 request or device.command messages are accepted");
}

static int16_t read_i16_le(const uint8_t *p)
{
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

