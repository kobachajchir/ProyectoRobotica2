# Bridge Wi-Fi y backends UNER/AT

ID: `f4:module/Core/Inc/wifi_bridge.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Ofrece operaciones de conectividad sobre el backend Web/UNER o un módem con firmware ESP-AT.

Detección de backend, UART y conectividad, exploración/red, capacidades, eventos y encaminamiento de operaciones. ESP-AT es una alternativa de firmware de módem; no es el mismo servidor Web de este proyecto.

<a id="modularity"></a>
## Modularidad observada

UART/reloj y UNER de aplicación se inyectan por WifiBridge_Hardware/Services. Persisten SystemHandle y tipos de aplicación en el contrato. Los backends comparten una fachada, no implementaciones idénticas.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

WifiBridge_Handle conserva contexto de hardware/servicios, backend, recepción/transmisión, temporizadores y estado de red. Las credenciales runtime son diferentes de un registro de preferencias. hardware y services son prestados; callbacks devuelven los eventos al integrador.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init/Start configura y detecta backend; Process/RxCheck atiende máquina de estados y recepción.
- Callbacks IDLE, half/full RX, TX y error notifican actividad; el planificador completa el trabajo diferido.
- Backend AT procesa comandos, prompt, +IPD y SEND OK/FAIL; QueueFrame puede esperar CIPSEND y fin de envío.
- Alive y sus respuestas actualizan disponibilidad; conectividad de red no demuestra una sesión Web autenticada.

## Trabajo en tándem

- Backend UNER delega en UNER_App por services; backend AT transporta frames por su servicio TCP/UDP.
- AP, permisos y UI reciben eventos/estados desde la composición de la aplicación.

## Precauciones y límites

- Firmware ESP de este repositorio no debe describirse como intérprete AT.
- Capacidad anunciada, backend elegido, socket abierto, frame aceptado y respuesta final son hitos distintos.
- No hay tráfico físico demostrado por esta documentación.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/wifi_bridge.h` | `wifi_bridge_types.h` | `344b4327fd45da23c92daa4068fdd31e5f8dc5e756217dcf4452a2be743f22d7` |
| `Core/Inc/wifi_bridge_types.h` | `wifi_bridge_port.h`, `wifi_bridge_config.h`, `stdbool.h`, `stddef.h`, `stdint.h` | `2aba0dc8b46c0298b9d2dea7860d6b8afcc65182136cd1f44c47cd355cb2d9bb` |
| `Core/Inc/wifi_bridge_port.h` | `am_port.h`, `system.h`, `app_ap_config_data.h`, `binary_asset_types.h`, `uner_v2.h`, `uner_status.h` | `cb4fca0e61ce93df11b087e37d43cae7cc86c0d1e55560c898050fd6bce87f15` |
| `Core/Inc/wifi_bridge_config.h` |  | `d1103e933d06cbcf259d693bc0e199e1dfeff58fe06f2cb7a571f9e4b2dd8b36` |
| `Core/Inc/wifi_bridge_uner.h` | `wifi_bridge_types.h` | `3009d73ca4c37f78dab056a1e21dc586b9d40495ad3fda8785b914b57c54e708` |
| `Core/Inc/wifi_bridge_at.h` | `wifi_bridge_types.h` | `3e18f4c656923057bdb49776b074e45f33bd909f74599ca6db4c9b2e3b40831e` |
| `Core/Src/wifi_bridge.c` | `system_flags.h`, `utils/macros_utils.h`, `wifi_bridge.h`, `string.h`, `wifi_bridge_at.h`, `wifi_bridge_uner.h` | `d915e6c87e93fca0828759d7523c61d5ea9fa3d9aff040d0686bc73f2da81ade` |
| `Core/Src/wifi_bridge_uner.c` | `utils/macros_utils.h`, `wifi_bridge_uner.h`, `wifi_bridge.h`, `string.h` | `2d51b5df5c01a743defb801ef7638c5b366cbc2f3c6fe2f1a8b66260cc181c13` |
| `Core/Src/wifi_bridge_at.c` | `system_flags.h`, `utils/macros_utils.h`, `wifi_bridge_at.h`, `wifi_bridge.h`, `string.h`, `uner_v2.h` | `0add11ca300e375b946aa05c1baec815fb3753eb2822e533985ae43b14bf2edc` |
| `Core/Src/wifi_bridge_stub.c` |  | `306f4d75a362ab64410a86458c87ccbe4389f172a6d61bd59e6a464ed622a8e2` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `WifiBridge_Event`

`f4:Core/Inc/wifi_bridge_types.h:57`

| Atributo / firma | Función y precaución |
| --- | --- |
| `WifiBridge_EventId id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t connected` | Dato de estado/configuración `connected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t got_ip` | Dato de estado/configuración `got_ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ip[WIFI_BRIDGE_IP_LEN]` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `WifiBridge_Flags`

`f4:Core/Inc/wifi_bridge_types.h:67`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t raw` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } bits` | Dato de estado/configuración `bits`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bits`

`f4:Core/Inc/wifi_bridge_types.h:69`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t busy : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_busy : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t rx_pending : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t got_ok : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t got_error : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t got_ip : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t connected : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reserved : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_BinaryAssetView`

`f4:Core/Inc/wifi_bridge_types.h:95`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t asset_id` | Dato de estado/configuración `asset_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t width` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t height` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *data` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_CredentialFlags`

`f4:Core/Inc/wifi_bridge_types.h:103`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t raw` | Dato de estado/configuración `raw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } bits` | Dato de estado/configuración `bits`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bits`

`f4:Core/Inc/wifi_bridge_types.h:105`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t ssid_set : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t password_set : 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reserved : 6` | Dato de estado/configuración `6`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_Credentials`

`f4:Core/Inc/wifi_bridge_types.h:112`

| Atributo / firma | Función y precaución |
| --- | --- |
| `WifiBridge_CredentialFlags flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char password[WIFI_BRIDGE_PASS_MAX_LEN + 1u]` | Dato de estado/configuración `password`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_AT_NetworkCounters`

`f4:Core/Inc/wifi_bridge_types.h:194`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t at_timeouts` | Dato de estado/configuración `at_timeouts`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t send_failures` | Dato de estado/configuración `send_failures`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t queue_drops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t ipd_header_errors` | Dato de estado/configuración `ipd_header_errors`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t udp_datagram_drops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t rejected_peers` | Dato de estado/configuración `rejected_peers`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_Handle`

