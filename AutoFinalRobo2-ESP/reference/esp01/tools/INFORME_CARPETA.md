# Carpeta reference/esp01/tools

[Maestro](../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Servidor, bridge JSON y sesiones](../../../docs/modules/http-ws.md)

HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas.

Reformas: Portar whitelist y mensajes robot.*; fuente de autoridad Mega, ESP no inventa permiso ni estado. Quitar bypass de desarrollo como valor productivo, MPU/balance/modelo anterior. Integrar LittleFS con rutas index/assets y Content-Encoding correcto para cada gzip. Preservar arreglos de buffers del origen al decidir versiones de librerías. Esqueleto actual solo Serial/yield; red/bridge no activos.

## Archivos vecinos

- [websocket_copy_buffers.py](<websocket_copy_buffers.py>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../AutoFinalRobo2-Contracts/provenance/files.json).
