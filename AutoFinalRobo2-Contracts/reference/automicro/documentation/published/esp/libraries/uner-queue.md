# Cola UNER

ID: `esp:module/src/uner_queue.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Mantiene colas acotadas de datos/paquetes del protocolo.

Operaciones de inserción/extracción y estado de capacidad, sin radio ni sesiones.

<a id="modularity"></a>
## Modularidad observada

Capa interna del protocolo, con dependencia de sus tipos; no es un servicio HTTP.

<a id="state"></a>
## Handle, estado y atributos externos

Índices y memoria pertenecen a la instancia/almacenamiento configurado; una cola llena debe propagarse como rechazo, no éxito de envío.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Inicializar memoria/índices antes de producir.
- Consumir y liberar según las operaciones de cola y la propiedad real del payload.

<a id="interactions"></a>
## Interacciones

- El handle/dispositivo usa la cola para separar producción de consumo.

<a id="limitations"></a>
## Precauciones y límites

- No suponer seguridad multihilo ni copia profunda de cada puntero por ser una cola.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uner_queue.c` | c-cpp-structural | `24b52687673efe2251a9910a94a537ebbbdd7faba1d725863dde25ff5863b877` |
| `src/uner_queue.h` | c-cpp-structural | `bd7cb6d852b00e1a88675051bc99c2f2fd8b06ace5a684b32fa9f05ab8c33df4` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNER_Queue`

`esp:src/uner_queue.h:9`

| Atributo / firma | Propósito |
| --- | --- |
| `UNER_Packet *pkt_slots` | Dato de estado/configuración `pkt_slots`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t *payload_pool` | Dato de estado/configuración `payload_pool`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t payload_stride` | Dato de estado/configuración `payload_stride`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t slot_count` | Dato de estado/configuración `slot_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t q_head` | Dato de estado/configuración `q_head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t q_tail` | Dato de estado/configuración `q_tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t q_count` | Dato de estado/configuración `q_count`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-uner_queue_dequeue"></a>`uint8_t UNER_Queue_Dequeue(UNER_Queue *q, UNER_Packet *out)` | Operación Dequeue de Cola UNER; semántica fina pendiente de ampliación. | `esp:src/uner_queue.c:41`; inferred-name |
| <a id="fn-uner_queue_enqueue"></a>`UNER_Status UNER_Queue_Enqueue(UNER_Queue *q, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len, uint8_t ver, uint8_t transport_id)` | Coordina `memcpy` para Cola UNER. | `esp:src/uner_queue.c:19`; structural |
| <a id="fn-uner_queue_init"></a>`UNER_Status UNER_Queue_Init(UNER_Queue *q, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool, uint16_t payload_pool_size)` | Inicializa Cola UNER dentro de Cola UNER. | `esp:src/uner_queue.c:4`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
