#include "system_flags.h"
#include "utils/macros_utils.h"
/*
 * wifi_bridge_at.c - non-blocking ESP-01 AT backend using UART DMA.
 */
#include "wifi_bridge_at.h"
#include "wifi_bridge.h"

#include <string.h>


#include "uner_v2.h"

static void WifiBridge_AT_NetworkAdvance(WifiBridge_Handle *h, uint8_t ok);
static void WifiBridge_AT_ProcessNetwork(WifiBridge_Handle *h);
static void WifiBridge_AT_FinishQueuedSend(WifiBridge_Handle *h, uint8_t ok);
static uint8_t WifiBridge_AT_ParseIpdHeader(WifiBridge_Handle *h, const char *header);
static void WifiBridge_AT_CompleteIpd(WifiBridge_Handle *h);

static WifiBridge_Result WifiBridge_AT_SendApConfig(WifiBridge_Handle *h);
static WifiBridge_Result WifiBridge_AT_SendApIpConfig(WifiBridge_Handle *h);
static WifiBridge_Result WifiBridge_AT_StartAP(WifiBridge_Handle *h);
static WifiBridge_Result WifiBridge_AT_Send(WifiBridge_Handle *h,
                                            WifiBridge_AT_Operation op,
                                            const char *cmd);

static size_t WifiBridge_AT_StrLenBounded(const char *text, size_t max_len)
{
    size_t len = 0u;

    if (text == NULL) {
        return 0u;
    }

    while (text[len] != '\0' && len < max_len) {
        len++;
    }

    return len;
}

static void WifiBridge_AT_CopyText(char *dst, size_t dst_len, const char *src)
{
    size_t i = 0u;

    if ((dst == NULL) || (dst_len == 0u)) {
        return;
    }

    if (src != NULL) {
        while (src[i] != '\0' && i + 1u < dst_len) {
            dst[i] = src[i];
            i++;
        }
    }

    dst[i] = '\0';
}

static uint8_t WifiBridge_AT_AppendText(char *buf, size_t buf_len, uint16_t *pos, const char *text)
{
    if ((buf == NULL) || (pos == NULL) || (text == NULL)) {
        return 0u;
    }

    while (*text != '\0') {
        if ((*pos + 1u) >= buf_len) {
            return 0u;
        }
        buf[*pos] = *text;
        *pos = (uint16_t)(*pos + 1u);
        text++;
    }

    buf[*pos] = '\0';
    return 1u;
}

static uint8_t WifiBridge_AT_AppendQuoted(char *buf, size_t buf_len, uint16_t *pos, const char *text)
{
    if (!WifiBridge_AT_AppendText(buf, buf_len, pos, "\"")) {
        return 0u;
    }

    if (text != NULL) {
        while (*text != '\0') {
            if ((*text == '"') || (*text == '\\') || (*text == ',')) {
                if ((*pos + 1u) >= buf_len) {
                    return 0u;
                }
                buf[*pos] = '\\';
                *pos = (uint16_t)(*pos + 1u);
            }
            if ((*pos + 1u) >= buf_len) {
                return 0u;
            }
            buf[*pos] = *text;
            *pos = (uint16_t)(*pos + 1u);
            text++;
        }
        buf[*pos] = '\0';
    }

    return WifiBridge_AT_AppendText(buf, buf_len, pos, "\"");
}

static uint8_t WifiBridge_AT_AppendU8(WifiBridge_Handle *h, char *buf, size_t buf_len, uint16_t *pos, uint8_t value)
{
    char tmp[3];
    uint8_t len = 0u;

    if ((buf == NULL) || (pos == NULL)) {
        return 0u;
    }

    if (value >= 100u) {
        tmp[len++] = (char)('0' + (value / 100u));
        value %= 100u;
    }
    if (len != 0u || value >= 10u) {
        tmp[len++] = (char)('0' + (value / 10u));
        value %= 10u;
    }
    tmp[len++] = (char)('0' + value);

    for (uint8_t i = 0u; i < len; ++i) {
        if ((*pos + 1u) >= buf_len) {
            return 0u;
        }
        buf[*pos] = tmp[i];
        *pos = (uint16_t)(*pos + 1u);
    }

    buf[*pos] = '\0';
    return 1u;
}
static uint8_t WifiBridge_AT_AppendU16(char *buf, size_t buf_len, uint16_t *pos, uint16_t value)
{
    char tmp[5];
    uint8_t len = 0u;
    uint16_t divisor = 10000u;
    uint8_t started = 0u;

    while (divisor > 0u) {
        uint8_t digit = (uint8_t)(value / divisor);
        value = (uint16_t)(value % divisor);
        if (digit != 0u || started || divisor == 1u) {
            tmp[len++] = (char)('0' + digit);
            started = 1u;
        }
        divisor = (uint16_t)(divisor / 10u);
    }
    for (uint8_t i = 0u; i < len; ++i) {
        if ((*pos + 1u) >= buf_len) {
            return 0u;
        }
        buf[(*pos)++] = tmp[i];
    }
    buf[*pos] = '\0';
    return 1u;
}

static uint8_t WifiBridge_AT_AppendIp(char *buf,
                                     size_t buf_len,
                                     uint16_t *pos,
                                     const uint8_t ip[WIFI_BRIDGE_IP_LEN])
{
    for (uint8_t i = 0u; i < WIFI_BRIDGE_IP_LEN; ++i) {
        if (!WifiBridge_AT_AppendU16(buf, buf_len, pos, ip[i])) {
            return 0u;
        }
        if (i + 1u < WIFI_BRIDGE_IP_LEN &&
            !WifiBridge_AT_AppendText(buf, buf_len, pos, ".")) {
            return 0u;
        }
    }
    return 1u;
}

static int16_t WifiBridge_AT_ParseSigned(const char **cursor)
{
    int16_t value = 0;
    int16_t sign = 1;
    const char *p;

    if ((cursor == NULL) || (*cursor == NULL)) {
        return 0;
    }

    p = *cursor;
    if (*p == '-') {
        sign = -1;
        p++;
    }

    while (*p >= '0' && *p <= '9') {
        value = (int16_t)((value * 10) + (*p - '0'));
        p++;
    }

    *cursor = p;
    return (int16_t)(value * sign);
}

static uint8_t WifiBridge_AT_ParseIp(const char *line, uint8_t ip[WIFI_BRIDGE_IP_LEN])
{
    const char *p = line;

    if ((line == NULL) || (ip == NULL)) {
        return 0u;
    }

    while (*p != '\0' && (*p < '0' || *p > '9')) {
        p++;
    }

    for (uint8_t i = 0u; i < WIFI_BRIDGE_IP_LEN; ++i) {
        int16_t part = WifiBridge_AT_ParseSigned(&p);

        if (part < 0 || part > 255) {
            return 0u;
        }
        ip[i] = (uint8_t)part;

        if (i < (WIFI_BRIDGE_IP_LEN - 1u)) {
            if (*p != '.') {
                return 0u;
            }
            p++;
        }
    }

    return 1u;
}

static uint8_t WifiBridge_AT_IpIsZero(const uint8_t ip[WIFI_BRIDGE_IP_LEN])
{
    return (ip != NULL &&
            ip[0] == 0u && ip[1] == 0u && ip[2] == 0u && ip[3] == 0u) ? 1u : 0u;
}

static void WifiBridge_AT_ClearStaIp(WifiBridge_Handle *h)
{
    memset((void *)&(*h->system).connectivity.espStaIp, 0, sizeof((*h->system).connectivity.espStaIp));
    memset(&(*h->system).connectivity.espWifiConnection.staIp, 0, sizeof((*h->system).connectivity.espWifiConnection.staIp));
    (*h->system).connectivity.espWifiConnection.staIpValid = 0u;
}

static void WifiBridge_AT_ClearApIp(WifiBridge_Handle *h)
{
    memset((void *)&(*h->system).connectivity.espApIp, 0, sizeof((*h->system).connectivity.espApIp));
    memset(&(*h->system).connectivity.espWifiConnection.apIp, 0, sizeof((*h->system).connectivity.espWifiConnection.apIp));
    (*h->system).connectivity.espWifiConnection.apIpValid = 0u;
}

