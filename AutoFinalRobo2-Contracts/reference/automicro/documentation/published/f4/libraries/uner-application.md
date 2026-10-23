# UNER de aplicación y contrato del robot

ID: `f4:module/Core/Inc/uner_app.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Implementa comandos, eventos, autorización, serialización y políticas de transporte del robot.

Preferencias, movimiento, sensores/streams, UI, conectividad, firmware, assets, perfiles y reset diferido. La tabla de comandos describe registros; el switch/handler determina el efecto.

<a id="modularity"></a>
## Modularidad observada

Capa de composición deliberadamente acoplada a sensores, control, menús y HAL/USB. No debe presentarse como core transportable ni importarse desde librerías genéricas.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Posee estado estático de aplicación: streams por endpoint, colas de respuestas de autenticación, reset pendiente, proxy ESP y assets. El origen se identifica con transporte+nodo; requestId JSON permanece en ESP. Los paquetes recibidos deben consumirse/copiarse antes de reutilizar slots del core.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init registra rutas/transportes/tabla; Poll atiende parser, streams, respuestas y operaciones diferidas.
- Los handlers comprueban transporte, nodo, autorización, contexto, argumentos y estado antes del efecto.
- ACK/NACK y respuesta con mismo ID son alternativas o etapas según comando; leer status y correlación.
- Reset difiere la acción hasta condiciones de transmisión; no confundir ACK encolado con ACK físicamente enviado.

## Trabajo en tándem

- Web → JSON ESP → broker UART → UNER F4; Qt usa USB UNER directo.
- Streams MPU/IR se mantienen por transporte+nodo, con recepción/consumo independientes de una lectura puntual.
- Consultar ../shared/wire-contracts.md y el catálogo de registro para tamaños y semántica de campos.

## Precauciones y límites

- No todos los IDs definidos tienen handler ni todos los declarados producen la misma respuesta.
- Timeout de metadata F4, broker ESP y Promise Web son capas distintas.
- No se acredita interoperabilidad por copiar el mismo catálogo.

## Hallazgos vinculados

- [AMDOC-F4-003: La marca RESP permite longitudes fuera del rango de solicitud](../../../review/FINDINGS.md#amdoc-f4-003)
- [AMDOC-F4-006: 0x74 declara ACK en F4 pero el handler responde por el mismo comando](../../../review/FINDINGS.md#amdoc-f4-006)
- [AMDOC-ESP-001: El validador AutoMotion acepta formas de argumentos de otros comandos](../../../review/FINDINGS.md#amdoc-esp-001)
- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/uner_app.h` | `stdint.h`, `uner_core.h`, `uner_handle.h`, `wifi_bridge_types.h`, `binary_asset_types.h` | `6fa521fb444dd9df546275a57da8acdebfd20e499b6c547b5636907f348265f0` |
| `Core/Src/uner_app.c` | `uner_app.h`, `stdbool.h`, `math.h`, `string.h`, `automicro_boot_contract.h`, `app_preferences_binding.h`, `app_ap_config.h`, `app_tasks.h`, `balance_control.h`, `globals.h`, `i2c_manager.h`, `line_follow_control.h`, `main.h`, `app_menu_binding.h`, `motion_control.h`, `motion_estimator.h`, `motor_control.h`, `mpu9250.h`, `oled_utils.h`, `oled_handle.h`, `oled_canvas.h`, `permissions.h`, `screenWrappers.h`, `ui_event_router.h`, `uner_handle.h`, `uner_transport_uart1_dma.h`, `uner_transport_usbcdc.h`, `usbd_cdc_if.h`, `uner_v2.h`, `app_wifi_binding.h` | `e1a1ba49bd011bd0a2a787ecad7e732226770d4c6220c44e5fc482521841694c` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UNER_EspProxyState`

`f4:Core/Src/uner_app.c:134`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t origin_transport_id` | Dato de estado/configuración `origin_transport_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t origin_node` | Dato de estado/configuración `origin_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint8_t ack_seen` | Dato de estado/configuración `ack_seen`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_StreamState`

`f4:Core/Src/uner_app.c:165`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t dst_node` | Dato de estado/configuración `dst_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t last_seq` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t next_due_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `UNER_AuthReply`

`f4:Core/Src/uner_app.c:228`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t dst_node` | Dato de estado/configuración `dst_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload[UNER_AUTH_REPLY_MAX_PAYLOAD]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |

### `UNER_AuthReplyQueue`

`f4:Core/Src/uner_app.c:236`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UNER_AuthReply slots[UNER_AUTH_REPLY_QUEUE_SLOTS]` | Dato de estado/configuración `slots`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t head` | Dato de estado/configuración `head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tail` | Dato de estado/configuración `tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t count` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |

### `v`

`f4:Core/Src/uner_app.c:3374`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float f` | Dato de estado/configuración `f`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b[4]` | Dato de estado/configuración `b`; declaración estructural, interpretación específica pendiente de ampliar. |

### `v`

