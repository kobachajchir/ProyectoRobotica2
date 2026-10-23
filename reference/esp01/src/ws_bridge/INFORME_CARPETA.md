# Carpeta reference/esp01/src/ws_bridge

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Servidor, bridge JSON y sesiones](../../../../docs/modules/http-ws.md)

HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas.

Reformas: Portar whitelist y mensajes robot.*; fuente de autoridad Mega, ESP no inventa permiso ni estado. Quitar bypass de desarrollo como valor productivo, MPU/balance/modelo anterior. Integrar LittleFS con rutas index/assets y Content-Encoding correcto para cada gzip. Preservar arreglos de buffers del origen al decidir versiones de librerías. Esqueleto actual solo Serial/yield; red/bridge no activos.

## Archivos vecinos

- [common.inc](<common.inc>)
- [debug_wifi.inc](<debug_wifi.inc>)
- [dispatch.inc](<dispatch.inc>)
- [esp_commands.inc](<esp_commands.inc>)
- [lifecycle.inc](<lifecycle.inc>)
- [oled_canvas.inc](<oled_canvas.inc>)
- [private_context.h](<private_context.h>)
- [reports.inc](<reports.inc>)
- [requests.inc](<requests.inc>)
- [services.inc](<services.inc>)
- [stm_commands.inc](<stm_commands.inc>)
- [validation.inc](<validation.inc>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
