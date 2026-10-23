// Private bridge implementation; included once by ws_json_bridge.cpp.
static bool oled_canvas_deadline_reached(uint32_t now, uint32_t deadline)
{
    return deadline != 0u && (int32_t)(now - deadline) >= 0;
}

static bool oled_canvas_uart_reserved()
{
    return s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART ||
           s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UART ||
           s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_CANCEL_UART;
}

static void oled_canvas_reset()
{
    memset(&s_bridge.oled_canvas, 0, sizeof(s_bridge.oled_canvas));
    OledCanvasAssembly_Init(&s_bridge.oled_canvas.assembly);
}

static bool oled_canvas_session_valid()
{
    WsClientState *state = find_client(s_bridge.oled_canvas.client_id);
    if (!state || state->generation != s_bridge.oled_canvas.generation) return false;
    WsmSession *session = wsm_find(&s_bridge.sessions, state->id, state->generation);
    return session && session->state == WSM_SLOT_ACTIVE &&
           session->stm_authenticated && session->node == s_bridge.oled_canvas.node &&
           session->stm_expires_at_ms != 0u &&
           !oled_canvas_deadline_reached(s_bridge.platform.now_ms(), session->stm_expires_at_ms);
}

static void oled_canvas_mark_cancel_requested(uint32_t deadline_ms)
{
    s_bridge.oled_canvas.cancel_requested = 1u;
    OledCanvasAssembly_ScrubFramebuffer(&s_bridge.oled_canvas.assembly);
    s_bridge.oled_canvas.total_deadline_ms = deadline_ms;
}

static void oled_canvas_send_commit_error(const char *code,
                                          const char *message,
                                          uint8_t f4_status)
{
    if (s_bridge.oled_canvas.commit_resolved || !s_bridge.oled_canvas.owner_connected ||
        !s_bridge.oled_canvas.commit_request_id[0]) return;
    StaticJsonDocument<512> doc;
    JsonObject details = make_response_base_for_api(
        doc, s_bridge.oled_canvas.api_v1, s_bridge.oled_canvas.commit_request_id,
        "esp", "oledCanvas.commit", false, code, message);
    details["transferId"] = s_bridge.oled_canvas.assembly.transfer_id;
    details["f4Status"] = f4_status;
    send_doc_to_client(s_bridge.oled_canvas.client_id, doc);
    s_bridge.oled_canvas.commit_resolved = 1u;
}

static void oled_canvas_send_cancel_response(uint32_t client_id,
                                             uint8_t api_v1,
                                             const char *request_id,
                                             uint16_t transfer_id)
{
    if (!request_id || !request_id[0]) return;
    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base_for_api(doc, api_v1, request_id,
                                                 "esp", "oledCanvas.cancel",
                                                 true, "OK", "OLED Canvas canceled");
    data["transferId"] = transfer_id;
    data["state"] = "canceled";
    send_doc_to_client(client_id, doc);
}

static void oled_canvas_send_cancel_error(const char *code, const char *message)
{
    if (!s_bridge.oled_canvas.owner_connected ||
        !s_bridge.oled_canvas.cancel_request_id[0]) return;
    send_error_for_api(s_bridge.oled_canvas.client_id, s_bridge.oled_canvas.api_v1,
                       s_bridge.oled_canvas.cancel_request_id, "esp",
                       "oledCanvas.cancel", code, message);
}

static void oled_canvas_send_rendered()
{
    if (!s_bridge.oled_canvas.owner_connected || s_bridge.oled_canvas.commit_resolved) return;
    char crc_hex[9];
    OledCanvas_FormatCrc32Hex(s_bridge.oled_canvas.assembly.expected_crc32, crc_hex);
    StaticJsonDocument<512> doc;
    JsonObject data = make_response_base_for_api(
        doc, s_bridge.oled_canvas.api_v1, s_bridge.oled_canvas.commit_request_id,
        "esp", "oledCanvas.commit", true, "OK", "OLED Canvas rendered");
    data["transferId"] = s_bridge.oled_canvas.assembly.transfer_id;
    data["state"] = "rendered";
    data["bytes"] = OLED_CANVAS_BYTES;
    data["crc32"] = crc_hex;
    data["screenCode"] = OLED_CANVAS_SCREEN_CODE;
    send_doc_to_client(s_bridge.oled_canvas.client_id, doc);
    s_bridge.oled_canvas.commit_resolved = 1u;
}

