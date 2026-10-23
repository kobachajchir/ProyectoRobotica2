#include "uner_device.h"

/* ======= Parser interno ======= */
typedef enum {
    P_WAIT_HEADER = 0,
    P_WAIT_LEN,
    P_WAIT_TOKEN,
    P_WAIT_CMD,
    P_WAIT_PAYLOAD,
    P_WAIT_CHK
} P_State;

static const uint8_t UNER_HDR[UNER_HEADER_LENGTH] = { 'U','N','E','R' };

/* XOR de todos los bytes [0..len-1] */
static uint8_t xor_cksum(const uint8_t *p, size_t len) {
    uint8_t x = 0;
    for (size_t i=0; i<len; ++i) x ^= p[i];
    return x;
}

/* Construye y envía un frame UNER (cmd + payload). */
static UNER_Status send_frame(UNER_Device *dev, uint8_t cmd,
                              const uint8_t *payload, uint8_t pay_len)
{
    if (!dev || !dev->send_bytes) return UNER_STATUS_ERR_NULL_PTR;
    if (pay_len > UNER_PCK_MAX_PAYLOAD) return UNER_STATUS_ERR_RANGE;

    uint8_t buf[UNER_PCK_MAX_TOTAL];
    uint8_t i = 0;

    /* Header "UNER" */
    for (uint8_t k=0;k<UNER_HEADER_LENGTH;k++) buf[i++] = UNER_HDR[k];
    /* Length */
    const uint8_t len_field = (uint8_t)(1u /*CMD*/ + pay_len + 1u /*CHK*/);
    buf[i++] = len_field;
    /* Token ':' */
    buf[i++] = UNER_TOKEN;
    /* CMD */
    buf[i++] = cmd;
    /* Payload */
    for (uint8_t k=0;k<pay_len;k++) buf[i++] = payload[k];

    /* Checksum: XOR(header..payload) */
    const uint8_t chk = xor_cksum(buf, i);
    buf[i++] = chk;

    const uint16_t total = i;
    const uint8_t txst = dev->send_bytes(buf, total);
    return (txst == 0) ? UNER_STATUS_OK : UNER_STATUS_ERR_TX_FAIL;
}

/* ======= API ======= */

void uner_dev_init(UNER_Device *dev,
                   uner_send_bytes_fn tx_fn,
                   uner_wifi_get_mode_fn get_mode_fn,
                   uner_wifi_request_scan_fn req_scan_fn,
                   uner_wifi_set_ap_fn set_ap_fn,
                   uner_wifi_set_sta_fn set_sta_fn,
                   uner_telemetry_fill_fn tlm_fill_fn)
{
    if (!dev) return;

    dev->head = dev->tail = 0;
    dev->state = (uint8_t)P_WAIT_HEADER;
    dev->header_prog = 0;
    dev->len_field = 0;
    dev->expected_payload = 0;
    dev->cmd = 0;
    dev->pay_idx = 0;
    dev->cks_running = 0;

    dev->send_bytes      = tx_fn;
    dev->wifi_get_mode   = get_mode_fn;
    dev->wifi_request_scan = req_scan_fn;
    dev->wifi_set_ap     = set_ap_fn;
    dev->wifi_set_sta    = set_sta_fn;
    dev->telemetry_fill  = tlm_fill_fn;

    dev->heartbeat_period_ms = 0;
    dev->heartbeat_elapsed_ms = 0;

    dev->telemetry_period_ms = 0;
    dev->telemetry_elapsed_ms = 0;
    dev->telemetry_seq = 0;
    dev->telemetry_due = false;

    dev->last_cmd_valid = false;
    dev->last_cmd = 0;
}

void uner_dev_reset_parser(UNER_Device *dev) {
    if (!dev) return;
    dev->state = (uint8_t)P_WAIT_HEADER;
    dev->header_prog = 0;
    dev->len_field = 0;
    dev->expected_payload = 0;
    dev->cmd = 0;
    dev->pay_idx = 0;
    dev->cks_running = 0;
}

/* Ring helpers */
static inline uint16_t ring_count(const UNER_Device *d) {
    return (uint16_t)(d->head - d->tail);
}

UNER_Status uner_dev_push_byte(UNER_Device *dev, uint8_t b) {
    if (!dev) return UNER_STATUS_ERR_NULL_PTR;
    uint16_t used = ring_count(dev);
    if (used >= (UNER_RX_RING_CAP - 1u)) {
        /* overflow: descartar 1 (advance tail) para no bloquear */
        dev->tail++;
        /* opcional: retornar error */
        return UNER_STATUS_ERR_RX_OVERFLOW;
    }
    dev->rx_ring[ dev->head & (UNER_RX_RING_CAP - 1u) ] = b;
    dev->head++;
    return UNER_STATUS_OK;
}

