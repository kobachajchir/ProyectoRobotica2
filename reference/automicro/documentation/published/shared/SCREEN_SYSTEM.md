# Pantallas F4 ↔ ESP ↔ Web: identidad, dibujo, estado y controles

ID: `shared:screen-system`. Fuente del checkout de trabajo del 20-10-2026.
Revisión por lectura y extracción documental; sin tests, build, comparaciones
automáticas de paridad, navegador ni placa. Ninguna pantalla se etiqueta exacta.

La meta acordada es copiar/adaptar toda la biblioteca OLED del F4 para la Web y
obtener la misma pantalla. El estado actual combina adaptaciones TS, fuentes y
bitmaps con un renderer SVG y reportes parciales de estado. Se documentan ambas
cosas por separado para que el port posterior tenga un contrato verificable.

<a id="code-register"></a>
## Identidad y catálogo de códigos

`ScreenCode_t` se transmite como u32 LE; los códigos actuales usan 24 bits:
`(menu << 16) | (submenu << 8) | page`. `page` es parte de la identidad y no
debe interpretarse automáticamente como número de viewport de un menú.
En TS es `number`; no es una cadena hexadecimal aunque la UI también publique
`screenCodeHex`. La fuente física de identidad es `Core/Inc/screen_codes.h`.

La extracción registra 147 macros literales F4 y 145 constantes Web. Los 145
símbolos compartidos tienen el mismo literal. Dos identidades F4 no aparecen en
Web: Wi-Fi Retry `0x020211` y AP fallback `0x05041D`. Ambas se localizaron como
identidades de acciones de ítems; no se deduce un renderer dedicado ausente.
Se registran en [AMDOC-WEB-008](../../review/FINDINGS.md#amdoc-web-008).

Hay ocho identidades sin binding candidato extraído del visor: `NONE`, esas dos
acciones y cinco LINE. `NONE` es centinela. De los LINE, `LINE_FOLLOW` identifica
la acción toggle; menú de calibración, blanco, pista y aviso sí tienen caminos
F4 de visualización localizados. Las constantes no prueban alcance, datos o
renderer. La metadata descriptiva contiene 84 entradas literales y se documenta
independientemente del binding.

- [Registro navegable por código](SCREEN_CODE_REGISTER.md).
- [Manifest de declaraciones, referencias y familias](screen-manifest.json).
- Fichas `screens/<código>.md/.json`: identidad, variantes, datos disponibles,
  acciones, limitaciones, referencias y hallazgos. Las referencias F4 son
  candidatas léxicas; el grafo de llamadas sigue sin resolverse completamente.

<a id="exact-port"></a>
## Qué comprende el port de toda la biblioteca OLED

| Capa F4 | Consumidor Web actual | Qué debe conservar una copia exacta |
| --- | --- | --- |
| `ssd1306.c`, tipos y buffer | `OledCommandPreview` traduce comandos a SVG. El rasterizador del editor es otro motor. | Dimensiones, layout por páginas, clipping, enteros/redondeo, orden de operaciones, cursor, colores, inversión y efecto sobre el mismo buffer. |
| Fuentes y bitmaps | Font data y bitmapAssets adaptados; bitmap desconocido usa placeholder. | Mismos bytes, métricas, espaciado, bit order LSB/MSB y recursos completos. Un rectángulo con etiqueta no reemplaza un bitmap. |
| `oled_utils` y renderers | Builders TS por familia. | Misma función/variante, parámetros, unidades y reglas de dibujo; no sustituir datos ausentes por una muestra. |
| wrappers/menús/OSK/permisos | Resolver, ScreenContext y controles React. | Estado efectivo, selección, ventana, borradores, fase temporal y transiciones que eligieron el dibujo. |
| Puerto de pantalla/I2C | DOM/SVG o futuro framebuffer Web. | Adaptar salida/plataforma sin introducir HAL en el núcleo ni reproducir arbitraje físico en el navegador. |

El F4 dibuja sobre memoria monocroma de 128×64 (1024 bytes, páginas verticales de
8 píxeles). Una futura presentación Web puede escalar esos píxeles conservando
su raster. El SVG actual usa líneas/curvas y paint de dos colores sin conservar
necesariamente el historial de bits del framebuffer.

La semántica `Inverse` ya es inconsistente en F4: píxel borra salvo White,
mientras líneas horizontal/vertical rápidas tienen XOR. Web paint lo interpreta
como White salvo inversión. Está registrada en
[AMDOC-F4-012](../../review/FINDINGS.md#amdoc-f4-012). Decidir la corrección es un
cambio funcional distinto de portar literalmente una revisión. No corregirlo
solo en la copia Web y luego afirmar equivalencia.

El port deberá conservar revisión/hash de los archivos F4 copiados, adaptaciones
de plataforma declaradas y lista de recursos/modelos necesarios. No se eligió ni
implementó aquí TS, WASM o un mecanismo de build nuevo. Ninguno, por su nombre,
demuestra fidelidad. El pendiente actual es
[AMDOC-WEB-003](../../review/FINDINGS.md#amdoc-web-003).

<a id="reports"></a>
## Reportes que realmente cruzan el enlace

| Mensaje | Longitud payload | Offsets y significado |
| --- | --- | --- |
| `0x52 GET_CURRENT_SCREEN`, base | 5 | 0..3 screenCode u32 LE; 4 source u8. |
| `0x52`, source MENU=1 | 7 | Base + 5 selectedIndex; 6 itemCount. |
| `0x52`, dashboard source RENDER=2 | 13 + SSID | 5 flags; 6 backend; 7 carMode; 8..11 IPv4; 12 ssidLength; 13.. texto SSID acotado. |
| `0x52` / `0x95`, aviso source NOTIFICATION=3 | 9 o 10 | Base + 5..6 remainingMs u16 LE; 7..8 totalMs u16 LE; 9 clientNumber opcional. |
| `0x95 SCREEN_CHANGED`, base | 5 | Identidad y source; no selección por implicación. |
| `0x96 MENU_SELECTION_CHANGED`, actual | 7 | 0..3 screenCode; 4 selectedIndex; 5 itemCount; 6 source. |
| `0x96`, formato histórico aceptado ESP | 6 | 0..3 screenCode; 4 itemCount; 5 source. No lleva selectedIndex. |

Las extensiones dependen de comando, longitud, source y a veces identidad.
No leer siete bytes de selección usando el orden de `0x96` para un `0x52`.
En dashboard flags: bit0 ESP presente, bit1 STA, bit2 AP, bit3 USB, bit4 RF.
ESP traduce backend a `No ESP/AT/WEB`, IP a string y modo a número más nombre.
Un evento corto no contiene necesariamente toda esa extensión.

`add_screen_report` publica `screen_code`/`screenCode`, hex, descomposición,
source/sourceName y bytes crudos; actualmente también emite ceros para selección
y conteo ausentes. Ese relleno pierde presencia del dato:
[AMDOC-ESP-002](../../review/FINDINGS.md#amdoc-esp-002). Conservar esta salvedad
al consumir el JSON; cero observado y campo no recibido son estados diferentes.

Evidencia: `f4:Core/Src/uner_app.c:UNER_CMD_ID_GET_CURRENT_SCREEN`,
`UNER_App_ReportScreenChanged/ReportMenuSelectionChanged/ReportNotificationChanged`,
`esp:src/ws_bridge/reports.inc:add_screen_report`.

<a id="state-model"></a>
## Modelo compartido y disponibilidad

Cada pantalla necesita identidad, variante, campos/unidades, validez, procedencia
y acciones. Las fichas distinguen `reported` (en el reporte),
`separately-queryable` (otro request/stream) y `unavailable-remotely` (estado local
sin representación remota localizada). Una consulta posterior puede observar
otro instante y no reconstruir un aviso o un frame anterior.

| Familia | Estado necesario además de código | Disponibilidad actual |
| --- | --- | --- |
| Menú | Catálogo visible, selección, ventana efectiva, cantidad de filas, iconos, scroll de texto. | Selección/conteo en ciertos reportes; ventana y catálogo dinámico no completos. |
| Dashboard | Modo, conexiones, SSID/IP, fase de scroll/tiempo. | Extensión de 0x52 disponible; eventos cortos y fase temporal parciales. |
| PIN | Fase de permiso, cursor, dígitos o enmascarado, timeout y alcance de concesión. | Identidad/resultado de auth; edición física no enviada. |
| OSK | Alpha/numeric, layout, buffer, fila/columna, Shift/Symbols, límites y error. | Códigos efectivos; falta snapshot OSK. |
| Sensores | Muestra/seq/tiempo, unidades, vista seleccionada, escalas y estado de renderer. | Telemetría consultable; no está acoplada íntegramente al modelo de visor. |
| Aviso | Texto, variante, tiempo, cliente y contexto a restaurar. | Código/source/tiempos en ciertos reportes; no todas las variantes/textos. |
| Calibración de línea | Fase, target, sesión, motivo/resultado y controles permitidos. | Estado por API específica; falta binding del visor y unión por variante. |

El modo tiene wire `0 IDLE, 1 FOLLOW, 2 TEST, 3 GOTO`. CarModeContext y ESP conocen
GOTO, pero normalizadores paralelos del visor lo pierden y pueden caer a IDLE
([AMDOC-WEB-005](../../review/FINDINGS.md#amdoc-web-005)). Unknown debe permanecer
distinto de un modo confirmado. Un DTO único normalizado evitará estas divergencias,
pero su implementación queda para la etapa de código.

<a id="menu-window"></a>
## Scroll y selección: la fila visible forma parte del contrato

F4 `MenuSys_EnsureCurrentSelectionVisible` desplaza la ventana hasta incluir el
ítem seleccionado y conserva historia. Usa max_visible_items, con default tres;
favoritos usa dos. La Web, sin firstVisible, deriva bloques de tres con
`floor(selectedIndex/3)*3`. No son la misma regla.

Ejemplo derivado del código, sin ejecutar: seis ítems visibles 0..5, ventana
inicial 0..2, avanzar hasta selección 3. F4 puede mostrar 1..3 y Web 3..5. Fila 1
Web corresponde al ítem 3; `0x53 position=1` se resuelve con la ventana F4 y puede
activar el ítem 1. El mismo código y selectedIndex no evitan el desacuerdo.

Debe acordarse ventana/catálogo y cantidad de filas, o un contrato alternativo
de selección por identidad/índice confirmado. El dibujo y la acción se corrigen
juntos. No asumir que el byte page del screenCode resuelve la ventana.
[AMDOC-WEB-004](../../review/FINDINGS.md#amdoc-web-004) enlaza productor,
traductor, normalizador, builder y control afectados.

<a id="remote-actions"></a>
## Acciones remotas y qué confirma la respuesta

Todas las acciones siguientes incluyen screenCode esperado u32 LE en 0..3.
F4 compara con pantalla actual y devuelve status 2 ante mismatch. Contexto,
permisos y router siguen teniendo autoridad aunque el código coincida.

| ID | Ruta JSON API 1 | Bytes adicionales | Efecto lógico |
| --- | --- | --- | --- |
| 0x53 | `stm.menu.click` | Byte4 posición visible desde 1. | Selecciona ítem de la ventana F4 y procesa encoder corto. |
| 0x54 | `stm.input.encoderButton` | Byte4 pressKind 0 corto / 1 largo; variante de 4 bytes usa corto. | UE_ENC_SHORT_PRESS / UE_ENC_LONG_PRESS. |
| 0x55 | `stm.input.userButton` | Igual pressKind. | UE_SHORT_PRESS / UE_LONG_PRESS. |
| 0x56 | `stm.screen.page` | Byte4 direction 0 arriba / 1 abajo. | Mueve ventana por MenuSys_MoveVisiblePage según contexto. |
| 0x57 | `stm.input.rotateLeft` | Ninguno. | UE_ROTATE_CCW; OSK lo adapta a OnRight. |
| 0x58 | `stm.input.rotateRight` | Ninguno. | UE_ROTATE_CW; OSK lo adapta a OnLeft. |

El cliente conserva alias históricos (`menuItemClick`, `triggerEncoderButton`,
etc.) y modo legacy hex; no equivalen a un protocolo nuevo. El catálogo documenta
rutas por separado. La respuesta de acción usa el comando original y status:
0 aceptado, 2 screen mismatch, 3 estado inválido, 4 no consumido. Aceptado indica
tratamiento de la acción, no garantía de una transición visual particular.

Prioridad del router físico: Permission activo; descarte de notificación con
encoder; ScreenLock; USER largo global al dashboard; handler activo. Tras una
acción, esperar el reporte efectivo y resincronizar ante mismatch. Una respuesta
queued no autoriza repetir automáticamente una acción que puede mutar estado.

El visor clasifica pulsación larga a 1500 ms; el encoder físico la clasifica
desde 30 muestras de 10 ms. Se transmiten eventos lógicos, no la duración.
[AMDOC-WEB-007](../../review/FINDINGS.md#amdoc-web-007) deja pendiente decidir si
se busca equivalencia de gesto temporal o solo de acción.

El teclado del visor usa `u` USER, `m` encoder, flechas para giros y `r` refresco,
dentro de su ámbito de foco; las pulsaciones se completan al soltar y se cancelan
en blur/cambio de visibilidad. Es entrada remota a través del mismo contrato,
no ejecución local de la función C desde el navegador.

<a id="pin-osk"></a>
## PIN, teclados y salidas especiales

En permiso, el código puede compartir aviso/formulario y source participa del
discriminante. Los datos actuales no incluyen digits/pinIndex del formulario
físico; el resolver usa cuatro ceros e índice cero si faltan. No son una lectura
del PIN real ([AMDOC-WEB-002](../../review/FINDINGS.md#amdoc-web-002)). Login Web y
`esp.auth.pin.validateScreen` tienen solicitudes/alcances propios y resultado F4;
el estado de autorización no reconstruye el cursor físico.

La integración OSK publica códigos efectivos `0x090101` alpha y `0x090102`
numérico. Los códigos de invocador Wi-Fi/test no prueban que permanezcan activos
durante edición. Giro CCW llama OnRight y CW llama OnLeft. Encoder corto selecciona,
USER corto cancela con OnBack; Shift conserva/clampa posición y Symbols/ABC la
reinicia. Respetar filas, wrap, validación y restricciones numéricas del núcleo
OSK, según [comportamiento físico](../../../docs/SCREEN_BEHAVIOR.md).

USER largo es consumido antes por la salida global del router. No alcanza la
rama local OSK de cancelación y puede dejar active/callbacks/bloqueo de avisos
sin limpiar ([AMDOC-F4-011](../../review/FINDINGS.md#amdoc-f4-011)). La futura
adaptación debe documentar la decisión de cleanup y orden de restauración;
no simular en Web una cancelación que el F4 no ejecutó.

<a id="sensor-model"></a>
## Datos de sensores: tipos compatibles y misma adquisición

ESP publica MPU con `accel:{x,y,z}` en g y `gyro:{x,y,z}` en grados/s, junto con
seq/dt y orientación. El builder busca `accelX/Y/Z`, `gyroX/Y/Z` y cae a muestras
como 120/-40/980. También existe fallback IR escalonado. El modelo de telemetría
y los argumentos del renderer no son la misma forma ni necesariamente la misma
unidad/variante ([AMDOC-WEB-006](../../review/FINDINGS.md#amdoc-web-006)).

| Dato | C/F4 → ESP → TS | Regla de adaptación |
| --- | --- | --- |
| Entero raw | i16/u16 LE a number | Conservar signo/rango y no convertir ADC en distancia por el nombre. |
| Magnitud física | float32 LE a number IEEE754 de JS | Nombrar g, dps, cm, grados, porcentaje; no copiar bytes de memoria JS como struct. |
| Disponibilidad | Flag o longitud en productor | Distinguir cero recibido, ausencia, stale y ejemplo. |
| Tiempo | u32 ms/us y seq | Distinguir adquisición, publicación y render; un query posterior no es el mismo frame. |
| Enum/vista | u8/enumeración a unión TS | Cubrir todos los valores y preservar unknown; no colapsar a default físico. |

La misma pantalla exige la misma selección de vista y estado, además de nombres
y unidades compatibles. Un adapter puede traducir representaciones; no debe
rellenar un dato inexistente para obtener una imagen plausible.

<a id="line-family"></a>
## Línea y calibraciones especiales

| Código | Uso F4 localizado | Estado/acción que debe portar el visor |
| --- | --- | --- |
| 0x030204 | Submenú de calibración blanco/pista/volver. | Catálogo, ventana y selección. |
| 0x030205 / 0x030206 | Renderer de calibración según target del estado. | Fase, sesión, texto/motivo, progreso y captura/cancelación. |
| 0x030207 | Identidad de acción toggle de seguimiento. | Mostrar resultado/aviso real; no inventar una vista dedicada. |
| 0x030208 | Renderer de aviso de rechazo/resultado. | Motivo local y texto secundario; eventos de salida del handler. |

`MenuLineCalEvents` usa encoder largo para cancelar la sesión y volver; los
eventos cortos capturan en fase prompt o salen cuando deja de estar busy.
La página Web de seguimiento tiene una API de control, pero no reemplaza esos
bindings OLED. [AMDOC-WEB-009](../../review/FINDINGS.md#amdoc-web-009) conserva el
pendiente y sus puntos documentales.

<a id="canvas-direction"></a>
## OLED Canvas y mantenimiento de conexiones

Canvas construye un documento Web, rasteriza una imagen y la envía
transaccionalmente Web → ESP → F4 para la pantalla de servicio correspondiente.
No descarga el framebuffer de la UI F4. Mantener sus offsets, orden por páginas,
chunks, commit/status y recursos separados de los reportes del visor.

Las [conexiones](CONNECTIONS.md) enlazan fuentes, módulos, pantallas y hallazgos.
Al corregir un caso actualizar su registro, overlay y secciones aquí enlazadas;
las fichas por código pueden regenerarse de forma focalizada. Si se amplía wire,
registrar ficha AMI y consumidores pendientes antes de declarar compatibilidad.