static const char *oled_canvas_f4_error_code(uint8_t status)
{
    if (status == OLED_CANVAS_STATUS_BUSY) return "busy";
    if (status == OLED_CANVAS_STATUS_TIMEOUT) return "timeout";
    if (status == OLED_CANVAS_STATUS_DISPLAY_IO_ERROR) return "display_io_error";
    return "f4_nack";
}

static const char *oled_canvas_f4_error_message(uint8_t status)
{
    switch (status)
    {
    case OLED_CANVAS_STATUS_NOT_TEST_MODE:
        return "FirmwareF4 is not in TEST mode";
    case OLED_CANVAS_STATUS_WRONG_SCREEN:
        return "Open Testeo > Pantalla > OLED Canvas on FirmwareF4";
    case OLED_CANVAS_STATUS_BUSY:
        return "FirmwareF4 is processing another OLED Canvas transfer";
    case OLED_CANVAS_STATUS_BAD_METADATA:
        return "FirmwareF4 rejected OLED Canvas metadata";
    case OLED_CANVAS_STATUS_OUT_OF_ORDER:
        return "FirmwareF4 rejected an out-of-order OLED Canvas fragment";
    case OLED_CANVAS_STATUS_CRC_MISMATCH:
        return "FirmwareF4 CRC32 does not match the uploaded canvas";
    case OLED_CANVAS_STATUS_TIMEOUT:
        return "FirmwareF4 OLED Canvas transfer timed out";
    case OLED_CANVAS_STATUS_DISPLAY_IO_ERROR:
        return "FirmwareF4 could not complete the OLED I2C refresh";
    case OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI:
        return "OLED Canvas was canceled from the local encoder UI";
    default:
        return "FirmwareF4 rejected OLED Canvas";
    }
}

static void oled_canvas_request_cancel(const char *message, uint8_t status)
{
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE) return;
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UPLOADING)
    {
        oled_canvas_reset();
        return;
    }

    oled_canvas_send_commit_error("f4_nack", message, status);
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART)
    {
        oled_canvas_reset();
        return;
    }
    if (!s_bridge.oled_canvas.cancel_requested)
    {
        oled_canvas_mark_cancel_requested(
            s_bridge.platform.now_ms() + OLED_CANVAS_CANCEL_TIMEOUT_MS);
    }
}

static void oled_canvas_backend_lost()
{
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE) return;
    oled_canvas_send_commit_error("uart_error",
                                  "FirmwareF4 backend is offline",
                                  OLED_CANVAS_STATUS_TIMEOUT);
    oled_canvas_send_cancel_error("uart_error",
                                  "FirmwareF4 backend is offline");
    oled_canvas_reset();
}

