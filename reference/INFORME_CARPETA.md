# Carpeta reference

[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Servidor, bridge JSON y sesiones](../docs/modules/http-ws.md)

HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas.

Reformas: Portar whitelist y mensajes robot.*; fuente de autoridad Mega, ESP no inventa permiso ni estado. Quitar bypass de desarrollo como valor productivo, MPU/balance/modelo anterior. Integrar LittleFS con rutas index/assets y Content-Encoding correcto para cada gzip. Preservar arreglos de buffers del origen al decidir versiones de librerías. Esqueleto actual solo Serial/yield; red/bridge no activos.

### [UART, parser y transacciones](../docs/modules/uart.md)

Parser/cola/transporte Arduino y broker separan aceptación, respuesta y timeout. Tablas f4_* son del robot anterior; no basta cambiar su nombre.

Reformas: Nuevo catálogo/payloads Mega con capabilities. Mantener FIFO y una transacción correlacionable en vuelo; resolver ACK tardíos. Revisar try_send que exige availableForWrite>=frame completo; si buffer es menor que frame, segmentar TX sin intercalar tramas. No reutilizar streams MPU ni IR8. UART0 ESP limpia de logs; residuos ROM resincronizados. Reutilizar decoders solo tras fijar wire propio.

### [Wi-Fi y preferencias LittleFS](../docs/modules/wifi.md)

Conservar estados STA/AP, scan asíncrono, reintento/fallback, cache de preferencias y escrituras temporales. Sin comandos AT.

Reformas: Configurar identidad AutoFinalRobo2 y provisioning sin defaults privados. ESP12S flash/mode/partición pendientes; no heredar combinación board esp01_1m con linker4m3m a ciegas. Preservar configuración al actualizar assets; evitar formato automático ante fallo FS. Confirmar GPIO LED/boot del shield. Los ajustes de red no bloquean control AVR.

## Subcarpetas

- [esp01](esp01/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).
