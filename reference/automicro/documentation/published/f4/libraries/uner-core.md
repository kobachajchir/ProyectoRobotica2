# Parser y cola UNER v2

ID: `f4:module/Core/Inc/uner_core.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Reconoce frames UNER v2 byte a byte, comprueba estructura/checksum y encola paquetes con memoria del llamador.

Header UNER, largo, token, versión, ruta, ID, payload y XOR. No conoce nombres JSON ni efectos físicos de comandos.

<a id="modularity"></a>
## Modularidad observada

Sin HAL; el protocolo tiene sus propias capas de datos. Los transportes entregan bytes con ID y límite de payload.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

UNER_Core conserva configuración, FSM, acumuladores, slots/payload_pool prestados, índices y contadores. UNER_Packet contiene una vista del payload y transporte de origen; no es propietario de un bloque dinámico.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init conecta slots y pool con tamaño suficiente.
- PushByte avanza parser y descarta/resincroniza al detectar frame inválido.
- Al completar encola y avisa por on_packet; Dequeue copia descriptor, no copia sus bytes.
- ReleasePacket es actualmente no-op; el consumidor debe terminar antes de reutilizar el slot.

## Trabajo en tándem

- UNER_Handle registra callback de paquete para diferir despacho; UART/USB implementan la entrada de bytes.
- UNER_BuildFrame escribe encabezado/ruta/payload y XOR en un buffer externo.

## Precauciones y límites

- Payload máximo de un byte: 255 bytes; total frame=10+len.
- Cola llena, frame inválido y falta de ruta no tienen el mismo tratamiento.
- El core no incorpora requestId ni correlación de transacciones Web.

## Hallazgos vinculados

- [AMDOC-F4-004: BuildFrame acepta payload nulo con longitud positiva](../../../review/FINDINGS.md#amdoc-f4-004)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/uner_core.h` | `uner_core_types.h` | `c0b74272f7ecd9baab12183ada21e273bfaaf74c096f172b688d0f90339e0dd4` |
| `Core/Inc/uner_core_types.h` | `stdint.h`, `stddef.h`, `uner_v2.h`, `uner_status.h` | `fce205d1af3264b3107992cee49588d783f7049264941b367f9d7e30959fde5b` |
| `Core/Inc/uner_v2.h` | `stdint.h` | `db6d48d6e9578032d39e47cc24e096b86160ea285e987769e540891c5094f1d0` |
| `Core/Inc/uner_status.h` |  | `e3ebf968db51ef05e3cf787d2c9204eef81cdaded731ace3d0539862d551eac4` |
| `Core/Src/uner_core.c` | `uner_core.h`, `string.h` | `59d0f1ba072abeafad4d6e69f8f1e8d7366ce4596f87a1abf1daded77b194d84` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UNER_Packet`

`f4:Core/Inc/uner_core_types.h:29`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t ver` | Versión de frame decodificada; el protocolo actual usa 2. |
| `uint8_t src` | Nodo origen de cuatro bits de la ruta UNER. |
| `uint8_t dst` | Nodo destino de cuatro bits de la ruta UNER. |
| `uint8_t cmd` | Identificador numérico del comando/evento recibido. |
| `uint8_t len` | Cantidad válida de bytes de payload, entre 0 y 255. |
| `uint8_t *payload` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |

### `UNER_CoreConfig`