static void WifiBridge_AT_StoreFirmwareVersion(WifiBridge_Handle *h, const char *line)
{
    static const char prefix[] = "AT version:";
    const char *src;
    size_t pos = 0u;

    if (line == NULL || strncmp(line, prefix, sizeof(prefix) - 1u) != 0) {
        return;
    }

    src = line + sizeof(prefix) - 1u;
    (*h->system).connectivity.espFirmwareVersion[pos++] = 'A';
    (*h->system).connectivity.espFirmwareVersion[pos++] = 'T';
    (*h->system).connectivity.espFirmwareVersion[pos++] = ' ';
    while (*src != 0 && *src != '(' && pos + 1u < sizeof((*h->system).connectivity.espFirmwareVersion)) {
        (*h->system).connectivity.espFirmwareVersion[pos++] = *src++;
    }
    (*h->system).connectivity.espFirmwareVersion[pos] = 0;
}
static const char *WifiBridge_AT_EncryptionText(uint8_t enc)
{
    switch (enc) {
        case 0u: return "OPEN";
        case 1u: return "WEP";
        case 2u: return "WPA";
        case 3u: return "WPA2";
        case 4u: return "WPA/WPA2";
        case 5u: return "WPA2-ENT";
        case 6u: return "WPA3";
        case 7u: return "WPA2/WPA3";
        default: return "ENC?";
    }
}

static int8_t WifiBridge_AT_FindNetwork(WifiBridge_Handle *h, const char *ssid)
{
    if (ssid == NULL) {
        return -1;
    }

    for (uint8_t i = 0u; i < (*h->system).connectivity.networksFound && i < WIFI_SCAN_MAX_NETWORKS; ++i) {
        if (strncmp((*h->system).connectivity.wifiNetworkSsids[i], ssid, WIFI_SSID_MAX_LEN) == 0) {
            return (int8_t)i;
        }
    }

    return -1;
}

static void WifiBridge_AT_StoreScanResult(WifiBridge_Handle *h, const char *ssid,
                                          uint8_t enc,
                                          int8_t rssi,
                                          uint8_t channel)
{
    int8_t existing;
    uint8_t index;

    if ((ssid == NULL) || (ssid[0] == '\0')) {
        return;
    }

    existing = WifiBridge_AT_FindNetwork(h, ssid);
    if (existing >= 0) {
        index = (uint8_t)existing;
    } else {
        if ((*h->system).connectivity.networksFound >= WIFI_SCAN_MAX_NETWORKS) {
            return;
        }
        index = (*h->system).connectivity.networksFound++;
    }

    WifiBridge_AT_CopyText((*h->system).connectivity.wifiNetworkSsids[index],
                           sizeof((*h->system).connectivity.wifiNetworkSsids[index]),
                           ssid);
    WifiBridge_AT_CopyText((*h->system).connectivity.wifiNetworkEncryptions[index],
                           sizeof((*h->system).connectivity.wifiNetworkEncryptions[index]),
                           WifiBridge_AT_EncryptionText(enc));
    (*h->system).connectivity.wifiNetworkSignalStrengths[index] = rssi;
    (*h->system).connectivity.wifiNetworkChannels[index] = channel;
    (*h->system).connectivity.wifiNetworkEncryptionTypes[index] = enc;
    (*h->system).connectivity.wifiNetworkDetailValid[index] = 1u;
}

static void WifiBridge_AT_ParseCwlap(WifiBridge_Handle *h, const char *line)
{
    const char *p;
    const char *q1;
    const char *q2;
    char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u];
    uint8_t enc;
    int8_t rssi;
    uint8_t channel = 0u;
    size_t ssid_len;

    p = strchr(line, '(');
    if (p == NULL) {
        return;
    }
    p++;

    enc = (uint8_t)WifiBridge_AT_ParseSigned(&p);
    q1 = strchr(p, '"');
    if (q1 == NULL) {
        return;
    }
    q2 = strchr(q1 + 1, '"');
    if (q2 == NULL) {
        return;
    }

    ssid_len = (size_t)(q2 - q1 - 1);
    if (ssid_len > WIFI_BRIDGE_SSID_MAX_LEN) {
        ssid_len = WIFI_BRIDGE_SSID_MAX_LEN;
    }
    memcpy(ssid, q1 + 1, ssid_len);
    ssid[ssid_len] = '\0';

    p = q2 + 1;
    if (*p == ',') {
        p++;
    }
    rssi = (int8_t)WifiBridge_AT_ParseSigned(&p);

    q1 = strchr(p, '"');
    if (q1 != NULL) {
        q2 = strchr(q1 + 1, '"');
        if (q2 != NULL) {
            p = q2 + 1;
            if (*p == ',') {
                p++;
                channel = (uint8_t)WifiBridge_AT_ParseSigned(&p);
            }
        }
    }

    WifiBridge_AT_StoreScanResult(h, ssid, enc, rssi, channel);
}

static void WifiBridge_AT_SetStaIp(WifiBridge_Handle *h, const uint8_t ip[WIFI_BRIDGE_IP_LEN])
{
    (*h->system).connectivity.espStaIp.block1 = ip[0];
    (*h->system).connectivity.espStaIp.block2 = ip[1];
    (*h->system).connectivity.espStaIp.block3 = ip[2];
    (*h->system).connectivity.espStaIp.block4 = ip[3];
    (*h->system).connectivity.espWifiConnection.staIp.block1 = ip[0];
    (*h->system).connectivity.espWifiConnection.staIp.block2 = ip[1];
    (*h->system).connectivity.espWifiConnection.staIp.block3 = ip[2];
    (*h->system).connectivity.espWifiConnection.staIp.block4 = ip[3];
    (*h->system).connectivity.espWifiConnection.staIpValid = 1u;
}

static void WifiBridge_AT_SetApIp(WifiBridge_Handle *h, const uint8_t ip[WIFI_BRIDGE_IP_LEN])
{
    (*h->system).connectivity.espApIp.block1 = ip[0];
    (*h->system).connectivity.espApIp.block2 = ip[1];
    (*h->system).connectivity.espApIp.block3 = ip[2];
    (*h->system).connectivity.espApIp.block4 = ip[3];
    (*h->system).connectivity.espWifiConnection.apIp.block1 = ip[0];
    (*h->system).connectivity.espWifiConnection.apIp.block2 = ip[1];
    (*h->system).connectivity.espWifiConnection.apIp.block3 = ip[2];
    (*h->system).connectivity.espWifiConnection.apIp.block4 = ip[3];
    (*h->system).connectivity.espWifiConnection.apIpValid = 1u;
}

static void WifiBridge_AT_Emit(WifiBridge_Handle *h, WifiBridge_EventId id, uint8_t status)
{
    WifiBridge_Event evt;

    if (h == NULL) {
        return;
    }

    memset(&evt, 0, sizeof(evt));
    evt.id = id;
    evt.mode = h->mode;
    evt.status = status;
    evt.connected = WIFI_FLAG_IS_SET(h, WIFI_FLAG_CONNECTED) ? 1u : 0u;
    evt.got_ip = WIFI_FLAG_IS_SET(h, WIFI_FLAG_GOT_IP) ? 1u : 0u;
    memcpy(evt.ip, h->ip, sizeof(evt.ip));
    WifiBridge_AT_CopyText(evt.ssid, sizeof(evt.ssid), h->ssid);
    WifiBridge_EmitEvent(h, &evt);
}

