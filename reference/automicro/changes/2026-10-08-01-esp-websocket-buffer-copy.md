# AMI-20261008-01 — copia de buffers TCP para WebSocket ESP8266

- Fecha: 2026-10-08 (America/Buenos_Aires).
- Origen: Firmware ESP01 / FirmESP01MicroArduino2025.
- Referencia: cambios locales en platformio.ini y tools/websocket_copy_buffers.py.
- Estado: correccion preparada en fuentes; pendiente de compilar y cargar.
- Area: transporte HTTP upgrade y WebSocket; sin cambio de API o autenticacion.

## Comportamiento anterior y nuevo

La dependencia ESPAsyncWebServer 3.10.3 entrega la cabecera de trama a AsyncClient::add y libera inmediatamente su buffer. ESPAsyncTCP 2.0.0 usa apiflags=0 por defecto, conservando una referencia en TCP. La respuesta HTTP de upgrade tambien usa un String temporal sin copia explicita. Es una incompatibilidad de vida util de buffers que puede corromper cabeceras y retransmisiones.

El firmware agrega un middleware PRE de PlatformIO. Cuando se construye el nodo AsyncWebSocket.cpp, el middleware introduce ASYNC_WRITE_FLAG_COPY en las llamadas que entregan cabecera, payload y respuesta de upgrade a TCP. Conserva bytes y tiempos de API, pero hace que TCP sea propietario de la copia. Es idempotente y falla con un mensaje explicito si una futura dependencia elimina los puntos de parche. El siguiente build vuelve a aplicarlo si se reinstalan dependencias.

## Flujo y contrato

1. El navegador abre /ws por HTTP y recibe el upgrade WebSocket.
2. La ESP copia los buffers en TCP antes de liberar memoria temporal. El servidor conserva JSON API v1, hello, resumeToken, correlacion y formatos existentes.
3. Los comandos, ACK, timeouts, sesiones PIN F4 y permisos no cambian.
4. La Web y Qt no necesitan adaptar sus tramas. El firmware ESP requiere rebuild y upload; esta correccion no exige regenerar LittleFS.

## Impacto

| Repositorio | Estado | Motivo |
| --- | --- | --- |
| FirmwareF4 | no_aplica | No se modifica UART, UNER ni autenticacion F4. |
| Firmware ESP01 | origen | Middleware y configuracion de build preparados; no compilados ni cargados. |
| Web ESP01 | compatible | Mismos mensajes y formato WebSocket; sin cambios de consumidor para esta correccion. |
| AutoMicroQtDesktop | compatible | Mismo transporte y contrato de mensajes; sin cambios de consumidor para esta correccion. |

## Evidencia y limites

Inspeccion de las dependencias locales ESPAsyncWebServer 3.10.3 y ESPAsyncTCP 2.0.0. El navegador reporto bits RSV invalidos. El volcado suministrado por el usuario muestra una asignacion fallida de 32 bytes, pila y nuevo arranque. La direccion de la asignacion no pudo simbolizarse porque addr2line fallo al arrancar con Illegal System DLL Relocation. No se atribuye todo el reinicio a este defecto sin simbolizar el volcado o probar la placa.

No se ejecutaron builds, tests, validaciones de runtime ni upload. La biblioteca instalada no se modifica en este turno: el middleware aplica la correccion en el siguiente build.

## Trabajo separado pendiente

El usuario solicito acceso de depuracion local tambien en la SPA servida por ESP, con modo desarrollador activo y permiso limitado al Wi-Fi nativo. Se preparo una propuesta separada, pero la revision automatica rechazo aplicarla por no considerar suficientemente demostrado el alcance de autenticacion. No se modifico UserContext ni los permisos del firmware; el acceso de depuracion sigue limitado al servidor de desarrollo. Esta ficha no declara ese flujo implementado.

## Actualizaciones posteriores

- 2026-10-09: middleware ampliado para buffers HTTP y conservacion del ultimo bloque ante TCP parcial, documentado en AMI-20261009-01. Mantiene el parche WebSocket previo. Fuente sin nuevo build ni upload.

- 2026-10-09: el usuario reporto FileNotFoundError durante CollectBuildFiles, antes de compilar, en esp01_1m. El middleware leia node.get_abspath(), que apunta al nodo virtual VariantDir en .pio/build. Corregido a node.srcnode().get_abspath() para leer/parchear el fuente real de la dependencia. El codigo local de PlatformIO piobuild.py registra VariantDir con duplicate=false y tambien usa srcnode() para filtrar middleware. Se conserva el nodo de build retornado y el parche idempotente TCP; no cambia el contrato. Correccion de fuente local, sin build/tests/validaciones ni upload ejecutados; exito de compilacion pendiente del siguiente intento.

- 2026-10-08: tras aprobacion explicita adicional del usuario, el flujo local de depuracion se aplico en fuentes Web/ESP y quedo registrado en AMI-20261008-02. Sustituye el estado pendiente del apartado anterior; sigue sin compilarse ni cargarse en placa.