static bool oled_canvas_enqueue_operation(uint8_t operation)
{
    uint8_t payload[6u + OLED_CANVAS_F4_CHUNK_BYTES];
    size_t payload_len = 0u;
    uint16_t expected_next = s_bridge.oled_canvas.uart_next_offset;
    uint32_t now = s_bridge.platform.now_ms();

    if (operation == OLED_CANVAS_OP_BEGIN)
    {
        payload_len = OledCanvas_BuildBeginPayload(&s_bridge.oled_canvas.assembly,
                                                   payload, sizeof(payload));
        expected_next = 0u;
    }
    else if (operation == OLED_CANVAS_OP_CHUNK)
    {
        payload_len = OledCanvas_BuildChunkPayload(&s_bridge.oled_canvas.assembly,
                                                   s_bridge.oled_canvas.uart_next_offset,
                                                   payload, sizeof(payload));
        if (payload_len >= 6u)
            expected_next = (uint16_t)(s_bridge.oled_canvas.uart_next_offset + payload[5]);
    }
    else
    {
        payload_len = OledCanvas_BuildControlPayload(operation,
                                                     s_bridge.oled_canvas.assembly.transfer_id,
                                                     payload, sizeof(payload));
        expected_next = operation == OLED_CANVAS_OP_CANCEL
                            ? 0u : OLED_CANVAS_BYTES;
    }
    if (payload_len == 0u || payload_len > UTM_MAX_PAYLOAD) return false;

    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command(OLED_CANVAS_COMMAND_ID,
                                                   spec_storage);
    if (!spec || !F4_CommandRegistry_IsWireAllowed(spec) ||
        !F4_CommandRegistry_IsRequestLengthValid(spec, payload_len)) return false;

    UtmRequest request;
    utm_request_init(&request);
    request.client_id = OLED_CANVAS_INTERNAL_CLIENT_ID;
    request.generation = s_bridge.oled_canvas.generation;
    request.node = s_bridge.oled_canvas.node;
    request.dst_node = UNER_NODE_MCU;
    request.command_id = OLED_CANVAS_COMMAND_ID;
    request.owner = UTM_OWNER_F4;
    request.response_policy = UTM_RESPONSE_COMMAND;
    request.retry_policy = UTM_RETRY_IDEMPOTENT;
    request.idempotent = 1u;
    request.max_retries = 1u;
    request.payload_len = (uint16_t)payload_len;
    memcpy(request.payload, payload, payload_len);
    copy_cstr(request.request_id, sizeof(request.request_id),
              s_bridge.oled_canvas.commit_request_id);
    copy_cstr(request.command_name, sizeof(request.command_name),
              "oledCanvas.transfer");
    request.api_v1 = s_bridge.oled_canvas.api_v1;
    request.internal_kind = UTM_INTERNAL_OLED_CANVAS;
    request.timeout_ms = OLED_CANVAS_UART_PHASE_TIMEOUT_MS;
    request.deadline_ms = s_bridge.oled_canvas.total_deadline_ms;

    UtmEnqueueResult result = utm_enqueue(&s_bridge.transactions, &request, now);
    if (result != UTM_ENQUEUE_OK) return false;
    s_bridge.oled_canvas.expected_operation = operation;
    s_bridge.oled_canvas.expected_next_offset = expected_next;
    s_bridge.oled_canvas.uart_inflight = 1u;
    s_bridge.oled_canvas.phase = operation == OLED_CANVAS_OP_CANCEL
                            ? OLED_CANVAS_BRIDGE_CANCEL_UART
                            : OLED_CANVAS_BRIDGE_UART;
    return true;
}

static void oled_canvas_transport_failure(const char *code, const char *message)
{
    oled_canvas_send_commit_error(code, message, OLED_CANVAS_STATUS_TIMEOUT);
    s_bridge.oled_canvas.phase = OLED_CANVAS_BRIDGE_UART;
    oled_canvas_mark_cancel_requested(s_bridge.platform.now_ms() + OLED_CANVAS_CANCEL_TIMEOUT_MS);
}

