#include "screens_internal.h"
/*
 * oled_canvas.c
 *
 * Receives one SSD1306 page-major framebuffer at a time, validates it with
 * CRC-32/ISO-HDLC, presents it without exposing the driver's private buffer,
 * and acknowledges COMMIT only after the asynchronous I2C transfer finishes.
 */
#include "oled_canvas.h"

#include <string.h>

#include "fonts.h"
#include "types/menu_types.h"
#include "oled_utils.h"
#include "screenWrappers.h"
#include "screen_codes.h"
#include "uner_v2.h"
#include "utils/macros_utils.h"












static uint16_t OledCanvas_ReadU16Le(const uint8_t *data)
{
    return (uint16_t)(((uint16_t)data[0]) | ((uint16_t)data[1] << 8));
}

static uint32_t OledCanvas_ReadU32Le(const uint8_t *data)
{
    return ((uint32_t)data[0]) |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

static void OledCanvas_WriteU16Le(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)(value & 0xFFu);
    data[1] = (uint8_t)((value >> 8) & 0xFFu);
}

static void OledCanvas_WriteU32Le(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)(value & 0xFFu);
    data[1] = (uint8_t)((value >> 8) & 0xFFu);
    data[2] = (uint8_t)((value >> 16) & 0xFFu);
    data[3] = (uint8_t)((value >> 24) & 0xFFu);
}

