# Servidor, bridge JSON y sesiones

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas.

## Adaptación

Portar whitelist y mensajes robot.*; fuente de autoridad Mega, ESP no inventa permiso ni estado. Quitar bypass de desarrollo como valor productivo, MPU/balance/modelo anterior. Integrar LittleFS con rutas index/assets y Content-Encoding correcto para cada gzip. Preservar arreglos de buffers del origen al decidir versiones de librerías. Esqueleto actual solo Serial/yield; red/bridge no activos.

## Evidencia y alcance

- [reference/esp01/src/webserver_ws.cpp](../../reference/esp01/src/webserver_ws.cpp)
- [reference/esp01/src/ws_json_bridge.cpp](../../reference/esp01/src/ws_json_bridge.cpp)
- [reference/esp01/src/ws_bridge/dispatch.inc](../../reference/esp01/src/ws_bridge/dispatch.inc)
- [reference/esp01/src/ws_session_manager.c](../../reference/esp01/src/ws_session_manager.c)
- [reference/esp01/tools/websocket_copy_buffers.py](../../reference/esp01/tools/websocket_copy_buffers.py)

Hallazgos: AFRDOC-008, AFRDOC-009. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