static void WifiBridge_AT_CommandDone(WifiBridge_Handle *h, uint8_t ok)
{
    uint8_t keep_active_op = 0u;

    if (h == NULL) {
        return;
    }

    WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);

    if (h->at_active_op == WIFI_BRIDGE_AT_OP_NETWORK_CONFIG) {
        WifiBridge_AT_NetworkAdvance(h, ok);
        return;
    }
    if (h->at_active_op == WIFI_BRIDGE_AT_OP_NETWORK_CLOSE) {
        h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
        return;
    }
    if (h->at_active_op == WIFI_BRIDGE_AT_OP_CIPSEND) {
        if (!ok) {
            WifiBridge_AT_FinishQueuedSend(h, 0u);
        }
        return;
    }

    if (!ok && (h->at_active_op == WIFI_BRIDGE_AT_OP_START_AP_MODE ||
                h->at_active_op == WIFI_BRIDGE_AT_OP_APPLY_AP_IP ||
                h->at_active_op == WIFI_BRIDGE_AT_OP_START_AP)) {
        h->services->AppApConfig_OnEspApplyResponse(h->services_ctx, 1u);
    }

    if (ok) {
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_OK);
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_ERROR);
    } else {
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_ERROR);
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, h->status);
    }

    if (ok) {
        switch (h->at_active_op) {
            case WIFI_BRIDGE_AT_OP_CLEAR_CREDENTIALS:
            case WIFI_BRIDGE_AT_OP_FACTORY_RESET:
                (void)WifiBridge_SetRuntimeCredentials_H(h, "", "");
                break;
            case WIFI_BRIDGE_AT_OP_SET_STA:
                h->mode = WIFI_BRIDGE_MODE_STA;
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_MODE_CHANGED, 0u);
                break;
            case WIFI_BRIDGE_AT_OP_SET_AP:
                h->mode = WIFI_BRIDGE_MODE_AP;
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_MODE_CHANGED, 0u);
                break;
            case WIFI_BRIDGE_AT_OP_START_AP_MODE:
                h->mode = WIFI_BRIDGE_MODE_AP;
                if (WifiBridge_AT_SendApIpConfig(h) == WIFI_BRIDGE_OK) {
                    keep_active_op = 1u;
                } else {
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_BAD_ARG);
                }
                break;
            case WIFI_BRIDGE_AT_OP_APPLY_AP_IP:
                if (WifiBridge_AT_SendApConfig(h) == WIFI_BRIDGE_OK) {
                    keep_active_op = 1u;
                } else {
                    h->services->AppApConfig_OnEspApplyResponse(h->services_ctx, 1u);
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_BAD_ARG);
                }
                break;
            case WIFI_BRIDGE_AT_OP_START_AP:
                h->mode = WIFI_BRIDGE_MODE_AP;
                h->services->AppApConfig_OnEspApplyResponse(h->services_ctx, 0u);
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_AP_STARTED, 0u);
                break;
            case WIFI_BRIDGE_AT_OP_STOP_AP:
                h->mode = WIFI_BRIDGE_MODE_STA;
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_AP_STOPPED, 0u);
                break;
            case WIFI_BRIDGE_AT_OP_DISCONNECT:
                WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_IP);
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_STA_DISCONNECTED, 0u);
                break;
            case WIFI_BRIDGE_AT_OP_SCAN:
                if ((*h->system).connectivity.wifiScanSessionActive || IS_FLAG_SET((*h->system).runtime.systemFlags3, WIFI_SEARCHING)) {
                    (*h->system).connectivity.wifiScanResultsPending = 1u;
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_SCAN_DONE, h->at_scan_count);
                }
                break;
            case WIFI_BRIDGE_AT_OP_PING:
                WifiBridge_RegisterBackendAlive(h, WIFI_BRIDGE_BACKEND_AT, h->mode);
                break;
            case WIFI_BRIDGE_AT_OP_STATUS_MODE:
                if (WifiBridge_AT_Send(h,
                                       WIFI_BRIDGE_AT_OP_STATUS_LINK,
                                       "AT+CIPSTATUS\r\n") == WIFI_BRIDGE_OK) {
                    keep_active_op = 1u;
                } else {
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_BUSY);
                }
                break;
            case WIFI_BRIDGE_AT_OP_STATUS_LINK:
                if (WifiBridge_AT_Send(h,
                                       WIFI_BRIDGE_AT_OP_STATUS_IP,
                                       "AT+CIFSR\r\n") == WIFI_BRIDGE_OK) {
                    keep_active_op = 1u;
                } else {
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_BUSY);
                }
                break;
            case WIFI_BRIDGE_AT_OP_STATUS_IP:
                WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_STATUS_REFRESHED, h->status);
                break;
            case WIFI_BRIDGE_AT_OP_FIRMWARE:
                if ((*h->system).connectivity.espFirmwareVersion[0] != 0) {
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_FIRMWARE_RECEIVED, 0u);
                } else {
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_ERROR);
                }
                break;
            case WIFI_BRIDGE_AT_OP_REBOOT:
                break;
            default:
                break;
        }
    }

    if (!keep_active_op) {
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_COMMAND_DONE, ok ? 0u : 1u);
        h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
    }
}

static uint8_t WifiBridge_AT_ParseUnsigned(const char **cursor, uint32_t *value)
{
    const char *p;
    uint32_t parsed = 0u;
    uint8_t digits = 0u;

    if (cursor == NULL || *cursor == NULL || value == NULL) {
        return 0u;
    }
    p = *cursor;
    while (*p >= '0' && *p <= '9') {
        parsed = (parsed * 10u) + (uint32_t)(*p - '0');
        p++;
        digits = 1u;
    }
    *cursor = p;
    *value = parsed;
    return digits;
}

static uint8_t WifiBridge_AT_FrameIsValid(WifiBridge_Handle *h, const uint8_t *frame, uint16_t len)
{
    uint16_t expected;

    if (frame == NULL || len < 10u ||
        frame[0] != 'U' || frame[1] != 'N' ||
        frame[2] != 'E' || frame[3] != 'R') {
        return 0u;
    }
    expected = (uint16_t)(10u + frame[4]);
    if (len != expected || frame[5] != UNER_TOKEN || frame[6] != UNER_VERSION ||
        (frame[7] >> 4) != UNER_NODE_PC_QT ||
        ((frame[7] & 0x0Fu) != UNER_NODE_MCU &&
         (frame[7] & 0x0Fu) != UNER_NODE_BROADCAST)) {
        return 0u;
    }
    return h->services->checksum(frame, (uint16_t)(len - 1u)) ==
           frame[len - 1u] ? 1u : 0u;
}

static uint8_t WifiBridge_AT_PeerMatches(const uint8_t a[WIFI_BRIDGE_IP_LEN],
                                         uint16_t a_port,
                                         const uint8_t b[WIFI_BRIDGE_IP_LEN],
                                         uint16_t b_port)
{
    return (a_port == b_port && memcmp(a, b, WIFI_BRIDGE_IP_LEN) == 0) ? 1u : 0u;
}

static uint8_t WifiBridge_AT_ParseIpdHeader(WifiBridge_Handle *h, const char *header)
{
    char copy[WIFI_BRIDGE_AT_IPD_HEADER_SIZE];
    char *tokens[4] = {0};
    char *p;
    uint8_t count = 0u;
    uint32_t link = 0u;
    uint32_t payload_len = 0u;

    if (h == NULL || header == NULL || strncmp(header, "+IPD,", 5u) != 0) {
        return 0u;
    }
    WifiBridge_AT_CopyText(copy, sizeof(copy), header + 5u);
    p = copy;
    tokens[count++] = p;
    while (*p != '\0' && count < 4u) {
        if (*p == ',') {
            *p = '\0';
            tokens[count++] = p + 1;
        }
        p++;
    }
    p = tokens[0];
    if (count < 2u || !WifiBridge_AT_ParseUnsigned((const char **)&p, &link) ||
        *p != '\0' || link > 4u) {
        return 0u;
    }
    p = tokens[1];
    if (!WifiBridge_AT_ParseUnsigned((const char **)&p, &payload_len) ||
        *p != '\0' || payload_len == 0u || payload_len > 65535u) {
        return 0u;
    }

    h->at_ipd_link_id = (int8_t)link;
    h->at_ipd_expected_len = (uint16_t)payload_len;
    h->at_ipd_received_len = 0u;
    memset(h->at_ipd_peer_ip, 0, sizeof(h->at_ipd_peer_ip));
    h->at_ipd_peer_port = 0u;
    if (count >= 4u) {
        char *ip_text = tokens[2];
        size_t ip_len = strlen(ip_text);
        uint32_t port = 0u;

        if (ip_len >= 2u && ip_text[0] == '"' && ip_text[ip_len - 1u] == '"') {
            ip_text[ip_len - 1u] = '\0';
            ip_text++;
        }
        if (!WifiBridge_AT_ParseIp(ip_text, h->at_ipd_peer_ip)) {
            return 0u;
        }
        p = tokens[3];
        if (!WifiBridge_AT_ParseUnsigned((const char **)&p, &port) ||
            *p != '\0' || port == 0u || port > 65535u) {
            return 0u;
        }
        h->at_ipd_peer_port = (uint16_t)port;
    }
    h->at_ipd_active = 1u;
    return 1u;
}

