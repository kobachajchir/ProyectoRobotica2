# Fachada y compatibilidad UNER ESP

ID: `esp:module/src/uner_handle.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Coordina parser, dispositivo, transporte e interfaces históricas de aplicación ESP.

Handle y wrappers de compatibilidad, recepción/envío y extensión de hooks/interceptores.

<a id="modularity"></a>
## Modularidad observada

Integra capas del mismo protocolo y adaptación C/C++; los wrappers conservan acoplamientos históricos del ESP.

<a id="state"></a>
## Handle, estado y atributos externos

Conserva recursos internos de protocolo y referencias a callbacks/transportes; la fachada histórica no debe interpretarse como API idéntica a UNER_Handle F4.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Conectar servicios/transportes e inicializar antes de atender loop.
- Al recibir se notifican/interceptan paquetes y se despacha al dueño; enviar conserva src/dst del solicitante.

<a id="interactions"></a>
## Interacciones

- El bridge JSON usa backend/interceptor; handlers implementan comandos propios del ESP.
- TransactionManager conserva requestId fuera del frame, no dentro del parser.

<a id="limitations"></a>
## Precauciones y límites

- Revisar memoria prestada al guardar un interceptor o diferir payloads.
- Callbacks sin contexto explícito pueden conservar una instancia global activa.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/UNERUtils.cpp` | c-cpp-structural | `ac3927fe98c8a79363d558e596ed0c7d2418c6269b6be1d3089b3fcda8a3e47f` |
| `src/UNERUtils.h` | c-cpp-structural | `42a44e610d9bee290f825b7b5349f55d62f34bf0e4348a68280d83a361921191` |
| `src/uner_handle.cpp` | c-cpp-structural | `88b1c206af6b8143d3d1b0ce60022ebba0fdb48bd5ede1895cc73663e3b786ad` |
| `src/uner_handle.h` | c-cpp-structural | `f2bb53f03c69310d3ebe012b25809c39111918b7cd8f410397edcb2280bed47b` |
| `src/uner_handle_compat.cpp` | c-cpp-structural | `ba7995ec766eb290bafdd0034e3465f9b70de7893418eb0c6564322404e39628` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNER_Transport`

`esp:src/uner_handle.h:19`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t max_payload` | Dato de estado/configuración `max_payload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_TransportPollRxFn poll_rx` | Dato de estado/configuración `poll_rx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_TransportTrySendFn try_send` | Dato de estado/configuración `try_send`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_HandleOps`

`esp:src/uner_handle.h:30`

| Atributo / firma | Propósito |
| --- | --- |
| `UNER_NowMsFn now_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `void *clock_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_PacketLogFn log_packet` | Dato de estado/configuración `log_packet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *log_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |

### `UNER_Handle`

`esp:src/uner_handle.h:37`

| Atributo / firma | Propósito |
| --- | --- |
| `UNER_NowMsFn now_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `void *clock_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_PacketLogFn log_packet` | Dato de estado/configuración `log_packet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *log_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_Core core` | Dato de estado/configuración `core`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_Transport *transports[UNER_MAX_TRANSPORTS]` | Dato de estado/configuración `transports`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_count` | Dato de estado/configuración `transport_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const UNER_CommandSpec *command_table` | Dato de estado/configuración `command_table`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t command_count` | Dato de estado/configuración `command_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *app_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_PacketInterceptorFn packet_interceptor` | Dato de estado/configuración `packet_interceptor`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *packet_interceptor_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_Packet rx_slots[UNER_RX_SLOTS]` | Dato de estado/configuración `rx_slots`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t rx_payload_pool[UNER_RX_SLOTS * UNER_MAX_PAYLOAD]` | Dato de estado/configuración `rx_payload_pool`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_buf[UNER_MAX_FRAME]` | Buffer de construcción de frames de esta instancia; no conservar una referencia tras el siguiente envío. |
| `uint8_t tx_q_data[UNER_TX_SLOTS][UNER_MAX_FRAME]` | Dato de estado/configuración `tx_q_data`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t tx_q_len[UNER_TX_SLOTS]` | Dato de estado/configuración `tx_q_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_q_head` | Dato de estado/configuración `tx_q_head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_q_tail` | Dato de estado/configuración `tx_q_tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_q_count` | Dato de estado/configuración `tx_q_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t flag_1ms` | Dato de estado/configuración `flag_1ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t flag_10ms` | Dato de estado/configuración `flag_10ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t flag_50ms` | Dato de estado/configuración `flag_50ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t flag_100ms` | Dato de estado/configuración `flag_100ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t tick10` | Dato de estado/configuración `tick10`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tick50` | Dato de estado/configuración `tick50`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tick100` | Dato de estado/configuración `tick100`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_TransportPollRxFn`

