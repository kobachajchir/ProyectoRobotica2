// Private bridge implementation; included once by ws_json_bridge.cpp.
static WsmSession *session_for_client(uint32_t client_id)
{
    WsClientState *state = find_client(client_id);
    if (!state) return nullptr;
    return wsm_find(&s_bridge.sessions, client_id, state->generation);
}

static UtmCommandOwner transaction_owner(F4_CommandOwner owner)
{
    switch (owner)
    {
    case F4_COMMAND_OWNER_F4: return UTM_OWNER_F4;
    case F4_COMMAND_OWNER_DUAL: return UTM_OWNER_DUAL;
    case F4_COMMAND_OWNER_ESP: return UTM_OWNER_ESP;
    default: return UTM_OWNER_RESERVED;
    }
}

static UtmResponsePolicy transaction_response_policy(F4_ResponsePolicy policy)
{
    switch (policy)
    {
    case F4_RESPONSE_ACK: return UTM_RESPONSE_ACK;
    case F4_RESPONSE_ACK_THEN_SAME_COMMAND: return UTM_RESPONSE_COMMAND;
    case F4_RESPONSE_SAME_COMMAND: return UTM_RESPONSE_COMMAND;
    default: return UTM_RESPONSE_COMMAND;
    }
}

static void emit_request_accepted(uint32_t client_id, const char *request_id,
                                  const char *command, uint8_t queue_depth)
{
    StaticJsonDocument<384> doc;
    WsClientState *state = find_client(client_id);
    JsonObject data;
    if (state && state->preferred_api_v1)
    {
        doc["api"] = 1;
        doc["type"] = "event";
        doc["event"] = "requestAccepted";
        data = doc.createNestedObject("data");
        data["requestId"] = request_id ? request_id : "";
    }
    else
    {
        data = make_event_base(doc, "request.accepted", "esp", request_id);
    }
    data["command"] = command ? command : "";
    data["state"] = queue_depth > 0u ? "queued" : "accepted";
    data["queueDepth"] = queue_depth;
    send_doc_to_client(client_id, doc);
}

static bool start_stm_request(uint32_t client_id, const char *request_id, const char *command,
                              uint8_t stm_cmd, uint8_t response_cmd, const uint8_t *payload, uint8_t payload_len,
                              uint16_t period_ms)
{
    (void)response_cmd;
    (void)period_ms;
    if (!s_bridge.app)
    {
        send_error(client_id, request_id, "stm", command, "ERR_INTERNAL", "UNER app not initialized");
        return false;
    }
    if (s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_IDLE &&
        s_bridge.oled_canvas.phase != OLED_CANVAS_BRIDGE_UPLOADING)
    {
        send_error(client_id, request_id, "stm", command, "busy",
                   "UART is reserved by an OLED Canvas transfer");
        return false;
    }
    if ((uint16_t)(payload_len + STM_UNER_OVERHEAD) > STM_FRAME_MAX)
    {
        send_error(client_id, request_id, "stm", command, "ERR_STM_FRAME_TOO_LARGE", "UNER frame would exceed 254 bytes");
        return false;
    }
    WsmSession *session = session_for_client(client_id);
    if (!session || session->state != WSM_SLOT_ACTIVE)
    {
        send_error(client_id, request_id, "stm", command, "unauthorized", "WebSocket session is not active");
        return false;
    }

    F4_CommandSpec spec_storage;
    const F4_CommandSpec *spec = lookup_f4_command(stm_cmd, spec_storage);
    if (!spec || !F4_CommandRegistry_IsWireAllowed(spec) ||
        (spec->owner != F4_COMMAND_OWNER_F4 && spec->owner != F4_COMMAND_OWNER_DUAL))
    {
        send_error(client_id, request_id, "stm", command, "unsupported_by_f4", "Command is not implemented by the current F4 contract");
        return false;
    }
    if (!F4_CommandRegistry_IsRequestLengthValid(spec, payload_len))
    {
        send_error(client_id, request_id, "stm", command, "invalid_request", "Payload length does not match the F4 command contract");
        return false;
    }
    if (spec->privilege == F4_PRIVILEGE_STM_SESSION && !session->stm_authenticated)
    {
        send_error(client_id, request_id, "stm", command, "unauthorized", "An active STM32 session is required");
        return false;
    }

    UtmRequest request;
    utm_request_init(&request);
    request.client_id = client_id;
    request.generation = session->generation;
    request.node = session->node;
    request.dst_node = UNER_NODE_MCU;
    request.command_id = stm_cmd;
    request.owner = transaction_owner(spec->owner);
    request.response_policy = transaction_response_policy(spec->response_policy);
    request.retry_policy = spec->idempotent ? UTM_RETRY_IDEMPOTENT : UTM_RETRY_NONE;
    request.idempotent = spec->idempotent ? 1u : 0u;
    request.max_retries = spec->idempotent ? 1u : 0u;
    request.payload_len = payload_len;
    if (payload_len > 0u && payload) memcpy(request.payload, payload, payload_len);
    copy_cstr(request.request_id, sizeof(request.request_id), request_id);
    copy_cstr(request.command_name, sizeof(request.command_name), command);
    WsClientState *client_state = find_client(client_id);
    request.api_v1 = client_state ? client_state->preferred_api_v1 : 0u;
    request.internal_kind = UTM_INTERNAL_WEB;
    request.timeout_ms = spec->timeout_ms ? spec->timeout_ms : STM_TIMEOUT_MS;

    UtmEnqueueResult queued = utm_enqueue(&s_bridge.transactions, &request, s_bridge.platform.now_ms());
    if (queued != UTM_ENQUEUE_OK)
    {
        const char *code = queued == UTM_ENQUEUE_INCOMPATIBLE_OWNER ? "unsupported_by_f4" : "busy";
        send_error(client_id, request_id, "stm", command, code,
                   queued == UTM_ENQUEUE_CLIENT_LIMIT ? "Per-client UART queue limit reached" : "UART transaction queue is full");
        return false;
    }
    emit_request_accepted(client_id, request_id, command, utm_queued_count(&s_bridge.transactions));
    return true;
}