static void oled_canvas_transaction_complete(UtmCompletionStatus status,
                                             const UtmFrame *frame)
{
    uint8_t expected_operation = s_bridge.oled_canvas.expected_operation;
    s_bridge.oled_canvas.uart_inflight = 0u;

    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE) return;
    if (status != UTM_COMPLETED_RESPONSE || !frame)
    {
        if (expected_operation == OLED_CANVAS_OP_CANCEL)
        {
            oled_canvas_send_cancel_error(status == UTM_COMPLETED_TIMEOUT
                                              ? "timeout" : "uart_error",
                                          "OLED Canvas cancel could not be confirmed");
            oled_canvas_send_commit_error(status == UTM_COMPLETED_TIMEOUT
                                              ? "timeout" : "uart_error",
                                          "OLED Canvas transfer could not be canceled",
                                          OLED_CANVAS_STATUS_TIMEOUT);
            oled_canvas_reset();
            return;
        }
        oled_canvas_transport_failure(status == UTM_COMPLETED_TIMEOUT
                                          ? "timeout" : "uart_error",
                                      status == UTM_COMPLETED_TIMEOUT
                                          ? "OLED Canvas UART phase timed out"
                                          : "OLED Canvas UART phase failed");
        return;
    }

    OledCanvasF4Response response;
    if (!OledCanvas_ParseF4Response(frame->payload, frame->payload_len, &response) ||
        response.operation != expected_operation ||
        response.transfer_id != s_bridge.oled_canvas.assembly.transfer_id)
    {
        oled_canvas_transport_failure("uart_error",
                                      "OLED Canvas response correlation mismatch");
        return;
    }

    if (response.status != OLED_CANVAS_STATUS_OK)
    {
        if (s_bridge.oled_canvas.cancel_request_id[0])
        {
            if (response.status == OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI)
            {
                oled_canvas_send_cancel_response(s_bridge.oled_canvas.client_id,
                                                 s_bridge.oled_canvas.api_v1,
                                                 s_bridge.oled_canvas.cancel_request_id,
                                                 s_bridge.oled_canvas.assembly.transfer_id);
            }
            else
            {
                oled_canvas_send_cancel_error(
                    oled_canvas_f4_error_code(response.status),
                    oled_canvas_f4_error_message(response.status));
            }
        }
        oled_canvas_send_commit_error(oled_canvas_f4_error_code(response.status),
                                      oled_canvas_f4_error_message(response.status),
                                      response.status);
        oled_canvas_reset();
        return;
    }

    if (s_bridge.oled_canvas.cancel_requested &&
        expected_operation != OLED_CANVAS_OP_CANCEL)
    {
        return;
    }

    if (expected_operation == OLED_CANVAS_OP_CANCEL)
    {
        if (s_bridge.oled_canvas.cancel_request_id[0])
            oled_canvas_send_cancel_response(s_bridge.oled_canvas.client_id,
                                             s_bridge.oled_canvas.api_v1,
                                             s_bridge.oled_canvas.cancel_request_id,
                                             s_bridge.oled_canvas.assembly.transfer_id);
        oled_canvas_send_commit_error("f4_nack", "OLED Canvas transfer was canceled",
                                      OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI);
        oled_canvas_reset();
        return;
    }

    if (response.next_offset != s_bridge.oled_canvas.expected_next_offset)
    {
        oled_canvas_transport_failure("uart_error",
                                      "OLED Canvas nextOffset correlation mismatch");
        return;
    }

    if (expected_operation == OLED_CANVAS_OP_BEGIN)
    {
        s_bridge.oled_canvas.begin_confirmed = 1u;
        return;
    }
    if (expected_operation == OLED_CANVAS_OP_CHUNK)
    {
        s_bridge.oled_canvas.uart_next_offset = response.next_offset;
        return;
    }
    if (expected_operation == OLED_CANVAS_OP_COMMIT)
    {
        if (response.computed_crc32 != s_bridge.oled_canvas.assembly.expected_crc32)
        {
            oled_canvas_send_commit_error("f4_nack",
                                          "FirmwareF4 returned a different OLED Canvas CRC32",
                                          OLED_CANVAS_STATUS_CRC_MISMATCH);
            oled_canvas_reset();
            return;
        }
        oled_canvas_send_rendered();
        oled_canvas_reset();
    }
}

