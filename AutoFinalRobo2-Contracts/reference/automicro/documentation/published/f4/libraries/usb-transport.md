# Transporte UNER USB CDC

ID: `f4:module/Core/Inc/uner_transport_usbcdc.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Conecta bytes USB CDC a UNER y mantiene TX diferido con prioridades y diagnóstico de colas.

Recepción, cola TX, reintentos, conexión y contadores; el host Qt implementa correlación/interpretación por separado.

<a id="modularity"></a>
## Modularidad observada

Adaptador ligado a stack ST USB CDC y política de conexión del dispositivo.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Contexto con buffers/colas RX/TX y contadores de overflow/full/drop/deferred; UNER_Transport.ctx conserva su instancia de plataforma. Consultar PendingTx no es equivalente a DTR activo.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init instala transporte; OnRx copia/entrega recepción.
- ServiceTx intenta enviar; OnTxComplete libera y avanza.
- OnDisconnect limpia estado según política; la aplicación revoca sesión/streams correspondientes.

## Trabajo en tándem

- usbd_cdc_if.c entrega callbacks; UNER_App gobierna alive, DTR, reset y permisos Qt.

## Precauciones y límites

- BUSY del stack requiere reintento; encolado no equivale a enviado.
- Contadores de descarte deben interpretarse junto a prioridad y profundidad de cola.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/uner_transport_usbcdc.h` | `stdint.h`, `uner_transport.h` | `3fa0241dafb3531e4970bff6d5bd1ef86ee566024c7b74727045bdebab70679f` |
| `Core/Src/uner_transport_usbcdc.c` | `uner_transport_usbcdc.h`, `string.h`, `usbd_cdc_if.h` | `0014ed86781e12531c06ae7fe1252e47f07cf83450dec2260eb2713c311b3c6d` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UNER_UsbCdcTxFrame`

`f4:Core/Inc/uner_transport_usbcdc.h:22`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t priority` | Prioridad base registrada; no confundir con pending_prio ni asumir que gobierna la selección. |
| `uint8_t data[UNER_USB_CDC_TX_BUFFER_SIZE]` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_TransportUsbCdc`

`f4:Core/Inc/uner_transport_usbcdc.h:28`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UNER_Transport base` | Dato de estado/configuración `base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t rx_head` | Dato de estado/configuración `rx_head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t rx_tail` | Dato de estado/configuración `rx_tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t tx_busy` | Dato de estado/configuración `tx_busy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t tx_service_pending` | Dato de estado/configuración `tx_service_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_queue_count` | Dato de estado/configuración `tx_queue_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t next_tx_priority` | Dato de estado/configuración `next_tx_priority`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint32_t rx_overflow_count` | Dato de estado/configuración `rx_overflow_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint32_t tx_queue_full_count` | Dato de estado/configuración `tx_queue_full_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint32_t tx_queue_drop_count` | Dato de estado/configuración `tx_queue_drop_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint32_t tx_deferred_count` | Dato de estado/configuración `tx_deferred_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t rx_ring[UNER_USB_CDC_RX_RING_SIZE]` | Dato de estado/configuración `rx_ring`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tx_buffer[UNER_USB_CDC_TX_BUFFER_SIZE]` | Dato de estado/configuración `tx_buffer`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_UsbCdcTxFrame tx_queue[UNER_USB_CDC_TX_QUEUE_SLOTS]` | Dato de estado/configuración `tx_queue`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uner_transportusbcdc_getqueuedtxcount"></a>`uint8_t UNER_TransportUsbCdc_GetQueuedTxCount(void)` | Consulta profundidad de cola TX. | `f4:Core/Inc/uner_transport_usbcdc.h:53`; editorial |
| <a id="fn-uner_transportusbcdc_getrxoverflowcount"></a>`uint32_t UNER_TransportUsbCdc_GetRxOverflowCount(void)` | Consulta desbordamientos de recepción. | `f4:Core/Inc/uner_transport_usbcdc.h:54`; editorial |
| <a id="fn-uner_transportusbcdc_gettxdeferredcount"></a>`uint32_t UNER_TransportUsbCdc_GetTxDeferredCount(void)` | Consulta envíos diferidos por indisponibilidad. | `f4:Core/Inc/uner_transport_usbcdc.h:57`; editorial |
| <a id="fn-uner_transportusbcdc_gettxqueuedropcount"></a>`uint32_t UNER_TransportUsbCdc_GetTxQueueDropCount(void)` | Consulta frames descartados de la cola TX. | `f4:Core/Inc/uner_transport_usbcdc.h:56`; editorial |
| <a id="fn-uner_transportusbcdc_gettxqueuefullcount"></a>`uint32_t UNER_TransportUsbCdc_GetTxQueueFullCount(void)` | Consulta rechazos por cola TX llena. | `f4:Core/Inc/uner_transport_usbcdc.h:55`; editorial |
| <a id="fn-uner_transportusbcdc_haspendingrx"></a>`uint8_t UNER_TransportUsbCdc_HasPendingRx(void)` | Consulta si quedan bytes RX por consumir. | `f4:Core/Inc/uner_transport_usbcdc.h:51`; editorial |
| <a id="fn-uner_transportusbcdc_haspendingtx"></a>`uint8_t UNER_TransportUsbCdc_HasPendingTx(void)` | Consulta si hay TX encolado o en curso. | `f4:Core/Inc/uner_transport_usbcdc.h:52`; editorial |
| <a id="fn-uner_transportusbcdc_init"></a>`UNER_Status UNER_TransportUsbCdc_Init(UNER_TransportUsbCdc *transport, uint8_t transport_id)` | Conecta el transporte CDC y sus buffers/colas. | `f4:Core/Inc/uner_transport_usbcdc.h:45`; editorial |
| <a id="fn-uner_transportusbcdc_ondisconnect"></a>`void UNER_TransportUsbCdc_OnDisconnect(void)` | Limpia recepción/transmisión pendiente al desconectar. | `f4:Core/Inc/uner_transport_usbcdc.h:48`; editorial |
| <a id="fn-uner_transportusbcdc_onrx"></a>`void UNER_TransportUsbCdc_OnRx(const uint8_t *buf, uint32_t len)` | Entrega bytes USB recibidos a la cola del transporte. | `f4:Core/Inc/uner_transport_usbcdc.h:46`; editorial |
| <a id="fn-uner_transportusbcdc_ontxcomplete"></a>`void UNER_TransportUsbCdc_OnTxComplete(void)` | Marca fin de transferencia USB y permite avanzar la cola. | `f4:Core/Inc/uner_transport_usbcdc.h:47`; editorial |
| <a id="fn-uner_transportusbcdc_servicetx"></a>`void UNER_TransportUsbCdc_ServiceTx(void)` | Intenta despachar el siguiente frame disponible al stack CDC. | `f4:Core/Inc/uner_transport_usbcdc.h:49`; editorial |
| <a id="fn-uner_transportusbcdc_setnexttxpriority"></a>`void UNER_TransportUsbCdc_SetNextTxPriority(uint8_t priority)` | Configura prioridad para la siguiente solicitud de envío. | `f4:Core/Inc/uner_transport_usbcdc.h:50`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-uner_usbnextindex"></a>`static uint16_t UNER_UsbNextIndex(uint16_t index)` | Operación UNER Usb Next Index de Transporte UNER USB CDC; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_usbcdc.c:17`; inferred-name |
| <a id="fn-uner_usbringpush"></a>`static uint8_t UNER_UsbRingPush(UNER_TransportUsbCdc *usb, uint8_t byte)` | Adapta la llamada a `UNER_UsbNextIndex` con el contexto de esta integración. | `f4:Core/Src/uner_transport_usbcdc.c:23`; structural |
| <a id="fn-uner_usbringpop"></a>`static uint8_t UNER_UsbRingPop(UNER_TransportUsbCdc *usb, uint8_t *byte)` | Adapta la llamada a `UNER_UsbNextIndex` con el contexto de esta integración. | `f4:Core/Src/uner_transport_usbcdc.c:41`; structural |
| <a id="fn-uner_usbclearrx"></a>`static void UNER_UsbClearRx(UNER_TransportUsbCdc *usb)` | Operación UNER Usb Clear Rx de Transporte UNER USB CDC; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_usbcdc.c:52`; inferred-name |
| <a id="fn-uner_usbcleartx"></a>`static void UNER_UsbClearTx(UNER_TransportUsbCdc *usb)` | Operación UNER Usb Clear Tx de Transporte UNER USB CDC; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_usbcdc.c:62`; inferred-name |
| <a id="fn-uner_usbpriorityfromframe"></a>`static uint8_t UNER_UsbPriorityFromFrame(UNER_TransportUsbCdc *usb, const uint8_t *buf, uint16_t len)` | Operación UNER Usb Priority From Frame de Transporte UNER USB CDC; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_usbcdc.c:74`; inferred-name |
| <a id="fn-uner_usbqueueremoveat"></a>`static void UNER_UsbQueueRemoveAt(UNER_TransportUsbCdc *usb, uint8_t index)` | Operación UNER Usb Queue Remove At de Transporte UNER USB CDC; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_transport_usbcdc.c:103`; inferred-name |
| <a id="fn-uner_usbqueueframe"></a>`static UNER_Status UNER_UsbQueueFrame(UNER_TransportUsbCdc *usb, const uint8_t *buf, uint16_t len, uint8_t priority)` | Coordina `UNER_UsbQueueRemoveAt`, `memcpy` para Transporte UNER USB CDC. | `f4:Core/Src/uner_transport_usbcdc.c:116`; structural |
| <a id="fn-uner_usbstarttx"></a>`static UNER_Status UNER_UsbStartTx(UNER_TransportUsbCdc *usb, const uint8_t *buf, uint16_t len)` | Coordina `CDC_IsDtrActive_FS`, `CDC_Transmit_FS`, `memcpy` para Transporte UNER USB CDC. | `f4:Core/Src/uner_transport_usbcdc.c:163`; structural |
| <a id="fn-uner_transportusbcdc_pollrx"></a>`static UNER_Status UNER_TransportUsbCdc_PollRx(UNER_Transport *transport, UNER_Core *core)` | Coordina `UNER_Core_PushByte`, `UNER_UsbRingPop` para Transporte UNER USB CDC. | `f4:Core/Src/uner_transport_usbcdc.c:189`; structural |
| <a id="fn-uner_transportusbcdc_trysend"></a>`static UNER_Status UNER_TransportUsbCdc_TrySend(UNER_Transport *transport, const uint8_t *buf, uint16_t len)` | Coordina `CDC_IsDtrActive_FS`, `UNER_UsbPriorityFromFrame`, `UNER_UsbQueueFrame` para Transporte UNER USB CDC. | `f4:Core/Src/uner_transport_usbcdc.c:206`; structural |