static uint32_t OledCanvas_Crc32(const uint8_t *data, uint16_t len)
{
    uint32_t crc = 0xFFFFFFFFu;

    for (uint16_t i = 0u; i < len; ++i) {
        crc ^= data[i];
        for (uint8_t bit = 0u; bit < 8u; ++bit) {
            const uint32_t reflected_polynomial =
                (crc & 1u) ? 0xEDB88320u : 0u;
            crc = (crc >> 1) ^ reflected_polynomial;
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

static uint8_t OledCanvas_GuardStatus(uint8_t source_node)
{
    if (GET_CAR_MODE() != TEST_MODE) {
        return OLED_CANVAS_STATUS_NOT_TEST_MODE;
    }
    if (MenuSys_GetCurrentScreenCode(&menuSystem) !=
        SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS) {
        return OLED_CANVAS_STATUS_WRONG_SCREEN;
    }
    if (source_node != UNER_NODE_PC_QT &&
        (source_node < UNER_NODE_WEB_CLIENT_FIRST ||
         source_node > UNER_NODE_WEB_CLIENT_LAST)) {
        return OLED_CANVAS_STATUS_BAD_METADATA;
    }
    if (!Permission_IsRemoteNodeAuthorizedNow(source_node)) {
        return OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI;
    }

    return OLED_CANVAS_STATUS_OK;
}

static void OledCanvas_TrySendPendingResponse(void)
{
    OledCanvasPendingResponse_t *pending = &oled_canvas.pending_response;

    if (!pending->valid || oled_canvas.send_response == NULL) {
        return;
    }

    if (oled_canvas.send_response(pending->transport_id,
                                  pending->destination_node,
                                  pending->payload,
                                  OLED_CANVAS_RESPONSE_BYTES,
                                  oled_canvas.send_ctx)) {
        memset(pending, 0, sizeof(*pending));
    }
}

static void OledCanvas_QueueResponse(uint8_t operation,
                                     uint8_t status,
                                     uint8_t transport_id,
                                     uint8_t destination_node,
                                     uint16_t transfer_id,
                                     uint16_t next_offset,
                                     uint32_t computed_crc32)
{
    OledCanvasPendingResponse_t *pending = &oled_canvas.pending_response;

    OledCanvas_TrySendPendingResponse();
    if (pending->valid) {
        return;
    }

    pending->transport_id = transport_id;
    pending->destination_node = destination_node;
    pending->payload[0] = operation;
    pending->payload[1] = status;
    OledCanvas_WriteU16Le(&pending->payload[2], transfer_id);
    OledCanvas_WriteU16Le(&pending->payload[4], next_offset);
    OledCanvas_WriteU32Le(&pending->payload[6], computed_crc32);
    pending->valid = 1u;
    OledCanvas_TrySendPendingResponse();
}

static void OledCanvas_RememberResult(uint8_t status)
{
    oled_canvas.last_result_valid = 1u;
    oled_canvas.last_result_source_node = oled_canvas.source_node;
    oled_canvas.last_result_transfer_id = oled_canvas.transfer_id;
    oled_canvas.last_result_status = status;
    oled_canvas.last_result_crc32 = oled_canvas.computed_crc32;
}

static void OledCanvas_ResetTransferState(void)
{
    oled_canvas.state = OLED_CANVAS_STATE_IDLE;
    oled_canvas.transport_id = 0u;
    oled_canvas.source_node = 0u;
    oled_canvas.transfer_id = 0u;
    oled_canvas.received_len = 0u;
    oled_canvas.expected_crc32 = 0u;
    oled_canvas.computed_crc32 = 0u;
    oled_canvas.last_activity_ms = 0u;
    oled_canvas.display_error_count_before = 0u;
    oled_canvas.cancel_display_status = OLED_CANVAS_STATUS_OK;
    oled_canvas.deferred_ui_action = OLED_CANVAS_UI_NONE;
    memset(oled_canvas.framebuffer, 0, sizeof(oled_canvas.framebuffer));
}

static void OledCanvas_ShowReadyUi(void)
{
    menuSystem.renderFn = OledUtils_RenderOledCanvas_Wrapper;
    menuSystem.userEventManagerFn = oledCanvasEventManager;
    menuSystem.allowPeriodicRefresh = false;
    if (menuSystem.insideMenuFlag != NULL) {
        *menuSystem.insideMenuFlag = 0u;
    }
    autoMicroSystem.runtime.inside_menu_flag = 0u;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    autoMicroSystem.runtime.oled_first_draw = false;
    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS,
                                 SCREEN_REPORT_SOURCE_RENDER);
    menuSystem.renderFlag = true;
}

static void OledCanvas_ExitToDisplayMenu(void)
{
    menuSystem.currentMenu = &submenuSensorsDisplay;
    submenuSensorsDisplay.lastSelectedItemIndex = -1;
    submenuSensorsDisplay.lastVisibleItem = -1;
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.allowPeriodicRefresh = false;
    if (menuSystem.insideMenuFlag != NULL) {
        *menuSystem.insideMenuFlag = 1u;
    }
    autoMicroSystem.runtime.inside_menu_flag = 1u;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }
    MenuSys_SetCurrentMenuScreenCode(&menuSystem);
    menuSystem.renderFlag = true;
}

static void OledCanvas_ExitToDashboard(void)
{
    menuSystem.currentMenu = &mainMenu;
    menuSystem.renderFn = (menuSystem.dashboardRender != NULL)
        ? menuSystem.dashboardRender
        : OledUtils_RenderDashboard_Wrapper;
    menuSystem.userEventManagerFn = dashboardEventManager;
    menuSystem.allowPeriodicRefresh = false;
    if (menuSystem.insideMenuFlag != NULL) {
        *menuSystem.insideMenuFlag = 0u;
    }
    autoMicroSystem.runtime.inside_menu_flag = 0u;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    if (menuSystem.clearScreen != NULL) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
}

static void OledCanvas_ApplyUiAction(OledCanvasUiAction_t action)
{
    switch (action) {
        case OLED_CANVAS_UI_SHOW_READY:
            OledCanvas_ShowReadyUi();
            break;
        case OLED_CANVAS_UI_EXIT_TO_MENU:
            OledCanvas_ExitToDisplayMenu();
            break;
        case OLED_CANVAS_UI_EXIT_TO_DASHBOARD:
            OledCanvas_ExitToDashboard();
            break;
        default:
            break;
    }
}

static void OledCanvas_CancelForUi(uint8_t status, OledCanvasUiAction_t action)
{
    if (oled_canvas.state == OLED_CANVAS_STATE_DISPLAYING) {
        oled_canvas.cancel_display_status = status;
        oled_canvas.deferred_ui_action = action;
        return;
    }

    if (oled_canvas.state == OLED_CANVAS_STATE_COMMIT_PENDING) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 status,
                                 oled_canvas.transport_id,
                                 oled_canvas.source_node,
                                 oled_canvas.transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        OledCanvas_RememberResult(status);
    } else if (oled_canvas.state == OLED_CANVAS_STATE_RECEIVING) {
        OledCanvas_RememberResult(status);
    }

    OledCanvas_ResetTransferState();
    OledCanvas_ApplyUiAction(action);
}

