# Contrato de arranque y mailbox

ID: `f4:module/Core/Inc/automicro_boot_contract.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Intercambia metadatos y solicitudes de arranque entre aplicación y bootloader mediante mailbox y CRC.

Layout del mailbox, validación, consumo y solicitudes; no documenta internamente el bootloader externo.

<a id="modularity"></a>
## Modularidad observada

Ligado a memoria y contrato de arranque STM32; no es una biblioteca de aplicación portable sin adaptar dirección y layout.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Mailbox en dirección/área fijada por contrato de plataforma; sus campos y CRC deben coincidir con el otro productor/consumidor. Consumir una solicitud tiene efecto sobre su disponibilidad posterior.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Arranque consulta/valida mailbox; Consume copia y elimina la marca según implementación.
- WriteRequest prepara solicitud antes del reset; el bootloader la consume en otro ciclo de ejecución.

## Trabajo en tándem

- UNER_App informa versión/boot report y prepara resets; el bootloader es un consumidor externo a esta fotografía.

## Precauciones y límites

- No suponer compatibilidad con otra versión de bootloader solo por tener el mismo nombre de struct.
- El análisis no modifica linker, sectores ni imagen de firmware.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/automicro_boot_contract.h` | `stdbool.h`, `stddef.h`, `stdint.h` | `d60581b850062cba9502d1f1ee2ecf5780ba2601b602a258a15ceb01db461ba3` |
| `Core/Src/automicro_boot_contract.c` | `automicro_boot_contract.h`, `string.h` | `18f6838024f68deaebe551b5c3aff74f8c2654262987e7e521f642d0f4436e31` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AutoMicroAppManifest`

`f4:Core/Inc/automicro_boot_contract.h:97`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t schema_version` | Dato de estado/configuración `schema_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t struct_size` | Dato de estado/configuración `struct_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_base` | Dato de estado/configuración `app_base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t image_size` | Dato de estado/configuración `image_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t image_crc32` | Dato de estado/configuración `image_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t firmware_version` | Dato de estado/configuración `firmware_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t build_id` | Dato de estado/configuración `build_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint32_t reserved[8]` | Dato de estado/configuración `reserved`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMicroBootMailbox`

`f4:Core/Inc/automicro_boot_contract.h:111`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t magic` | Dato de estado/configuración `magic`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t schema_version` | Dato de estado/configuración `schema_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t struct_size` | Dato de estado/configuración `struct_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t bootloader_version` | Dato de estado/configuración `bootloader_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t bootloader_build_id` | Dato de estado/configuración `bootloader_build_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reset_reason_flags` | Dato de estado/configuración `reset_reason_flags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_validation_flags` | Dato de estado/configuración `app_validation_flags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_base` | Dato de estado/configuración `app_base`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_size` | Dato de estado/configuración `app_size`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_version` | Dato de estado/configuración `app_version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_build_id` | Dato de estado/configuración `app_build_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t app_crc32` | Dato de estado/configuración `app_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t handoff` | Dato de estado/configuración `handoff`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t request` | Dato de estado/configuración `request`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reserved[2]` | Dato de estado/configuración `reserved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t mailbox_crc32` | Dato de estado/configuración `mailbox_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-automicrobootmailbox_consume"></a>`bool AutoMicroBootMailbox_Consume(AutoMicroBootMailbox *out)` | Copia y consume el reporte de arranque disponible. | `f4:Core/Inc/automicro_boot_contract.h:145`; editorial |
| <a id="fn-automicrobootmailbox_consumerequest"></a>`AutoMicroBootRequest AutoMicroBootMailbox_ConsumeRequest(void)` | Lee y consume una solicitud de arranque pendiente. | `f4:Core/Inc/automicro_boot_contract.h:147`; editorial |
| <a id="fn-automicrobootmailbox_get"></a>`const AutoMicroBootMailbox *AutoMicroBootMailbox_Get(void)` | Obtiene vista del mailbox en la ubicación de plataforma. | `f4:Core/Inc/automicro_boot_contract.h:143`; editorial |
| <a id="fn-automicrobootmailbox_validate"></a>`bool AutoMicroBootMailbox_Validate(const AutoMicroBootMailbox *mailbox)` | Comprueba identificación, versión/integridad del mailbox. | `f4:Core/Inc/automicro_boot_contract.h:144`; editorial |
| <a id="fn-automicrobootmailbox_writerequest"></a>`bool AutoMicroBootMailbox_WriteRequest(AutoMicroBootRequest request)` | Escribe una solicitud de arranque con su integridad. | `f4:Core/Inc/automicro_boot_contract.h:146`; editorial |
| <a id="fn-automicroboot_crc32"></a>`uint32_t AutoMicroBoot_Crc32(const void *data, size_t length)` | Calcula el CRC32 usado por el contrato de arranque. | `f4:Core/Inc/automicro_boot_contract.h:148`; editorial |
