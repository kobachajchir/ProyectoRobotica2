# Parser UNER v2 ESP

ID: `esp:module/src/uner_core.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Reconoce frames UNER y construye su representación binaria con longitud, ruta, ID y checksum.

Parser/serializador; no autentica sesiones Web ni interpreta acciones físicas.

<a id="modularity"></a>
## Modularidad observada

Núcleo C separado del transporte Arduino; revisar callbacks y almacenamiento suministrado, sin asumir que el handle ESP es idéntico al F4.

<a id="state"></a>
## Handle, estado y atributos externos

El core mantiene FSM y almacenamiento de recepción según sus tipos; los payloads son bytes de protocolo, no structs F4 compartidos por dirección.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Inicializar antes de entregar bytes del transporte.
- Procesar por byte, completar/descarte por estructura y entregar paquetes a la capa dispositivo/handle.

<a id="interactions"></a>
## Interacciones

- Serial entrega bytes; device/handle conectan callbacks y comandos.
- El esquema wire es común a F4, pero funciones y organización internas pueden diferir.

<a id="limitations"></a>
## Precauciones y límites

- Checksum válido no acredita argumentos válidos ni autorización.
- La vida del payload termina según su dueño; copiar si se difiere el procesamiento.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uner_core.c` | c-cpp-structural | `2af4d9f75a7d83e7f6c4fc270671f0b7bcb6eb9865b3de235edd27545b4d6e70` |
| `src/uner_core.h` | c-cpp-structural | `50eb06b7f10210d7a14fdb42a562e93d0930431fa289d78fcea28b2addc4cc26` |
| `src/uner_v2.h` | c-cpp-structural | `da8f5d054d25643c8b4547fa3d1517ed54631a0c03975051bcd4c553acdeb50d` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNER_CoreConfig`

`esp:src/uner_core.h:11`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t this_node` | Dato de estado/configuración `this_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t accept_src_mask` | Dato de estado/configuración `accept_src_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t accept_dst_mask` | Dato de estado/configuración `accept_dst_mask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t accept_broadcast` | Dato de estado/configuración `accept_broadcast`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t default_max_payload` | Dato de estado/configuración `default_max_payload`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_OnPacketCb on_packet` | Dato de estado/configuración `on_packet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *cb_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |

### `UNER_Core`

`esp:src/uner_core.h:35`

| Atributo / firma | Propósito |
| --- | --- |
| `UNER_CoreConfig cfg` | Dato de estado/configuración `cfg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_Queue queue` | Dato de estado/configuración `queue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_ParserState state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t len_expected` | Dato de estado/configuración `len_expected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload_index` | Dato de estado/configuración `payload_index`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t chk_acc` | Dato de estado/configuración `chk_acc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t route` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ver` | Dato de estado/configuración `ver`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t max_payload_transport` | Dato de estado/configuración `max_payload_transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t drop_frame` | Dato de estado/configuración `drop_frame`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload_buf[UNER_MAX_PAYLOAD]` | Dato de estado/configuración `payload_buf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ok_frames` | Dato de estado/configuración `ok_frames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t chk_fail` | Dato de estado/configuración `chk_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t len_fail` | Dato de estado/configuración `len_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t header_resync` | Dato de estado/configuración `header_resync`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t token_fail` | Dato de estado/configuración `token_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ver_fail` | Dato de estado/configuración `ver_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t queue_overflow` | Dato de estado/configuración `queue_overflow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t filter_drop` | Dato de estado/configuración `filter_drop`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_OnPacketCb`

`esp:src/uner_core.h:8`

Callback: `void (*UNER_OnPacketCb)(void *ctx, const UNER_Packet *p)`. El parser invoca este callback cuando completa un paquete válido; packet/buffer son vistas del parser y user pertenece al registrador. Consumir o copiar antes de reutilizar almacenamiento.

### `UNER_Packet`

`esp:src/uner_v2.h:46`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t ver` | Dato de estado/configuración `ver`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t src` | Dato de estado/configuración `src`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t dst` | Dato de estado/configuración `dst`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t *payload` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-uner_buildframe"></a>`UNER_Status UNER_BuildFrame(uint8_t *out, uint16_t out_max, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Construye un frame UNER v2 en el buffer suministrado. | `esp:src/uner_core.c:130`; editorial |
| <a id="fn-uner_core_computechecksum"></a>`uint8_t UNER_Core_ComputeChecksum(const uint8_t *data, uint16_t len)` | Operación Compute Checksum de Parser UNER v2 ESP; semántica fina pendiente de ampliación. | `esp:src/uner_core.c:124`; inferred-name |
| <a id="fn-uner_core_dequeue"></a>`uint8_t UNER_Core_Dequeue(UNER_Core *core, UNER_Packet *out)` | Adapta la llamada a `UNER_Queue_Dequeue` con el contexto de esta integración. | `esp:src/uner_core.c:122`; structural |
| <a id="fn-uner_core_init"></a>`UNER_Status UNER_Core_Init(UNER_Core *core, const UNER_CoreConfig *cfg, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool_bytes, uint16_t payload_pool_size)` | Inicializa Parser UNER v2 ESP dentro de Parser UNER v2 ESP. | `esp:src/uner_core.c:23`; inferred-name |
| <a id="fn-uner_core_pushbyte"></a>`UNER_Status UNER_Core_PushByte(UNER_Core *core, uint8_t transport_id, uint8_t max_payload_for_transport, uint8_t byte)` | Coordina `UNER_Core_ResetParser`, `UNER_Queue_Enqueue`, `UNER_ROUTE_DST` para Parser UNER v2 ESP. | `esp:src/uner_core.c:44`; structural |
| <a id="fn-uner_core_resetparser"></a>`void UNER_Core_ResetParser(UNER_Core *core)` | Restablece Parser dentro de Parser UNER v2 ESP. | `esp:src/uner_core.c:32`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-acc"></a>`static void acc(UNER_Core *core, uint8_t b)` | Operación acc de Parser UNER v2 ESP; semántica fina pendiente de ampliación. | `esp:src/uner_core.c:4`; inferred-name |
| <a id="fn-resync_from_byte"></a>`static void resync_from_byte(UNER_Core *core, uint8_t byte)` | Coordina `UNER_Core_ResetParser`, `acc` para Parser UNER v2 ESP. | `esp:src/uner_core.c:6`; structural |
| <a id="fn-should_accept"></a>`static uint8_t should_accept(const UNER_Core *core, uint8_t src, uint8_t dst)` | Operación should accept de Parser UNER v2 ESP; semántica fina pendiente de ampliación. | `esp:src/uner_core.c:14`; inferred-name |