`f4:Core/Inc/wifi_bridge_types.h:203`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *huart` | Dato de estado/configuración `huart`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const WifiBridge_Hardware *hardware` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `const WifiBridge_Services *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `void *services_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `SystemHandle *system` | Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación. |
| `WifiBridge_Credentials *credentials` | Dato de estado/configuración `credentials`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_Flags flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `WifiBridge_EventCallback on_event` | Dato de estado/configuración `on_event`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_Event last_event` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t ip[WIFI_BRIDGE_IP_LEN]` | Dato de estado/configuración `ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t rx_dma[WIFI_BRIDGE_UART_RX_DMA_SIZE]` | Dato de estado/configuración `rx_dma`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t rx_started` | Dato de estado/configuración `rx_started`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t tx_busy` | Dato de estado/configuración `tx_busy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_Backend active_backend` | Dato de estado/configuración `active_backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_Probe probe` | Dato de estado/configuración `probe`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_probe_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t last_rx_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t alive_pending` | Dato de estado/configuración `alive_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t missed_alive_count` | Dato de estado/configuración `missed_alive_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t connection_lost_event` | Dato de estado/configuración `connection_lost_event`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_rx_last_pos` | Dato de estado/configuración `at_rx_last_pos`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char at_line[WIFI_BRIDGE_AT_LINE_SIZE]` | Dato de estado/configuración `at_line`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_line_len` | Dato de estado/configuración `at_line_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char at_tx_buf[WIFI_BRIDGE_AT_TX_SIZE]` | Dato de estado/configuración `at_tx_buf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_tx_len` | Dato de estado/configuración `at_tx_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t at_deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `WifiBridge_AT_Operation at_active_op` | Dato de estado/configuración `at_active_op`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_scan_count` | Dato de estado/configuración `at_scan_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_status_sta_ip_seen` | Dato de estado/configuración `at_status_sta_ip_seen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_status_ap_ip_seen` | Dato de estado/configuración `at_status_ap_ip_seen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_reboot_pending` | Dato de estado/configuración `at_reboot_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_reboot_mode` | Dato de estado/configuración `at_reboot_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_CapabilityMask_t at_detected_capabilities` | Dato de estado/configuración `at_detected_capabilities`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridgeNetworkProtocol at_network_protocol` | Dato de estado/configuración `at_network_protocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_network_stage` | Dato de estado/configuración `at_network_stage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_network_restart_pending` | Dato de estado/configuración `at_network_restart_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_network_ready` | Dato de estado/configuración `at_network_ready`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t at_tcp_link_id` | Dato de estado/configuración `at_tcp_link_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t at_pending_close_link_id` | Dato de estado/configuración `at_pending_close_link_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_udp_peer_valid` | Dato de estado/configuración `at_udp_peer_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_udp_peer_ip[WIFI_BRIDGE_IP_LEN]` | Dato de estado/configuración `at_udp_peer_ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_udp_peer_port` | Dato de estado/configuración `at_udp_peer_port`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t at_udp_peer_last_rx_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char at_ipd_header[WIFI_BRIDGE_AT_IPD_HEADER_SIZE]` | Dato de estado/configuración `at_ipd_header`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_ipd_header_len` | Dato de estado/configuración `at_ipd_header_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_ipd_expected_len` | Dato de estado/configuración `at_ipd_expected_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_ipd_received_len` | Dato de estado/configuración `at_ipd_received_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t at_ipd_link_id` | Dato de estado/configuración `at_ipd_link_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_ipd_peer_ip[WIFI_BRIDGE_IP_LEN]` | Dato de estado/configuración `at_ipd_peer_ip`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_ipd_peer_port` | Dato de estado/configuración `at_ipd_peer_port`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_ipd_active` | Dato de estado/configuración `at_ipd_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_ipd_payload[WIFI_BRIDGE_AT_MAX_FRAME_SIZE]` | Dato de estado/configuración `at_ipd_payload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_frame_queue[WIFI_BRIDGE_AT_TX_QUEUE_SLOTS][WIFI_BRIDGE_AT_MAX_FRAME_SIZE]` | Dato de estado/configuración `at_frame_queue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t at_frame_queue_len[WIFI_BRIDGE_AT_TX_QUEUE_SLOTS]` | Dato de estado/configuración `at_frame_queue_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_frame_queue_head` | Dato de estado/configuración `at_frame_queue_head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_frame_queue_tail` | Dato de estado/configuración `at_frame_queue_tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_frame_queue_count` | Dato de estado/configuración `at_frame_queue_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_AT_SendState at_send_state` | Dato de estado/configuración `at_send_state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t at_send_payload_started` | Dato de estado/configuración `at_send_payload_started`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiBridge_AT_NetworkCounters at_network_counters` | Dato de estado/configuración `at_network_counters`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiBridge_EventCallback`

`f4:Core/Inc/wifi_bridge_types.h:80`

Firma: `void (*WifiBridge_EventCallback)(const WifiBridge_Event *evt)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `WifiBridge_Hardware`

`f4:Core/Inc/wifi_bridge_port.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void *ctx)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `AM_Result (*rx_start)(void *ctx, uint8_t *buffer, uint16_t length)` | Operación rx start de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `AM_Result (*tx)(void *ctx, const uint8_t *data, uint16_t length)` | Operación tx de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `void (*enable_idle)(void *ctx)` | Operación enable idle de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `bool (*rx_ready)(void *ctx)` | Operación rx ready de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `uint16_t (*rx_remaining)(void *ctx)` | Operación rx remaining de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |

### `WifiBridge_Services`