static void OledCanvas_HandleBegin(const UNER_Packet *packet)
{
    uint16_t transfer_id;
    uint16_t total_len;
    uint32_t expected_crc32;

    if (packet->len != 12u || packet->payload == NULL) {
        transfer_id = (packet->payload != NULL && packet->len >= 3u)
            ? OledCanvas_ReadU16Le(&packet->payload[1])
            : 0u;
        OledCanvas_QueueResponse(OLED_CANVAS_OP_BEGIN,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 0u,
                                 0u);
        return;
    }

    transfer_id = OledCanvas_ReadU16Le(&packet->payload[1]);
    total_len = OledCanvas_ReadU16Le(&packet->payload[6]);
    expected_crc32 = OledCanvas_ReadU32Le(&packet->payload[8]);

    if (packet->payload[3] != OLED_CANVAS_WIDTH ||
        packet->payload[4] != OLED_CANVAS_HEIGHT ||
        packet->payload[5] != OLED_CANVAS_FORMAT_SSD1306_PAGE_LSB ||
        total_len != OLED_CANVAS_FRAMEBUFFER_BYTES) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_BEGIN,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 0u,
                                 0u);
        return;
    }

    if (oled_canvas.state != OLED_CANVAS_STATE_IDLE &&
        oled_canvas.source_node == packet->src &&
        oled_canvas.transfer_id == transfer_id) {
        const uint8_t status = (oled_canvas.expected_crc32 == expected_crc32)
            ? OLED_CANVAS_STATUS_OK
            : OLED_CANVAS_STATUS_BAD_METADATA;

        if (status == OLED_CANVAS_STATUS_OK &&
            oled_canvas.state == OLED_CANVAS_STATE_RECEIVING) {
            oled_canvas.last_activity_ms = Screens_NowMs();
        }
        OledCanvas_QueueResponse(OLED_CANVAS_OP_BEGIN,
                                 status,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    if (oled_canvas.state != OLED_CANVAS_STATE_IDLE &&
        oled_canvas.state != OLED_CANVAS_STATE_DISPLAYED) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_BEGIN,
                                 OLED_CANVAS_STATUS_BUSY,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    OledCanvas_ResetTransferState();
    oled_canvas.state = OLED_CANVAS_STATE_RECEIVING;
    oled_canvas.transport_id = packet->transport_id;
    oled_canvas.source_node = packet->src;
    oled_canvas.transfer_id = transfer_id;
    oled_canvas.expected_crc32 = expected_crc32;
    oled_canvas.last_activity_ms = Screens_NowMs();
    oled_canvas.last_result_valid = 0u;

    OledCanvas_QueueResponse(OLED_CANVAS_OP_BEGIN,
                             OLED_CANVAS_STATUS_OK,
                             packet->transport_id,
                             packet->src,
                             transfer_id,
                             0u,
                             0u);
}

static uint8_t OledCanvas_ExpectedChunkLength(uint16_t offset)
{
    const uint16_t remaining = OLED_CANVAS_FRAMEBUFFER_BYTES - offset;
    return (remaining > OLED_CANVAS_UART_CHUNK_BYTES)
        ? OLED_CANVAS_UART_CHUNK_BYTES
        : (uint8_t)remaining;
}

static void OledCanvas_HandleChunk(const UNER_Packet *packet)
{
    uint16_t transfer_id;
    uint16_t offset;
    uint8_t data_len;
    uint8_t expected_len;

    if (packet->payload == NULL || packet->len < 7u) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 0u,
                                 0u,
                                 0u);
        return;
    }

    transfer_id = OledCanvas_ReadU16Le(&packet->payload[1]);
    offset = OledCanvas_ReadU16Le(&packet->payload[3]);
    data_len = packet->payload[5];

    if (offset >= OLED_CANVAS_FRAMEBUFFER_BYTES ||
        data_len == 0u ||
        data_len > OLED_CANVAS_UART_CHUNK_BYTES ||
        packet->len != (uint8_t)(6u + data_len) ||
        (uint16_t)(offset + data_len) > OLED_CANVAS_FRAMEBUFFER_BYTES) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    expected_len = OledCanvas_ExpectedChunkLength(offset);
    if (data_len != expected_len ||
        (offset % OLED_CANVAS_UART_CHUNK_BYTES) != 0u) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    if (oled_canvas.state != OLED_CANVAS_STATE_RECEIVING ||
        oled_canvas.source_node != packet->src ||
        oled_canvas.transfer_id != transfer_id) {
        uint8_t status = OLED_CANVAS_STATUS_BUSY;
        if (oled_canvas.last_result_valid &&
            oled_canvas.last_result_source_node == packet->src &&
            oled_canvas.last_result_transfer_id == transfer_id) {
            status = oled_canvas.last_result_status;
        }
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 status,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    if (offset < oled_canvas.received_len) {
        const uint8_t duplicate_matches =
            ((uint16_t)(offset + data_len) <= oled_canvas.received_len &&
             memcmp(&oled_canvas.framebuffer[offset],
                    &packet->payload[6],
                    data_len) == 0) ? 1u : 0u;
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 duplicate_matches
                                     ? OLED_CANVAS_STATUS_OK
                                     : OLED_CANVAS_STATUS_OUT_OF_ORDER,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    if (offset != oled_canvas.received_len) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                                 OLED_CANVAS_STATUS_OUT_OF_ORDER,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    memcpy(&oled_canvas.framebuffer[offset], &packet->payload[6], data_len);
    oled_canvas.received_len = (uint16_t)(offset + data_len);
    oled_canvas.last_activity_ms = Screens_NowMs();
    OledCanvas_QueueResponse(OLED_CANVAS_OP_CHUNK,
                             OLED_CANVAS_STATUS_OK,
                             packet->transport_id,
                             packet->src,
                             transfer_id,
                             oled_canvas.received_len,
                             0u);
}

static void OledCanvas_HandleCommit(const UNER_Packet *packet)
{
    uint16_t transfer_id;

    if (packet->payload == NULL || packet->len != 3u) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 0u,
                                 0u,
                                 0u);
        return;
    }

    transfer_id = OledCanvas_ReadU16Le(&packet->payload[1]);

    if (oled_canvas.state == OLED_CANVAS_STATE_DISPLAYED &&
        oled_canvas.source_node == packet->src &&
        oled_canvas.transfer_id == transfer_id) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 OLED_CANVAS_STATUS_OK,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 OLED_CANVAS_FRAMEBUFFER_BYTES,
                                 oled_canvas.computed_crc32);
        return;
    }

    if ((oled_canvas.state == OLED_CANVAS_STATE_COMMIT_PENDING ||
         oled_canvas.state == OLED_CANVAS_STATE_DISPLAYING) &&
        oled_canvas.source_node == packet->src &&
        oled_canvas.transfer_id == transfer_id) {
        /* The original deferred response remains authoritative. */
        return;
    }

    if (oled_canvas.state != OLED_CANVAS_STATE_RECEIVING ||
        oled_canvas.source_node != packet->src ||
        oled_canvas.transfer_id != transfer_id) {
        uint8_t status = OLED_CANVAS_STATUS_BAD_METADATA;
        uint32_t crc = 0u;
        if (oled_canvas.last_result_valid &&
            oled_canvas.last_result_source_node == packet->src &&
            oled_canvas.last_result_transfer_id == transfer_id) {
            status = oled_canvas.last_result_status;
            crc = oled_canvas.last_result_crc32;
        }
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 status,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 0u,
                                 crc);
        return;
    }

    if (oled_canvas.received_len != OLED_CANVAS_FRAMEBUFFER_BYTES) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 OLED_CANVAS_STATUS_OUT_OF_ORDER,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 0u);
        return;
    }

    oled_canvas.computed_crc32 = OledCanvas_Crc32(
        oled_canvas.framebuffer,
        OLED_CANVAS_FRAMEBUFFER_BYTES);
    if (oled_canvas.computed_crc32 != oled_canvas.expected_crc32) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 OLED_CANVAS_STATUS_CRC_MISMATCH,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        OledCanvas_RememberResult(OLED_CANVAS_STATUS_CRC_MISMATCH);
        OledCanvas_ResetTransferState();
        return;
    }

    oled_canvas.state = OLED_CANVAS_STATE_COMMIT_PENDING;
    oled_canvas.last_activity_ms = Screens_NowMs();
}