UNER_Status uner_dev_push_bytes(UNER_Device *dev, const uint8_t *data, size_t len) {
    if (!dev || !data) return UNER_STATUS_ERR_NULL_PTR;
    UNER_Status agg = UNER_STATUS_OK;
    for (size_t i=0;i<len;i++) {
        UNER_Status st = uner_dev_push_byte(dev, data[i]);
        if (st != UNER_STATUS_OK) agg = st;
    }
    return agg;
}

/* Resync para header: intenta considerar 'b' como posible 'U' */
static void resync_header(UNER_Device *dev, uint8_t b) {
    uner_dev_reset_parser(dev);
    if (b == 'U') {
        dev->state = (uint8_t)P_WAIT_HEADER;
        dev->header_prog = 1;
        dev->cks_running = 'U';
    }
}

/* --------- Respuestas específicas --------- */

UNER_Status uner_dev_send_wifi_mode(UNER_Device *dev, uint8_t mode) {
    const uint8_t p[1] = { (uint8_t)(mode ? 1u : 0u) };
    return send_frame(dev, (uint8_t)UNER_CMD_WIFI_MODE, p, 1);
}

uint8_t uner_dev_send_wifi_scan_list(UNER_Device *dev,
                                     const UNER_WifiNetworkDesc *list, uint8_t n)
{
    if (!dev || !list || n == 0) return 0;
    /* Empaquetar hasta que se llene payload (<=30 bytes) */
    uint8_t p[UNER_PCK_MAX_PAYLOAD];
    uint8_t i = 0;
    uint8_t sent = 0;

    /* Primero N (podría ser truncado si no entra todo) */
    uint8_t Npos = i++;
    p[Npos] = 0; /* lo rellenamos al final con la cantidad real */

    for (uint8_t k=0; k<n; ++k) {
        const UNER_WifiNetworkDesc *nw = &list[k];
        uint8_t need = 1 + nw->ssid_len + 1 + 1; /* ssidLen + ssid + rssi(i8) + sec(u8) */
        if (nw->ssid_len > 32) need = 255; /* fuerza corte si ssid es ridículo */
        if ((i + need) > UNER_PCK_MAX_PAYLOAD) break;

        p[i++] = nw->ssid_len;
        for (uint8_t j=0; j<nw->ssid_len; ++j) p[i++] = nw->ssid[j];
        p[i++] = (uint8_t)nw->rssi_dbm;       /* interpretado como i8 en host */
        p[i++] = (uint8_t)nw->security;

        sent++;
    }

    p[Npos] = sent;
    (void)send_frame(dev, (uint8_t)UNER_CMD_WIFI_SCAN_LIST, p, i);
    return sent;
}

UNER_Status uner_dev_send_heartbeat_beat(UNER_Device *dev) {
    uint8_t p[2];
    uner_le16_write(p, dev->heartbeat_period_ms);
    return send_frame(dev, (uint8_t)UNER_CMD_HEARTBEAT_BEAT, p, 2);
}

void uner_dev_set_telemetry_rate(UNER_Device *dev, uint16_t period_ms) {
    if (!dev) return;
    dev->telemetry_period_ms = period_ms;
    dev->telemetry_elapsed_ms = 0;
}

UNER_Status uner_dev_send_telemetry_mpu6050(UNER_Device *dev,
                                            uint16_t seq,
                                            int16_t accX, int16_t accY, int16_t accZ,
                                            int16_t gyroX, int16_t gyroY, int16_t gyroZ,
                                            int16_t tempRaw)
{
    if (!dev) return UNER_STATUS_ERR_NULL_PTR;

    uint8_t p[UNER_PCK_MAX_PAYLOAD];
    uint8_t i = 0;

    /* schema */
    p[i++] = 0x01; /* TELEMETRY_SCHEMA.MPU6050_INT16 */
    /* seq LE16 */
    uner_le16_write(&p[i], seq); i += 2;
    /* i16[6] + tempRaw */
    /* Escribir en LE16 */
    int16_t vals[7] = { accX, accY, accZ, gyroX, gyroY, gyroZ, tempRaw };
    for (uint8_t k=0;k<7;k++) {
        uner_le16_write(&p[i], (uint16_t)vals[k]); i += 2;
    }

    dev->telemetry_due = false; /* consumido */
    return send_frame(dev, (uint8_t)UNER_CMD_TELEM_DATA, p, i);
}