`f4:Core/Src/uner_app.c:3600`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float f` | Dato de estado/configuración `f`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b[4]` | Dato de estado/configuración `b`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uner_app_feeduart1bytes"></a>`void UNER_App_FeedUart1Bytes(const uint8_t *data, uint16_t len)` | Entrega bytes recibidos al camino UNER de UART1. | `f4:Core/Inc/uner_app.h:183`; editorial |
| <a id="fn-uner_app_getbinaryasset"></a>`uint8_t UNER_App_GetBinaryAsset(uint8_t asset_id, BinaryAssetView_t *asset)` | Expone una vista de asset binario disponible sin transferir propiedad de su memoria. | `f4:Core/Inc/uner_app.h:173`; editorial |
| <a id="fn-uner_app_getbootloaderversion"></a>`uint8_t UNER_App_GetBootloaderVersion(uint32_t *version)` | Obtiene versión de bootloader si el contrato de arranque la hace disponible. | `f4:Core/Inc/uner_app.h:159`; editorial |
| <a id="fn-uner_app_getrouterpacketsnapshot"></a>`void UNER_App_GetRouterPacketSnapshot(uint8_t tx_view, uint8_t transport_id, UNER_RouterSnapshot *out)` | Adapta la llamada a `UNER_Handle_GetRouterPacketSnapshot` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:177`; structural |
| <a id="fn-uner_app_getroutersnapshot"></a>`void UNER_App_GetRouterSnapshot(UNER_RouterSnapshot *out)` | Adapta la llamada a `UNER_Handle_GetRouterSnapshot` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:176`; structural |
| <a id="fn-uner_app_getwaitingcommandid"></a>`uint8_t UNER_App_GetWaitingCommandId(void)` | Consulta el ID cuya finalización espera la capa de aplicación. | `f4:Core/Inc/uner_app.h:157`; editorial |
| <a id="fn-uner_app_init"></a>`UNER_Status UNER_App_Init(WifiBridge_Handle *bridge)` | Conecta bridge, transportes, rutas y contratos de aplicación. | `f4:Core/Inc/uner_app.h:147`; editorial |
| <a id="fn-uner_app_ismcuresetpending"></a>`uint8_t UNER_App_IsMcuResetPending(void)` | Consulta si existe un reset diferido aún pendiente. | `f4:Core/Inc/uner_app.h:158`; editorial |
| <a id="fn-uner_app_isrouterforwardingenabled"></a>`uint8_t UNER_App_IsRouterForwardingEnabled(void)` | Adapta la llamada a `UNER_Handle_IsForwardingEnabled` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:175`; structural |
| <a id="fn-uner_app_iswaitingvalidation"></a>`uint8_t UNER_App_IsWaitingValidation(void)` | Indica si una solicitud de aplicación sigue esperando su resultado. | `f4:Core/Inc/uner_app.h:156`; editorial |
| <a id="fn-uner_app_notifyuart1rx"></a>`void UNER_App_NotifyUart1Rx(void)` | Marca recepción UART1 pendiente para el procesamiento de aplicación. | `f4:Core/Inc/uner_app.h:182`; editorial |
| <a id="fn-uner_app_onatbridgeevent"></a>`void UNER_App_OnAtBridgeEvent(const WifiBridge_Event *event)` | Integra un evento del backend AT en el estado de protocolo/aplicación. | `f4:Core/Inc/uner_app.h:16`; editorial |
| <a id="fn-uner_app_onatnetworkdisconnected"></a>`void UNER_App_OnAtNetworkDisconnected(void)` | Limpia el estado de enlace/sesiones correspondiente a desconexión AT. | `f4:Core/Inc/uner_app.h:184`; editorial |
| <a id="fn-uner_app_onnrflinkchanged"></a>`void UNER_App_OnNrfLinkChanged(uint8_t connected)` | Actualiza estado de enlace NRF y sus reportes. | `f4:Core/Inc/uner_app.h:152`; editorial |
| <a id="fn-uner_app_onuart1txcomplete"></a>`void UNER_App_OnUart1TxComplete(void)` | Notifica fin de TX y habilita transiciones diferidas. | `f4:Core/Inc/uner_app.h:181`; editorial |
| <a id="fn-uner_app_onusbdtrchanged"></a>`void UNER_App_OnUsbDtrChanged(uint8_t connected)` | Actualiza sesión/estado USB al cambiar DTR. | `f4:Core/Inc/uner_app.h:151`; editorial |
| <a id="fn-uner_app_poll"></a>`void UNER_App_Poll(void)` | Atiende protocolos y servicios diferidos de la aplicación. | `f4:Core/Inc/uner_app.h:148`; editorial |
| <a id="fn-uner_app_reportcarmodechanged"></a>`UNER_Status UNER_App_ReportCarModeChanged(uint8_t mode)` | Publica el modo de automóvil confirmado. | `f4:Core/Inc/uner_app.h:170`; editorial |
| <a id="fn-uner_app_reportmenuselectionchanged"></a>`UNER_Status UNER_App_ReportMenuSelectionChanged(uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Publica selección visible y cantidad vinculadas a screen code. | `f4:Core/Inc/uner_app.h:166`; editorial |
| <a id="fn-uner_app_reportnotificationchanged"></a>`UNER_Status UNER_App_ReportNotificationChanged(uint32_t screen_code, uint8_t source, uint16_t remaining_ms, uint16_t total_ms)` | Publica identidad y tiempos de notificación. | `f4:Core/Inc/uner_app.h:162`; editorial |
| <a id="fn-uner_app_reportscreenchanged"></a>`UNER_Status UNER_App_ReportScreenChanged(uint32_t screen_code, uint8_t source)` | Serializa identidad/origen de pantalla y publica evento. | `f4:Core/Inc/uner_app.h:161`; editorial |
| <a id="fn-uner_app_reportusbqtstate"></a>`UNER_Status UNER_App_ReportUsbQtState(uint8_t connected)` | Adapta la llamada a `UNER_App_ServiceUsbQtState` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:171`; structural |
| <a id="fn-uner_app_requestbinaryasset"></a>`UNER_Status UNER_App_RequestBinaryAsset(uint8_t asset_id)` | Solicita al otro extremo un asset por su ID lógico. | `f4:Core/Inc/uner_app.h:172`; editorial |
| <a id="fn-uner_app_requestwifinetworkdetail"></a>`UNER_Status UNER_App_RequestWifiNetworkDetail(const char *ssid)` | Solicita detalle del SSID indicado mediante el backend de protocolo. | `f4:Core/Inc/uner_app.h:154`; editorial |
| <a id="fn-uner_app_resetuart1rxpos"></a>`void UNER_App_ResetUart1RxPos(void)` | Adapta la llamada a `UNER_App_Uart1RxGetWritePos` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:186`; structural |
| <a id="fn-uner_app_sendcommand"></a>`UNER_Status UNER_App_SendCommand(uint8_t cmd, const uint8_t *payload, uint8_t len)` | Envía un comando de aplicación por la ruta configurada. | `f4:Core/Inc/uner_app.h:149`; editorial |
| <a id="fn-uner_app_sendespreboot"></a>`UNER_Status UNER_App_SendEspReboot(uint8_t boot_mode)` | Adapta la llamada a `UNER_App_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:155`; structural |
| <a id="fn-uner_app_sendusbalive"></a>`UNER_Status UNER_App_SendUsbAlive(uint32_t alive_count)` | Emite mantenimiento de presencia hacia el host USB según política de aplicación. | `f4:Core/Inc/uner_app.h:150`; editorial |
| <a id="fn-uner_app_sendwificredentialrequest"></a>`UNER_Status UNER_App_SendWifiCredentialRequest(const char *ssid)` | Inicia solicitud de ingreso de credenciales Wi-Fi; no transporta PIN de autorización como equivalente. | `f4:Core/Inc/uner_app.h:153`; editorial |
| <a id="fn-uner_app_setrouterforwardingenabled"></a>`void UNER_App_SetRouterForwardingEnabled(uint8_t enabled)` | Adapta la llamada a `UNER_Handle_SetForwardingEnabled` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:174`; structural |
| <a id="fn-uner_app_uart1rxgetwritepos"></a>`uint16_t UNER_App_Uart1RxGetWritePos(void)` | Adapta la llamada a `__HAL_DMA_GET_COUNTER` con el contexto de esta integración. | `f4:Core/Inc/uner_app.h:185`; structural |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-uner_app_findstream"></a>`static UNER_StreamState *UNER_App_FindStream(UNER_StreamState *streams, uint8_t transport_id, uint8_t node, uint8_t allocate)` | Operación Find Stream de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:177`; inferred-name |
| <a id="fn-uner_app_stopstreamendpoint"></a>`static void UNER_App_StopStreamEndpoint(UNER_StreamState *streams, uint8_t transport_id, uint8_t node)` | Adapta la llamada a `UNER_App_FindStream` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:191`; structural |
| <a id="fn-uner_app_stoptransportstreams"></a>`static void UNER_App_StopTransportStreams(uint8_t transport_id)` | Detiene Transport Streams dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:198`; inferred-name |
| <a id="fn-uner_app_minmpustreamperiod"></a>`static uint16_t UNER_App_MinMpuStreamPeriod(void)` | Operación Min Mpu Stream Period de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:208`; inferred-name |
| <a id="fn-uner_app_anympustreamactive"></a>`static uint8_t UNER_App_AnyMpuStreamActive(void)` | Operación Any Mpu Stream Active de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:221`; inferred-name |
| <a id="fn-uner_app_clearespproxy"></a>`static void UNER_App_ClearEspProxy(void)` | Limpia Esp Proxy dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:502`; inferred-name |
| <a id="fn-uner_app_clearwaitingvalidation"></a>`static void UNER_App_ClearWaitingValidation(void)` | Adapta la llamada a `UNER_App_ClearEspProxy` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:507`; structural |
| <a id="fn-uner_app_shownotification"></a>`static void UNER_App_ShowNotification(RenderFunction renderFn, uint16_t timeout_ms)` | Adapta la llamada a `OledUtils_ShowNotificationMs` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:516`; structural |
| <a id="fn-uner_app_showsimplenotification"></a>`static void UNER_App_ShowSimpleNotification(OledSimpleNotificationId notificationId, uint16_t timeout_ms)` | Adapta la llamada a `OledUtils_ShowSimpleNotificationMs` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:521`; structural |
| <a id="fn-uner_app_showconnectivitynotification"></a>`static void UNER_App_ShowConnectivityNotification(RenderFunction renderFn, uint16_t timeout_ms)` | Coordina `Dashboard_InvalidateConnectivity`, `OledUtils_ShowNotificationMsEx` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:526`; structural |
| <a id="fn-uner_app_showconnectivitysimplenotification"></a>`static void UNER_App_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId, uint16_t timeout_ms)` | Coordina `Dashboard_InvalidateConnectivity`, `OledUtils_ShowSimpleNotificationMsEx` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:535`; structural |
| <a id="fn-uner_app_showclientnotification"></a>`static void UNER_App_ShowClientNotification(OledSimpleNotificationId notificationId, uint8_t clientNumber, uint16_t timeout_ms)` | Coordina `Dashboard_InvalidateConnectivity`, `OledUtils_ShowNumberedSimpleNotificationMsEx` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:545`; structural |
| <a id="fn-uner_app_isconnectivitymutationcommand"></a>`static uint8_t UNER_App_IsConnectivityMutationCommand(uint8_t cmd)` | Indica si se cumple la condición Connectivity Mutation Command dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:554`; inferred-name |
| <a id="fn-uner_app_queueauthreply"></a>`static bool UNER_App_QueueAuthReply(uint8_t transport_id, uint8_t dst_node, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Coordina `memcpy` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:571`; structural |
| <a id="fn-uner_app_serviceauthreplyqueue"></a>`static void UNER_App_ServiceAuthReplyQueue(void)` | Coordina `UNER_Send`, `memset` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:599`; structural |
| <a id="fn-uner_app_serviceusbqtstate"></a>`static void UNER_App_ServiceUsbQtState(void)` | Coordina `UNER_Send` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:623`; structural |
| <a id="fn-uner_app_requestauthnotification"></a>`static void UNER_App_RequestAuthNotification(OledSimpleNotificationId notification_id)` | Solicita Auth Notification dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:643`; inferred-name |
| <a id="fn-uner_app_servicedeferredauthnotification"></a>`static void UNER_App_ServiceDeferredAuthNotification(void)` | Coordina `Permission_GetRequestedPermission`, `UNER_App_ShowSimpleNotification` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:678`; structural |
| <a id="fn-uner_app_servicedeferredmcureset"></a>`static void UNER_App_ServiceDeferredMcuReset(void)` | Coordina `CDC_IsDtrActive_FS`, `HAL_GetTick`, `NVIC_SystemReset` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:689`; structural |
| <a id="fn-uner_app_findspecbyid"></a>`static const UNER_CommandSpec *UNER_App_FindSpecById(uint8_t cmd)` | Operación Find Spec By Id de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:725`; inferred-name |
| <a id="fn-uner_app_sendoledcanvasresponse"></a>`static uint8_t UNER_App_SendOledCanvasResponse(uint8_t transport_id, uint8_t destination_node, const uint8_t *payload, uint8_t len, void *ctx)` | Adapta la llamada a `UNER_Send` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:736`; structural |
| <a id="fn-uner_app_isespevent"></a>`static uint8_t UNER_App_IsEspEvent(uint8_t cmd)` | Indica si se cumple la condición Esp Event dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:752`; inferred-name |
| <a id="fn-uner_app_isespcontrolplanecommand"></a>`static uint8_t UNER_App_IsEspControlPlaneCommand(uint8_t cmd)` | Indica si se cumple la condición Esp Control Plane Command dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:760`; inferred-name |
| <a id="fn-uner_app_isespproxycommand"></a>`static uint8_t UNER_App_IsEspProxyCommand(uint8_t cmd)` | Indica si se cumple la condición Esp Proxy Command dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:793`; inferred-name |
| <a id="fn-uner_app_relayesppackettousb"></a>`static uint8_t UNER_App_RelayEspPacketToUsb(const UNER_Packet *packet)` | Coordina `CDC_IsDtrActive_FS`, `UNER_App_IsEspEvent`, `UNER_Send` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:823`; structural |
| <a id="fn-uner_app_forwardusbcommandtoesp"></a>`static uint8_t UNER_App_ForwardUsbCommandToEsp(const UNER_Packet *packet)` | Coordina `HAL_GetTick`, `UNER_App_FindSpecById`, `UNER_App_IsEspProxyCommand` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:878`; structural |
| <a id="fn-uner_app_ispublicremotecommand"></a>`static uint8_t UNER_App_IsPublicRemoteCommand(uint8_t cmd)` | Indica si se cumple la condición Public Remote Command dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1044`; inferred-name |
| <a id="fn-uner_app_allowforward"></a>`static uint8_t UNER_App_AllowForward(void *ctx, const UNER_Packet *packet, uint8_t out_transport_id)` | Coordina `HAL_GetTick`, `Permission_IsRemoteNodeAuthorizedNow`, `Permission_TouchRemoteNode` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1065`; structural |
| <a id="fn-uner_app_isremoteuicommand"></a>`static uint8_t UNER_App_IsRemoteUiCommand(uint8_t cmd)` | Indica si se cumple la condición Remote Ui Command dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1103`; inferred-name |
| <a id="fn-uner_app_onanypacket"></a>`static void UNER_App_OnAnyPacket(void *ctx, const UNER_Packet *packet)` | Atiende la notificación de Any Packet dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1109`; inferred-name |
| <a id="fn-uner_app_broadcastiremitterschanged"></a>`static void UNER_App_BroadcastIrEmittersChanged(void)` | Coordina `APP_IsIrLightsEnabled`, `APP_IsIrLineEnabled`, `APP_IsIrObjectsEnabled` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1136`; structural |
| <a id="fn-uner_app_executecommand"></a>`static void UNER_App_ExecuteCommand(void *ctx, const UNER_Packet *packet)` | Coordina `APP_AutoMotionRequest`, `APP_ESPWifiApFallbackResponse`, `APP_GetLineState` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1158`; structural |
| <a id="fn-uner_app_initconfig"></a>`static void UNER_App_InitConfig(UNER_CoreConfig *cfg)` | Inicializa Config dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:1821`; inferred-name |
| <a id="fn-uner_app_setstaip"></a>`static void UNER_App_SetStaIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)` | Configura Sta Ip dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2381`; inferred-name |
| <a id="fn-uner_app_setapip"></a>`static void UNER_App_SetApIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)` | Configura Ap Ip dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2394`; inferred-name |
| <a id="fn-uner_app_updatemodeflags"></a>`static void UNER_App_UpdateModeFlags(uint8_t mode)` | Actualiza Mode Flags dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2407`; inferred-name |
| <a id="fn-uner_app_setoledssidfrombytes"></a>`static void UNER_App_SetOledSsidFromBytes(const uint8_t *ssid, uint8_t len)` | Configura Oled Ssid From Bytes dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2431`; inferred-name |
| <a id="fn-uner_app_storewifiinfossid"></a>`static void UNER_App_StoreWifiInfoSsid(const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_App_SetOledSsidFromBytes` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:2452`; structural |
| <a id="fn-uner_app_readespbootresetreason"></a>`static uint8_t UNER_App_ReadEspBootResetReason(const UNER_Packet *packet)` | Lee Esp Boot Reset Reason dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2466`; inferred-name |
| <a id="fn-uner_app_storetargetcredentialsssid"></a>`static void UNER_App_StoreTargetCredentialsSsid(const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_App_SetOledSsidFromBytes` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:2484`; structural |
| <a id="fn-uner_app_storenetworkippayload"></a>`static void UNER_App_StoreNetworkIpPayload(const uint8_t *payload, uint8_t len)` | Coordina `SET_FLAG`, `UNER_App_MarkStaActive`, `UNER_App_SetApIp` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2498`; structural |
| <a id="fn-uner_app_setwifinetworkencryptiontext"></a>`static void UNER_App_SetWifiNetworkEncryptionText(uint8_t index, uint8_t encryption_type)` | Configura Wifi Network Encryption Text dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2518`; inferred-name |
| <a id="fn-uner_app_findwifinetworkindexbyssid"></a>`static int8_t UNER_App_FindWifiNetworkIndexBySsid(const uint8_t *ssid, uint8_t len)` | Coordina `memcmp`, `strlen` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2563`; structural |
| <a id="fn-uner_app_isstaactive"></a>`static uint8_t UNER_App_IsStaActive(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:2579`; structural |
| <a id="fn-uner_app_markstaactive"></a>`static void UNER_App_MarkStaActive(void)` | Coordina `CLEAR_FLAG`, `SET_FLAG` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2584`; structural |
| <a id="fn-uner_app_showwificonnectednotification"></a>`static void UNER_App_ShowWifiConnectedNotification(uint8_t was_wifi_active)` | Coordina `OledUtils_IsNotificationShowing`, `OledUtils_IsSimpleNotificationShowing`, `UNER_App_ShowConnectivityNotification` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2593`; structural |
| <a id="fn-uner_app_requestnetworkuirefresh"></a>`static void UNER_App_RequestNetworkUiRefresh(void)` | Adapta la llamada a `Dashboard_InvalidateConnectivity` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:2607`; structural |
| <a id="fn-uner_app_parsefirmwareresponse"></a>`static void UNER_App_ParseFirmwareResponse(const UNER_Packet *packet)` | Decodifica Firmware Response dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2612`; inferred-name |
| <a id="fn-uner_app_handlegetstatuslocal"></a>`static void UNER_App_HandleGetStatusLocal(const UNER_Packet *packet)` | Procesa Get Status Local dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2640`; inferred-name |
| <a id="fn-uner_app_parsestatusresponse"></a>`static void UNER_App_ParseStatusResponse(const UNER_Packet *packet)` | Decodifica Status Response dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2695`; inferred-name |
| <a id="fn-uner_app_parsewififinalizer"></a>`static void UNER_App_ParseWifiFinalizer(const UNER_Packet *packet)` | Decodifica Wifi Finalizer dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2802`; inferred-name |
| <a id="fn-uner_app_servicewifiscanresults"></a>`static void UNER_App_ServiceWifiScanResults(void)` | Coordina `HAL_GetTick`, `IS_FLAG_SET`, `UNER_App_SendCommand` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2857`; structural |
| <a id="fn-uner_app_servicempustream"></a>`static void UNER_App_ServiceMpuStream(void)` | Coordina `HAL_GetTick`, `Permission_TouchRemoteNode`, `UNER_App_BuildMpuSnapshotPayload` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2891`; structural |
| <a id="fn-uner_app_serviceirstream"></a>`static void UNER_App_ServiceIrStream(void)` | Coordina `HAL_GetTick`, `Permission_TouchRemoteNode`, `UNER_App_BuildIrSnapshotPayload` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2923`; structural |
| <a id="fn-uner_app_stopunauthorizedremotestreams"></a>`static void UNER_App_StopUnauthorizedRemoteStreams(void)` | Detiene Unauthorized Remote Streams dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2944`; inferred-name |
| <a id="fn-uner_app_parsescanresultsresponse"></a>`static void UNER_App_ParseScanResultsResponse(const UNER_Packet *packet)` | Decodifica Scan Results Response dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:2963`; inferred-name |
| <a id="fn-uner_app_parsenetworkdetail"></a>`static void UNER_App_ParseNetworkDetail(const UNER_Packet *packet)` | Decodifica Network Detail dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3036`; inferred-name |
| <a id="fn-uner_app_parsewificredentialswebresult"></a>`static void UNER_App_ParseWifiCredentialsWebResult(const UNER_Packet *packet)` | Decodifica Wifi Credentials Web Result dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3075`; inferred-name |
| <a id="fn-uner_app_resetbinaryassetstate"></a>`static void UNER_App_ResetBinaryAssetState(uint8_t asset_id)` | Restablece Binary Asset State dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3131`; inferred-name |
| <a id="fn-uner_app_parsebinaryassetstream"></a>`static void UNER_App_ParseBinaryAssetStream(const UNER_Packet *packet)` | Decodifica Binary Asset Stream dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3143`; inferred-name |
| <a id="fn-uner_app_writescreenreportpayload"></a>`static void UNER_App_WriteScreenReportPayload(uint8_t *payload, uint32_t screen_code, uint8_t source)` | Escribe Screen Report Payload dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3193`; inferred-name |
| <a id="fn-uner_app_writemenuselectionreportpayload"></a>`static void UNER_App_WriteMenuSelectionReportPayload(uint8_t *payload, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Escribe Menu Selection Report Payload dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3206`; inferred-name |
| <a id="fn-uner_app_handleresetmcu"></a>`static void UNER_App_HandleResetMcu(const UNER_Packet *packet)` | Procesa Reset Mcu dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3221`; inferred-name |
| <a id="fn-uner_app_reply"></a>`static UNER_Status UNER_App_Reply(const UNER_Packet *packet, uint8_t command_id, const uint8_t *payload, uint8_t length)` | Adapta la llamada a `UNER_Send` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:3259`; structural |
| <a id="fn-uner_app_sendcommandstatus"></a>`static void UNER_App_SendCommandStatus(const UNER_Packet *packet, uint8_t status)` | Adapta la llamada a `UNER_App_Reply` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:3273`; structural |
| <a id="fn-uner_app_sendcommandnack"></a>`static void UNER_App_SendCommandNack(const UNER_Packet *packet, uint8_t status)` | Envía Command Nack dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3289`; inferred-name |
| <a id="fn-uner_app_commandtransportallowed"></a>`static uint8_t UNER_App_CommandTransportAllowed(uint8_t cmd, uint8_t transport_id)` | Coordina `AppPreferences_GetExtProfile`, `UNER_App_FindSpecById` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3316`; structural |
| <a id="fn-uner_app_packetscreenmatchescurrent"></a>`static uint8_t UNER_App_PacketScreenMatchesCurrent(const UNER_Packet *packet)` | Coordina `MenuSys_GetCurrentScreenCode`, `UNER_App_ReadU32Le` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3341`; structural |
| <a id="fn-uner_app_dispatchremoteuievent"></a>`static uint8_t UNER_App_DispatchRemoteUiEvent(UserEvent_t event)` | Adapta la llamada a `UiEventRouter_HandleEvent` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:3351`; structural |
| <a id="fn-uner_app_readu32le"></a>`static uint32_t UNER_App_ReadU32Le(const uint8_t *payload)` | Decodifica uint32 little endian desde payload. | `f4:Core/Src/uner_app.c:3358`; editorial |
| <a id="fn-uner_app_readu16le"></a>`static uint16_t UNER_App_ReadU16Le(const uint8_t *payload)` | Decodifica uint16 little endian desde payload. | `f4:Core/Src/uner_app.c:3366`; editorial |
| <a id="fn-uner_app_readfloatle"></a>`static float UNER_App_ReadFloatLe(const uint8_t *payload)` | Decodifica float de cuatro bytes según representación del target. | `f4:Core/Src/uner_app.c:3372`; editorial |
| <a id="fn-uner_app_writeu16le"></a>`static void UNER_App_WriteU16Le(uint8_t *payload, uint16_t value)` | Escribe uint16 en little endian. | `f4:Core/Src/uner_app.c:3386`; editorial |
| <a id="fn-uner_app_writeu32le"></a>`static void UNER_App_WriteU32Le(uint8_t *payload, uint32_t value)` | Escribe uint32 en little endian. | `f4:Core/Src/uner_app.c:3392`; editorial |
| <a id="fn-uner_app_authdigitsarevalid"></a>`static uint8_t UNER_App_AuthDigitsAreValid(const uint8_t *digits)` | Operación Auth Digits Are Valid de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:3400`; inferred-name |
| <a id="fn-uner_app_sendauthresponse"></a>`static void UNER_App_SendAuthResponse(const UNER_Packet *packet, uint8_t request_id, uint8_t scope, uint8_t granted, uint32_t ttl_ms, uint8_t attempts_left)` | Envía Auth Response dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3414`; inferred-name |
| <a id="fn-uner_app_handleauthvalidatepin"></a>`static void UNER_App_HandleAuthValidatePin(const UNER_Packet *packet)` | Procesa Auth Validate Pin dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3436`; inferred-name |
| <a id="fn-uner_app_handleauthsession"></a>`static void UNER_App_HandleAuthSession(const UNER_Packet *packet)` | Procesa Auth Session dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3565`; inferred-name |
| <a id="fn-uner_app_writefloatle"></a>`static void UNER_App_WriteFloatLe(uint8_t *payload, float value)` | Escribe float de cuatro bytes en little endian. | `f4:Core/Src/uner_app.c:3598`; editorial |
| <a id="fn-uner_app_appendtextfield"></a>`static uint8_t UNER_App_AppendTextField(uint8_t *payload, uint8_t payload_len, uint8_t *offset, const char *text, uint8_t max_text_len)` | Coordina `memcpy` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3612`; structural |
| <a id="fn-uner_app_parsepreferencevalue"></a>`static uint8_t UNER_App_ParsePreferenceValue(const uint8_t *payload, uint8_t len, uint8_t *offset, const AppPreferencesDescriptor_t *desc, AppPreferences_t *prefs)` | Decodifica Preference Value dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3638`; inferred-name |
| <a id="fn-uner_app_handlegetpreferences"></a>`static void UNER_App_HandleGetPreferences(const UNER_Packet *packet)` | Procesa Get Preferences dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3719`; inferred-name |
| <a id="fn-uner_app_handlesetpreferences"></a>`static void UNER_App_HandleSetPreferences(const UNER_Packet *packet)` | Procesa Set Preferences dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3807`; inferred-name |
| <a id="fn-uner_app_handlegetapconfig"></a>`static void UNER_App_HandleGetApConfig(const UNER_Packet *packet)` | Procesa Get Ap Config dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3869`; inferred-name |
| <a id="fn-uner_app_handleupdateapconfig"></a>`static void UNER_App_HandleUpdateApConfig(const UNER_Packet *packet)` | Procesa Update Ap Config dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3893`; inferred-name |
| <a id="fn-uner_app_handleseti2cpolicy"></a>`static void UNER_App_HandleSetI2CPolicy(const UNER_Packet *packet)` | Procesa Set I2 CPolicy dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3974`; inferred-name |
| <a id="fn-uner_app_handlegeti2cpolicy"></a>`static void UNER_App_HandleGetI2CPolicy(const UNER_Packet *packet)` | Procesa Get I2 CPolicy dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:3995`; inferred-name |
| <a id="fn-uner_app_buildmpusnapshotpayload"></a>`static uint8_t UNER_App_BuildMpuSnapshotPayload(uint8_t *payload, MPU9250_Snapshot_t *snapshot_out)` | Construye Mpu Snapshot Payload dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4009`; inferred-name |
| <a id="fn-uner_app_handlegetmpusnapshot"></a>`static void UNER_App_HandleGetMpuSnapshot(const UNER_Packet *packet)` | Procesa Get Mpu Snapshot dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4077`; inferred-name |
| <a id="fn-uner_app_mpustreamtransportallowed"></a>`static uint8_t UNER_App_MpuStreamTransportAllowed(uint8_t transport_id)` | Operación Mpu Stream Transport Allowed de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:4091`; inferred-name |
| <a id="fn-uner_app_handlesetmpustream"></a>`static void UNER_App_HandleSetMpuStream(const UNER_Packet *packet)` | Procesa Set Mpu Stream dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4097`; inferred-name |
| <a id="fn-uner_app_handlestopmpustream"></a>`static void UNER_App_HandleStopMpuStream(const UNER_Packet *packet)` | Procesa Stop Mpu Stream dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4138`; inferred-name |
| <a id="fn-uner_app_normalizeirraw"></a>`static uint16_t UNER_App_NormalizeIrRaw(uint8_t sensor_index, uint16_t raw)` | Adapta la llamada a `TCRT5000_NormalizeForDisplay` con el contexto de esta integración. | `f4:Core/Src/uner_app.c:4146`; structural |
| <a id="fn-uner_app_buildirsnapshotpayload"></a>`static uint8_t UNER_App_BuildIrSnapshotPayload(uint8_t *payload, uint16_t sample_seq, uint16_t period_ms, uint32_t tick_ms)` | Construye Ir Snapshot Payload dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4158`; inferred-name |
| <a id="fn-uner_app_irstreamtransportallowed"></a>`static uint8_t UNER_App_IrStreamTransportAllowed(uint8_t transport_id)` | Operación Ir Stream Transport Allowed de UNER de aplicación y contrato del robot; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_app.c:4228`; inferred-name |
| <a id="fn-uner_app_handlegetirsnapshot"></a>`static void UNER_App_HandleGetIrSnapshot(const UNER_Packet *packet)` | Procesa Get Ir Snapshot dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4234`; inferred-name |
| <a id="fn-uner_app_handlesetirstream"></a>`static void UNER_App_HandleSetIrStream(const UNER_Packet *packet)` | Procesa Set Ir Stream dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4253`; inferred-name |
| <a id="fn-uner_app_handlestopirstream"></a>`static void UNER_App_HandleStopIrStream(const UNER_Packet *packet)` | Procesa Stop Ir Stream dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4291`; inferred-name |
| <a id="fn-uner_app_handlegettelemetrysnapshot"></a>`static void UNER_App_HandleGetTelemetrySnapshot(const UNER_Packet *packet)` | Procesa Get Telemetry Snapshot dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4299`; inferred-name |
| <a id="fn-uner_app_handlegetcontrolsnapshot"></a>`static void UNER_App_HandleGetControlSnapshot(const UNER_Packet *packet)` | Procesa Get Control Snapshot dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4378`; inferred-name |
| <a id="fn-uner_app_handlegetfirmwareinfo"></a>`static void UNER_App_HandleGetFirmwareInfo(const UNER_Packet *packet)` | Procesa Get Firmware Info dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4449`; inferred-name |
| <a id="fn-uner_app_handlerequestfirmwarecompat"></a>`static void UNER_App_HandleRequestFirmwareCompat(const UNER_Packet *packet)` | Procesa Request Firmware Compat dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4473`; inferred-name |
| <a id="fn-uner_app_handlegetbuildinfo"></a>`static void UNER_App_HandleGetBuildInfo(const UNER_Packet *packet)` | Procesa Get Build Info dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4503`; inferred-name |
| <a id="fn-uner_app_ispidvaluevalid"></a>`static uint8_t UNER_App_IsPidValueValid(float value)` | Indica si se cumple la condición Pid Value Valid dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4530`; inferred-name |
| <a id="fn-uner_app_handlemotion"></a>`static void UNER_App_HandleMotion(const UNER_Packet *packet)` | Procesa Motion dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4538`; inferred-name |
| <a id="fn-uner_app_handlegetbalancepid"></a>`static void UNER_App_HandleGetBalancePid(const UNER_Packet *packet)` | Procesa Get Balance Pid dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4591`; inferred-name |
| <a id="fn-uner_app_handlesetbalancepid"></a>`static void UNER_App_HandleSetBalancePid(const UNER_Packet *packet)` | Procesa Set Balance Pid dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4607`; inferred-name |
| <a id="fn-uner_app_buildbootreportpayload"></a>`static uint8_t UNER_App_BuildBootReportPayload( uint8_t payload[WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE])` | Construye Boot Report Payload dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4690`; inferred-name |
| <a id="fn-uner_app_sendbootreport"></a>`static UNER_Status UNER_App_SendBootReport(uint8_t transport_id, uint8_t destination_node)` | Envía Boot Report dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4722`; inferred-name |
| <a id="fn-uner_app_servicebootreport"></a>`static void UNER_App_ServiceBootReport(void)` | Coordina `AppPreferences_GetExtProfile`, `CDC_IsDtrActive_FS`, `UNER_App_SendBootReport` para UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4748`; structural |
| <a id="fn-uner_app_handlegetbootreport"></a>`static void UNER_App_HandleGetBootReport(const UNER_Packet *packet)` | Procesa Get Boot Report dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4775`; inferred-name |
| <a id="fn-uner_app_handlegetextensionprofile"></a>`static void UNER_App_HandleGetExtensionProfile(const UNER_Packet *packet)` | Procesa Get Extension Profile dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4789`; inferred-name |
| <a id="fn-uner_app_handlesetextensionprofile"></a>`static void UNER_App_HandleSetExtensionProfile(const UNER_Packet *packet)` | Guarda el perfil y responde por el mismo ID con status; revisar metadata ACK. | `f4:Core/Src/uner_app.c:4798`; editorial |
| <a id="fn-uner_app_handlerebootintoprofile"></a>`static void UNER_App_HandleRebootIntoProfile(const UNER_Packet *packet)` | Procesa Reboot Into Profile dentro de UNER de aplicación y contrato del robot. | `f4:Core/Src/uner_app.c:4818`; inferred-name |
