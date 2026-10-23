# Dispositivo y dispatch UNER

ID: `esp:module/src/uner_device.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Asocia identidad del dispositivo, comando y handler a los paquetes parseados.

Composición interna de protocolo y tabla de funciones; el efecto pertenece al handler registrado.

<a id="modularity"></a>
## Modularidad observada

Núcleo de protocolo dependiente de core/colas legítimamente, separado del significado de comandos de aplicación.

<a id="state"></a>
## Handle, estado y atributos externos

Instancia, callbacks y contexto conservan identidad/ruta y referencias prestadas; no compartir datos de un socket con otro por usar el mismo dispositivo UART.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Configurar identidad y registro antes de despachar.
- Recibir paquete, resolver handler y conservar semántica de respuesta/error del comando.

<a id="interactions"></a>
## Interacciones

- UNER_App registra handlers; transport serial transmite frames construidos por la capa de protocolo.

<a id="limitations"></a>
## Precauciones y límites

- Un ID registrado no garantiza soporte físico ni equivalencia de metadata F4/ESP.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uner_device.c` | c-cpp-structural | `e67e04e0d3877dea5688a22eba52ed8f65fd21e7f56cf865e9df32b89c8faa79` |
| `src/uner_device.h` | c-cpp-structural | `d9d7c0cf6672ed4f233b0f5e150f44c6e75c134eb14488f6159a175d63ed789a` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNER_WifiNetworkDesc`

`esp:src/uner_device.h:113`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t ssid_len` | Dato de estado/configuración `ssid_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *ssid` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t rssi_dbm` | Dato de estado/configuración `rssi_dbm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t security` | Dato de estado/configuración `security`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_Device`

`esp:src/uner_device.h:122`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t rx_ring[UNER_RX_RING_CAP]` | Dato de estado/configuración `rx_ring`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t head` | Dato de estado/configuración `head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t tail` | Dato de estado/configuración `tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t header_prog` | Dato de estado/configuración `header_prog`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t len_field` | Dato de estado/configuración `len_field`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t expected_payload` | Dato de estado/configuración `expected_payload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pay_idx` | Dato de estado/configuración `pay_idx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cks_running` | Dato de estado/configuración `cks_running`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pay_buf[UNER_PCK_MAX_PAYLOAD]` | Dato de estado/configuración `pay_buf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_send_bytes_fn send_bytes` | Dato de estado/configuración `send_bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_wifi_get_mode_fn wifi_get_mode` | Dato de estado/configuración `wifi_get_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_wifi_request_scan_fn wifi_request_scan` | Dato de estado/configuración `wifi_request_scan`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_wifi_set_ap_fn wifi_set_ap` | Dato de estado/configuración `wifi_set_ap`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_wifi_set_sta_fn wifi_set_sta` | Dato de estado/configuración `wifi_set_sta`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uner_telemetry_fill_fn telemetry_fill` | Dato de estado/configuración `telemetry_fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t heartbeat_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t heartbeat_elapsed_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t telemetry_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t telemetry_elapsed_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t telemetry_seq` | Dato de estado/configuración `telemetry_seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool telemetry_due` | Dato de estado/configuración `telemetry_due`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool last_cmd_valid` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_cmd` | Memoria de la observación anterior usada por la máquina de estados. |

### `uner_send_bytes_fn`

`esp:src/uner_device.h:82`

Callback: `uint8_t (*uner_send_bytes_fn)(const uint8_t *data, uint16_t len)`. Puerto de envío de bytes de la fachada histórica UNER; devuelve resultado del transporte según firma y usa contexto prestado. No acredita entrega/respuesta del destino.

### `uner_wifi_get_mode_fn`

`esp:src/uner_device.h:87`

Callback: `uint8_t (*uner_wifi_get_mode_fn)(void)`. Consulta modo Wi-Fi mediante contexto inyectado al dispositivo histórico, sin acoplar parser al SDK.

### `uner_wifi_request_scan_fn`

`esp:src/uner_device.h:90`

Callback: `void (*uner_wifi_request_scan_fn)(void)`. Solicita scan al backend desde un comando de la fachada histórica; el resultado requiere el flujo de scan.

### `uner_wifi_set_ap_fn`

`esp:src/uner_device.h:94`

Callback: `uint8_t (*uner_wifi_set_ap_fn)( const uint8_t *ssid, uint8_t ssid_len, const uint8_t *pass, uint8_t pass_len, const uint8_t ip[4])`. Delega configuración/inicio AP al adaptador Wi-Fi de la fachada histórica.

### `uner_wifi_set_sta_fn`

`esp:src/uner_device.h:100`

Callback: `uint8_t (*uner_wifi_set_sta_fn)( const uint8_t *ssid, uint8_t ssid_len, const uint8_t *pass, uint8_t pass_len, uint8_t fixed_ip, const uint8_t ip[4])`. Delega configuración/conexión STA al adaptador Wi-Fi de la fachada histórica.

### `uner_telemetry_fill_fn`

