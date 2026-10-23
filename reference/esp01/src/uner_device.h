#ifndef UNER_DEVICE_H
#define UNER_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* =================== Protocolo y límites =================== */

#define UNER_PCK_MAX_PAYLOAD     30u
#define UNER_PCK_OVERHEAD        8u  /* 4 hdr + 1 len + 1 token + 1 cmd + 1 chk */
#define UNER_PCK_MAX_TOTAL       (UNER_PCK_OVERHEAD + UNER_PCK_MAX_PAYLOAD) /* 38 */
#define UNER_HEADER_LENGTH       4u
#define UNER_TOKEN               0x3Au /* ':' */
#define UNER_LENGTH_MIN          2u    /* 1 CMD + 0 PAY + 1 CHK */
#define UNER_LENGTH_MAX          32u   /* 1 CMD + 30 PAY + 1 CHK */

#define UNER_RX_RING_CAP         256u  /* Ajustable (potencia de 2 recomendado) */

/* =================== Comandos =================== */

/* Heartbeat */
typedef enum {
    UNER_CMD_HEARTBEAT_SET  = 0xA0,
    UNER_CMD_HEARTBEAT_ACK  = 0xA1,
    UNER_CMD_HEARTBEAT_BEAT = 0xA2,
    /* Wi-Fi */
    UNER_CMD_WIFI_GET_MODE  = 0x10,
    UNER_CMD_WIFI_MODE      = 0x11,
    UNER_CMD_WIFI_GET_SCAN  = 0x12,
    UNER_CMD_WIFI_SCAN_LIST = 0x13,
    UNER_CMD_WIFI_SET_AP    = 0x14,
    UNER_CMD_WIFI_SET_STA   = 0x15,
    UNER_CMD_WIFI_ACK       = 0x16,
    /* Telemetría */
    UNER_CMD_TELEM_SET_RATE = 0x20,
    UNER_CMD_TELEM_ACK      = 0x21,
    UNER_CMD_TELEM_DATA     = 0x22
} UNER_Command;

/* Modos Wi-Fi */
typedef enum {
    UNER_WIFI_AP = 0,
    UNER_WIFI_STATION = 1
} UNER_WifiMode;

/* Seguridad Wi-Fi (para WIFI_SCAN_LIST.sec) */
typedef enum {
    UNER_WIFI_SEC_OPEN = 0,
    UNER_WIFI_SEC_WEP = 1,
    UNER_WIFI_SEC_WPA_PSK = 2,
    UNER_WIFI_SEC_WPA2_PSK = 3,
    UNER_WIFI_SEC_WPA3_PERSONAL = 4,
    UNER_WIFI_SEC_WPA_WPA2_MIXED = 5
} UNER_WifiSecurity;

/* Códigos de error (WIFI_ACK / TELEMETRY_ACK.code) */
typedef enum {
    UNER_ERR_OK          = 0,
    UNER_ERR_ARG         = 1,
    UNER_ERR_UNSUPPORTED = 2,
    UNER_ERR_BUSY        = 3,
    UNER_ERR_SCAN_FAIL   = 4,
    UNER_ERR_AUTH_FAIL   = 5,
    UNER_ERR_SSID_NOT_FOUND = 6,
    UNER_ERR_TIMEOUT     = 7,
    UNER_ERR_APPLY_FAIL  = 8
} UNER_ErrorCode;

/* =================== Tipos de retorno =================== */

typedef int8_t UNER_Status;
#define UNER_STATUS_OK                 0
#define UNER_STATUS_ERR_NULL_PTR      -1
#define UNER_STATUS_ERR_RX_OVERFLOW   -2
#define UNER_STATUS_ERR_RANGE         -3
#define UNER_STATUS_ERR_TX_FAIL       -4

/* =================== Callbacks HAL / Proveedor =================== */

/* TX: debe enviar 'len' bytes por el medio físico (UART/USB/WS). Devuelve 0 si OK. */
typedef uint8_t (*uner_send_bytes_fn)(const uint8_t *data, uint16_t len);

/* Wi-Fi: retorna el modo actual (0=AP,1=STATION). */
typedef uint8_t (*uner_wifi_get_mode_fn)(void);

/* Wi-Fi: dispara un escaneo asíncrono. Cuando el escaneo termine,
   el app debe llamar a uner_dev_send_wifi_scan_list(). */
typedef void (*uner_wifi_request_scan_fn)(void);

/* Wi-Fi: aplicar credenciales AP. Devuelve UNER_ErrorCode. */
typedef uint8_t (*uner_wifi_set_ap_fn)(
    const uint8_t *ssid, uint8_t ssid_len,
    const uint8_t *pass, uint8_t pass_len,
    const uint8_t ip[4]);

/* Wi-Fi: aplicar credenciales STA. Devuelve UNER_ErrorCode. */
typedef uint8_t (*uner_wifi_set_sta_fn)(
    const uint8_t *ssid, uint8_t ssid_len,
    const uint8_t *pass, uint8_t pass_len,
    uint8_t fixed_ip, const uint8_t ip[4]);

/* Telemetría (opcional): si se provee, la librería la invoca para armar y enviar
   TELEMETRY_DATA automáticamente al ritmo configurado. Debe devolver true si envió algo. */
