# AMI-20261009-01 — copia TCP HTTP y bloques finales pendientes

- Fecha: 2026-10-09 (America/Buenos_Aires).
- Origen: Firmware ESP01 / FirmESP01MicroArduino2025.
- Referencia: tools/websocket_copy_buffers.py, cambio local sin commit ni push del firmware.
- Estado: implementado en fuente; middleware aplica el parche durante el siguiente build.
- Area: transporte HTTP, sin nuevos comandos, IDs, campos ni autenticacion.
- Antecedente: AMI-20261008-01, copia TCP WebSocket y resolucion srcnode de SCons.

## Comportamiento anterior y nuevo

El usuario reporta GET /login con HTTP 200 y ERR_CONTENT_LENGTH_MISMATCH. La ruta SPA sirve /dist/index.html.gz mediante AsyncFileResponse de ESPAsyncWebServer 3.10.3. ESPAsyncTCP 2.0.0 usa apiflags=0 por defecto, conservando referencias TCP.

WebResponses.cpp entrega cabeceras sin copia y limpia _assembled_headers antes de su ACK. Tambien entrega _send_buffer sin copia, luego lo reutiliza/libera. El middleware ahora aplica ASYNC_WRITE_FLAG_COPY a cabeceras HTTP, contenido de respuestas basicas y bloques de AsyncAbstractResponse. Mantiene el parche WebSocket previo.

Ademas, el bucle de AsyncAbstractResponse marca RESPONSE_END al leer el ultimo bloque. Si TCP acepta solo parte (o cero bytes), deja un buffer pendiente pero no vuelve a procesarlo en el siguiente callback porque solo entra con RESPONSE_CONTENT. El parche mantiene RESPONSE_CONTENT mientras queden bytes y contabiliza cada escritura parcial en payloadlen/_writtenLength. Conserva Content-Length real; no oculta el truncamiento retirando la cabecera.

## Flujo completo

1. Navegador solicita /login por HTTP; fallback SPA responde con gzip desde LittleFS.
2. El build aplica un middleware a WebResponses.cpp, resolviendo node.srcnode().get_abspath(). Conserva el nodo virtual retornado para compilacion. El parche es idempotente y falla explicitamente si cambian los puntos de insercion de la dependencia.
3. Cabeceras y bloques pasan a TCP con copia antes de liberar/reutilizar memoria del servidor.
4. Ante escritura parcial, el resto queda pendiente y se vuelve a intentar; solo termina tras entregar el bloque entero. El formato gzip, MIME, rutas, cache, Content-Length y API permanecen iguales.
5. No cambia permisos, timeouts, correlacion ni ACK de comandos. No afecta el scope Wi-Fi temporal, preferencias F4 o persistencia.

## Impacto en repositorios

| Repositorio | Estado | Motivo concreto |
| --- | --- | --- |
| FirmwareF4 | no_aplica | Cambio exclusivamente HTTP/TCP ESP; UART y UNER intactos. |
| Firmware ESP01 | origen | Middleware extendido en fuente; biblioteca se parchea al siguiente build. |
| Web ESP01 | compatible | Consume las mismas rutas y gzip; no cambia request, cabeceras ni bundle por esta correccion. |
| AutoMicroQtDesktop | compatible | Sin comandos nuevos ni cambios en mensajes; no requiere entradas de catalogo adicionales. |

## Compatibilidad y orden de integracion

Compilar y cargar firmware ESP para aplicar la correccion. Este cambio no requiere regenerar/cargar LittleFS; los cambios Web de bypass anteriores si requieren su bundle actualizado. Uploadfs por si solo no incorpora un cambio C++ ni el middleware al firmware instalado.

## Evidencia y limites

Actualizacion 2026-10-09 por pedido expreso: platformio run --environment esp01_1m termino SUCCESS (salida 0) en 7,928 s. Build incremental recompilo WebResponses.cpp, archivo ESPAsyncWebServer, link y firmware.bin. RAM 70072/81920 bytes (85,5 %); Flash 519123/1044464 bytes (49,7 %). Sin clean, tests, upload, uploadfs ni validacion fisica/HTTP. El middleware no produjo FileNotFoundError ni fallo de anclas. Este resultado acredita compilacion, no solucion del sintoma en placa.

Inspeccion de rutas locales, WebResponses.cpp y ESPAsyncTCP.cpp/.h. No se ejecutaron tests, compilaciones, validaciones HTTP/navegador ni cargas, conforme a instruccion del usuario. El sintoma puede tambien provenir de un reinicio o lectura incompleta de LittleFS; no se afirma causalidad exclusiva ni solucion fisica comprobada. Si persiste tras cargar firmware, falta el log serie completo durante la descarga y la respuesta HTTP real para distinguir esas causas.