static void OledCanvas_HandleCancel(const UNER_Packet *packet)
{
    uint16_t transfer_id;

    if (packet->payload == NULL || packet->len != 3u) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CANCEL,
                                 OLED_CANVAS_STATUS_BAD_METADATA,
                                 packet->transport_id,
                                 packet->src,
                                 0u,
                                 0u,
                                 0u);
        return;
    }

    transfer_id = OledCanvas_ReadU16Le(&packet->payload[1]);
    if (oled_canvas.state == OLED_CANVAS_STATE_DISPLAYING &&
        oled_canvas.source_node == packet->src &&
        oled_canvas.transfer_id == transfer_id) {
        oled_canvas.cancel_display_status = OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI;
        oled_canvas.deferred_ui_action = OLED_CANVAS_UI_SHOW_READY;
    } else if ((oled_canvas.state != OLED_CANVAS_STATE_IDLE) &&
               (oled_canvas.source_node != packet->src ||
                oled_canvas.transfer_id != transfer_id)) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_CANCEL,
                                 OLED_CANVAS_STATUS_BUSY,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    } else {
        OledCanvas_ResetTransferState();
        OledCanvas_ShowReadyUi();
    }

    OledCanvas_QueueResponse(OLED_CANVAS_OP_CANCEL,
                             OLED_CANVAS_STATUS_OK,
                             packet->transport_id,
                             packet->src,
                             transfer_id,
                             0u,
                             0u);
}