static void service_oled_canvas()
{
    uint32_t now = s_bridge.platform.now_ms();
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE) return;

    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UPLOADING)
    {
        if (!oled_canvas_session_valid())
        {
            oled_canvas_reset();
            return;
        }
        if (oled_canvas_deadline_reached(now, s_bridge.oled_canvas.upload_deadline_ms))
            oled_canvas_reset();
        return;
    }

    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART)
    {
        if (!oled_canvas_session_valid())
        {
            oled_canvas_send_commit_error("unauthorized",
                                          "FirmwareF4 PIN session expired before transfer",
                                          OLED_CANVAS_STATUS_BAD_METADATA);
            oled_canvas_reset();
            return;
        }
        if (oled_canvas_deadline_reached(now, s_bridge.oled_canvas.total_deadline_ms))
        {
            oled_canvas_send_commit_error("timeout",
                                          "OLED Canvas waited too long for the UART reservation",
                                          OLED_CANVAS_STATUS_TIMEOUT);
            oled_canvas_reset();
            return;
        }
        if (!utm_inflight(&s_bridge.transactions) && utm_queued_count(&s_bridge.transactions) == 0u)
            (void)oled_canvas_enqueue_operation(OLED_CANVAS_OP_BEGIN);
        return;
    }

    if (s_bridge.oled_canvas.owner_connected && !oled_canvas_session_valid() &&
        !s_bridge.oled_canvas.cancel_requested)
    {
        oled_canvas_send_commit_error("unauthorized",
                                      "FirmwareF4 PIN session expired during transfer",
                                      OLED_CANVAS_STATUS_BAD_METADATA);
        oled_canvas_mark_cancel_requested(now + OLED_CANVAS_CANCEL_TIMEOUT_MS);
    }
    if (!s_bridge.oled_canvas.owner_connected && !s_bridge.oled_canvas.cancel_requested)
    {
        oled_canvas_mark_cancel_requested(now + OLED_CANVAS_CANCEL_TIMEOUT_MS);
    }
    if (oled_canvas_deadline_reached(now, s_bridge.oled_canvas.total_deadline_ms) &&
        !s_bridge.oled_canvas.cancel_requested)
    {
        oled_canvas_send_commit_error("timeout", "OLED Canvas transfer exceeded 15 seconds",
                                      OLED_CANVAS_STATUS_TIMEOUT);
        oled_canvas_mark_cancel_requested(now + OLED_CANVAS_CANCEL_TIMEOUT_MS);
    }
    if (s_bridge.oled_canvas.uart_inflight) return;

    if (s_bridge.oled_canvas.cancel_requested)
    {
        (void)oled_canvas_enqueue_operation(OLED_CANVAS_OP_CANCEL);
        return;
    }
    if (!s_bridge.oled_canvas.begin_confirmed)
    {
        (void)oled_canvas_enqueue_operation(OLED_CANVAS_OP_BEGIN);
        return;
    }
    if (s_bridge.oled_canvas.uart_next_offset < OLED_CANVAS_BYTES)
    {
        (void)oled_canvas_enqueue_operation(OLED_CANVAS_OP_CHUNK);
        return;
    }
    (void)oled_canvas_enqueue_operation(OLED_CANVAS_OP_COMMIT);
}

static void oled_canvas_on_disconnect(uint32_t client_id, uint32_t generation)
{
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE ||
        s_bridge.oled_canvas.client_id != client_id ||
        s_bridge.oled_canvas.generation != generation) return;
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UPLOADING ||
        s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART)
    {
        oled_canvas_reset();
        return;
    }
    s_bridge.oled_canvas.owner_connected = 0u;
    oled_canvas_mark_cancel_requested(s_bridge.platform.now_ms() + OLED_CANVAS_CANCEL_TIMEOUT_MS);
}

static void oled_canvas_on_screen_code(uint32_t screen_code)
{
    if (screen_code != OLED_CANVAS_SCREEN_CODE)
        oled_canvas_request_cancel("FirmwareF4 left the OLED Canvas screen",
                                   OLED_CANVAS_STATUS_WRONG_SCREEN);
}

static void oled_canvas_on_car_mode(uint8_t mode)
{
    if (mode != STM_TEST_MODE)
        oled_canvas_request_cancel("FirmwareF4 left TEST mode",
                                   OLED_CANVAS_STATUS_NOT_TEST_MODE);
}

static bool oled_canvas_require_owner(uint32_t client_id,
                                      uint16_t transfer_id,
                                      const char *request_id,
                                      const char *command)
{
    WsClientState *state = find_client(client_id);
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_IDLE)
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "No OLED Canvas transfer is active");
        return false;
    }
    if (!state || s_bridge.oled_canvas.client_id != client_id ||
        s_bridge.oled_canvas.generation != state->generation)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "Another client owns the OLED Canvas transfer");
        return false;
    }
    if (s_bridge.oled_canvas.assembly.transfer_id != transfer_id)
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "OLED Canvas transferId does not match");
        return false;
    }
    if (!oled_canvas_session_valid())
    {
        send_error(client_id, request_id, "esp", command, "unauthorized",
                   "The FirmwareF4 PIN session expired during OLED Canvas transfer");
        if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UPLOADING ||
            s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART)
        {
            oled_canvas_reset();
        }
        else
        {
            oled_canvas_send_commit_error(
                "unauthorized", "FirmwareF4 PIN session expired during transfer",
                OLED_CANVAS_STATUS_BAD_METADATA);
            if (!s_bridge.oled_canvas.cancel_requested)
                oled_canvas_mark_cancel_requested(
                    s_bridge.platform.now_ms() + OLED_CANVAS_CANCEL_TIMEOUT_MS);
        }
        return false;
    }
    return true;
}