static void WifiBridge_AT_CompleteIpd(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }
    if (h->at_network_protocol == WIFI_BRIDGE_NETWORK_UDP) {
        uint8_t valid = h->at_ipd_expected_len <= WIFI_BRIDGE_AT_MAX_FRAME_SIZE &&
                        WifiBridge_AT_FrameIsValid(h, h->at_ipd_payload,
                                                   h->at_ipd_expected_len);
        if (!valid || h->at_ipd_peer_port == 0u) {
            h->at_network_counters.udp_datagram_drops++;
        } else if (h->at_udp_peer_valid &&
                   !WifiBridge_AT_PeerMatches(h->at_udp_peer_ip,
                                              h->at_udp_peer_port,
                                              h->at_ipd_peer_ip,
                                              h->at_ipd_peer_port)) {
            h->at_network_counters.rejected_peers++;
        } else {
            if (!h->at_udp_peer_valid) {
                memcpy(h->at_udp_peer_ip, h->at_ipd_peer_ip,
                       sizeof(h->at_udp_peer_ip));
                h->at_udp_peer_port = h->at_ipd_peer_port;
                h->at_udp_peer_valid = 1u;
            }
            h->at_udp_peer_last_rx_ms = h->hardware->now_ms(h->huart);
            h->services->UNER_App_FeedUart1Bytes(h->services_ctx, h->at_ipd_payload, h->at_ipd_expected_len);
        }
    }
    h->at_ipd_active = 0u;
    h->at_ipd_expected_len = 0u;
    h->at_ipd_received_len = 0u;
}

static void WifiBridge_AT_NetworkAdvance(WifiBridge_Handle *h, uint8_t ok)
{
    WifiBridge_Result result = WIFI_BRIDGE_ERROR;

    if (h == NULL) {
        return;
    }
    h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
    if (!ok && h->at_network_stage != 4u && h->at_network_stage != 6u) {
        h->at_network_ready = 0u;
        h->at_network_restart_pending = 1u;
        return;
    }

    switch (h->at_network_stage) {
        case 0u:
            h->at_network_stage = 1u;
            result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                        "AT+CIPMODE=0\r\n");
            break;
        case 1u:
            h->at_network_stage = 2u;
            result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                        "AT+CIPDINFO=1\r\n");
            break;
        case 2u:
            h->at_network_stage = 3u;
            result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                        "AT+CIPMUX=1\r\n");
            break;
        case 3u:
            if (h->at_network_protocol == WIFI_BRIDGE_NETWORK_TCP) {
                h->at_network_stage = 4u;
                result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                            "AT+CIPSERVER=0\r\n");
            } else {
                h->at_network_stage = 6u;
                result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                            "AT+CIPCLOSE=0\r\n");
            }
            break;
        case 4u:
            h->at_network_stage = 5u;
            result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                                        "AT+CIPSERVER=1,3333\r\n");
            break;
        case 5u:
            h->at_network_ready = ok ? 1u : 0u;
            if (h->at_network_ready) {
                h->at_detected_capabilities |= WIFI_BRIDGE_CAP_AT_TCP;
            }
            return;
        case 6u:
            h->at_network_stage = 7u;
            result = WifiBridge_AT_Send(
                h, WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
                "AT+CIPSTART=0,\"UDP\",\"255.255.255.255\",3333,3333,2\r\n");
            break;
        case 7u:
            h->at_network_ready = ok ? 1u : 0u;
            if (h->at_network_ready) {
                h->at_detected_capabilities |= WIFI_BRIDGE_CAP_AT_UDP;
            }
            return;
        default:
            h->at_network_restart_pending = 1u;
            return;
    }
    if (result != WIFI_BRIDGE_OK) {
        h->at_network_restart_pending = 1u;
    }
}

static void WifiBridge_AT_FinishQueuedSend(WifiBridge_Handle *h, uint8_t ok)
{
    if (h == NULL) {
        return;
    }
    if (h->at_frame_queue_count > 0u) {
        h->at_frame_queue_len[h->at_frame_queue_head] = 0u;
        h->at_frame_queue_head =
            (uint8_t)((h->at_frame_queue_head + 1u) % WIFI_BRIDGE_AT_TX_QUEUE_SLOTS);
        h->at_frame_queue_count--;
    }
    if (!ok) {
        h->at_network_counters.send_failures++;
    }
    h->at_send_state = WIFI_BRIDGE_AT_SEND_IDLE;
    h->at_send_payload_started = 0u;
    h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
    h->at_deadline_ms = 0u;
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
    h->tx_busy = 0u;
}

static WifiBridge_Result WifiBridge_AT_StartQueuedSend(WifiBridge_Handle *h)
{
    uint16_t pos = 0u;
    uint16_t len;

    if (h == NULL || h->at_frame_queue_count == 0u ||
        !h->at_network_ready || h->at_send_state != WIFI_BRIDGE_AT_SEND_IDLE) {
        return WIFI_BRIDGE_BUSY;
    }
    if (h->at_network_protocol == WIFI_BRIDGE_NETWORK_TCP) {
        if (h->at_tcp_link_id < 0 ||
            !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "AT+CIPSEND=") ||
            !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                   (uint8_t)h->at_tcp_link_id) ||
            !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",")) {
            return WIFI_BRIDGE_BUSY;
        }
    } else {
        if (!h->at_udp_peer_valid ||
            !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                     "AT+CIPSEND=0,")) {
            return WIFI_BRIDGE_BUSY;
        }
    }
    len = h->at_frame_queue_len[h->at_frame_queue_head];
    if (!WifiBridge_AT_AppendU16(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, len)) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    if (h->at_network_protocol == WIFI_BRIDGE_NETWORK_UDP) {
        if (!WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",\"") ||
            !WifiBridge_AT_AppendIp(h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                   h->at_udp_peer_ip) ||
            !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\",") ||
            !WifiBridge_AT_AppendU16(h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                    h->at_udp_peer_port)) {
            return WIFI_BRIDGE_BAD_ARG;
        }
    }
    if (!WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\r\n") ||
        WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_CIPSEND, h->at_tx_buf) != WIFI_BRIDGE_OK) {
        return WIFI_BRIDGE_BUSY;
    }
    h->at_send_state = WIFI_BRIDGE_AT_SEND_WAIT_PROMPT;
    h->at_send_payload_started = 0u;
    return WIFI_BRIDGE_OK;
}

static void WifiBridge_AT_ProcessNetwork(WifiBridge_Handle *h)
{
    uint32_t now;

    if (h == NULL || h->active_backend != WIFI_BRIDGE_BACKEND_AT) {
        return;
    }
    now = h->hardware->now_ms(h->huart);
    if (h->at_udp_peer_valid &&
        (int32_t)(now - (h->at_udp_peer_last_rx_ms +
                         WIFI_BRIDGE_AT_UDP_PEER_TIMEOUT_MS)) >= 0) {
        h->at_udp_peer_valid = 0u;
        h->services->Permission_RevokeRemoteNode(h->services_ctx, UNER_NODE_PC_QT);
        h->services->UNER_App_OnAtNetworkDisconnected(h->services_ctx);
    }
    if (h->at_network_restart_pending && !WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY) &&
        !h->tx_busy) {
        h->at_network_restart_pending = 0u;
        h->at_network_stage = 0u;
        WifiBridge_AT_NetworkAdvance(h, 1u);
        return;
    }
    if (h->at_pending_close_link_id >= 0 &&
        !WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY) && !h->tx_busy) {
        uint16_t pos = 0u;
        h->at_tx_buf[0] = '\0';
        if (WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                     "AT+CIPCLOSE=") &&
            WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos,
                                   (uint8_t)h->at_pending_close_link_id) &&
            WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\r\n") &&
            WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_NETWORK_CLOSE,
                               h->at_tx_buf) == WIFI_BRIDGE_OK) {
            h->at_pending_close_link_id = -1;
        }
        return;
    }
    if (h->at_frame_queue_count > 0u &&
        !WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY) && !h->tx_busy) {
        (void)WifiBridge_AT_StartQueuedSend(h);
    }
}