/* --------- Tick periódico --------- */

void uner_dev_tick(UNER_Device *dev, uint16_t elapsed_ms) {
    if (!dev) return;

    /* Heartbeat BEAT automático */
    if (dev->heartbeat_period_ms > 0) {
        uint32_t e = (uint32_t)dev->heartbeat_elapsed_ms + elapsed_ms;
        while (e >= dev->heartbeat_period_ms) {
            e -= dev->heartbeat_period_ms;
            /* enviar BEAT */
            (void)uner_dev_send_heartbeat_beat(dev);
        }
        dev->heartbeat_elapsed_ms = (uint16_t)e;
    } else {
        dev->heartbeat_elapsed_ms = 0;
    }

    /* Telemetría periódica */
    if (dev->telemetry_period_ms > 0) {
        uint32_t e = (uint32_t)dev->telemetry_elapsed_ms + elapsed_ms;
        if (e >= dev->telemetry_period_ms) {
            e %= dev->telemetry_period_ms;
            /* Si hay callback, que él llene y envíe; si no, marcar due */
            if (dev->telemetry_fill) {
                uint8_t pay[UNER_PCK_MAX_PAYLOAD]; uint8_t n=0;
                if (dev->telemetry_fill(pay, &n) && n <= UNER_PCK_MAX_PAYLOAD) {
                    (void)send_frame(dev, (uint8_t)UNER_CMD_TELEM_DATA, pay, n);
                }
            } else {
                dev->telemetry_due = true;
                /* el app puede leer dev->telemetry_seq si lo usa */
            }
            dev->telemetry_seq++; /* incrementar secuencia sugerida */
        }
        dev->telemetry_elapsed_ms = (uint16_t)e;
    } else {
        dev->telemetry_elapsed_ms = 0;
        dev->telemetry_due = false;
    }
}

/* --------- Procesamiento RX --------- */

static void handle_cmd(UNER_Device *dev, uint8_t cmd, const uint8_t *pay, uint8_t n) {
    dev->last_cmd_valid = true;
    dev->last_cmd = cmd;

    switch (cmd) {
    /* Heartbeat */
    case UNER_CMD_HEARTBEAT_SET: {
        if (n < 2) return; /* ignorar silenciosamente si payload inválido */
        const uint16_t ms = uner_le16_read(pay);
        dev->heartbeat_period_ms = ms;
        dev->heartbeat_elapsed_ms = 0;
        /* ACK con period_ms aplicado */
        uint8_t p[2]; uner_le16_write(p, ms);
        (void)send_frame(dev, (uint8_t)UNER_CMD_HEARTBEAT_ACK, p, 2);
    } break;

    /* Wi-Fi */
    case UNER_CMD_WIFI_GET_MODE: {
        uint8_t mode = 0;
        if (dev->wifi_get_mode) mode = (uint8_t)(dev->wifi_get_mode() ? 1u : 0u);
        (void)uner_dev_send_wifi_mode(dev, mode);
    } break;

    case UNER_CMD_WIFI_GET_SCAN: {
        if (dev->wifi_request_scan) dev->wifi_request_scan();
        /* la respuesta (SCAN_LIST) llegará cuando el app llame uner_dev_send_wifi_scan_list() */
    } break;

    case UNER_CMD_WIFI_SET_AP: {
        uint8_t code = (uint8_t)UNER_ERR_UNSUPPORTED;
        if (dev->wifi_set_ap && n >= 1) {
            uint8_t off = 0;
            const uint8_t ssidLen = pay[off++]; if (ssidLen > 32 || (off+ssidLen) > n) goto ap_fail;
            const uint8_t *ssid = &pay[off]; off += ssidLen;

            if (off >= n) goto ap_fail;
            const uint8_t passLen = pay[off++]; if ((off+passLen) > n) goto ap_fail;
            const uint8_t *pass = &pay[off]; off += passLen;

            if ((off+4) > n) goto ap_fail;
            const uint8_t *ip = &pay[off];

            code = dev->wifi_set_ap(ssid, ssidLen, pass, passLen, ip);
        }
ap_fail: ;
        /* WIFI_ACK: payload (u8 code) ó (u8 cmdRef, u8 code).
           Tu lado TS definió (cmdRef, code), pero el texto habla de solo 'code'.
           Para máxima compatibilidad, enviamos SOLO 'code' como acordaste en la doc. */
        uint8_t p[1] = { code };
        (void)send_frame(dev, (uint8_t)UNER_CMD_WIFI_ACK, p, 1);
    } break;

    case UNER_CMD_WIFI_SET_STA: {
        uint8_t code = (uint8_t)UNER_ERR_UNSUPPORTED;
        if (dev->wifi_set_sta && n >= 1) {
            uint8_t off = 0;
            const uint8_t ssidLen = pay[off++]; if (ssidLen == 0 || ssidLen > 32 || (off+ssidLen) > n) goto sta_fail;
            const uint8_t *ssid = &pay[off]; off += ssidLen;

            if (off >= n) goto sta_fail;
            const uint8_t passLen = pay[off++]; if ((off+passLen) > n) goto sta_fail;
            const uint8_t *pass = &pay[off]; off += passLen;

            if (off >= n) goto sta_fail;
            const uint8_t fixedIp = pay[off++];

            if ((off+4) > n) goto sta_fail;
            const uint8_t *ip = &pay[off];

            code = dev->wifi_set_sta(ssid, ssidLen, pass, passLen, fixedIp, ip);
        }
sta_fail: ;
        uint8_t p[1] = { code };
        (void)send_frame(dev, (uint8_t)UNER_CMD_WIFI_ACK, p, 1);
    } break;

    /* Telemetría */
    case UNER_CMD_TELEM_SET_RATE: {
        if (n < 2) return;
        const uint16_t ms = uner_le16_read(pay);
        dev->telemetry_period_ms = ms;
        dev->telemetry_elapsed_ms = 0;
        /* ACK: (u8 code=0, u16 period_ms) */
        uint8_t p[3] = { (uint8_t)UNER_ERR_OK, 0, 0 };
        uner_le16_write(&p[1], ms);
        (void)send_frame(dev, (uint8_t)UNER_CMD_TELEM_ACK, p, 3);
    } break;

    default:
        /* desconocido: ignorar */
        break;
    }
}