`f4:Core/Inc/uner_core_types.h:41`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t this_node` | Nodo local usado para aceptación y despacho. |
| `uint16_t accept_src_mask` | Máscara de nodos origen admitidos por parser. |
| `uint16_t accept_dst_mask` | Máscara de destinos admitidos por parser. |
| `uint8_t accept_broadcast` | Permite frames dirigidos al nodo broadcast. |
| `uint8_t default_max_payload` | Máximo payload si el transporte no impone uno más estricto. |
| `UNER_OnPacketCb on_packet` | Notifica paquete encolado; el handle instala un wrapper para diferir procesamiento. |
| `void *cb_ctx` | Contexto prestado al callback de paquete. |

### `UNER_Core`

`f4:Core/Inc/uner_core_types.h:65`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UNER_CoreConfig cfg` | Dato de estado/configuración `cfg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_ParserState state` | Estado del reconocimiento byte a byte del frame. |
| `uint8_t len_expected` | Longitud de payload declarada por el frame en recepción. |
| `uint8_t payload_index` | Cantidad/posición de bytes de payload ya recibidos. |
| `uint8_t chk_acc` | XOR acumulado de los bytes del frame. |
| `uint8_t route` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ver` | Dato de estado/configuración `ver`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t max_payload_transport` | Dato de estado/configuración `max_payload_transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t drop_frame` | Dato de estado/configuración `drop_frame`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_Packet *pkt_slots` | Array prestado de descriptores usado como cola circular. |
| `uint8_t *payload_pool` | Memoria prestada dividida en regiones por slot; los paquetes apuntan a esta memoria. |
| `uint16_t payload_stride` | Separación en bytes entre regiones de payload de slots. |
| `uint8_t slot_count` | Capacidad de paquetes de la cola. |
| `volatile uint8_t q_head` | Índice del siguiente slot a producir. |
| `volatile uint8_t q_tail` | Índice del siguiente slot a consumir. |
| `volatile uint8_t q_count` | Cantidad actual de paquetes en cola. |
| `uint32_t ok_frames` | Dato de estado/configuración `ok_frames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t chk_fail` | Dato de estado/configuración `chk_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t len_fail` | Dato de estado/configuración `len_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t header_resync` | Dato de estado/configuración `header_resync`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t token_fail` | Dato de estado/configuración `token_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ver_fail` | Dato de estado/configuración `ver_fail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t queue_overflow` | Dato de estado/configuración `queue_overflow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_ResetReason last_reset_reason` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_reset_state` | Memoria de la observación anterior usada por la máquina de estados. |

### `UNER_OnPacketCb`

`f4:Core/Inc/uner_core_types.h:38`

Firma: `void (*UNER_OnPacketCb)(void *ctx, const UNER_Packet *p)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uner_buildframe"></a>`UNER_Status UNER_BuildFrame( uint8_t *out, uint16_t out_max, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Serializa frame UNER v2 y checksum en el buffer del llamador. | `f4:Core/Inc/uner_core.h:26`; editorial |
| <a id="fn-uner_core_computechecksum"></a>`uint8_t UNER_Core_ComputeChecksum(const uint8_t *data, uint16_t len)` | Calcula XOR de los bytes recibidos. | `f4:Core/Inc/uner_core.h:25`; editorial |
| <a id="fn-uner_core_dequeue"></a>`uint8_t UNER_Core_Dequeue(UNER_Core *core, UNER_Packet *out)` | Extrae un descriptor de paquete manteniendo su payload prestado. | `f4:Core/Inc/uner_core.h:22`; editorial |
| <a id="fn-uner_core_init"></a>`UNER_Status UNER_Core_Init( UNER_Core *core, const UNER_CoreConfig *cfg, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool_bytes, uint16_t payload_pool_size)` | Inicializa parser y cola sobre memoria prestada del llamador. | `f4:Core/Inc/uner_core.h:7`; editorial |
| <a id="fn-uner_core_pushbyte"></a>`UNER_Status UNER_Core_PushByte( UNER_Core *core, uint8_t transport_id, uint8_t max_payload_for_transport, uint8_t byte)` | Procesa un byte del transporte con su límite de payload. | `f4:Core/Inc/uner_core.h:16`; editorial |
| <a id="fn-uner_core_releasepacket"></a>`void UNER_Core_ReleasePacket(UNER_Core *core, const UNER_Packet *p)` | Entrada de liberación actualmente sin trabajo; no extiende vida del payload. | `f4:Core/Inc/uner_core.h:23`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-uner_accepts_src"></a>`static uint8_t uner_accepts_src(const UNER_Core *core, uint8_t src)` | Operación uner accepts src de Parser y cola UNER v2; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_core.c:8`; inferred-name |
| <a id="fn-uner_accepts_dst"></a>`static uint8_t uner_accepts_dst(const UNER_Core *core, uint8_t dst)` | Operación uner accepts dst de Parser y cola UNER v2; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_core.c:13`; inferred-name |
| <a id="fn-uner_reset_parser"></a>`static void uner_reset_parser(UNER_Core *core, uint8_t reset_reason)` | Operación uner reset parser de Parser y cola UNER v2; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_core.c:21`; inferred-name |
| <a id="fn-uner_queue_commit"></a>`static UNER_Status uner_queue_commit(UNER_Core *core, uint8_t len)` | Coordina `on_packet` para Parser y cola UNER v2. | `f4:Core/Src/uner_core.c:74`; structural |