static void WifiBridge_AT_ParseLine(WifiBridge_Handle *h, const char *line)
{
    if ((h == NULL) || (line == NULL) || (line[0] == '\0')) {
        return;
    }
    if (strcmp(line, "SEND OK") == 0 &&
        h->at_send_state == WIFI_BRIDGE_AT_SEND_WAIT_RESULT) {
        WifiBridge_AT_FinishQueuedSend(h, 1u);
        return;
    }
    if ((strcmp(line, "SEND FAIL") == 0 || strcmp(line, "ERROR") == 0) &&
        h->at_send_state != WIFI_BRIDGE_AT_SEND_IDLE) {
        WifiBridge_AT_FinishQueuedSend(h, 0u);
        return;
    }
    if (line[0] >= '0' && line[0] <= '4' && line[1] == ',') {
        const int8_t link_id = (int8_t)(line[0] - '0');

        if (strcmp(&line[2], "CONNECT") == 0 &&
            h->at_network_protocol == WIFI_BRIDGE_NETWORK_TCP) {
            if (h->at_tcp_link_id < 0) {
                h->at_tcp_link_id = link_id;
            } else if (h->at_tcp_link_id != link_id) {
                h->at_pending_close_link_id = link_id;
                h->at_network_counters.rejected_peers++;
            }
            return;
        }
        if (strcmp(&line[2], "CLOSED") == 0) {
            if (link_id == h->at_tcp_link_id) {
                h->at_tcp_link_id = -1;
                h->services->Permission_RevokeRemoteNode(h->services_ctx, UNER_NODE_PC_QT);
                h->services->UNER_App_OnAtNetworkDisconnected(h->services_ctx);
            }
            return;
        }
    }


    if (strcmp(line, "OK") == 0) {
        WifiBridge_AT_CommandDone(h, 1u);
    } else if ((strcmp(line, "ERROR") == 0) ||
               (strcmp(line, "FAIL") == 0) ||
               (strcmp(line, "SEND FAIL") == 0)) {
        WifiBridge_AT_CommandDone(h, 0u);
    } else if (strcmp(line, "ready") == 0) {
        WifiBridge_AT_ResetNetworkSession(h);
        h->at_network_restart_pending = 1u;
        WifiBridge_RegisterBackendAlive(h, WIFI_BRIDGE_BACKEND_AT, h->mode);
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_BOOT, 0u);
        if (h->at_reboot_pending) {
            const uint8_t boot_mode = h->at_reboot_mode;

            h->at_reboot_pending = 0u;
            if (boot_mode == WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_AP) {
                (void)WifiBridge_AT_StartAP(h);
            }
        }
    } else if (strncmp(line, "busy ", 5u) == 0) {
        WIFI_FLAG_SET(h, WIFI_FLAG_BUSY);
    } else if (strcmp(line, "WIFI CONNECTED") == 0) {
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_STA_CONNECTED, 0u);
    } else if (strcmp(line, "WIFI GOT IP") == 0) {
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_IP);
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_GOT_IP, 0u);
    } else if (strcmp(line, "WIFI DISCONNECT") == 0) {
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_IP);
        WifiBridge_AT_ResetNetworkSession(h);
        if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
            h->at_network_restart_pending = 1u;
        }
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_STA_DISCONNECTED, 0u);
    } else if (strcmp(line, ">") == 0) {
        h->status = '>';
    } else if (strcmp(line, "SEND OK") == 0) {
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_OK);
    } else if (strncmp(line, "+CWLAP:", 7u) == 0) {
        WifiBridge_AT_ParseCwlap(h, line);
        h->at_scan_count = (*h->system).connectivity.networksFound;
    } else if (strncmp(line, "+CWMODE_CUR:", 12u) == 0) {
        const char *p = line + 12u;

        h->mode = (uint8_t)WifiBridge_AT_ParseSigned(&p);
    } else if (strncmp(line, "STATUS:", 7u) == 0) {
        const char *p = line + 7u;
        h->status = (uint8_t)WifiBridge_AT_ParseSigned(&p);
        if (h->status == 2u || h->status == 3u) {
            WIFI_FLAG_SET(h, WIFI_FLAG_GOT_IP);
        } else if (h->status == 5u) {
            WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_IP);
        }
    } else if (strncmp(line, "+CWJAP:", 7u) == 0) {
        const char *p = line + 7u;

        h->status = (uint8_t)WifiBridge_AT_ParseSigned(&p);
    } else if (strncmp(line, "AT version:", 11u) == 0) {
        WifiBridge_AT_StoreFirmwareVersion(h, line);
    } else if (strncmp(line, "+CIFSR:", 7u) == 0) {
        uint8_t ip[WIFI_BRIDGE_IP_LEN];

        if (WifiBridge_AT_ParseIp(line, ip)) {
            if (strstr(line, "APIP") != NULL) {
                h->at_status_ap_ip_seen = 1u;
                if (WifiBridge_AT_IpIsZero(ip)) {
                    WifiBridge_AT_ClearApIp(h);
                } else {
                    WifiBridge_AT_SetApIp(h, ip);
                    SET_FLAG((*h->system).runtime.systemFlags2, ESP_PRESENT);
                }
            } else if (strstr(line, "STAIP") != NULL) {
                h->at_status_sta_ip_seen = 1u;
                if (WifiBridge_AT_IpIsZero(ip)) {
                    WifiBridge_AT_ClearStaIp(h);
                    WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_IP);
                } else {
                    memcpy(h->ip, ip, sizeof(h->ip));
                    WifiBridge_AT_SetStaIp(h, ip);
                    WIFI_FLAG_SET(h, WIFI_FLAG_GOT_IP);
                    WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_GOT_IP, 0u);
                }
            }
        }
    } else if (strncmp(line, "+IPD,", 5u) == 0) {
        WIFI_FLAG_SET(h, WIFI_FLAG_RX_PENDING);
    }
}

