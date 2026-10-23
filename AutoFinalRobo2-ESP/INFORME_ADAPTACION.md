# Adaptación esp

[Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md).

reference/esp01/src conserva fuentes y fragmentos completos, linker y parche de buffers como referencia. El platformio activo usa framework Arduino/LittleFS y perfil candidato esp12e; confirmar flash/board del ESP12S antes de build. Fuente main.cpp actual solo inicia Serial y yield: no crea AP, servidor ni enlace al Mega todavía. No se importan preferencias/credenciales ni el filesystem anterior. Hay que portar catálogo, bridge y rutas antes de servir control real.

## Guías junto al código

| Subsistema | Para qué sirve |
| --- | --- |
| [UART, parser y transacciones](docs/modules/uart.md) | Parser/cola/transporte Arduino y broker separan aceptación, respuesta y timeout. Tablas f4_* son del robot anterior; no basta cambiar su nombre. |
| [Wi-Fi y preferencias LittleFS](docs/modules/wifi.md) | Conservar estados STA/AP, scan asíncrono, reintento/fallback, cache de preferencias y escrituras temporales. Sin comandos AT. |
| [Servidor, bridge JSON y sesiones](docs/modules/http-ws.md) | HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas. |

Cada carpeta contiene INFORME_CARPETA.md con enlaces. reference/ no participa en la configuración activa. Los archivos originales se identifican por hash en Contracts/provenance/files.json. No se modifica ni publica en AutoMicro.
