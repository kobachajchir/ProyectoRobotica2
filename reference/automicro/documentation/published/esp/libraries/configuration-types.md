# Configuración, flags y diagnósticos auxiliares

ID: `esp:module/src/config.h` · Capa: **datos** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Define modelos compartidos y superficies auxiliares de diagnóstico del firmware.

AppConfig y bits del runtime; los valores privados/defaults sensibles no se copian al manual.

<a id="modularity"></a>
## Modularidad observada

Vocabulario del proyecto que acopla esquemas, distinto de enlazar implementaciones.

<a id="state"></a>
## Handle, estado y atributos externos

Los modelos contienen configuración y credenciales locales; sus tipos son documentables pero su contenido real se excluye.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Config se carga antes de consumidores y puede tener copia temporal de sesión.
- Diagnósticos no deben transmitir secretos ni asumir que un bit sustituye el estado completo del servicio.

<a id="interactions"></a>
## Interacciones

- NVS, Wi-Fi, bridge y composición comparten estos tipos.

<a id="limitations"></a>
## Precauciones y límites

- Bitfields C y objetos C++ no son un contrato de memoria intercambiable con TypeScript.
- Los macros/config privados no se publican como ejemplos de valores.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/bitMap.h` | c-cpp-structural | `fa10cc2b559953f99fab7380f1e7def4e9f331115f865ba9537395458ee0bd05` |
| `src/config.h` | c-cpp-structural | `ae3fc02d7feb548bc93de9a1aa63959d411e50563705422e64e429817818be7a` |
| `src/uner_diagnostics_esp.cpp` | c-cpp-structural | `7d5f6c282a6d548f74d47652ab7775741d1e779d330c0986ae51b1e90a21c919` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `t_bitMap`

`esp:src/bitMap.h:14`

| Atributo / firma | Propósito |
| --- | --- |
| `struct { estructura anidada } bits` | Dato de estado/configuración `bits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } nibbles` | Dato de estado/configuración `nibbles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t byte` | Dato de estado/configuración `byte`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bits`

`esp:src/bitMap.h:15`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t b0:1` | Dato de estado/configuración `b0`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b1:1` | Dato de estado/configuración `b1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b2:1` | Dato de estado/configuración `b2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b3:1` | Dato de estado/configuración `b3`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b4:1` | Dato de estado/configuración `b4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b5:1` | Dato de estado/configuración `b5`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b6:1` | Dato de estado/configuración `b6`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t b7:1` | Dato de estado/configuración `b7`; declaración estructural, interpretación específica pendiente de ampliar. |

### `nibbles`

`esp:src/bitMap.h:19`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t low:4` | Dato de estado/configuración `low`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t high:4` | Dato de estado/configuración `high`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppConfig`

`esp:src/config.h:65`

| Atributo / firma | Propósito |
| --- | --- |
| `uint16_t schemaVersion` | Dato de estado/configuración `schemaVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t initializedNvs` | Dato de estado/configuración `initializedNvs`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char apSsid[SSID_MAX_LEN + 1]` | Dato de estado/configuración `apSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char apPass[PASS_MAX_LEN + 1]` | Dato de estado/configuración `apPass`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apProtected` | Dato de estado/configuración `apProtected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staSsid[SSID_MAX_LEN + 1]` | Dato de estado/configuración `staSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staPass[PASS_MAX_LEN + 1]` | Dato de estado/configuración `staPass`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staIp[IPV4_TEXT_MAX_LEN + 1]` | Dato de estado/configuración `staIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staGateway[IPV4_TEXT_MAX_LEN + 1]` | Dato de estado/configuración `staGateway`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staSubnet[IPV4_TEXT_MAX_LEN + 1]` | Dato de estado/configuración `staSubnet`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char apIp[IPV4_TEXT_MAX_LEN + 1]` | Dato de estado/configuración `apIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t nextBootMode` | Dato de estado/configuración `nextBootMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apFallbackEnabled` | Dato de estado/configuración `apFallbackEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apFallbackEnabled` | Dato de estado/configuración `apFallbackEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-espplatform_logunerpacket"></a>`void EspPlatform_LogUNERPacket(void *, const UNER_Packet *pkt)` | Operación Esp Platform Log UNERPacket de Configuración, flags y diagnósticos auxiliares; semántica fina pendiente de ampliación. | `esp:src/uner_diagnostics_esp.cpp:55`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-debug_payload_is_sensitive"></a>`static bool debug_payload_is_sensitive(uint8_t command)` | Operación debug payload is sensitive de Configuración, flags y diagnósticos auxiliares; semántica fina pendiente de ampliación. | `esp:src/uner_diagnostics_esp.cpp:21`; inferred-name |
| <a id="fn-debug_print_hex_bytes"></a>`static void debug_print_hex_bytes(const uint8_t *data, uint16_t len)` | Adapta la llamada a `print` con el contexto de esta integración. | `esp:src/uner_diagnostics_esp.cpp:9`; structural |