static void WifiBridge_AT_ConsumeBytes(WifiBridge_Handle *h, const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0u; i < len; ++i) {
        char ch = (char)data[i];
        if (h->at_ipd_active) {
            if (h->at_network_protocol == WIFI_BRIDGE_NETWORK_TCP) {
                if (h->at_ipd_link_id == h->at_tcp_link_id) {
                    h->services->UNER_App_FeedUart1Bytes(h->services_ctx, &data[i], 1u);
                }
            } else if (h->at_ipd_received_len < WIFI_BRIDGE_AT_MAX_FRAME_SIZE) {
                h->at_ipd_payload[h->at_ipd_received_len] = data[i];
            }
            h->at_ipd_received_len++;
            if (h->at_ipd_received_len >= h->at_ipd_expected_len) {
                WifiBridge_AT_CompleteIpd(h);
            }
            continue;
        }

        if (ch == '>' && h->at_send_state == WIFI_BRIDGE_AT_SEND_WAIT_PROMPT) {
            const uint8_t head = h->at_frame_queue_head;
            h->at_line_len = 0u;
            h->at_line[0] = '\0';
            if (!h->tx_busy && h->at_frame_queue_count > 0u &&
                h->hardware->tx(h->huart, h->at_frame_queue[head],
                                      h->at_frame_queue_len[head]) == AM_OK) {
                h->tx_busy = 1u;
                h->at_send_payload_started = 1u;
                h->at_send_state = WIFI_BRIDGE_AT_SEND_PAYLOAD_DMA;
                WIFI_FLAG_SET(h, WIFI_FLAG_TX_BUSY);
                h->at_deadline_ms = h->hardware->now_ms(h->huart) + WIFI_BRIDGE_AT_TIMEOUT_MS;
            } else {
                WifiBridge_AT_FinishQueuedSend(h, 0u);
            }
            continue;
        }

        if (ch == ':' && h->at_line_len >= 5u &&
            strncmp(h->at_line, "+IPD,", 5u) == 0) {
            h->at_line[h->at_line_len] = '\0';
            if (!WifiBridge_AT_ParseIpdHeader(h, h->at_line)) {
                h->at_network_counters.ipd_header_errors++;
            }
            h->at_line_len = 0u;
            h->at_line[0] = '\0';
            continue;
        }


        if (ch == '\r') {
            continue;
        }

        if (ch == '\n') {
            h->at_line[h->at_line_len] = '\0';
            WifiBridge_AT_ParseLine(h, h->at_line);
            h->at_line_len = 0u;
            h->at_line[0] = '\0';
            continue;
        }

        if (h->at_line_len + 1u < WIFI_BRIDGE_AT_LINE_SIZE) {
            h->at_line[h->at_line_len++] = ch;
        } else {
            h->at_line_len = 0u;
            h->at_line[0] = '\0';
        }
    }
}

static WifiBridge_Result WifiBridge_AT_Send(WifiBridge_Handle *h,
                                            WifiBridge_AT_Operation op,
                                            const char *cmd)
{
    size_t len;
    uint32_t timeout_ms = WIFI_BRIDGE_AT_TIMEOUT_MS;

    if (op == WIFI_BRIDGE_AT_OP_PING) {
        timeout_ms = WIFI_BRIDGE_AT_PING_TIMEOUT_MS;
    } else if (op == WIFI_BRIDGE_AT_OP_SCAN) {
        timeout_ms = WIFI_BRIDGE_AT_SCAN_TIMEOUT_MS;
    } else if (op == WIFI_BRIDGE_AT_OP_CONNECT) {
        timeout_ms = WIFI_BRIDGE_AT_JOIN_TIMEOUT_MS;
    }

    if ((h == NULL) || (h->huart == NULL) || (cmd == NULL)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->tx_busy || WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY)) {
        return WIFI_BRIDGE_BUSY;
    }

    len = WifiBridge_AT_StrLenBounded(cmd, WIFI_BRIDGE_AT_TX_SIZE);
    if ((len == 0u) || (len >= WIFI_BRIDGE_AT_TX_SIZE)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (cmd != h->at_tx_buf) {
        memcpy(h->at_tx_buf, cmd, len);
    }
    h->at_tx_len = (uint16_t)len;
    h->at_active_op = op;
    h->at_deadline_ms = h->hardware->now_ms(h->huart) + timeout_ms;
    WIFI_FLAG_SET(h, WIFI_FLAG_BUSY);
    WIFI_FLAG_SET(h, WIFI_FLAG_TX_BUSY);
    h->tx_busy = 1u;
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_OK);
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_GOT_ERROR);

    if (h->hardware->tx(h->huart, (uint8_t *)h->at_tx_buf, h->at_tx_len) != AM_OK) {
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
        h->tx_busy = 0u;
        h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
        return WIFI_BRIDGE_BUSY;
    }

    return WIFI_BRIDGE_OK;
}

static WifiBridge_Result WifiBridge_AT_SendJoin(WifiBridge_Handle *h)
{
    uint16_t pos = 0u;
    const WifiBridge_Credentials *credentials = WifiBridge_GetRuntimeCredentials_H(h);

    if ((h == NULL) ||
        (credentials == NULL) ||
        (credentials->flags.bits.ssid_set == 0u)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    WifiBridge_AT_CopyText(h->ssid, sizeof(h->ssid), credentials->ssid);
    WifiBridge_AT_CopyText((*h->system).connectivity.espWifiConnection.staSsid, sizeof((*h->system).connectivity.espWifiConnection.staSsid), credentials->ssid);

    h->at_tx_buf[0] = '\0';
    if (!WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "AT+CWJAP_CUR=") ||
        !WifiBridge_AT_AppendQuoted(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, credentials->ssid) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",") ||
        !WifiBridge_AT_AppendQuoted(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, credentials->password) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\r\n")) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_CONNECT, h->at_tx_buf);
}
void WifiBridge_AT_ResetNetworkSession(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    h->at_network_ready = 0u;
    h->at_network_stage = 0u;
    h->at_tcp_link_id = -1;
    h->at_pending_close_link_id = -1;
    h->at_udp_peer_valid = 0u;
    h->at_detected_capabilities = 0u;
    h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
    h->at_deadline_ms = 0u;
    h->at_tx_len = 0u;
    h->tx_busy = 0u;
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
    memset(h->at_udp_peer_ip, 0, sizeof(h->at_udp_peer_ip));
    h->at_udp_peer_port = 0u;
    h->at_udp_peer_last_rx_ms = 0u;
    h->at_ipd_active = 0u;
    h->at_ipd_header_len = 0u;
    h->at_ipd_expected_len = 0u;
    h->at_ipd_received_len = 0u;
    h->at_frame_queue_head = 0u;
    h->at_frame_queue_tail = 0u;
    h->at_frame_queue_count = 0u;
    memset(h->at_frame_queue_len, 0, sizeof(h->at_frame_queue_len));
    h->at_send_state = WIFI_BRIDGE_AT_SEND_IDLE;
    h->at_send_payload_started = 0u;
    h->services->Permission_RevokeRemoteNode(h->services_ctx, UNER_NODE_PC_QT);
    h->services->UNER_App_OnAtNetworkDisconnected(h->services_ctx);
}