`f4:Core/Inc/wifi_bridge_port.h:18`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*AppApConfig_Get)(void *ctx, AppApConfig_t *out)` | Copia configuración AP deseada. |
| `void (*AppApConfig_OnEspApplyResponse)(void *ctx, uint8_t status)` | Procesa el status recibido del ESP para la aplicación AP. |
| `uint8_t (*AppPreferences_GetWifiAtProtocol)(void *ctx)` | Operación App Preferences Get Wifi At Protocol de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `void (*Permission_RevokeRemoteNode)(void *ctx, uint8_t source_node)` | Revoca autorización del nodo indicado. |
| `void (*UNER_App_FeedUart1Bytes)(void *ctx, const uint8_t *data, uint16_t len)` | Entrega bytes recibidos al camino UNER de UART1. |
| `uint8_t (*UNER_App_GetBinaryAsset)(void *ctx, uint8_t asset_id, BinaryAssetView_t *asset)` | Expone una vista de asset binario disponible sin transferir propiedad de su memoria. |
| `uint8_t (*UNER_App_GetWaitingCommandId)(void *ctx)` | Consulta el ID cuya finalización espera la capa de aplicación. |
| `UNER_Status (*UNER_App_Init)(void *ctx, WifiBridge_Handle *bridge)` | Conecta bridge, transportes, rutas y contratos de aplicación. |
| `uint8_t (*UNER_App_IsWaitingValidation)(void *ctx)` | Indica si una solicitud de aplicación sigue esperando su resultado. |
| `void (*UNER_App_NotifyUart1Rx)(void *ctx)` | Marca recepción UART1 pendiente para el procesamiento de aplicación. |
| `void (*UNER_App_OnAtNetworkDisconnected)(void *ctx)` | Limpia el estado de enlace/sesiones correspondiente a desconexión AT. |
| `void (*UNER_App_OnUart1TxComplete)(void *ctx)` | Notifica fin de TX y habilita transiciones diferidas. |
| `void (*UNER_App_Poll)(void *ctx)` | Atiende protocolos y servicios diferidos de la aplicación. |
| `UNER_Status (*UNER_App_ReportCarModeChanged)(void *ctx, uint8_t mode)` | Publica el modo de automóvil confirmado. |
| `UNER_Status (*UNER_App_ReportMenuSelectionChanged)(void *ctx, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Publica selección visible y cantidad vinculadas a screen code. |
| `UNER_Status (*UNER_App_ReportScreenChanged)(void *ctx, uint32_t screen_code, uint8_t source)` | Serializa identidad/origen de pantalla y publica evento. |
| `UNER_Status (*UNER_App_RequestBinaryAsset)(void *ctx, uint8_t asset_id)` | Solicita al otro extremo un asset por su ID lógico. |
| `UNER_Status (*UNER_App_RequestWifiNetworkDetail)(void *ctx, const char *ssid)` | Solicita detalle del SSID indicado mediante el backend de protocolo. |
| `void (*UNER_App_ResetUart1RxPos)(void *ctx)` | Operación UNER App Reset Uart1 Rx Pos de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `UNER_Status (*UNER_App_SendCommand)(void *ctx, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Envía un comando de aplicación por la ruta configurada. |
| `UNER_Status (*UNER_App_SendEspReboot)(void *ctx, uint8_t boot_mode)` | Operación UNER App Send Esp Reboot de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `UNER_Status (*UNER_App_SendWifiCredentialRequest)(void *ctx, const char *ssid)` | Inicia solicitud de ingreso de credenciales Wi-Fi; no transporta PIN de autorización como equivalente. |
| `uint8_t (*checksum)(const uint8_t *data, uint16_t length)` | Operación checksum de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |
| `void (*clear_scan_results)(void)` | Operación clear scan results de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-wifibridge_at_connect"></a>`WifiBridge_Result WifiBridge_AT_Connect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_SendJoin` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:33`; structural |
| <a id="fn-wifibridge_at_disconnect"></a>`WifiBridge_Result WifiBridge_AT_Disconnect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_Send` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:34`; structural |
| <a id="fn-wifibridge_at_getbinaryasset"></a>`uint8_t WifiBridge_AT_GetBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id, WifiBridge_BinaryAssetView *asset)` | Consulta Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:59`; inferred-name |
| <a id="fn-wifibridge_at_getscanresults"></a>`WifiBridge_Result WifiBridge_AT_GetScanResults(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_StartScan` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:37`; structural |
| <a id="fn-wifibridge_at_getstatus"></a>`WifiBridge_Result WifiBridge_AT_GetStatus(WifiBridge_Handle *h)` | Consulta Status dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:38`; inferred-name |
| <a id="fn-wifibridge_at_getwaitingcommandid"></a>`uint8_t WifiBridge_AT_GetWaitingCommandId(WifiBridge_Handle *h)` | Consulta Waiting Command Id dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:63`; inferred-name |
| <a id="fn-wifibridge_at_init"></a>`WifiBridge_Result WifiBridge_AT_Init(WifiBridge_Handle *h, void *huart)` | Inicializa contexto de bridge o backend con sus puertos. | `f4:Core/Inc/wifi_bridge_at.h:13`; editorial |
| <a id="fn-wifibridge_at_iswaitingvalidation"></a>`uint8_t WifiBridge_AT_IsWaitingValidation(WifiBridge_Handle *h)` | Indica si el backend espera finalizar una solicitud. | `f4:Core/Inc/wifi_bridge_at.h:62`; editorial |
| <a id="fn-wifibridge_at_ontxcomplete"></a>`void WifiBridge_AT_OnTxComplete(WifiBridge_Handle *h)` | Notifica final de transmisión UART y permite avanzar la cola. | `f4:Core/Inc/wifi_bridge_at.h:17`; editorial |
| <a id="fn-wifibridge_at_process"></a>`void WifiBridge_AT_Process(WifiBridge_Handle *h)` | Avanza la máquina de estados y operaciones diferidas del backend. | `f4:Core/Inc/wifi_bridge_at.h:15`; editorial |
| <a id="fn-wifibridge_at_queueframe"></a>`WifiBridge_Result WifiBridge_AT_QueueFrame(WifiBridge_Handle *h, const uint8_t *frame, uint16_t len)` | Encola un frame para transporte de red AT. | `f4:Core/Inc/wifi_bridge_at.h:23`; editorial |
| <a id="fn-wifibridge_at_reportcarmodechanged"></a>`WifiBridge_Result WifiBridge_AT_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode)` | Publica el reporte de Car Mode Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:57`; inferred-name |
| <a id="fn-wifibridge_at_reportmenuselectionchanged"></a>`WifiBridge_Result WifiBridge_AT_ReportMenuSelectionChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Publica el reporte de Menu Selection Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:52`; inferred-name |
| <a id="fn-wifibridge_at_reportscreenchanged"></a>`WifiBridge_Result WifiBridge_AT_ReportScreenChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t source)` | Publica el reporte de Screen Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:49`; inferred-name |
| <a id="fn-wifibridge_at_requestbinaryasset"></a>`WifiBridge_Result WifiBridge_AT_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id)` | Solicita Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:58`; inferred-name |
| <a id="fn-wifibridge_at_requestwifinetworkdetail"></a>`WifiBridge_Result WifiBridge_AT_RequestWifiNetworkDetail(WifiBridge_Handle *h, const char *ssid)` | Adapta la llamada a `WifiBridge_AT_FindNetwork` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:46`; structural |
| <a id="fn-wifibridge_at_resetnetworksession"></a>`void WifiBridge_AT_ResetNetworkSession(WifiBridge_Handle *h)` | Restablece Network Session dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:19`; inferred-name |
| <a id="fn-wifibridge_at_resetrxpos"></a>`void WifiBridge_AT_ResetRxPos(WifiBridge_Handle *h)` | Restablece Rx Pos dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:18`; inferred-name |
| <a id="fn-wifibridge_at_rxcheck"></a>`void WifiBridge_AT_RxCheck(WifiBridge_Handle *h)` | Consume nuevos bytes del buffer circular de recepción. | `f4:Core/Inc/wifi_bridge_at.h:16`; editorial |
| <a id="fn-wifibridge_at_sendcommand"></a>`WifiBridge_Result WifiBridge_AT_SendCommand(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Envía Command dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:40`; inferred-name |
| <a id="fn-wifibridge_at_sendespreboot"></a>`WifiBridge_Result WifiBridge_AT_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode)` | Envía Esp Reboot dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:48`; inferred-name |
| <a id="fn-wifibridge_at_sendwificredentialrequest"></a>`WifiBridge_Result WifiBridge_AT_SendWifiCredentialRequest(WifiBridge_Handle *h, const char *ssid)` | Envía Wifi Credential Request dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:44`; inferred-name |
| <a id="fn-wifibridge_at_setcredentials"></a>`WifiBridge_Result WifiBridge_AT_SetCredentials(WifiBridge_Handle *h, const char *ssid, const char *pass)` | Configura Credentials dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:30`; inferred-name |
| <a id="fn-wifibridge_at_setmodeap"></a>`WifiBridge_Result WifiBridge_AT_SetModeAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_Send` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:29`; structural |
| <a id="fn-wifibridge_at_setmodesta"></a>`WifiBridge_Result WifiBridge_AT_SetModeSTA(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_Send` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:28`; structural |
| <a id="fn-wifibridge_at_setnetworkprotocol"></a>`WifiBridge_Result WifiBridge_AT_SetNetworkProtocol(WifiBridge_Handle *h, WifiBridgeNetworkProtocol protocol)` | Configura Network Protocol dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:21`; inferred-name |
| <a id="fn-wifibridge_at_start"></a>`WifiBridge_Result WifiBridge_AT_Start(WifiBridge_Handle *h)` | Inicia recepción y negociación del backend. | `f4:Core/Inc/wifi_bridge_at.h:14`; editorial |
| <a id="fn-wifibridge_at_startnetworkservice"></a>`WifiBridge_Result WifiBridge_AT_StartNetworkService(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_ResetNetworkSession` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_at.h:20`; structural |
| <a id="fn-wifibridge_at_startscan"></a>`WifiBridge_Result WifiBridge_AT_StartScan(WifiBridge_Handle *h)` | Inicia Scan dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:35`; inferred-name |
| <a id="fn-wifibridge_at_stopscan"></a>`WifiBridge_Result WifiBridge_AT_StopScan(WifiBridge_Handle *h)` | Detiene Scan dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_at.h:36`; inferred-name |
| <a id="fn-wifibridge_connect"></a>`WifiBridge_Result WifiBridge_Connect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:41`; structural |
| <a id="fn-wifibridge_consumeconnectionlostevent"></a>`uint8_t WifiBridge_ConsumeConnectionLostEvent(WifiBridge_Handle *h)` | Consume la señal pendiente de pérdida de conexión. | `f4:Core/Inc/wifi_bridge.h:65`; editorial |
| <a id="fn-wifibridge_disconnect"></a>`WifiBridge_Result WifiBridge_Disconnect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:42`; structural |
| <a id="fn-wifibridge_emitevent"></a>`void WifiBridge_EmitEvent(WifiBridge_Handle *h, const WifiBridge_Event *evt)` | Entrega un evento al callback de la aplicación. | `f4:Core/Inc/wifi_bridge.h:30`; editorial |
| <a id="fn-wifibridge_getactivebackend"></a>`WifiBridge_Backend WifiBridge_GetActiveBackend(const WifiBridge_Handle *h)` | Consulta backend seleccionado por la detección. | `f4:Core/Inc/wifi_bridge.h:81`; editorial |
| <a id="fn-wifibridge_getbackendlabel"></a>`const char *WifiBridge_GetBackendLabel(const WifiBridge_Handle *h)` | Consulta Backend Label dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:87`; inferred-name |
| <a id="fn-wifibridge_getbinaryasset"></a>`uint8_t WifiBridge_GetBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id, WifiBridge_BinaryAssetView *asset)` | Consulta Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:76`; inferred-name |
| <a id="fn-wifibridge_getcapabilities"></a>`WifiBridge_CapabilityMask_t WifiBridge_GetCapabilities(const WifiBridge_Handle *h)` | Consulta máscara de operaciones anunciadas por el backend. | `f4:Core/Inc/wifi_bridge.h:82`; editorial |
| <a id="fn-wifibridge_getmode"></a>`uint8_t WifiBridge_GetMode(const WifiBridge_Handle *h)` | Consulta Mode dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:86`; inferred-name |
| <a id="fn-wifibridge_getruntimecredentials_h"></a>`const WifiBridge_Credentials *WifiBridge_GetRuntimeCredentials_H(WifiBridge_Handle *h)` | Consulta Runtime Credentials H dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:34`; inferred-name |
| <a id="fn-wifibridge_getscanresults"></a>`WifiBridge_Result WifiBridge_GetScanResults(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:45`; structural |
| <a id="fn-wifibridge_getstatus"></a>`WifiBridge_Result WifiBridge_GetStatus(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:46`; structural |
| <a id="fn-wifibridge_getwaitingcommandid"></a>`uint8_t WifiBridge_GetWaitingCommandId(WifiBridge_Handle *h)` | Consulta Waiting Command Id dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:80`; inferred-name |
| <a id="fn-wifibridge_hascapability"></a>`uint8_t WifiBridge_HasCapability(const WifiBridge_Handle *h, WifiBridge_CapabilityMask_t capability)` | Comprueba un bit de capacidad del backend. | `f4:Core/Inc/wifi_bridge.h:83`; editorial |
| <a id="fn-wifibridge_init"></a>`WifiBridge_Result WifiBridge_Init(WifiBridge_Handle *h, void *huart, const WifiBridge_Hardware *hardware, const WifiBridge_Services *services, void *services_ctx, SystemHandle *system, WifiBridge_Credentials *credentials)` | Inicializa contexto de bridge o backend con sus puertos. | `f4:Core/Inc/wifi_bridge.h:17`; editorial |
| <a id="fn-wifibridge_isconnected"></a>`uint8_t WifiBridge_IsConnected(const WifiBridge_Handle *h)` | Consulta estado de conectividad del bridge, distinto de sesión PIN. | `f4:Core/Inc/wifi_bridge.h:85`; editorial |
| <a id="fn-wifibridge_iswaitingvalidation"></a>`uint8_t WifiBridge_IsWaitingValidation(WifiBridge_Handle *h)` | Indica si el backend espera finalizar una solicitud. | `f4:Core/Inc/wifi_bridge.h:79`; editorial |
| <a id="fn-wifibridge_onerror"></a>`void WifiBridge_OnError(WifiBridge_Handle *h)` | Atiende fallo del backend de comunicación. | `f4:Core/Inc/wifi_bridge.h:26`; editorial |
| <a id="fn-wifibridge_onidleirq"></a>`void WifiBridge_OnIdleIrq(WifiBridge_Handle *h)` | Notifica detección IDLE para atender bytes recibidos. | `f4:Core/Inc/wifi_bridge.h:22`; editorial |
| <a id="fn-wifibridge_onrxcomplete"></a>`void WifiBridge_OnRxComplete(WifiBridge_Handle *h)` | Notifica el hito de recepción DMA completa. | `f4:Core/Inc/wifi_bridge.h:25`; editorial |
| <a id="fn-wifibridge_onrxhalfcomplete"></a>`void WifiBridge_OnRxHalfComplete(WifiBridge_Handle *h)` | Notifica el hito de media recepción DMA. | `f4:Core/Inc/wifi_bridge.h:24`; editorial |
| <a id="fn-wifibridge_ontxcomplete"></a>`void WifiBridge_OnTxComplete(WifiBridge_Handle *h)` | Notifica final de transmisión UART y permite avanzar la cola. | `f4:Core/Inc/wifi_bridge.h:23`; editorial |
| <a id="fn-wifibridge_process"></a>`void WifiBridge_Process(WifiBridge_Handle *h)` | Avanza la máquina de estados y operaciones diferidas del backend. | `f4:Core/Inc/wifi_bridge.h:20`; editorial |
| <a id="fn-wifibridge_registeraliveresponse"></a>`void WifiBridge_RegisterAliveResponse(WifiBridge_Handle *h, uint8_t mode)` | Registra respuesta de mantenimiento de conexión. | `f4:Core/Inc/wifi_bridge.h:61`; editorial |
| <a id="fn-wifibridge_registerbackendalive"></a>`void WifiBridge_RegisterBackendAlive(WifiBridge_Handle *h, WifiBridge_Backend backend, uint8_t mode)` | Registra evidencia de actividad del backend. | `f4:Core/Inc/wifi_bridge.h:62`; editorial |
| <a id="fn-wifibridge_reportcarmodechanged"></a>`WifiBridge_Result WifiBridge_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode)` | Publica el reporte de Car Mode Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:74`; inferred-name |
| <a id="fn-wifibridge_reportmenuselectionchanged"></a>`WifiBridge_Result WifiBridge_ReportMenuSelectionChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Publica el reporte de Menu Selection Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:69`; inferred-name |
| <a id="fn-wifibridge_reportscreenchanged"></a>`WifiBridge_Result WifiBridge_ReportScreenChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t source)` | Publica el reporte de Screen Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:66`; inferred-name |
| <a id="fn-wifibridge_requestbinaryasset"></a>`WifiBridge_Result WifiBridge_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id)` | Solicita Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:75`; inferred-name |
| <a id="fn-wifibridge_requestwifinetworkdetail"></a>`WifiBridge_Result WifiBridge_RequestWifiNetworkDetail(WifiBridge_Handle *h, const char *ssid)` | Solicita Wifi Network Detail dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:55`; inferred-name |
| <a id="fn-wifibridge_resetrxpos"></a>`void WifiBridge_ResetRxPos(WifiBridge_Handle *h)` | Restablece Rx Pos dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:27`; inferred-name |
| <a id="fn-wifibridge_rxcheck"></a>`void WifiBridge_RxCheck(WifiBridge_Handle *h)` | Consume nuevos bytes del buffer circular de recepción. | `f4:Core/Inc/wifi_bridge.h:21`; editorial |
| <a id="fn-wifibridge_sendalive"></a>`WifiBridge_Result WifiBridge_SendAlive(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_SendAliveWithWebPeriod` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:58`; structural |
| <a id="fn-wifibridge_sendalivewithwebperiod"></a>`WifiBridge_Result WifiBridge_SendAliveWithWebPeriod(WifiBridge_Handle *h, uint16_t web_period_ms)` | Envía Alive With Web Period dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:59`; inferred-name |
| <a id="fn-wifibridge_sendcommand"></a>`WifiBridge_Result WifiBridge_SendCommand(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Envía Command dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:49`; inferred-name |
| <a id="fn-wifibridge_sendespreboot"></a>`WifiBridge_Result WifiBridge_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode)` | Envía Esp Reboot dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:57`; inferred-name |
| <a id="fn-wifibridge_sendwificredentialrequest"></a>`WifiBridge_Result WifiBridge_SendWifiCredentialRequest(WifiBridge_Handle *h, const char *ssid)` | Envía Wifi Credential Request dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:53`; inferred-name |
| <a id="fn-wifibridge_setcredentials"></a>`WifiBridge_Result WifiBridge_SetCredentials(WifiBridge_Handle *h, const char *ssid, const char *pass)` | Configura Credentials dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:38`; inferred-name |
| <a id="fn-wifibridge_seteventcallback"></a>`void WifiBridge_SetEventCallback(WifiBridge_Handle *h, WifiBridge_EventCallback cb)` | Registra receptor de eventos de bridge. | `f4:Core/Inc/wifi_bridge.h:29`; editorial |
| <a id="fn-wifibridge_setmodeap"></a>`WifiBridge_Result WifiBridge_SetModeAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:37`; structural |
| <a id="fn-wifibridge_setmodesta"></a>`WifiBridge_Result WifiBridge_SetModeSTA(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:36`; structural |
| <a id="fn-wifibridge_setruntimecredentials_h"></a>`WifiBridge_Result WifiBridge_SetRuntimeCredentials_H(WifiBridge_Handle *h, const char *ssid, const char *password)` | Configura Runtime Credentials H dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:31`; inferred-name |
| <a id="fn-wifibridge_setruntimepassword_h"></a>`WifiBridge_Result WifiBridge_SetRuntimePassword_H(WifiBridge_Handle *h, const char *password)` | Configura Runtime Password H dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:33`; inferred-name |
| <a id="fn-wifibridge_setruntimessid_h"></a>`WifiBridge_Result WifiBridge_SetRuntimeSsid_H(WifiBridge_Handle *h, const char *ssid)` | Configura Runtime Ssid H dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge.h:32`; inferred-name |
| <a id="fn-wifibridge_start"></a>`WifiBridge_Result WifiBridge_Start(WifiBridge_Handle *h)` | Inicia recepción y negociación del backend. | `f4:Core/Inc/wifi_bridge.h:19`; editorial |
| <a id="fn-wifibridge_startap"></a>`WifiBridge_Result WifiBridge_StartAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:47`; structural |
| <a id="fn-wifibridge_startscan"></a>`WifiBridge_Result WifiBridge_StartScan(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:43`; structural |
| <a id="fn-wifibridge_stopap"></a>`WifiBridge_Result WifiBridge_StopAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:48`; structural |
| <a id="fn-wifibridge_stopscan"></a>`WifiBridge_Result WifiBridge_StopScan(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_DispatchCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge.h:44`; structural |
| <a id="fn-wifibridge_uner_connect"></a>`WifiBridge_Result WifiBridge_UNER_Connect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:25`; structural |
| <a id="fn-wifibridge_uner_disconnect"></a>`WifiBridge_Result WifiBridge_UNER_Disconnect(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:26`; structural |
| <a id="fn-wifibridge_uner_getbinaryasset"></a>`uint8_t WifiBridge_UNER_GetBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id, WifiBridge_BinaryAssetView *asset)` | Consulta Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:51`; inferred-name |
| <a id="fn-wifibridge_uner_getscanresults"></a>`WifiBridge_Result WifiBridge_UNER_GetScanResults(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:29`; structural |
| <a id="fn-wifibridge_uner_getstatus"></a>`WifiBridge_Result WifiBridge_UNER_GetStatus(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:30`; structural |
| <a id="fn-wifibridge_uner_getwaitingcommandid"></a>`uint8_t WifiBridge_UNER_GetWaitingCommandId(WifiBridge_Handle *h)` | Adapta la llamada a `UNER_App_GetWaitingCommandId` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:55`; structural |
| <a id="fn-wifibridge_uner_init"></a>`WifiBridge_Result WifiBridge_UNER_Init(WifiBridge_Handle *h, void *huart)` | Inicializa contexto de bridge o backend con sus puertos. | `f4:Core/Inc/wifi_bridge_uner.h:13`; editorial |
| <a id="fn-wifibridge_uner_iswaitingvalidation"></a>`uint8_t WifiBridge_UNER_IsWaitingValidation(WifiBridge_Handle *h)` | Indica si el backend espera finalizar una solicitud. | `f4:Core/Inc/wifi_bridge_uner.h:54`; editorial |
| <a id="fn-wifibridge_uner_ontxcomplete"></a>`void WifiBridge_UNER_OnTxComplete(WifiBridge_Handle *h)` | Notifica final de transmisión UART y permite avanzar la cola. | `f4:Core/Inc/wifi_bridge_uner.h:17`; editorial |
| <a id="fn-wifibridge_uner_process"></a>`void WifiBridge_UNER_Process(WifiBridge_Handle *h)` | Avanza la máquina de estados y operaciones diferidas del backend. | `f4:Core/Inc/wifi_bridge_uner.h:15`; editorial |
| <a id="fn-wifibridge_uner_reportcarmodechanged"></a>`WifiBridge_Result WifiBridge_UNER_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode)` | Publica el reporte de Car Mode Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:49`; inferred-name |
| <a id="fn-wifibridge_uner_reportmenuselectionchanged"></a>`WifiBridge_Result WifiBridge_UNER_ReportMenuSelectionChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Publica el reporte de Menu Selection Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:44`; inferred-name |
| <a id="fn-wifibridge_uner_reportscreenchanged"></a>`WifiBridge_Result WifiBridge_UNER_ReportScreenChanged(WifiBridge_Handle *h, uint32_t screen_code, uint8_t source)` | Publica el reporte de Screen Changed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:41`; inferred-name |
| <a id="fn-wifibridge_uner_requestbinaryasset"></a>`WifiBridge_Result WifiBridge_UNER_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id)` | Solicita Binary Asset dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:50`; inferred-name |
| <a id="fn-wifibridge_uner_requestwifinetworkdetail"></a>`WifiBridge_Result WifiBridge_UNER_RequestWifiNetworkDetail(WifiBridge_Handle *h, const char *ssid)` | Solicita Wifi Network Detail dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:38`; inferred-name |
| <a id="fn-wifibridge_uner_resetrxpos"></a>`void WifiBridge_UNER_ResetRxPos(WifiBridge_Handle *h)` | Adapta la llamada a `UNER_App_ResetUart1RxPos` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:18`; structural |
| <a id="fn-wifibridge_uner_rxcheck"></a>`void WifiBridge_UNER_RxCheck(WifiBridge_Handle *h)` | Consume nuevos bytes del buffer circular de recepción. | `f4:Core/Inc/wifi_bridge_uner.h:16`; editorial |
| <a id="fn-wifibridge_uner_sendcommand"></a>`WifiBridge_Result WifiBridge_UNER_SendCommand(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Envía Command dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:32`; inferred-name |
| <a id="fn-wifibridge_uner_sendespreboot"></a>`WifiBridge_Result WifiBridge_UNER_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode)` | Envía Esp Reboot dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:40`; inferred-name |
| <a id="fn-wifibridge_uner_sendwificredentialrequest"></a>`WifiBridge_Result WifiBridge_UNER_SendWifiCredentialRequest(WifiBridge_Handle *h, const char *ssid)` | Envía Wifi Credential Request dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:36`; inferred-name |
| <a id="fn-wifibridge_uner_setcredentials"></a>`WifiBridge_Result WifiBridge_UNER_SetCredentials(WifiBridge_Handle *h, const char *ssid, const char *pass)` | Configura Credentials dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Inc/wifi_bridge_uner.h:22`; inferred-name |
| <a id="fn-wifibridge_uner_setmodeap"></a>`WifiBridge_Result WifiBridge_UNER_SetModeAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:21`; structural |
| <a id="fn-wifibridge_uner_setmodesta"></a>`WifiBridge_Result WifiBridge_UNER_SetModeSTA(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:20`; structural |
| <a id="fn-wifibridge_uner_start"></a>`WifiBridge_Result WifiBridge_UNER_Start(WifiBridge_Handle *h)` | Inicia recepción y negociación del backend. | `f4:Core/Inc/wifi_bridge_uner.h:14`; editorial |
| <a id="fn-wifibridge_uner_startscan"></a>`WifiBridge_Result WifiBridge_UNER_StartScan(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:27`; structural |
| <a id="fn-wifibridge_uner_stopscan"></a>`WifiBridge_Result WifiBridge_UNER_StopScan(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_UNER_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/wifi_bridge_uner.h:28`; structural |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-wifibridge_copyruntimetext"></a>`static WifiBridge_Result WifiBridge_CopyRuntimeText(char *dst, size_t dst_len, const char *src)` | Operación Copy Runtime Text de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge.c:34`; inferred-name |
| <a id="fn-wifibridge_setmodeflags"></a>`static void WifiBridge_SetModeFlags(WifiBridge_Handle *h, uint8_t mode)` | Configura Mode Flags dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge.c:61`; inferred-name |
| <a id="fn-wifibridge_clearmodeflags"></a>`static void WifiBridge_ClearModeFlags(WifiBridge_Handle *h)` | Adapta la llamada a `CLEAR_FLAG` con el contexto de esta integración. | `f4:Core/Src/wifi_bridge.c:75`; structural |
| <a id="fn-wifibridge_markconnectionlost"></a>`static void WifiBridge_MarkConnectionLost(WifiBridge_Handle *h)` | Coordina `IS_FLAG_SET`, `WifiBridge_AT_ResetNetworkSession`, `WifiBridge_ClearModeFlags` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge.c:82`; structural |
| <a id="fn-wifibridge_probebackend"></a>`static WifiBridge_Backend WifiBridge_ProbeBackend(const WifiBridge_Handle *h)` | Operación Probe Backend de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge.c:105`; inferred-name |
| <a id="fn-wifibridge_toggleprobe"></a>`static void WifiBridge_ToggleProbe(WifiBridge_Handle *h)` | Operación Toggle Probe de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge.c:112`; inferred-name |
| <a id="fn-wifibridge_sendaliveforbackend"></a>`static WifiBridge_Result WifiBridge_SendAliveForBackend(WifiBridge_Handle *h, WifiBridge_Backend backend, uint16_t web_period_ms)` | Envía Alive For Backend dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge.c:123`; inferred-name |
| <a id="fn-wifibridge_commandrequiredcapability"></a>`static WifiBridge_CapabilityMask_t WifiBridge_CommandRequiredCapability(uint8_t cmd)` | Operación Command Required Capability de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge.c:145`; inferred-name |
| <a id="fn-wifibridge_dispatchcommand"></a>`static WifiBridge_Result WifiBridge_DispatchCommand(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Coordina `WifiBridge_AT_SendCommand`, `WifiBridge_CommandRequiredCapability`, `WifiBridge_HasCapability` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge.c:235`; structural |
| <a id="fn-wifibridge_getalivemaxmisses"></a>`static uint8_t WifiBridge_GetAliveMaxMisses(const WifiBridge_Handle *h)` | Consulta Alive Max Misses dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge.c:322`; inferred-name |
| <a id="fn-wifibridge_at_strlenbounded"></a>`static size_t WifiBridge_AT_StrLenBounded(const char *text, size_t max_len)` | Operación Str Len Bounded de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:27`; inferred-name |
| <a id="fn-wifibridge_at_copytext"></a>`static void WifiBridge_AT_CopyText(char *dst, size_t dst_len, const char *src)` | Operación Copy Text de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:42`; inferred-name |
| <a id="fn-wifibridge_at_appendtext"></a>`static uint8_t WifiBridge_AT_AppendText(char *buf, size_t buf_len, uint16_t *pos, const char *text)` | Operación Append Text de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:60`; inferred-name |
| <a id="fn-wifibridge_at_appendquoted"></a>`static uint8_t WifiBridge_AT_AppendQuoted(char *buf, size_t buf_len, uint16_t *pos, const char *text)` | Coordina `WifiBridge_AT_AppendText` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:79`; structural |
| <a id="fn-wifibridge_at_appendu8"></a>`static uint8_t WifiBridge_AT_AppendU8(WifiBridge_Handle *h, char *buf, size_t buf_len, uint16_t *pos, uint8_t value)` | Operación Append U8 de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:107`; inferred-name |
| <a id="fn-wifibridge_at_appendu16"></a>`static uint8_t WifiBridge_AT_AppendU16(char *buf, size_t buf_len, uint16_t *pos, uint16_t value)` | Operación Append U16 de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:137`; inferred-name |
| <a id="fn-wifibridge_at_appendip"></a>`static uint8_t WifiBridge_AT_AppendIp(char *buf, size_t buf_len, uint16_t *pos, const uint8_t ip[WIFI_BRIDGE_IP_LEN])` | Coordina `WifiBridge_AT_AppendText`, `WifiBridge_AT_AppendU16` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:163`; structural |
| <a id="fn-wifibridge_at_parsesigned"></a>`static int16_t WifiBridge_AT_ParseSigned(const char **cursor)` | Decodifica Signed dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:180`; inferred-name |
| <a id="fn-wifibridge_at_parseip"></a>`static uint8_t WifiBridge_AT_ParseIp(const char *line, uint8_t ip[WIFI_BRIDGE_IP_LEN])` | Decodifica Ip dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:205`; inferred-name |
| <a id="fn-wifibridge_at_ipiszero"></a>`static uint8_t WifiBridge_AT_IpIsZero(const uint8_t ip[WIFI_BRIDGE_IP_LEN])` | Operación Ip Is Zero de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:236`; inferred-name |
| <a id="fn-wifibridge_at_clearstaip"></a>`static void WifiBridge_AT_ClearStaIp(WifiBridge_Handle *h)` | Limpia Sta Ip dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:242`; inferred-name |
| <a id="fn-wifibridge_at_clearapip"></a>`static void WifiBridge_AT_ClearApIp(WifiBridge_Handle *h)` | Limpia Ap Ip dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:249`; inferred-name |
| <a id="fn-wifibridge_at_storefirmwareversion"></a>`static void WifiBridge_AT_StoreFirmwareVersion(WifiBridge_Handle *h, const char *line)` | Coordina `strncmp` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:256`; structural |
| <a id="fn-wifibridge_at_encryptiontext"></a>`static const char *WifiBridge_AT_EncryptionText(uint8_t enc)` | Operación Encryption Text de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_at.c:275`; inferred-name |
| <a id="fn-wifibridge_at_findnetwork"></a>`static int8_t WifiBridge_AT_FindNetwork(WifiBridge_Handle *h, const char *ssid)` | Adapta la llamada a `strncmp` con el contexto de esta integración. | `f4:Core/Src/wifi_bridge_at.c:290`; structural |
| <a id="fn-wifibridge_at_storescanresult"></a>`static void WifiBridge_AT_StoreScanResult(WifiBridge_Handle *h, const char *ssid, uint8_t enc, int8_t rssi, uint8_t channel)` | Coordina `WifiBridge_AT_CopyText`, `WifiBridge_AT_EncryptionText`, `WifiBridge_AT_FindNetwork` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:305`; structural |
| <a id="fn-wifibridge_at_parsecwlap"></a>`static void WifiBridge_AT_ParseCwlap(WifiBridge_Handle *h, const char *line)` | Decodifica Cwlap dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:339`; inferred-name |
| <a id="fn-wifibridge_at_setstaip"></a>`static void WifiBridge_AT_SetStaIp(WifiBridge_Handle *h, const uint8_t ip[WIFI_BRIDGE_IP_LEN])` | Configura Sta Ip dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:394`; inferred-name |
| <a id="fn-wifibridge_at_setapip"></a>`static void WifiBridge_AT_SetApIp(WifiBridge_Handle *h, const uint8_t ip[WIFI_BRIDGE_IP_LEN])` | Configura Ap Ip dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:407`; inferred-name |
| <a id="fn-wifibridge_at_emit"></a>`static void WifiBridge_AT_Emit(WifiBridge_Handle *h, WifiBridge_EventId id, uint8_t status)` | Coordina `WIFI_FLAG_IS_SET`, `WifiBridge_AT_CopyText`, `WifiBridge_EmitEvent` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:420`; structural |
| <a id="fn-wifibridge_at_commanddone"></a>`static void WifiBridge_AT_CommandDone(WifiBridge_Handle *h, uint8_t ok)` | Coordina `AppApConfig_OnEspApplyResponse`, `IS_FLAG_SET`, `WIFI_FLAG_CLEAR` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:439`; structural |
| <a id="fn-wifibridge_at_parseunsigned"></a>`static uint8_t WifiBridge_AT_ParseUnsigned(const char **cursor, uint32_t *value)` | Decodifica Unsigned dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:571`; inferred-name |
| <a id="fn-wifibridge_at_frameisvalid"></a>`static uint8_t WifiBridge_AT_FrameIsValid(WifiBridge_Handle *h, const uint8_t *frame, uint16_t len)` | Coordina `checksum` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:591`; structural |
| <a id="fn-wifibridge_at_peermatches"></a>`static uint8_t WifiBridge_AT_PeerMatches(const uint8_t a[WIFI_BRIDGE_IP_LEN], uint16_t a_port, const uint8_t b[WIFI_BRIDGE_IP_LEN], uint16_t b_port)` | Adapta la llamada a `memcmp` con el contexto de esta integración. | `f4:Core/Src/wifi_bridge_at.c:611`; structural |
| <a id="fn-wifibridge_at_parseipdheader"></a>`static uint8_t WifiBridge_AT_ParseIpdHeader(WifiBridge_Handle *h, const char *header)` | Decodifica Ipd Header dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:619`; inferred-name |
| <a id="fn-wifibridge_at_completeipd"></a>`static void WifiBridge_AT_CompleteIpd(WifiBridge_Handle *h)` | Coordina `UNER_App_FeedUart1Bytes`, `WifiBridge_AT_FrameIsValid`, `WifiBridge_AT_PeerMatches` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:680`; structural |
| <a id="fn-wifibridge_at_networkadvance"></a>`static void WifiBridge_AT_NetworkAdvance(WifiBridge_Handle *h, uint8_t ok)` | Coordina `WifiBridge_AT_Send` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:713`; structural |
| <a id="fn-wifibridge_at_finishqueuedsend"></a>`static void WifiBridge_AT_FinishQueuedSend(WifiBridge_Handle *h, uint8_t ok)` | Coordina `WIFI_FLAG_CLEAR` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:786`; structural |
| <a id="fn-wifibridge_at_startqueuedsend"></a>`static WifiBridge_Result WifiBridge_AT_StartQueuedSend(WifiBridge_Handle *h)` | Inicia Queued Send dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:809`; inferred-name |
| <a id="fn-wifibridge_at_processnetwork"></a>`static void WifiBridge_AT_ProcessNetwork(WifiBridge_Handle *h)` | Coordina `Permission_RevokeRemoteNode`, `UNER_App_OnAtNetworkDisconnected`, `WIFI_FLAG_IS_SET` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:856`; structural |
| <a id="fn-wifibridge_at_parseline"></a>`static void WifiBridge_AT_ParseLine(WifiBridge_Handle *h, const char *line)` | Decodifica Line dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:899`; inferred-name |
| <a id="fn-wifibridge_at_consumebytes"></a>`static void WifiBridge_AT_ConsumeBytes(WifiBridge_Handle *h, const uint8_t *data, uint16_t len)` | Coordina `UNER_App_FeedUart1Bytes`, `WIFI_FLAG_SET`, `WifiBridge_AT_CompleteIpd` para Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:1026`; structural |
| <a id="fn-wifibridge_at_send"></a>`static WifiBridge_Result WifiBridge_AT_Send(WifiBridge_Handle *h, WifiBridge_AT_Operation op, const char *cmd)` | Envía Bridge Wi-Fi y backends UNER/AT dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:1096`; inferred-name |
| <a id="fn-wifibridge_at_sendjoin"></a>`static WifiBridge_Result WifiBridge_AT_SendJoin(WifiBridge_Handle *h)` | Envía Join dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:1147`; inferred-name |
| <a id="fn-wifibridge_at_sendapconfig"></a>`static WifiBridge_Result WifiBridge_AT_SendApConfig(WifiBridge_Handle *h)` | Envía Ap Config dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:1426`; inferred-name |
| <a id="fn-wifibridge_at_sendapipconfig"></a>`static WifiBridge_Result WifiBridge_AT_SendApIpConfig(WifiBridge_Handle *h)` | Envía Ap Ip Config dentro de Bridge Wi-Fi y backends UNER/AT. | `f4:Core/Src/wifi_bridge_at.c:1456`; inferred-name |
| <a id="fn-wifibridge_at_startap"></a>`static WifiBridge_Result WifiBridge_AT_StartAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_Send` con el contexto de esta integración. | `f4:Core/Src/wifi_bridge_at.c:1484`; structural |
| <a id="fn-wifibridge_at_stopap"></a>`static WifiBridge_Result WifiBridge_AT_StopAP(WifiBridge_Handle *h)` | Adapta la llamada a `WifiBridge_AT_Send` con el contexto de esta integración. | `f4:Core/Src/wifi_bridge_at.c:1489`; structural |
| <a id="fn-wifibridge_uner_result"></a>`static WifiBridge_Result WifiBridge_UNER_Result(UNER_Status status)` | Operación Result de Bridge Wi-Fi y backends UNER/AT; semántica fina pendiente de ampliación. | `f4:Core/Src/wifi_bridge_uner.c:11`; inferred-name |