static bool dispatch_stream_subscription(uint32_t client_id,
                                         const char *request_id,
                                         const char *command,
                                         SbStream stream,
                                         bool enable,
                                         uint16_t requested_period_ms)
{
    if (oled_canvas_uart_reserved())
    {
        send_error(client_id, request_id, "stm", command, "busy",
                   "UART is reserved by an OLED Canvas transfer");
        return false;
    }
    WsmSession *session = session_for_client(client_id);
    if (!session || !session->stm_authenticated)
    {
        send_error(client_id, request_id, "stm", command, "unauthorized",
                   "An active STM32 session is required");
        return false;
    }
    if (!s_bridge.app)
    {
        send_error(client_id, request_id, "stm", command, "uart_error",
                   "UNER UART backend is not initialized");
        return false;
    }
    if (srw_has_pending(&s_bridge.stream_waiters, client_id, session->generation, stream))
    {
        send_error(client_id, request_id, "stm", command, "busy",
                   "This client already has a pending request for the same stream");
        return false;
    }

    WsClientState *client_state = find_client(client_id);
    uint8_t api_v1 = client_state && client_state->preferred_api_v1 ? 1u : 0u;
    uint16_t period_ms = enable ? sb_clamp_period(requested_period_ms) : 0u;
    SrwAddResult add_result = srw_add(&s_bridge.stream_waiters,
                                      client_id, session->generation, api_v1,
                                      stream, enable ? 1u : 0u, period_ms,
                                      request_id, command,
                                      s_bridge.platform.now_ms() + STREAM_REQUEST_TIMEOUT_MS);
    if (add_result != SRW_ADD_OK)
    {
        send_error_for_api(client_id, api_v1, request_id, "stm", command,
                           "busy", add_result == SRW_ADD_FULL
                               ? "Stream response waiter capacity is exhausted"
                               : "A stream request is already pending");
        return false;
    }

    SbResult broker_result = enable
        ? sb_subscribe(&s_bridge.streams, client_id, session->generation, session->node,
                       stream, period_ms)
        : sb_unsubscribe(&s_bridge.streams, client_id, session->generation, stream);
    if (broker_result != SB_OK && broker_result != SB_NOT_FOUND)
    {
        SrwCompletion discarded;
        (void)srw_take_endpoint_stream(&s_bridge.stream_waiters,
                                       client_id, session->generation, stream,
                                       SRW_COMPLETED_CANCELED, &discarded);
        send_error_for_api(client_id, api_v1, request_id, "stm", command,
                           broker_result == SB_CAPACITY ? "busy" : "invalid_request",
                           broker_result == SB_CAPACITY
                               ? "Stream subscriber capacity is exhausted"
                               : "Invalid stream subscription state");
        return false;
    }

    if (sb_is_converged(&s_bridge.streams, stream))
    {
        SrwCompletion completion;
        if (srw_take_endpoint_stream(&s_bridge.stream_waiters,
                                     client_id, session->generation, stream,
                                     SRW_COMPLETED_CONVERGED, &completion))
            deliver_stream_completion(&completion);
        return true;
    }

    emit_request_accepted(client_id, request_id, command,
                          utm_queued_count(&s_bridge.transactions));
    return true;
}