WifiBridge_Result WifiBridge_AT_StartNetworkService(WifiBridge_Handle *h)
{
    if (h == NULL || h->active_backend != WIFI_BRIDGE_BACKEND_AT) {
        return WIFI_BRIDGE_TIMEOUT;
    }
    WifiBridge_AT_ResetNetworkSession(h);
    h->at_network_restart_pending = 1u;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_SetNetworkProtocol(WifiBridge_Handle *h,
                                                   WifiBridgeNetworkProtocol protocol)
{
    if (h == NULL ||
        (protocol != WIFI_BRIDGE_NETWORK_TCP &&
         protocol != WIFI_BRIDGE_NETWORK_UDP)) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    if (h->at_network_protocol == protocol && h->at_network_ready) {
        return WIFI_BRIDGE_OK;
    }
    WifiBridge_AT_ResetNetworkSession(h);
    h->at_network_protocol = protocol;
    h->at_network_restart_pending =
        h->active_backend == WIFI_BRIDGE_BACKEND_AT ? 1u : 0u;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_QueueFrame(WifiBridge_Handle *h,
                                           const uint8_t *frame,
                                           uint16_t len)
{
    uint8_t tail;

    if (h == NULL || frame == NULL || len == 0u ||
        len > WIFI_BRIDGE_AT_MAX_FRAME_SIZE) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    if (h->active_backend != WIFI_BRIDGE_BACKEND_AT ||
        !h->at_network_ready ||
        (h->at_network_protocol == WIFI_BRIDGE_NETWORK_TCP &&
         h->at_tcp_link_id < 0) ||
        (h->at_network_protocol == WIFI_BRIDGE_NETWORK_UDP &&
         !h->at_udp_peer_valid)) {
        return WIFI_BRIDGE_TIMEOUT;
    }
    if (h->at_frame_queue_count >= WIFI_BRIDGE_AT_TX_QUEUE_SLOTS) {
        h->at_network_counters.queue_drops++;
        return WIFI_BRIDGE_BUSY;
    }

    tail = h->at_frame_queue_tail;
    memcpy(h->at_frame_queue[tail], frame, len);
    h->at_frame_queue_len[tail] = len;
    h->at_frame_queue_tail =
        (uint8_t)((tail + 1u) % WIFI_BRIDGE_AT_TX_QUEUE_SLOTS);
    h->at_frame_queue_count++;
    return WIFI_BRIDGE_OK;
}


WifiBridge_Result WifiBridge_AT_Init(WifiBridge_Handle *h, void *huart)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    h->huart = huart;
    h->at_rx_last_pos = 0u;
    h->at_line_len = 0u;
    h->at_tx_len = 0u;
    h->at_deadline_ms = 0u;
    h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
    h->at_scan_count = 0u;
    h->at_status_sta_ip_seen = 0u;
    h->at_status_ap_ip_seen = 0u;
    h->at_reboot_pending = 0u;
    h->at_reboot_mode = WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_NORMAL;
    h->at_detected_capabilities = 0u;
    h->at_network_protocol =
        h->services->AppPreferences_GetWifiAtProtocol(h->services_ctx) == WIFI_BRIDGE_NETWORK_UDP
            ? WIFI_BRIDGE_NETWORK_UDP
            : WIFI_BRIDGE_NETWORK_TCP;
    h->at_network_restart_pending = 0u;
    h->at_tcp_link_id = -1;
    h->at_pending_close_link_id = -1;
    h->at_send_state = WIFI_BRIDGE_AT_SEND_IDLE;
    memset(&h->at_network_counters, 0, sizeof(h->at_network_counters));
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_Start(WifiBridge_Handle *h)
{
    if ((h == NULL) || (h->huart == NULL)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    WifiBridge_AT_ResetRxPos(h);
    return WIFI_BRIDGE_OK;
}

void WifiBridge_AT_Process(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }
    WifiBridge_AT_RxCheck(h);
    if (h->at_send_state != WIFI_BRIDGE_AT_SEND_IDLE &&
        h->at_deadline_ms != 0u &&
        ((int32_t)(h->hardware->now_ms(h->huart) - h->at_deadline_ms) >= 0) &&
        h->at_active_op == WIFI_BRIDGE_AT_OP_CIPSEND) {
        h->at_network_counters.at_timeouts++;
        WifiBridge_AT_FinishQueuedSend(h, 0u);
    } else if (WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY) &&
               h->at_deadline_ms != 0u &&
               ((int32_t)(h->hardware->now_ms(h->huart) - h->at_deadline_ms) >= 0) &&
               h->at_active_op == WIFI_BRIDGE_AT_OP_NETWORK_CONFIG) {
        h->at_network_counters.at_timeouts++;
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
        h->tx_busy = 0u;
        h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
        h->at_network_restart_pending = 1u;
    }


    if (WIFI_FLAG_IS_SET(h, WIFI_FLAG_BUSY) &&
        h->at_deadline_ms != 0u &&
        ((int32_t)(h->hardware->now_ms(h->huart) - h->at_deadline_ms) >= 0)) {
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
        h->tx_busy = 0u;
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_ERROR);
        if (h->at_active_op == WIFI_BRIDGE_AT_OP_REBOOT) {
            h->at_reboot_pending = 0u;
        }
        h->at_active_op = WIFI_BRIDGE_AT_OP_NONE;
        WifiBridge_AT_Emit(h, WIFI_BRIDGE_EVT_ERROR, WIFI_BRIDGE_TIMEOUT);
    }
    WifiBridge_AT_ProcessNetwork(h);

}

void WifiBridge_AT_RxCheck(WifiBridge_Handle *h)
{
    uint16_t pos;

    if ((h == NULL) || (h->huart == NULL) || (!h->hardware->rx_ready(h->huart))) {
        return;
    }
    WIFI_FLAG_CLEAR(h, WIFI_FLAG_RX_PENDING);
    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        WifiBridge_AT_ResetRxPos(h);
        return;
    }


    pos = (uint16_t)(WIFI_BRIDGE_UART_RX_DMA_SIZE -
                     h->hardware->rx_remaining(h->huart));
    if (pos >= WIFI_BRIDGE_UART_RX_DMA_SIZE) {
        pos = 0u;
    }

    if (pos == h->at_rx_last_pos) {
        return;
    }

    if (pos > h->at_rx_last_pos) {
        WifiBridge_AT_ConsumeBytes(h,
                                   &h->rx_dma[h->at_rx_last_pos],
                                   (uint16_t)(pos - h->at_rx_last_pos));
    } else {
        WifiBridge_AT_ConsumeBytes(h,
                                   &h->rx_dma[h->at_rx_last_pos],
                                   (uint16_t)(WIFI_BRIDGE_UART_RX_DMA_SIZE - h->at_rx_last_pos));
        if (pos > 0u) {
            WifiBridge_AT_ConsumeBytes(h, h->rx_dma, pos);
        }
    }

    h->at_rx_last_pos = pos;
}

void WifiBridge_AT_OnTxComplete(WifiBridge_Handle *h)
{
    if (h != NULL) {
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
        h->tx_busy = 0u;
        if (h->at_send_state == WIFI_BRIDGE_AT_SEND_PAYLOAD_DMA) {
            h->at_send_state = WIFI_BRIDGE_AT_SEND_WAIT_RESULT;
            h->at_deadline_ms = h->hardware->now_ms(h->huart) + WIFI_BRIDGE_AT_TIMEOUT_MS;
        }

    }
}

void WifiBridge_AT_ResetRxPos(WifiBridge_Handle *h)
{
    if ((h == NULL) || (h->huart == NULL) || (!h->hardware->rx_ready(h->huart))) {
        return;
    }

    h->at_rx_last_pos = (uint16_t)(WIFI_BRIDGE_UART_RX_DMA_SIZE -
                                   h->hardware->rx_remaining(h->huart));
    if (h->at_rx_last_pos >= WIFI_BRIDGE_UART_RX_DMA_SIZE) {
        h->at_rx_last_pos = 0u;
    }
}

WifiBridge_Result WifiBridge_AT_SetModeSTA(WifiBridge_Handle *h)
{
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_SET_STA, "AT+CWMODE_CUR=1\r\n");
}

WifiBridge_Result WifiBridge_AT_SetModeAP(WifiBridge_Handle *h)
{
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_SET_AP, "AT+CWMODE_CUR=2\r\n");
}

static WifiBridge_Result WifiBridge_AT_SendApConfig(WifiBridge_Handle *h)
{
    AppApConfig_t config;
    uint16_t pos = 0u;
    const char *password;
    uint8_t encryption;

    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    h->services->AppApConfig_Get(h->services_ctx, &config);
    password = config.protected ? config.password : "";
    encryption = config.protected ? WIFI_BRIDGE_AT_AP_DEFAULT_ENCRYPTION : 0u;

    if (!WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "AT+CWSAP_CUR=") ||
        !WifiBridge_AT_AppendQuoted(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, config.ssid) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",") ||
        !WifiBridge_AT_AppendQuoted(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, password) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, WIFI_BRIDGE_AT_AP_DEFAULT_CHANNEL) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, encryption) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ",") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, WIFI_BRIDGE_AT_AP_DEFAULT_MAX_CONN) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\r\n")) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_START_AP, h->at_tx_buf);
}

static WifiBridge_Result WifiBridge_AT_SendApIpConfig(WifiBridge_Handle *h)
{
    AppApConfig_t config;
    uint8_t ip[4];
    uint16_t pos = 0u;

    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    h->services->AppApConfig_Get(h->services_ctx, &config);
    if ((config.ipv4[0] | config.ipv4[1] | config.ipv4[2] | config.ipv4[3]) == 0u) {
        ip[0] = 192u; ip[1] = 168u; ip[2] = 4u; ip[3] = 1u;
    } else {
        memcpy(ip, config.ipv4, sizeof(ip));
    }
    if (!WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "AT+CIPAP_CUR=\"") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ip[0]) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ".") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ip[1]) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ".") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ip[2]) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ".") ||
        !WifiBridge_AT_AppendU8(h, h->at_tx_buf, sizeof(h->at_tx_buf), &pos, ip[3]) ||
        !WifiBridge_AT_AppendText(h->at_tx_buf, sizeof(h->at_tx_buf), &pos, "\"\r\n")) {
        return WIFI_BRIDGE_BAD_ARG;
    }
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_APPLY_AP_IP, h->at_tx_buf);
}
static WifiBridge_Result WifiBridge_AT_StartAP(WifiBridge_Handle *h)
{
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_START_AP_MODE, "AT+CWMODE_CUR=2\r\n");
}

