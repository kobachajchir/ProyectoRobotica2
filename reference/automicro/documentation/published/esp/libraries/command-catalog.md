# Registros de comandos y sus políticas

ID: `esp:module/src/f4_command_registry.h` · Capa: **contrato** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Describe IDs, dueño, longitudes, timeout, privilegio e interpretación de finalización.

Metadata de catálogo para broker/bridge y especificación UNER; no demuestra por sí misma la implementación del peer.

<a id="modularity"></a>
## Modularidad observada

Contrato de aplicación, no HAL; conserva conocimiento de F4/ESP y privilegios Web.

<a id="state"></a>
## Handle, estado y atributos externos

Tablas estáticas; en ESP8266 se conservan en PROGMEM y se copian mediante acceso apropiado. Los nombres pueden no ocupar RAM en esa variante.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Consultar/cargar una entrada antes de construir solicitud.
- La política elegida determina si ACK o mismo comando completa al cliente.

<a id="interactions"></a>
## Interacciones

- TransactionManager y dispatch usan estos registros.
- La documentación cruza cada registro con tabla/handler F4 sin fusionar contradicciones.

<a id="limitations"></a>
## Precauciones y límites

- implementedMetadata no garantiza que la placa instalada soporte el ID.
- Mantener nombres JSON separados de IDs numéricos y comandos reservados.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/f4_command_registry.c` | c-cpp-structural | `fad61527bebf3527009fce13c0f2789279a1b015cd2c4cf07aaf7bdf2a988400` |
| `src/f4_command_registry.h` | c-cpp-structural | `f3419c4319bdc3d92597415adc84603039fd9dda9f2f3741c2bd8a7636d6c972` |
| `src/uner_cmdspec.c` | c-cpp-structural | `89c6a4a8e78c8d3954bec60d59885073647561f808264358e6457d4c02b40e63` |
| `src/uner_cmdspec.h` | c-cpp-structural | `aaad42fa2defd6039e9c09a4908a9a0e7e2674a54f557b1286ebb5f56a67234c` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `F4_CommandSpec`

`esp:src/f4_command_registry.h:35`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *name` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `F4_CommandOwner owner` | Dato de estado/configuración `owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t request_min_len` | Dato de estado/configuración `request_min_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t request_max_len` | Dato de estado/configuración `request_max_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t response_min_len` | Dato de estado/configuración `response_min_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t response_max_len` | Dato de estado/configuración `response_max_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `F4_ResponsePolicy response_policy` | Dato de estado/configuración `response_policy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `bool idempotent` | Dato de estado/configuración `idempotent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `F4_CommandPrivilege privilege` | Dato de estado/configuración `privilege`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool implemented` | Dato de estado/configuración `implemented`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool sensitive` | Dato de estado/configuración `sensitive`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_CommandSpec`

`esp:src/uner_cmdspec.h:16`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t min_args` | Dato de estado/configuración `min_args`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t max_args` | Dato de estado/configuración `max_args`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `UNER_CommandHandler handler` | Dato de estado/configuración `handler`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_CommandHandler`

`esp:src/uner_cmdspec.h:7`

Callback: `void (*UNER_CommandHandler)(void *ctx, const UNER_Packet *packet)`. Handler de la tabla invocado con handle/paquete según firma; valida semántica y decide respuesta de ese comando. Una entrada NULL puede usar dispatcher de aplicación.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-f4_commandregistry_at"></a>`const F4_CommandSpec *F4_CommandRegistry_At(size_t index)` | Coordina `F4_CommandRegistry_Count`, `F4_REGISTRY_COPY`, `defined` para Registros de comandos y sus políticas. | `esp:src/f4_command_registry.c:176`; structural |
| <a id="fn-f4_commandregistry_count"></a>`size_t F4_CommandRegistry_Count(void)` | Operación Command Registry Count de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/f4_command_registry.c:208`; inferred-name |
| <a id="fn-f4_commandregistry_find"></a>`const F4_CommandSpec *F4_CommandRegistry_Find(uint8_t id)` | Coordina `F4_CommandRegistry_Lookup`, `defined` para Registros de comandos y sus políticas. | `esp:src/f4_command_registry.c:157`; structural |
| <a id="fn-f4_commandregistry_isrequestlengthvalid"></a>`bool F4_CommandRegistry_IsRequestLengthValid(const F4_CommandSpec *spec, size_t len)` | Operación Command Registry Is Request Length Valid de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/f4_command_registry.c:213`; inferred-name |
| <a id="fn-f4_commandregistry_isresponselengthvalid"></a>`bool F4_CommandRegistry_IsResponseLengthValid(const F4_CommandSpec *spec, size_t len)` | Operación Command Registry Is Response Length Valid de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/f4_command_registry.c:225`; inferred-name |
| <a id="fn-f4_commandregistry_iswireallowed"></a>`bool F4_CommandRegistry_IsWireAllowed(const F4_CommandSpec *spec)` | Operación Command Registry Is Wire Allowed de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/f4_command_registry.c:235`; inferred-name |
| <a id="fn-f4_commandregistry_lookup"></a>`bool F4_CommandRegistry_Lookup(uint8_t id, F4_CommandSpec *out)` | Coordina `F4_REGISTRY_COPY` para Registros de comandos y sus políticas. | `esp:src/f4_command_registry.c:188`; structural |
| <a id="fn-uner_defaultcommandtable"></a>`const UNER_CommandSpec *UNER_DefaultCommandTable(uint8_t *count)` | Operación UNER Default Command Table de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/uner_cmdspec.c:107`; inferred-name |
| <a id="fn-uner_findcommandspec"></a>`const UNER_CommandSpec *UNER_FindCommandSpec(const UNER_CommandSpec *table, uint8_t count, uint8_t cmd)` | Operación UNER Find Command Spec de Registros de comandos y sus políticas; semántica fina pendiente de ampliación. | `esp:src/uner_cmdspec.c:4`; inferred-name |
| <a id="fn-uner_mutablecommandtable"></a>`UNER_CommandSpec *UNER_MutableCommandTable(uint8_t *count)` | Coordina `memcpy` para Registros de comandos y sus políticas. | `esp:src/uner_cmdspec.c:112`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
