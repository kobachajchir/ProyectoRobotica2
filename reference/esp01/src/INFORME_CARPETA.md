# Carpeta reference/esp01/src

[Maestro](../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Servidor, bridge JSON y sesiones](../../../docs/modules/http-ws.md)

HTTP/WebSocket, ownership de buffers, fragmentos .inc, sesiones y traducción se copian como conjunto. Los .inc son parte de la unidad C++, no librerías aisladas.

Reformas: Portar whitelist y mensajes robot.*; fuente de autoridad Mega, ESP no inventa permiso ni estado. Quitar bypass de desarrollo como valor productivo, MPU/balance/modelo anterior. Integrar LittleFS con rutas index/assets y Content-Encoding correcto para cada gzip. Preservar arreglos de buffers del origen al decidir versiones de librerías. Esqueleto actual solo Serial/yield; red/bridge no activos.

### [UART, parser y transacciones](../../../docs/modules/uart.md)

Parser/cola/transporte Arduino y broker separan aceptación, respuesta y timeout. Tablas f4_* son del robot anterior; no basta cambiar su nombre.

Reformas: Nuevo catálogo/payloads Mega con capabilities. Mantener FIFO y una transacción correlacionable en vuelo; resolver ACK tardíos. Revisar try_send que exige availableForWrite>=frame completo; si buffer es menor que frame, segmentar TX sin intercalar tramas. No reutilizar streams MPU ni IR8. UART0 ESP limpia de logs; residuos ROM resincronizados. Reutilizar decoders solo tras fijar wire propio.

### [Wi-Fi y preferencias LittleFS](../../../docs/modules/wifi.md)

Conservar estados STA/AP, scan asíncrono, reintento/fallback, cache de preferencias y escrituras temporales. Sin comandos AT.

Reformas: Configurar identidad AutoFinalRobo2 y provisioning sin defaults privados. ESP12S flash/mode/partición pendientes; no heredar combinación board esp01_1m con linker4m3m a ciegas. Preservar configuración al actualizar assets; evitar formato automático ante fallo FS. Confirmar GPIO LED/boot del shield. Los ajustes de red no bloquean control AVR.

## Archivos vecinos

- [app_bindings.cpp](<app_bindings.cpp>)
- [app_bindings.h](<app_bindings.h>)
- [bitMap.h](<bitMap.h>)
- [boot_mode.cpp](<boot_mode.cpp>)
- [boot_mode.h](<boot_mode.h>)
- [bridge_platform_esp.cpp](<bridge_platform_esp.cpp>)
- [bridge_ports.h](<bridge_ports.h>)
- [config.h](<config.h>)
- [esp_platform.cpp](<esp_platform.cpp>)
- [esp_platform.h](<esp_platform.h>)
- [esp_system.cpp](<esp_system.cpp>)
- [esp_system.h](<esp_system.h>)
- [f4_command_registry.c](<f4_command_registry.c>)
- [f4_command_registry.h](<f4_command_registry.h>)
- [f4_extension_profiles.c](<f4_extension_profiles.c>)
- [f4_extension_profiles.h](<f4_extension_profiles.h>)
- [f4_payload_decoders.c](<f4_payload_decoders.c>)
- [f4_payload_decoders.h](<f4_payload_decoders.h>)
- [globals.h](<globals.h>)
- [main.cpp](<main.cpp>)
- [nvs_littlefs.cpp](<nvs_littlefs.cpp>)
- [nvs_littlefs.h](<nvs_littlefs.h>)
- [nvs_store.cpp](<nvs_store.cpp>)
- [nvs_store.h](<nvs_store.h>)
- [nvs_utils.cpp](<nvs_utils.cpp>)
- [nvs_utils.h](<nvs_utils.h>)
- [oled_canvas_transfer.c](<oled_canvas_transfer.c>)
- [oled_canvas_transfer.h](<oled_canvas_transfer.h>)
- [sensors.cpp](<sensors.cpp>)
- [sensors.h](<sensors.h>)
- [stream_broker.c](<stream_broker.c>)
- [stream_broker.h](<stream_broker.h>)
- [stream_request_waiters.c](<stream_request_waiters.c>)
- [stream_request_waiters.h](<stream_request_waiters.h>)
- [timebase.cpp](<timebase.cpp>)
- [timebase.h](<timebase.h>)
- [timebase_core.cpp](<timebase_core.cpp>)
- [uart_transaction_manager.c](<uart_transaction_manager.c>)
- [uart_transaction_manager.h](<uart_transaction_manager.h>)
- [uner_app_handlers.cpp](<uner_app_handlers.cpp>)
- [uner_app_handlers.h](<uner_app_handlers.h>)
- [uner_app_ports.h](<uner_app_ports.h>)
- [uner_cmdspec.c](<uner_cmdspec.c>)
- [uner_cmdspec.h](<uner_cmdspec.h>)
- [uner_core.c](<uner_core.c>)
- [uner_core.h](<uner_core.h>)
- [uner_device.c](<uner_device.c>)
- [uner_device.h](<uner_device.h>)
- [uner_diagnostics_esp.cpp](<uner_diagnostics_esp.cpp>)
- [uner_handle.cpp](<uner_handle.cpp>)
- [uner_handle.h](<uner_handle.h>)
- [uner_handle_compat.cpp](<uner_handle_compat.cpp>)
- [uner_platform_esp.h](<uner_platform_esp.h>)
- [uner_queue.c](<uner_queue.c>)
- [uner_queue.h](<uner_queue.h>)
- [uner_transport_serial.cpp](<uner_transport_serial.cpp>)
- [uner_transport_serial.h](<uner_transport_serial.h>)
- [uner_v2.h](<uner_v2.h>)
- [UNERUtils.cpp](<UNERUtils.cpp>)
- [UNERUtils.h](<UNERUtils.h>)
- [webserver_ports.h](<webserver_ports.h>)
- [webserver_ws.cpp](<webserver_ws.cpp>)
- [webserver_ws.h](<webserver_ws.h>)
- [wifi_driver_port.h](<wifi_driver_port.h>)
- [wifi_scan_arbiter.c](<wifi_scan_arbiter.c>)
- [wifi_scan_arbiter.h](<wifi_scan_arbiter.h>)
- [wifi_scan_snapshot.c](<wifi_scan_snapshot.c>)
- [wifi_scan_snapshot.h](<wifi_scan_snapshot.h>)
- [wifi_service.cpp](<wifi_service.cpp>)
- [wifi_service.h](<wifi_service.h>)
- [wifi_service_types.h](<wifi_service_types.h>)
- [wifi_status_led.cpp](<wifi_status_led.cpp>)
- [wifi_status_led.h](<wifi_status_led.h>)
- [wifi_status_led_core.cpp](<wifi_status_led_core.cpp>)
- [wifi_utils.cpp](<wifi_utils.cpp>)
- [wifi_utils.h](<wifi_utils.h>)
- [ws_json_bridge.cpp](<ws_json_bridge.cpp>)
- [ws_json_bridge.h](<ws_json_bridge.h>)
- [ws_limits.h](<ws_limits.h>)
- [ws_session_manager.c](<ws_session_manager.c>)
- [ws_session_manager.h](<ws_session_manager.h>)

## Subcarpetas

- [ws_bridge](ws_bridge/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../AutoFinalRobo2-Contracts/provenance/files.json).
