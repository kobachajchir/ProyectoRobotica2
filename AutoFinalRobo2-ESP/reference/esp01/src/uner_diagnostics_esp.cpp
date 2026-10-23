#include "uner_platform_esp.h"
#include <Arduino.h>

#ifndef UNER_BRIDGE_DEBUG
#define UNER_BRIDGE_DEBUG 0
#endif

#if UNER_BRIDGE_DEBUG
static void debug_print_hex_bytes(const uint8_t *data, uint16_t len) {
    if (!data || len == 0) {
        Serial.print("(empty)");
        return;
    }
    for (uint16_t i = 0; i < len; ++i) {
        if (i > 0) Serial.print(' ');
        if (data[i] < 0x10) Serial.print('0');
        Serial.print(data[i], HEX);
    }
}

static bool debug_payload_is_sensitive(uint8_t command) {
    switch (command) {
        case UNER_CMD_SET_CREDENTIALS:
        case UNER_CMD_GET_CREDENTIALS:
        case UNER_CMD_SET_AP_CONFIG:
        case UNER_CMD_WIFI_CREDENTIALS_FLOW:
        case UNER_CMD_AUTH_VALIDATE_PIN:
        case UNER_CMD_AUTH_REMOTE_RESULT:
            return true;
        default:
            return false;
    }
}

void EspPlatform_LogUNERPacket(void *, const UNER_Packet *pkt) {
    if (!pkt || pkt->src != UNER_NODE_MCU) return;
    uint8_t frame[UNER_MAX_FRAME];
    uint16_t frame_len = (uint16_t)(UNER_OVERHEAD + pkt->len);
    UNER_Status built = UNER_BuildFrame(frame, sizeof(frame), pkt->src, pkt->dst, pkt->cmd, pkt->payload, pkt->len);

    Serial.printf("[UNER][RX] cmd=0x%02X len=%u src=%u dst=%u payload=",
                  pkt->cmd, pkt->len, pkt->src, pkt->dst);
    if (debug_payload_is_sensitive(pkt->cmd)) Serial.print("<redacted>");
    else debug_print_hex_bytes(pkt->payload, pkt->len);
    Serial.println();

    if (built == UNER_OK) {
        Serial.print("[UNER][RX_FRAME] ");
        if (debug_payload_is_sensitive(pkt->cmd)) Serial.print("<redacted>");
        else debug_print_hex_bytes(frame, frame_len);
        Serial.println();
    }
}
#else
void EspPlatform_LogUNERPacket(void *, const UNER_Packet *pkt) {
    (void)pkt;
}
#endif

