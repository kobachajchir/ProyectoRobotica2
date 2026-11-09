# AFR2-20261109-03 — Cierre de fuentes y regeneración

Versión: 1.9.1 (subversión mínima).

Pedido: terminar el código y regenerar los recursos; el usuario compila y carga. Se usan afr2-screen-authoring y afr2-wire-commands. Continúa la fuente 1.9.0 (política AP y OLED/Web) sin ejecutar compilación, tests, validadores independientes, servidor, COM ni hardware.

Cambios terminados en fuente:
- Restituida la autorización PWM con `Afr2Product_PwmStatus`: motores habilitados explícitamente, sin fallo, captura Hall, escritura EEPROM, radar o secuencia. Se aplica al servicio local/remoto y a la capacidad anunciada. No enciende motores, limpia fallos ni cancela tareas por recibir una consigna. Sin requisitos nuevos de odometría o HC-SR04. Deshabilitar motores corta también PWM manual.
- Web no sustituye movimiento local ni la edición del servo/teclado/confirmación. ESP descarta el último slot PWM si pierde permiso o su observación del estado AVR caduca (2s), manteniendo slot reciente/ALIVE/STOP/TTL sin ACK bloqueante por consigna. Las respuestas robot.state también actualizan el instante de observación real. Payloads/EX01/EEPROM conservados.
- Wi-Fi: el modo apagado/desconocido no se interpreta como STA para buscar; selector de modo acotado al abrir desde OFF. Política desconocida conserva el valor2 y muestra -- en lugar de SI. Catálogo de pantallas mantiene contador máximo numérico y variantes6/7 por modo. Biblioteca y simulador acompañan OFF, inicio/detención y persistencia de modo/política.
- Versión común ATmega/ESP/Web 1.9.1. Catálogos UART, WS y Protocol Studio actualizados.

Generación ejecutada, sin compilar:
1. `node tools/sync-oled-library.mjs` en Web: completado, 75 nodos AVR, 26 iconos, 950bytes de fuente. Salidas `generated/firmwareLibrary.ts` y `touchBitmap.ts`. Guard de fuentes revisadas conservado, sin --copy-only ni omisión.
2. `node tools/catalog-model-assets.mjs` en Web: completado. Salida `src/components/three/modelAssetSizes.json`.

Bitmaps AP y largo exactos del usuario conservados/copias regeneradas. Fuente de la UI local y copia TS entregadas juntas. Revisiones de hashes solo de algoritmos portados, no para ocultar diferencias. No se afirma paridad raster por ejecutar el generador.

Estado: ATmega/ESP/Web=fuentes completadas para estos cambios, recursos regenerados; **sin compilar, cargar ni probar físicamente**. Loader/CAD sin cambios. Presupuesto SRAM/pila, imagen resultante y comportamiento en placa los comprobará la etapa de compilación/carga del usuario. `ESP/data/dist` y binarios no se reemplazan con fuentes: el build Web ESP prepara el paquete actualizado antes de generar/cargar LittleFS.

No se convierte el diseño futuro de aproximación al brazo en un modo físico: su comportamiento, geometría y aceptación no están especificados. PCB/soporte/cubierta dependen de planos. Esos desarrollos futuros no son dependencias de compilar el producto actual.

Procedencia por archivo en `provenance/source-completion-20261109-03.json`.