`esp:src/uner_handle.h:14`

Callback: `uint16_t (*UNER_TransportPollRxFn)(void *ctx, uint8_t *out, uint16_t max_len)`. El servicio del handle solicita bytes RX al transporte registrado; escribir en buffer prestado hasta su capacidad, devolver longitud/estado conforme a firma.

### `UNER_TransportTrySendFn`

`esp:src/uner_handle.h:16`

Callback: `int (*UNER_TransportTrySendFn)(void *ctx, const uint8_t *data, uint16_t len)`. El handle ofrece bytes TX al transporte; el puerto indica admisión/busy/error según retorno. Su contexto debe durar mientras está registrado.

### `UNER_PacketInterceptorFn`

`esp:src/uner_handle.h:17`

Callback: `uint8_t (*UNER_PacketInterceptorFn)(void *ctx, const UNER_Packet *packet)`. El handle consulta intercepción antes del despacho normal para que el bridge/broker capture respuestas; resultado decide consumo según firma. No retener puntero de paquete temporal.

### `UNER_NowMsFn`

`esp:src/uner_handle.h:26`

Callback: `uint32_t (*UNER_NowMsFn)(void *ctx)`. El handle invoca reloj de plataforma en ms para vencimientos; registro no transfiere propiedad del contexto.

### `UNER_PacketLogFn`

`esp:src/uner_handle.h:28`

