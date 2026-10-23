# Botón de usuario

ID: `f4:module/Core/Inc/user_button.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Convierte un nivel eléctrico periódico en estado y banderas de pulsación del botón.

No configura GPIO ni decide navegación; interpreta activeLevel y mantiene contadores del pulsador.

<a id="modularity"></a>
## Modularidad observada

Lectura de GPIO inyectada y sin HAL en el núcleo. Usa tipos y macros de bits compartidos.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

UserButton_Handle_t posee una copia de UserButton_Port, ButtonState_t y la polaridad activa. read_level recibe port.ctx. La aplicación consume state y sus flags; no existe una cola de eventos propia del botón.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- UserButton_Init copia el puerto e inicializa el estado.
- La aplicación llama Task10ms con cadencia de 10 ms y traduce flags a UserEvent_t.

## Trabajo en tándem

- app_stm32_ports.h realiza la lectura HAL; app_tasks.c integra el muestreo con UiEventRouter.

## Precauciones y límites

- Cambiar la cadencia cambia la interpretación temporal de contadores y pulsaciones.
- No llamar desde dos productores sin definir quién consume/limpia las banderas.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/user_button.h` | `stdbool.h`, `types/button_state.h` | `5a90152231665fa84ca2ae33d92f2e4de4e45243891f62da10f47d87ca9102a2` |
| `Core/Inc/types/button_state.h` | `stdint.h`, `bitmap_type.h`, `utils/macros_utils.h` | `93cc2f94233675c07dbf44acff15dde3e5a899c8cd669cc9bc4fc5d7d7bb5429` |
| `Core/Src/user_button.c` | `user_button.h`, `utils/macros_utils.h` | `6239f1be6e3a50f73065a91a8b21af9f6b3588fa9845dc0bf7aef5f44e2a19b4` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UserButton_Port`

`f4:Core/Inc/user_button.h:23`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `bool (*read_level)(void *ctx)` | Lee el nivel eléctrico del botón; activeLevel interpreta su polaridad. |

### `UserButton_Handle_t`

`f4:Core/Inc/user_button.h:28`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UserButton_Port port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `ButtonState_t state` | Contadores y flags de pulsación que consume la tarea principal. |
| `UserButton_ActiveLevel activeLevel` | Polaridad que convierte nivel eléctrico en botón presionado. |

### `ButtonState_t`

`f4:Core/Inc/types/button_state.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `Byte_Flag_Struct flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint8_t counter` | Dato de estado/configuración `counter`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-userbutton_init"></a>`void UserButton_Init(UserButton_Handle_t *btn, const UserButton_Port *port, UserButton_ActiveLevel activeLevel)` | Copia puerto y polaridad e inicializa el estado del botón. | `f4:Core/Inc/user_button.h:34`; editorial |
| <a id="fn-userbutton_task10ms"></a>`void UserButton_Task10ms(UserButton_Handle_t *btn)` | Muestrea el nivel y actualiza contadores y flags de pulsación cada 10 ms. | `f4:Core/Inc/user_button.h:38`; editorial |
