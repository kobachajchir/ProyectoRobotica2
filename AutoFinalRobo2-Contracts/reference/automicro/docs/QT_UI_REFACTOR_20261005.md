# Qt 2.0: interfaz, calibración y representación OLED

Cambio interno de Qt, implementado en fuente el 2026-10-05. No modifica IDs UNER, payloads, permisos, política del router F4 ni formatos JSON compartidos. Sin compilación, pruebas ni validación física por instrucción del usuario. Los demás repositorios de código no se editaron.

## Pantallas y transición

Inicio consume los estados existentes y muestra enlace/red/adquisición/orientación, sin renderizador 3D. Las vistas de cámara se comparten entre MPU, Recorrido, Control y Seguidor. Seguidor cambia de posición visual en la barra; su ID permanece 10. La navegación prepara la ruta QML antes de mostrar el host compartido y las pantallas sin escena no crean el visor oculto.

## OLED

Qt incorpora una copia portable de las funciones de dibujo de `FirmwareF4/Core/Src/ssd1306.c` y las fuentes/bitmaps originales. El handle contiene cursor, color, inversión y 1024 bytes page-major (8 páginas × 128 columnas), sin HAL, DMA, I2C ni System. Dibujo C no implica que todos los renderers dependientes del estado F4 se hayan portado.

Los reportes de pantalla existentes comunican identidad, selección, conteo, origen y datos de dashboard/progreso disponibles; no incluyen framebuffer completo. Qt muestra un raster 128×64 con escala entera. Reconstruye menús conocidos solo si el conteo coincide con su lista: principal, WiFi, ESP, sensores y configuración. Usa selección 0-based y ventana de tres ítems. Otras pantallas muestran identidad reportada, sin inventar texto de teclado, cursor, favoritos, variante de notificación ni estado faltante. No es captura de píxeles remotos.

Las acciones conservan los comandos y guardas existentes: giro, encoder/USER corto y largo, página arriba/abajo y click visible 1/2/3. USER largo mantiene su prioridad global; los teclados mantienen el giro descrito en SCREEN_BEHAVIOR.md. La transferencia de un archivo OLED local conserva BEGIN/CHUNK/COMMIT/CANCEL, offsets, CRC y confirmaciones existentes.

## Calibración

Se adaptan a Qt los flujos de los componentes Web actuales de Objetos, Línea y Balance. Se conservan los esquemas IR v2, línea v1 y balance-session v1. Objetos guarda un canal por punto y combina los demás canales existentes a esa distancia; baseline conserva los cinco. Línea sigue separada y normalizada por grupo. Balance registra tiempos de llegada al cliente, no ciclos F4; el optimizador no declara esos registros sincronizados. La persistencia nativa sustituye localStorage/IndexedDB y no altera umbrales del firmware. PID se aplica explícitamente mediante el comando existente y su confirmación.

La equivalencia visual/runtime y la fidelidad de todos los renderers OLED permanecen sin verificar. Alcance y dependencias detallados en `AutoMicroQtDesktop/docs/QT_UI_REFACTOR_20261005.md`. No se crea ficha de cambio wire porque no hay un contrato nuevo; este documento registra las limitaciones de representación.