static WifiBridge_Result WifiBridge_AT_StopAP(WifiBridge_Handle *h)
{
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_STOP_AP, "AT+CWMODE_CUR=1\r\n");
}

WifiBridge_Result WifiBridge_AT_SetCredentials(WifiBridge_Handle *h,
                                               const char *ssid,
                                               const char *pass)
{
    if ((h == NULL) || (ssid == NULL) || (pass == NULL)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    WifiBridge_Result status = WifiBridge_SetRuntimeCredentials_H(h, ssid, pass);
    if (status == WIFI_BRIDGE_OK) {
        WifiBridge_AT_CopyText(h->ssid, sizeof(h->ssid), (*h->credentials).ssid);
    }

    return status;
}

WifiBridge_Result WifiBridge_AT_Connect(WifiBridge_Handle *h)
{
    return WifiBridge_AT_SendJoin(h);
}

WifiBridge_Result WifiBridge_AT_Disconnect(WifiBridge_Handle *h)
{
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_DISCONNECT, "AT+CWQAP\r\n");
}

WifiBridge_Result WifiBridge_AT_StartScan(WifiBridge_Handle *h)
{
    h->services->clear_scan_results();
    if (h != NULL) {
        h->at_scan_count = 0u;
    }
    return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_SCAN, "AT+CWLAP\r\n");
}

WifiBridge_Result WifiBridge_AT_StopScan(WifiBridge_Handle *h)
{
    (void)h;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_GetScanResults(WifiBridge_Handle *h)
{
    return WifiBridge_AT_StartScan(h);
}

WifiBridge_Result WifiBridge_AT_GetStatus(WifiBridge_Handle *h)
{
    if (h != NULL) {
        h->status = 0u;
        h->at_status_sta_ip_seen = 0u;
        h->at_status_ap_ip_seen = 0u;
        WifiBridge_AT_ClearStaIp(h);
        WifiBridge_AT_ClearApIp(h);
    }

    return WifiBridge_AT_Send(h,
                              WIFI_BRIDGE_AT_OP_STATUS_MODE,
                              "AT+CWMODE_CUR?\r\n");
}

WifiBridge_Result WifiBridge_AT_SendCommand(WifiBridge_Handle *h,
                                            uint8_t cmd,
                                            const uint8_t *payload,
                                            uint8_t len)
{
    (void)payload;
    (void)len;

    switch (cmd) {
        case WIFI_BRIDGE_CMD_ID_CLEAR_CREDENTIALS:
            return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_CLEAR_CREDENTIALS, "AT+CWJAP_DEF=\"\",\"\"\r\n");
        case WIFI_BRIDGE_CMD_ID_FACTORY_RESET:
            return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_FACTORY_RESET, "AT+RESTORE\r\n");
        case WIFI_BRIDGE_CMD_ID_SET_AUTO_RECONNECT:
            if (payload == NULL || len != 1u || payload[0] > 1u) return WIFI_BRIDGE_BAD_ARG;
            return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_AUTO_RECONNECT,
                                      payload[0] ? "AT+CWAUTOCONN=1\r\n" : "AT+CWAUTOCONN=0\r\n");
        case WIFI_BRIDGE_CMD_ID_SET_MODE_AP:
            return WifiBridge_AT_SetModeAP(h);
        case WIFI_BRIDGE_CMD_ID_SET_MODE_STA:
            return WifiBridge_AT_SetModeSTA(h);
        case WIFI_BRIDGE_CMD_ID_START_AP:
            return WifiBridge_AT_StartAP(h);
        case WIFI_BRIDGE_CMD_ID_STOP_AP:
            return WifiBridge_AT_StopAP(h);
        case WIFI_BRIDGE_CMD_ID_START_SCAN:
        case WIFI_BRIDGE_CMD_ID_GET_SCAN_RESULTS:
            return WifiBridge_AT_StartScan(h);
        case WIFI_BRIDGE_CMD_ID_STOP_SCAN:
            return WifiBridge_AT_StopScan(h);
        case WIFI_BRIDGE_CMD_ID_GET_STATUS:
            return WifiBridge_AT_GetStatus(h);
        case WIFI_BRIDGE_CMD_ID_CONNECT_WIFI:
            return WifiBridge_AT_Connect(h);
        case WIFI_BRIDGE_CMD_ID_DISCONNECT_WIFI:
            return WifiBridge_AT_Disconnect(h);
        case WIFI_BRIDGE_CMD_ID_REBOOT_ESP:
            return WifiBridge_AT_SendEspReboot(h, WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_NORMAL);
        case WIFI_BRIDGE_CMD_ID_REQUEST_FIRMWARE:
            (*h->system).connectivity.espFirmwareVersion[0] = 0;
            return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_FIRMWARE, "AT+GMR\r\n");
        case WIFI_BRIDGE_CMD_ID_PING:
            return WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_PING, "AT\r\n");
        default:
            return WIFI_BRIDGE_UNSUPPORTED;
    }
}

WifiBridge_Result WifiBridge_AT_SendWifiCredentialRequest(WifiBridge_Handle *h,
                                                          const char *ssid)
{
    WifiBridge_Result status;

    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    status = WifiBridge_SetRuntimeSsid_H(h, ssid);
    if (status != WIFI_BRIDGE_OK) {
        return status;
    }

    WifiBridge_AT_CopyText(h->ssid, sizeof(h->ssid), (*h->credentials).ssid);
    return WifiBridge_AT_Connect(h);
}

WifiBridge_Result WifiBridge_AT_RequestWifiNetworkDetail(WifiBridge_Handle *h,
                                                         const char *ssid)
{
    int8_t index;

    (void)h;
    index = WifiBridge_AT_FindNetwork(h, ssid);
    if (index < 0) {
        return WIFI_BRIDGE_UNSUPPORTED;
    }

    (*h->system).connectivity.wifiNetworkDetailValid[(uint8_t)index] = 1u;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode)
{
    WifiBridge_Result result;

    if (h == NULL || boot_mode > WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_AP) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    h->at_reboot_pending = 1u;
    h->at_reboot_mode = boot_mode;
    result = WifiBridge_AT_Send(h, WIFI_BRIDGE_AT_OP_REBOOT, "AT+RST\r\n");
    if (result != WIFI_BRIDGE_OK) {
        h->at_reboot_pending = 0u;
    }
    return result;
}

WifiBridge_Result WifiBridge_AT_ReportScreenChanged(WifiBridge_Handle *h,
                                                    uint32_t screen_code,
                                                    uint8_t source)
{
    (void)h;
    (void)screen_code;
    (void)source;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_ReportMenuSelectionChanged(WifiBridge_Handle *h,
                                                           uint32_t screen_code,
                                                           uint8_t selected_index,
                                                           uint8_t item_count,
                                                           uint8_t source)
{
    (void)h;
    (void)screen_code;
    (void)selected_index;
    (void)item_count;
    (void)source;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode)
{
    (void)h;
    (void)mode;
    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_AT_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id)
{
    (void)h;
    (void)asset_id;
    return WIFI_BRIDGE_UNSUPPORTED;
}

uint8_t WifiBridge_AT_GetBinaryAsset(WifiBridge_Handle *h,
                                     uint8_t asset_id,
                                     WifiBridge_BinaryAssetView *asset)
{
    (void)h;
    (void)asset_id;
    (void)asset;
    return 0u;
}

uint8_t WifiBridge_AT_IsWaitingValidation(WifiBridge_Handle *h)
{
    return (h != NULL && h->at_active_op != WIFI_BRIDGE_AT_OP_NONE) ? 1u : 0u;
}

uint8_t WifiBridge_AT_GetWaitingCommandId(WifiBridge_Handle *h)
{
    (void)h;
    return 0u;
}