Callback: `void (*UNER_PacketLogFn)(void *ctx, const UNER_Packet *packet)`. Hook de diagnóstico para observar paquete/dirección según firma; no mutar buffer ni retener vistas; no registrar secretos.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-unerutils_getdevice"></a>`UNER_Device *UNERUtils_GetDevice()` | Consulta Device dentro de Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:237`; inferred-name |
| <a id="fn-unerutils_init"></a>`void UNERUtils_Init(AsyncWebSocket &ws)` | Adapta la llamada a `uner_dev_init` con el contexto de esta integración. | `esp:src/UNERUtils.cpp:126`; structural |
| <a id="fn-unerutils_loop"></a>`void UNERUtils_Loop()` | Coordina `RSSI`, `SSID`, `defined` para Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:155`; structural |
| <a id="fn-unerutils_onwsbinary"></a>`void UNERUtils_OnWSBinary(AsyncWebSocketClient *client, AwsFrameInfo *info, uint8_t *data, size_t len)` | Atiende la notificación de WSBinary dentro de Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:139`; inferred-name |
| <a id="fn-unerutils_setwifimode"></a>`void UNERUtils_SetWifiMode(uint8_t modeAP0_STA1)` | Configura Wifi Mode dentro de Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:232`; inferred-name |
| <a id="fn-uner_handle_addtransport"></a>`UNER_Status UNER_Handle_AddTransport(UNER_Handle *h, UNER_Transport *t)` | Procesa Add Transport dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:29`; inferred-name |
| <a id="fn-uner_handle_dispatch"></a>`void UNER_Handle_Dispatch(UNER_Handle *h)` | Procesa Dispatch dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:65`; inferred-name |
| <a id="fn-uner_handle_init"></a>`UNER_Status UNER_Handle_Init(UNER_Handle *h, const UNER_CoreConfig *cfg, const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx)` | Procesa Init dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle_compat.cpp:6`; inferred-name |
| <a id="fn-uner_handle_initwithops"></a>`UNER_Status UNER_Handle_InitWithOps(UNER_Handle *h, const UNER_CoreConfig *cfg, const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx, const UNER_HandleOps *ops)` | Procesa Init With Ops dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:13`; inferred-name |
| <a id="fn-uner_handle_pollrx"></a>`void UNER_Handle_PollRx(UNER_Handle *h)` | Procesa Poll Rx dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:53`; inferred-name |
| <a id="fn-uner_handle_sendack"></a>`UNER_Status UNER_Handle_SendAck(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t acked_cmd, uint8_t status)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_handle.cpp:133`; structural |
| <a id="fn-uner_handle_sendnack"></a>`UNER_Status UNER_Handle_SendNack(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, uint8_t reason)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_handle.cpp:138`; structural |
| <a id="fn-uner_handle_sendraw"></a>`UNER_Status UNER_Handle_SendRaw(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Procesa Send Raw dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:125`; inferred-name |
| <a id="fn-uner_handle_sendresponse"></a>`UNER_Status UNER_Handle_SendResponse(UNER_Handle *h, const UNER_Packet *req, uint8_t status, const uint8_t *data, uint8_t data_len)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_handle.cpp:143`; structural |
| <a id="fn-uner_handle_setclock"></a>`void UNER_Handle_SetClock(UNER_Handle *h, UNER_NowMsFn fn, void *ctx)` | Procesa Set Clock dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:42`; inferred-name |
| <a id="fn-uner_handle_setpacketinterceptor"></a>`void UNER_Handle_SetPacketInterceptor(UNER_Handle *h, UNER_PacketInterceptorFn fn, void *ctx)` | Procesa Set Packet Interceptor dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:36`; inferred-name |
| <a id="fn-uner_handle_setpacketlogger"></a>`void UNER_Handle_SetPacketLogger(UNER_Handle *h, UNER_PacketLogFn fn, void *ctx)` | Procesa Set Packet Logger dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:47`; inferred-name |
| <a id="fn-uner_handle_tick"></a>`void UNER_Handle_Tick(UNER_Handle *h)` | Procesa Tick dentro de Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:109`; inferred-name |
| <a id="fn-uner_handle_txpump"></a>`void UNER_Handle_TxPump(UNER_Handle *h)` | Adapta la llamada a `try_send` con el contexto de esta integración. | `esp:src/uner_handle.cpp:100`; structural |
| <a id="fn-apip"></a>`IPAddress apIP(ip[0], ip[1], ip[2], ip[3])` | Operación ap IP de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:60`; inferred-name |
| <a id="fn-gw"></a>`IPAddress gw(ip[0], ip[1], ip[2], 1)` | Operación gw de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:61`; inferred-name |
| <a id="fn-local"></a>`IPAddress local(ip[0], ip[1], ip[2], ip[3])` | Operación local de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:97`; inferred-name |
| <a id="fn-mask"></a>`IPAddress mask(255, 255, 255, 0)` | Operación mask de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:62`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-hal_telemetry_fill"></a>`static bool hal_telemetry_fill(uint8_t *out_payload, uint8_t *out_len)` | Operación hal telemetry fill de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:117`; inferred-name |
| <a id="fn-hal_uner_send_bytes"></a>`static uint8_t hal_uner_send_bytes(const uint8_t *data, uint16_t len)` | Coordina `binary`, `binaryAll`, `status` para Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:17`; structural |
| <a id="fn-hal_wifi_get_mode"></a>`static uint8_t hal_wifi_get_mode(void)` | Operación hal wifi get mode de Fachada y compatibilidad UNER ESP; semántica fina pendiente de ampliación. | `esp:src/UNERUtils.cpp:35`; inferred-name |
| <a id="fn-hal_wifi_request_scan"></a>`static void hal_wifi_request_scan(void)` | Coordina `scanDelete`, `scanNetworks` para Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:41`; structural |
| <a id="fn-hal_wifi_set_ap"></a>`static uint8_t hal_wifi_set_ap(const uint8_t *ssid, uint8_t ssid_len, const uint8_t *pass, uint8_t pass_len, const uint8_t ip[4])` | Coordina `IPAddress`, `String`, `apIP` para Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:48`; structural |
| <a id="fn-hal_wifi_set_sta"></a>`static uint8_t hal_wifi_set_sta(const uint8_t *ssid, uint8_t ssid_len, const uint8_t *pass, uint8_t pass_len, uint8_t fixed_ip, const uint8_t ip[4])` | Coordina `String`, `begin`, `c_str` para Fachada y compatibilidad UNER ESP. | `esp:src/UNERUtils.cpp:80`; structural |
| <a id="fn-tx_enqueue"></a>`static uint8_t tx_enqueue(UNER_Handle *h, const uint8_t *frame, uint16_t len)` | Coordina `memcpy` para Fachada y compatibilidad UNER ESP. | `esp:src/uner_handle.cpp:4`; structural |