`esp:src/uner_device.h:106`

Callback: `bool (*uner_telemetry_fill_fn)(uint8_t *out_payload, uint8_t *out_len)`. El dispositivo pide al proveedor rellenar datos de telemetría antes de responder; respetar capacidad de buffer y retorno.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-uner_dev_init"></a>`void uner_dev_init(UNER_Device *dev, uner_send_bytes_fn tx_fn, uner_wifi_get_mode_fn get_mode_fn, uner_wifi_request_scan_fn req_scan_fn, uner_wifi_set_ap_fn set_ap_fn, uner_wifi_set_sta_fn set_sta_fn, uner_telemetry_fill_fn tlm_fill_fn)` | Operación uner dev init de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.c:55`; inferred-name |
| <a id="fn-uner_dev_process_rx"></a>`void uner_dev_process_rx(UNER_Device *dev)` | Coordina `handle_cmd`, `resync_header`, `uner_dev_reset_parser` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:358`; structural |
| <a id="fn-uner_dev_push_byte"></a>`UNER_Status uner_dev_push_byte(UNER_Device *dev, uint8_t b)` | Coordina `ring_count` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:109`; structural |
| <a id="fn-uner_dev_push_bytes"></a>`UNER_Status uner_dev_push_bytes(UNER_Device *dev, const uint8_t *data, size_t len)` | Adapta la llamada a `uner_dev_push_byte` con el contexto de esta integración. | `esp:src/uner_device.c:123`; structural |
| <a id="fn-uner_dev_reset_parser"></a>`void uner_dev_reset_parser(UNER_Device *dev)` | Operación uner dev reset parser de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.c:93`; inferred-name |
| <a id="fn-uner_dev_send_heartbeat_beat"></a>`UNER_Status uner_dev_send_heartbeat_beat(UNER_Device *dev)` | Coordina `send_frame`, `uner_le16_write` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:182`; structural |
| <a id="fn-uner_dev_send_telemetry_mpu6050"></a>`UNER_Status uner_dev_send_telemetry_mpu6050(UNER_Device *dev, uint16_t seq, int16_t accX, int16_t accY, int16_t accZ, int16_t gyroX, int16_t gyroY, int16_t gyroZ, int16_t tempRaw)` | Coordina `send_frame`, `uner_le16_write` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:194`; structural |
| <a id="fn-uner_dev_send_wifi_mode"></a>`UNER_Status uner_dev_send_wifi_mode(UNER_Device *dev, uint8_t mode)` | Adapta la llamada a `send_frame` con el contexto de esta integración. | `esp:src/uner_device.c:145`; structural |
| <a id="fn-uner_dev_send_wifi_scan_list"></a>`uint8_t uner_dev_send_wifi_scan_list(UNER_Device *dev, const UNER_WifiNetworkDesc *list, uint8_t n)` | Coordina `send_frame` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:150`; structural |
| <a id="fn-uner_dev_set_telemetry_rate"></a>`void uner_dev_set_telemetry_rate(UNER_Device *dev, uint16_t period_ms)` | Operación uner dev set telemetry rate de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.c:188`; inferred-name |
| <a id="fn-uner_dev_tick"></a>`void uner_dev_tick(UNER_Device *dev, uint16_t elapsed_ms)` | Coordina `send_frame`, `telemetry_fill`, `uner_dev_send_heartbeat_beat` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:222`; structural |
| <a id="fn-uner_le16_read"></a>`static inline uint16_t uner_le16_read(const uint8_t *p)` | Operación uner le16 read de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.h:208`; inferred-name |
| <a id="fn-uner_le16_write"></a>`static inline void uner_le16_write(uint8_t *p, uint16_t v)` | Operación uner le16 write de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.h:211`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-handle_cmd"></a>`static void handle_cmd(UNER_Device *dev, uint8_t cmd, const uint8_t *pay, uint8_t n)` | Coordina `send_frame`, `uner_dev_send_wifi_mode`, `uner_le16_read` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:264`; structural |
| <a id="fn-resync_header"></a>`static void resync_header(UNER_Device *dev, uint8_t b)` | Adapta la llamada a `uner_dev_reset_parser` con el contexto de esta integración. | `esp:src/uner_device.c:134`; structural |
| <a id="fn-ring_count"></a>`static inline uint16_t ring_count(const UNER_Device *d)` | Operación ring count de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.c:105`; inferred-name |
| <a id="fn-send_frame"></a>`static UNER_Status send_frame(UNER_Device *dev, uint8_t cmd, const uint8_t *payload, uint8_t pay_len)` | Coordina `send_bytes`, `xor_cksum` para Dispositivo y dispatch UNER. | `esp:src/uner_device.c:23`; structural |
| <a id="fn-xor_cksum"></a>`static uint8_t xor_cksum(const uint8_t *p, size_t len)` | Operación xor cksum de Dispositivo y dispatch UNER; semántica fina pendiente de ampliación. | `esp:src/uner_device.c:16`; inferred-name |