static void cmd_oled_canvas_begin(uint32_t client_id, const char *request_id,
                                  const char *command, JsonObject params)
{
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_IDLE)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "An OLED Canvas transfer is already active");
        return;
    }
    WsmSession *session = session_for_client(client_id);
    WsClientState *client = find_client(client_id);
    uint32_t expected_crc32 = 0u;
    if (!session || !client || session->state != WSM_SLOT_ACTIVE ||
        !session->stm_authenticated || session->stm_expires_at_ms == 0u ||
        oled_canvas_deadline_reached(s_bridge.platform.now_ms(), session->stm_expires_at_ms) ||
        !OledCanvas_ParseCrc32Hex(params["crc32"].as<const char *>(),
                                  &expected_crc32))
    {
        send_error(client_id, request_id, "esp", command, "unauthorized",
                   "An active FirmwareF4 PIN session is required");
        return;
    }

    do { s_bridge.oled_canvas_next_transfer_id++; }
    while (s_bridge.oled_canvas_next_transfer_id == 0u);
    oled_canvas_reset();
    (void)OledCanvasAssembly_Begin(&s_bridge.oled_canvas.assembly,
                                   s_bridge.oled_canvas_next_transfer_id,
                                   expected_crc32);
    s_bridge.oled_canvas.phase = OLED_CANVAS_BRIDGE_UPLOADING;
    s_bridge.oled_canvas.api_v1 = client->preferred_api_v1;
    s_bridge.oled_canvas.client_id = client_id;
    s_bridge.oled_canvas.generation = client->generation;
    s_bridge.oled_canvas.node = session->node;
    s_bridge.oled_canvas.owner_connected = 1u;
    s_bridge.oled_canvas.upload_deadline_ms = s_bridge.platform.now_ms() + OLED_CANVAS_UPLOAD_TIMEOUT_MS;

    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base_for_api(doc, s_bridge.oled_canvas.api_v1,
                                                 request_id, "esp", command,
                                                 true, "OK", "OLED Canvas upload started");
    data["transferId"] = s_bridge.oled_canvas.assembly.transfer_id;
    data["chunkBytes"] = OLED_CANVAS_WEB_CHUNK_BYTES;
    data["nextOffset"] = 0u;
    send_doc_to_client(client_id, doc);
}

static void cmd_oled_canvas_chunk(uint32_t client_id, const char *request_id,
                                  const char *command, JsonObject params)
{
    uint32_t transfer_value = 0u;
    uint32_t offset_value = 0u;
    (void)read_json_u32(params, "transferId", &transfer_value);
    (void)read_json_u32(params, "offset", &offset_value);
    uint16_t transfer_id = (uint16_t)transfer_value;
    if (!oled_canvas_require_owner(client_id, transfer_id, request_id, command)) return;
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_UPLOADING)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "OLED Canvas upload is already committing");
        return;
    }
    if (oled_canvas_deadline_reached(s_bridge.platform.now_ms(), s_bridge.oled_canvas.upload_deadline_ms))
    {
        oled_canvas_reset();
        send_error(client_id, request_id, "esp", command, "timeout",
                   "OLED Canvas upload exceeded 5 seconds");
        return;
    }

    uint8_t decoded[OLED_CANVAS_WEB_CHUNK_BYTES];
    size_t decoded_len = 0u;
    const char *encoded = params["dataBase64"].as<const char *>();
    if (!OledCanvas_DecodeBase64(encoded, strlen(encoded), decoded,
                                 sizeof(decoded), &decoded_len) ||
        decoded_len != sizeof(decoded))
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   "dataBase64 is not a canonical 512-byte Base64 payload");
        return;
    }

    OledCanvasResult result = OledCanvasAssembly_Append(
        &s_bridge.oled_canvas.assembly, transfer_id, (uint16_t)offset_value,
        decoded, decoded_len);
    if (result != OLED_CANVAS_RESULT_OK && result != OLED_CANVAS_RESULT_DUPLICATE)
    {
        send_error(client_id, request_id, "esp", command, "invalid_request",
                   result == OLED_CANVAS_RESULT_DATA_MISMATCH
                       ? "Duplicate OLED Canvas chunk has different bytes"
                       : "OLED Canvas chunks must arrive at offsets 0 and 512");
        return;
    }

    StaticJsonDocument<384> doc;
    JsonObject data = make_response_base_for_api(doc, s_bridge.oled_canvas.api_v1,
                                                 request_id, "esp", command,
                                                 true, "OK", "OLED Canvas chunk accepted");
    data["transferId"] = transfer_id;
    data["nextOffset"] = s_bridge.oled_canvas.assembly.next_offset;
    send_doc_to_client(client_id, doc);
}