typedef bool (*uner_telemetry_fill_fn)(uint8_t *out_payload, uint8_t *out_len);

/* =================== Estructuras auxiliares =================== */

typedef struct {
    uint8_t ssid_len;
    const uint8_t *ssid;    /* UTF-8, sin terminador */
    int8_t  rssi_dbm;       /* signed dBm */
    uint8_t security;       /* UNER_WifiSecurity */
} UNER_WifiNetworkDesc;

/* =================== Estado principal del dispositivo =================== */

typedef struct {
    /* RX ring */
    uint8_t  rx_ring[UNER_RX_RING_CAP];
    uint16_t head;
    uint16_t tail;

    /* Parser */
    uint8_t  state;          /* enum interno */
    uint8_t  header_prog;
    uint8_t  len_field;      /* LENGTH (1 CMD + N + 1 CHK) */
    uint8_t  expected_payload;
    uint8_t  cmd;
    uint8_t  pay_idx;
    uint8_t  cks_running;
    uint8_t  pay_buf[UNER_PCK_MAX_PAYLOAD];

    /* Callbacks HAL / proveedores */
    uner_send_bytes_fn       send_bytes;
    uner_wifi_get_mode_fn    wifi_get_mode;
    uner_wifi_request_scan_fn wifi_request_scan;
    uner_wifi_set_ap_fn      wifi_set_ap;
    uner_wifi_set_sta_fn     wifi_set_sta;
    uner_telemetry_fill_fn   telemetry_fill; /* opcional */

    /* Timers/periodicidad (gestionados por uner_dev_tick) */
    uint16_t heartbeat_period_ms;
    uint16_t heartbeat_elapsed_ms;

    uint16_t telemetry_period_ms;
    uint16_t telemetry_elapsed_ms;
    uint16_t telemetry_seq;

    /* Flags de utilidad para la app */
    bool telemetry_due;      /* true cuando corresponde enviar telemetría (si no hay callback) */
    bool last_cmd_valid;
    uint8_t last_cmd;        /* último CMD recibido del host */
} UNER_Device;

/* =================== API pública =================== */

/* Init: setea callbacks (los no usados pueden ser NULL). */
void uner_dev_init(UNER_Device *dev,
                   uner_send_bytes_fn tx_fn,
                   uner_wifi_get_mode_fn get_mode_fn,
                   uner_wifi_request_scan_fn req_scan_fn,
                   uner_wifi_set_ap_fn set_ap_fn,
                   uner_wifi_set_sta_fn set_sta_fn,
                   uner_telemetry_fill_fn tlm_fill_fn);

/* Reinicia solo la máquina de estados del parser. */
void uner_dev_reset_parser(UNER_Device *dev);

/* Inserta bytes crudos al ring. Devuelve 0 si OK, negativo si overflow. */
UNER_Status uner_dev_push_byte(UNER_Device *dev, uint8_t b);
UNER_Status uner_dev_push_bytes(UNER_Device *dev, const uint8_t *data, size_t len);

/* Procesa el ring (consume y responde a comandos). */
void uner_dev_process_rx(UNER_Device *dev);

/* Debe llamarse periódicamente con delta en ms (ej.: desde SysTick/loop). */
void uner_dev_tick(UNER_Device *dev, uint16_t elapsed_ms);

/* ------------ Funciones de envío (desde app/firmware) ------------ */

/* Enviar WIFI_MODE (respuesta a GET_MODE). Si tenés get_mode callback, no necesitás llamarla. */
UNER_Status uner_dev_send_wifi_mode(UNER_Device *dev, uint8_t mode);

/* Enviar WIFI_SCAN_LIST con hasta 'n' redes. Devuelve cuántas redes efectivamente empaquetó. */
uint8_t uner_dev_send_wifi_scan_list(UNER_Device *dev,
                                     const UNER_WifiNetworkDesc *list,
                                     uint8_t n);

/* Enviar HEARTBEAT_BEAT manual (la lib también lo hace automático si period_ms>0). */
UNER_Status uner_dev_send_heartbeat_beat(UNER_Device *dev);

/* Configurar tasa de telemetría (desde app). Esto es lo mismo que recibir TELEMETRY_SET_RATE. */
void uner_dev_set_telemetry_rate(UNER_Device *dev, uint16_t period_ms);

/* Enviar TELEMETRY_DATA (schema=0x01: MPU6050 int16). Limpia telemetry_due. */
UNER_Status uner_dev_send_telemetry_mpu6050(UNER_Device *dev,
                                            uint16_t seq,
                                            int16_t accX, int16_t accY, int16_t accZ,
                                            int16_t gyroX, int16_t gyroY, int16_t gyroZ,
                                            int16_t tempRaw);

/* Utils LE16 */
static inline uint16_t uner_le16_read(const uint8_t *p) {
    return (uint16_t)( (uint16_t)p[0] | ((uint16_t)p[1] << 8) );
}
static inline void uner_le16_write(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF); p[1] = (uint8_t)((v >> 8) & 0xFF);
}

#ifdef __cplusplus
}
#endif
#endif /* UNER_DEVICE_H */