void uner_dev_process_rx(UNER_Device *dev) {
    if (!dev) return;

    while (dev->tail != dev->head) {
        uint8_t b = dev->rx_ring[ dev->tail & (UNER_RX_RING_CAP - 1u) ];
        dev->tail++;

        switch ((P_State)dev->state) {
        case P_WAIT_HEADER: {
            if (b == UNER_HDR[dev->header_prog]) {
                dev->cks_running ^= b; /* incluye header */
                dev->header_prog++;
                if (dev->header_prog >= UNER_HEADER_LENGTH) {
                    dev->header_prog = 0;
                    dev->state = (uint8_t)P_WAIT_LEN;
                }
            } else {
                resync_header(dev, b);
            }
        } break;

        case P_WAIT_LEN:
            dev->len_field = b;
            dev->cks_running ^= b;
            if (dev->len_field < UNER_LENGTH_MIN || dev->len_field > UNER_LENGTH_MAX) {
                resync_header(dev, b);
            } else {
                dev->state = (uint8_t)P_WAIT_TOKEN;
            }
            break;

        case P_WAIT_TOKEN:
            if (b != UNER_TOKEN) {
                resync_header(dev, b);
            } else {
                dev->cks_running ^= b;
                dev->state = (uint8_t)P_WAIT_CMD;
            }
            break;

        case P_WAIT_CMD:
            dev->cmd = b;
            dev->cks_running ^= b;
            dev->expected_payload = (uint8_t)(dev->len_field - 2u);
            dev->pay_idx = 0;
            dev->state = (dev->expected_payload == 0) ? (uint8_t)P_WAIT_CHK
                                                      : (uint8_t)P_WAIT_PAYLOAD;
            break;

        case P_WAIT_PAYLOAD:
            if (dev->pay_idx < UNER_PCK_MAX_PAYLOAD) {
                dev->pay_buf[dev->pay_idx++] = b;
                dev->cks_running ^= b;
                if (dev->pay_idx >= dev->expected_payload) {
                    dev->state = (uint8_t)P_WAIT_CHK;
                }
            } else {
                /* longitud inesperada → resync */
                resync_header(dev, b);
            }
            break;

        case P_WAIT_CHK: {
            const uint8_t chk = b;
            if (chk == dev->cks_running) {
                /* paquete completo y válido */
                handle_cmd(dev, dev->cmd, dev->pay_buf, dev->expected_payload);
            }
            /* Buscar siguiente header (permitir b como potencial 'U') */
            resync_header(dev, b);
        } break;

        default:
            uner_dev_reset_parser(dev);
            break;
        }
    }
}