static void cmd_oled_canvas_commit(uint32_t client_id, const char *request_id,
                                   const char *command, JsonObject params)
{
    uint32_t transfer_value = 0u;
    (void)read_json_u32(params, "transferId", &transfer_value);
    uint16_t transfer_id = (uint16_t)transfer_value;
    if (!oled_canvas_require_owner(client_id, transfer_id, request_id, command)) return;
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_UPLOADING)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "OLED Canvas commit is already active");
        return;
    }

    OledCanvasResult result = OledCanvasAssembly_Commit(&s_bridge.oled_canvas.assembly,
                                                        transfer_id);
    if (result != OLED_CANVAS_RESULT_OK)
    {
        const char *message = result == OLED_CANVAS_RESULT_INCOMPLETE
                                ? "Both 512-byte OLED Canvas chunks are required"
                                : "ESP CRC32 does not match oledCanvas.begin";
        send_error(client_id, request_id, "esp", command, "invalid_request", message);
        oled_canvas_reset();
        return;
    }
    copy_cstr(s_bridge.oled_canvas.commit_request_id,
              sizeof(s_bridge.oled_canvas.commit_request_id), request_id);
    s_bridge.oled_canvas.phase = OLED_CANVAS_BRIDGE_WAITING_UART;
    s_bridge.oled_canvas.total_deadline_ms = s_bridge.platform.now_ms() + OLED_CANVAS_UART_TOTAL_TIMEOUT_MS;
}

static void cmd_oled_canvas_cancel(uint32_t client_id, const char *request_id,
                                   const char *command, JsonObject params)
{
    uint32_t transfer_value = 0u;
    (void)read_json_u32(params, "transferId", &transfer_value);
    uint16_t transfer_id = (uint16_t)transfer_value;
    if (!oled_canvas_require_owner(client_id, transfer_id, request_id, command)) return;

    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_UPLOADING)
    {
        uint8_t api_v1 = s_bridge.oled_canvas.api_v1;
        oled_canvas_reset();
        oled_canvas_send_cancel_response(client_id, api_v1, request_id, transfer_id);
        return;
    }
    if (s_bridge.oled_canvas.phase == OLED_CANVAS_BRIDGE_WAITING_UART)
    {
        oled_canvas_send_commit_error("f4_nack", "OLED Canvas transfer was canceled",
                                      OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI);
        uint8_t api_v1 = s_bridge.oled_canvas.api_v1;
        oled_canvas_reset();
        oled_canvas_send_cancel_response(client_id, api_v1, request_id, transfer_id);
        return;
    }
    if (s_bridge.oled_canvas.cancel_requested)
    {
        send_error(client_id, request_id, "esp", command, "busy",
                   "OLED Canvas cancel is already pending");
        return;
    }
    copy_cstr(s_bridge.oled_canvas.cancel_request_id,
              sizeof(s_bridge.oled_canvas.cancel_request_id), request_id);
    oled_canvas_mark_cancel_requested(s_bridge.platform.now_ms() + OLED_CANVAS_CANCEL_TIMEOUT_MS);
}

