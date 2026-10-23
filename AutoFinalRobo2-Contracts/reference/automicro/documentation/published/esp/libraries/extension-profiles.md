# Perfiles de extensión F4

ID: `esp:module/src/f4_extension_profiles.h` · Capa: **contrato** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye e interpreta operaciones de selección y reinicio en perfil de extensión F4.

Perfil 0/1 y comandos 0x73/0x74/0x76; no redefine pinout de placa desde el ESP.

<a id="modularity"></a>
## Modularidad observada

Helper de contrato compartido, independiente de UI pero específico de perfiles F4.

<a id="state"></a>
## Handle, estado y atributos externos

Datos pequeños del request/response pertenecen al llamador; configuración deseada y perfil activo pueden diferir hasta reinicio.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Construir request correcto, esperar la política final y revisar status.
- Distinguir seleccionar perfil de seleccionar y reiniciar.

<a id="interactions"></a>
## Interacciones

- El bridge traduce nombres stm.extensionProfile.* y usa el registro de comandos.

<a id="limitations"></a>
## Precauciones y límites

- 0x75 no debe confundirse con reboot 0x76.
- Un cambio persistido no acredita que el reinicio se completó.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/f4_extension_profiles.c` | c-cpp-structural | `ef0e775646cdd9e16609247069000359f77e1d8ffa59e5c986dab4820c26b061` |
| `src/f4_extension_profiles.h` | c-cpp-structural | `b8071f6f0382f7ecaac4067841acb9b9170a939f88125fdb4c0040ee0ffa9c5a` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `F4_ExtensionProfileRequest`

`esp:src/f4_extension_profiles.h:15`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t command_id` | Dato de estado/configuración `command_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload[1]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `uint8_t payload_len` | Dato de estado/configuración `payload_len`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-f4_extensionprofile_buildgetrequest"></a>`bool F4_ExtensionProfile_BuildGetRequest(F4_ExtensionProfileRequest *out)` | Construye Get Request dentro de Perfiles de extensión F4. | `esp:src/f4_extension_profiles.c:23`; inferred-name |
| <a id="fn-f4_extensionprofile_buildrebootrequest"></a>`bool F4_ExtensionProfile_BuildRebootRequest(uint8_t profile_id, F4_ExtensionProfileRequest *out)` | Adapta la llamada a `build_selection_request` con el contexto de esta integración. | `esp:src/f4_extension_profiles.c:39`; structural |
| <a id="fn-f4_extensionprofile_buildsetrequest"></a>`bool F4_ExtensionProfile_BuildSetRequest(uint8_t profile_id, F4_ExtensionProfileRequest *out)` | Adapta la llamada a `build_selection_request` con el contexto de esta integración. | `esp:src/f4_extension_profiles.c:32`; structural |
| <a id="fn-f4_extensionprofile_decodecurrent"></a>`bool F4_ExtensionProfile_DecodeCurrent(const uint8_t *payload, uint8_t len, uint8_t *profile_id)` | Adapta la llamada a `F4_ExtensionProfile_IsValid` con el contexto de esta integración. | `esp:src/f4_extension_profiles.c:46`; structural |
| <a id="fn-f4_extensionprofile_decoderesult"></a>`bool F4_ExtensionProfile_DecodeResult(const uint8_t *payload, uint8_t len, bool *success)` | Operación Decode Result de Perfiles de extensión F4; semántica fina pendiente de ampliación. | `esp:src/f4_extension_profiles.c:55`; inferred-name |
| <a id="fn-f4_extensionprofile_isvalid"></a>`bool F4_ExtensionProfile_IsValid(uint8_t profile_id)` | Indica si se cumple la condición Valid dentro de Perfiles de extensión F4. | `esp:src/f4_extension_profiles.c:7`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-build_selection_request"></a>`static bool build_selection_request(uint8_t command_id, uint8_t profile_id, F4_ExtensionProfileRequest *out)` | Adapta la llamada a `F4_ExtensionProfile_IsValid` con el contexto de esta integración. | `esp:src/f4_extension_profiles.c:13`; structural |