void OledCanvas_Init(OledCanvasSendResponseFn send_response, void *ctx)
{
    memset(&oled_canvas, 0, sizeof(oled_canvas));
    oled_canvas.send_response = send_response;
    oled_canvas.send_ctx = ctx;
}

void OledCanvas_BeginFromMenu(void)
{
    OledCanvas_ResetTransferState();
    oled_canvas.last_result_valid = 0u;
    OledUtils_DismissNotification();
    OledCanvas_ShowReadyUi();
}

void OledCanvas_RenderReadyScreen(void)
{
    static char line1[] = "Listo para";
    static char line2[] = "recibir canvas";
    const uint8_t line1_width = (uint8_t)((sizeof(line1) - 1u) * Font_7x10.FontWidth);
    const uint8_t line2_width = (uint8_t)((sizeof(line2) - 1u) * Font_7x10.FontWidth);

    ssd1306_Clear();
    ssd1306_SetColor(White);
    ssd1306_SetCursor((uint8_t)((OLED_CANVAS_WIDTH - line1_width) / 2u), 20u);
    (void)ssd1306_WriteString(line1, Font_7x10);
    ssd1306_SetCursor((uint8_t)((OLED_CANVAS_WIDTH - line2_width) / 2u), 36u);
    (void)ssd1306_WriteString(line2, Font_7x10);
    autoMicroSystem.runtime.oled_first_draw = true;
}

void OledCanvas_HandleLocalEvent(UserEvent_t event)
{
    if (event == UE_ROTATE_CW || event == UE_ROTATE_CCW) {
        OledCanvas_CancelForUi(OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI,
                               OLED_CANVAS_UI_SHOW_READY);
    } else if (event == UE_ENC_SHORT_PRESS) {
        OledCanvas_CancelForUi(OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI,
                               OLED_CANVAS_UI_EXIT_TO_MENU);
    }
}

