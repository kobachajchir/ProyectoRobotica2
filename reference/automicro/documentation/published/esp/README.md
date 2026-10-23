# Firmware ESP01 — manual de unidades

Metodología [AMDOC 1](../../METHOD.md). Fotografía de fuente: 2026-10-20. Las inferencias y limitaciones se indican por ficha.

Ver [arquitectura](architecture.md), [conexiones entre manuales](../shared/CONNECTIONS.md) y [pantallas F4 ↔ ESP ↔ Web](../shared/SCREEN_SYSTEM.md).

| Unidad | Capa | Funciones públicas | Internas |
| --- | --- | --- |
| [Selección de modo de arranque ESP](libraries/boot-mode.md) | integración | 3 | 0 |
| [Puertos y adaptadores de plataforma ESP](libraries/ports-platform.md) | plataforma | 34 | 1 |
| [Configuración, flags y diagnósticos auxiliares](libraries/configuration-types.md) | datos | 1 | 2 |
| [Registros de comandos y sus políticas](libraries/command-catalog.md) | contrato | 10 | 0 |
| [Perfiles de extensión F4](libraries/extension-profiles.md) | contrato | 6 | 1 |
| [Decodificadores de payload F4](libraries/payload-decoders.md) | núcleo | 17 | 5 |
| [Composición y ciclo principal ESP](libraries/application.md) | composición | 10 | 7 |
| [Codec y almacenamiento LittleFS](libraries/littlefs-adapter.md) | plataforma | 2 | 7 |
| [Preferencias NVS y caché](libraries/preferences-store.md) | servicio | 11 | 8 |
| [Ensamblaje de framebuffer OLED](libraries/oled-canvas.md) | núcleo | 14 | 5 |
| [Fachada de sensores ESP](libraries/sensors-facade.md) | integración | 2 | 1 |
| [Streams MPU/IR y solicitudes en espera](libraries/streams.md) | núcleo | 21 | 16 |
| [Base temporal y timers](libraries/timebase.md) | servicio | 5 | 0 |
| [Broker de transacciones UART](libraries/uart-transactions.md) | núcleo | 11 | 10 |
| [Handlers UNER de aplicación ESP](libraries/uner-application.md) | integración | 25 | 67 |
| [Parser UNER v2 ESP](libraries/uner-core.md) | núcleo | 6 | 3 |
| [Dispositivo y dispatch UNER](libraries/uner-device.md) | núcleo | 13 | 5 |
| [Fachada y compatibilidad UNER ESP](libraries/uner-handle.md) | integración | 23 | 7 |
| [Cola UNER](libraries/uner-queue.md) | núcleo | 3 | 0 |
| [Transporte serial Arduino](libraries/serial-transport.md) | plataforma | 4 | 0 |
| [Servidor HTTP y conexiones WebSocket](libraries/http-websocket.md) | plataforma | 7 | 11 |
| [Arbitraje de exploración Wi-Fi](libraries/scan-arbiter.md) | núcleo | 5 | 1 |
| [Snapshot de resultados Wi-Fi](libraries/scan-snapshot.md) | datos | 5 | 0 |
| [Servicio de radio Wi-Fi](libraries/wifi-service.md) | servicio | 47 | 10 |
| [Indicador LED de conectividad](libraries/status-led.md) | plataforma | 2 | 1 |
| [Fachada Wi-Fi de aplicación](libraries/wifi-facade.md) | integración | 29 | 4 |
| [Bridge JSON, sesiones y traducción de pantallas](libraries/json-bridge.md) | integración | 22 | 328 |
| [Sesiones WebSocket y autoridad STM](libraries/sessions.md) | núcleo | 16 | 4 |
