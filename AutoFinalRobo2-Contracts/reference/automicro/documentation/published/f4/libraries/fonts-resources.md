# Fuentes, tipos y recursos auxiliares

ID: `f4:module/Core/Inc/fonts.h` · Capa: **datos** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Provee fuentes bitmap, tipos de flags/modos/eventos y vistas de assets sin poseer dispositivos.

Tablas de glifos, FontDef, máscaras y vocabulario compartido; otros bitmaps y constantes figuran en source-map.json.

<a id="modularity"></a>
## Modularidad observada

Los datos simples son reutilizables con su formato, pero macros_utils también conoce carmode y las convenciones de flags del proyecto.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Fuentes/bitmaps son recursos estáticos; BinaryAssetView_t presta bytes/metadata y no cambia su propiedad. Los tipos de bitfield son representación interna, no un contrato de serialización entre lenguajes.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- La aplicación/renderer presta FontDef y recursos al driver; los glifos no requieren inicialización de hardware.

## Trabajo en tándem

- SSD1306 y OSK integrado consumen fuentes; menús y protocolos comparten constantes lógicas, no punteros.

## Precauciones y límites

- Orden de bits de bitmap, stride y page-major son formatos diferentes; elegir la entrada DrawBitmap correcta.
- No transportar bitfields C ni punteros como bytes de protocolo.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/fonts.h` | `stdint.h` | `03e4a44764b6431a1da22fadf8e1a0b4a95f7393fba89a59b0965b702c2ad94f` |
| `Core/Src/fonts.c` | `fonts.h` | `ae39599174c06d70852ab2fd1f58aa820d233c0ba756bed34c8ee90ac48d7995` |
| `Core/Inc/binary_asset_types.h` | `stdint.h` | `094950a90369c0f2da11aa90cc4f388cf3f6d3fad3803b0deec9a978cbe1e74d` |
| `Core/Inc/types/bitmap_type.h` | `stdint.h` | `2388adc3a34d0a3f96f4caa9c621a9c2c351d700c2bad4b62d3dfb4a1cfd0421` |
| `Core/Inc/types/carmode_type.h` |  | `29ed225a874787323b7713fe051c2b81f1a08229afd900abc62d34279a688701` |
| `Core/Inc/types/userEvent_type.h` |  | `f8a2ff0ba68082512a6a90adebfb595c844041ad6255e63022c9322d71a6e2ef` |
| `Core/Inc/utils/macros_utils.h` | `stdint.h`, `../types/bitmap_type.h`, `../types/carmode_type.h` | `0984bb89c0b1e8446fa01b8672af741e6d817d2f371215f25f2c6d7fbacd932d` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `FontDef`

`f4:Core/Inc/fonts.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const uint8_t FontWidth` | Dato de estado/configuración `FontWidth`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t FontHeight` | Dato de estado/configuración `FontHeight`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const void *data` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BinaryAssetView_t`

`f4:Core/Inc/binary_asset_types.h:4`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t asset_id` | Dato de estado/configuración `asset_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t width` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t height` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *data` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Byte_Flag_Struct`

`f4:Core/Inc/types/bitmap_type.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `struct { estructura anidada } bitmap` | Dato de estado/configuración `bitmap`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } bitPair` | Dato de estado/configuración `bitPair`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } nibbles` | Dato de estado/configuración `nibbles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t byte` | Dato de estado/configuración `byte`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bitmap`

`f4:Core/Inc/types/bitmap_type.h:21`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bit0: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit1: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit2: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit3: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit4: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit5: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit6: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit7: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bitPair`

`f4:Core/Inc/types/bitmap_type.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bitPairL: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairCL: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairCH: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairH: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |

### `nibbles`

`f4:Core/Inc/types/bitmap_type.h:37`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bitL: 4` | Dato de estado/configuración `4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitH: 4` | Dato de estado/configuración `4`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