void OledCanvas_HandlePacket(const UNER_Packet *packet)
{
    uint8_t status;
    uint16_t transfer_id = 0u;

    if (packet == NULL || packet->payload == NULL || packet->len == 0u) {
        return;
    }

    if (packet->len >= 3u) {
        transfer_id = OledCanvas_ReadU16Le(&packet->payload[1]);
    }

    status = OledCanvas_GuardStatus(packet->src);
    if (status != OLED_CANVAS_STATUS_OK) {
        OledCanvas_QueueResponse(packet->payload[0],
                                 status,
                                 packet->transport_id,
                                 packet->src,
                                 transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        return;
    }

    switch (packet->payload[0]) {
        case OLED_CANVAS_OP_BEGIN:
            OledCanvas_HandleBegin(packet);
            break;
        case OLED_CANVAS_OP_CHUNK:
            OledCanvas_HandleChunk(packet);
            break;
        case OLED_CANVAS_OP_COMMIT:
            OledCanvas_HandleCommit(packet);
            break;
        case OLED_CANVAS_OP_CANCEL:
            OledCanvas_HandleCancel(packet);
            break;
        default:
            OledCanvas_QueueResponse(packet->payload[0],
                                     OLED_CANVAS_STATUS_BAD_METADATA,
                                     packet->transport_id,
                                     packet->src,
                                     transfer_id,
                                     oled_canvas.received_len,
                                     oled_canvas.computed_crc32);
            break;
    }
}

void OledCanvas_Poll(void)
{
    uint8_t guard_status;
    OledCanvasUiAction_t action;

    OledCanvas_TrySendPendingResponse();

    /* The mode transition is authoritative even when there is no transfer in
     * progress or the last image is already displayed.  Leaving TEST must not
     * strand the terminal Canvas screen on the OLED. */
    if (OledCanvas_IsActiveScreen() && GET_CAR_MODE() != TEST_MODE) {
        if (oled_canvas.state == OLED_CANVAS_STATE_DISPLAYING) {
            /* Keep polling the in-flight I2C update so the deferred COMMIT
             * response and dashboard transition can complete.  Re-entering
             * this guard on every loop must not strand DISPLAYING forever. */
            if (oled_canvas.cancel_display_status !=
                    OLED_CANVAS_STATUS_NOT_TEST_MODE ||
                oled_canvas.deferred_ui_action !=
                    OLED_CANVAS_UI_EXIT_TO_DASHBOARD) {
                OledCanvas_CancelForUi(OLED_CANVAS_STATUS_NOT_TEST_MODE,
                                       OLED_CANVAS_UI_EXIT_TO_DASHBOARD);
            }
        } else {
            OledCanvas_CancelForUi(OLED_CANVAS_STATUS_NOT_TEST_MODE,
                                   OLED_CANVAS_UI_EXIT_TO_DASHBOARD);
            return;
        }
    }

    if (oled_canvas.state == OLED_CANVAS_STATE_DISPLAYING &&
        oled_canvas.cancel_display_status != OLED_CANVAS_STATUS_OK) {
        if (!ssd1306_UpdateScreenCompleted()) {
            if ((uint32_t)(Screens_NowMs() - oled_canvas.last_activity_ms) >=
                OLED_CANVAS_DISPLAY_TIMEOUT_MS) {
                action = oled_canvas.deferred_ui_action;
                OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                         OLED_CANVAS_STATUS_DISPLAY_IO_ERROR,
                                         oled_canvas.transport_id,
                                         oled_canvas.source_node,
                                         oled_canvas.transfer_id,
                                         oled_canvas.received_len,
                                         oled_canvas.computed_crc32);
                OledCanvas_RememberResult(OLED_CANVAS_STATUS_DISPLAY_IO_ERROR);
                OledCanvas_ResetTransferState();
                OledCanvas_ApplyUiAction(action);
            }
            return;
        }

        action = oled_canvas.deferred_ui_action;
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 oled_canvas.cancel_display_status,
                                 oled_canvas.transport_id,
                                 oled_canvas.source_node,
                                 oled_canvas.transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        OledCanvas_RememberResult(oled_canvas.cancel_display_status);
        OledCanvas_ResetTransferState();
        OledCanvas_ApplyUiAction(action);
        return;
    }

    if (oled_canvas.state == OLED_CANVAS_STATE_IDLE) {
        return;
    }

    guard_status = OledCanvas_GuardStatus(oled_canvas.source_node);
    if (guard_status != OLED_CANVAS_STATUS_OK) {
        action = OLED_CANVAS_UI_NONE;
        if (guard_status == OLED_CANVAS_STATUS_NOT_TEST_MODE) {
            action = OLED_CANVAS_UI_EXIT_TO_DASHBOARD;
        } else if (guard_status == OLED_CANVAS_STATUS_CANCELED_BY_LOCAL_UI &&
                   OledCanvas_IsActiveScreen()) {
            action = OLED_CANVAS_UI_SHOW_READY;
        }
        OledCanvas_CancelForUi(guard_status, action);
        return;
    }

    if ((oled_canvas.state == OLED_CANVAS_STATE_RECEIVING ||
         oled_canvas.state == OLED_CANVAS_STATE_COMMIT_PENDING) &&
        (uint32_t)(Screens_NowMs() - oled_canvas.last_activity_ms) >=
            OLED_CANVAS_ASSEMBLY_TIMEOUT_MS) {
        if (oled_canvas.state == OLED_CANVAS_STATE_COMMIT_PENDING) {
            OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                     OLED_CANVAS_STATUS_TIMEOUT,
                                     oled_canvas.transport_id,
                                     oled_canvas.source_node,
                                     oled_canvas.transfer_id,
                                     oled_canvas.received_len,
                                     oled_canvas.computed_crc32);
        }
        OledCanvas_RememberResult(OLED_CANVAS_STATUS_TIMEOUT);
        OledCanvas_ResetTransferState();
        return;
    }

    if (oled_canvas.state == OLED_CANVAS_STATE_COMMIT_PENDING) {
        if (!ssd1306_UpdateScreenCompleted()) {
            return;
        }

        oled_canvas.display_error_count_before = ssd1306_GetErrorCount();
        if (ssd1306_PresentFramebuffer(oled_canvas.framebuffer,
                                       OLED_CANVAS_FRAMEBUFFER_BYTES) != AM_OK) {
            OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                     OLED_CANVAS_STATUS_DISPLAY_IO_ERROR,
                                     oled_canvas.transport_id,
                                     oled_canvas.source_node,
                                     oled_canvas.transfer_id,
                                     oled_canvas.received_len,
                                     oled_canvas.computed_crc32);
            OledCanvas_RememberResult(OLED_CANVAS_STATUS_DISPLAY_IO_ERROR);
            OledCanvas_ResetTransferState();
            return;
        }
        oled_canvas.state = OLED_CANVAS_STATE_DISPLAYING;
        oled_canvas.last_activity_ms = Screens_NowMs();
        return;
    }

    if (oled_canvas.state != OLED_CANVAS_STATE_DISPLAYING) {
        return;
    }

    if (!ssd1306_UpdateScreenCompleted()) {
        if ((uint32_t)(Screens_NowMs() - oled_canvas.last_activity_ms) >=
            OLED_CANVAS_DISPLAY_TIMEOUT_MS) {
            OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                     OLED_CANVAS_STATUS_DISPLAY_IO_ERROR,
                                     oled_canvas.transport_id,
                                     oled_canvas.source_node,
                                     oled_canvas.transfer_id,
                                     oled_canvas.received_len,
                                     oled_canvas.computed_crc32);
            OledCanvas_RememberResult(OLED_CANVAS_STATUS_DISPLAY_IO_ERROR);
            OledCanvas_ResetTransferState();
        }
        return;
    }

    action = oled_canvas.deferred_ui_action;
    if (oled_canvas.cancel_display_status != OLED_CANVAS_STATUS_OK) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 oled_canvas.cancel_display_status,
                                 oled_canvas.transport_id,
                                 oled_canvas.source_node,
                                 oled_canvas.transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        OledCanvas_RememberResult(oled_canvas.cancel_display_status);
        OledCanvas_ResetTransferState();
        OledCanvas_ApplyUiAction(action);
        return;
    }

    if (ssd1306_GetErrorCount() != oled_canvas.display_error_count_before) {
        OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                                 OLED_CANVAS_STATUS_DISPLAY_IO_ERROR,
                                 oled_canvas.transport_id,
                                 oled_canvas.source_node,
                                 oled_canvas.transfer_id,
                                 oled_canvas.received_len,
                                 oled_canvas.computed_crc32);
        OledCanvas_RememberResult(OLED_CANVAS_STATUS_DISPLAY_IO_ERROR);
        OledCanvas_ResetTransferState();
        return;
    }

    oled_canvas.state = OLED_CANVAS_STATE_DISPLAYED;
    OledCanvas_RememberResult(OLED_CANVAS_STATUS_OK);
    OledCanvas_QueueResponse(OLED_CANVAS_OP_COMMIT,
                             OLED_CANVAS_STATUS_OK,
                             oled_canvas.transport_id,
                             oled_canvas.source_node,
                             oled_canvas.transfer_id,
                             OLED_CANVAS_FRAMEBUFFER_BYTES,
                             oled_canvas.computed_crc32);
}

uint8_t OledCanvas_IsActiveScreen(void)
{
    return MenuSys_GetCurrentScreenCode(&menuSystem) ==
        SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS ? 1u : 0u;
}

void Screens_Init_oled_canvas(void)
{

}
