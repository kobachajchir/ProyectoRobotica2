# AMI-20261005-01 — Qt operativo, resultados AT y consulta AP fallback

- Fecha: 2026-10-05, America/Buenos_Aires.
- Origen: FirmwareF4 / AutoMicroQtDesktop.
- Referencias: Qt `6671624`, ESP `571be8a`, Web `eca75e5`, rama `work/nueva-etapa`, commits locales. F4 permanece sin commit para preservar su trabajo previo en los mismos archivos. Los commits de aplicaciones no se publicaron al remoto.
- Estado: implementado en fuentes; sin build, tests, validación ni prueba física en esta tarea.
- Área: UNER, transporte, WebSocket JSON y flujos de operación.

## Contrato anterior y nuevo

`0x1C GET_AP_FALLBACK`, antes reservado, es F4: request vacío, respuesta final de 3 bytes `[status, enabled, syncState]`. `enabled` es 0/1; syncState 0 desconocido, 1 pendiente, 2 aplicado, 3 error. Status satisfactorio 0. En AT la política pertenece a F4; en Web se contrasta la sincronización del espejo ESP. Requiere sesión STM, consulta idempotente, timeout 500 ms.

JSON agrega `stm.wifi.apFallback.get` y `stm.wifi.apFallback.set`. GET devuelve status, enabled bool, syncState entero. SET requiere exactamente `{enabled: boolean}` y usa el SET_PREFERENCES F4 existente `0x63`: máscara `0x01000000` little endian y un byte bool. Respuesta existente `[status, changedMask:u32LE]`, longitud 5; después se consulta 0x1C. No se confunde persistencia F4 con confirmación de aplicación ESP.

`0x1B SET_AP_FALLBACK` continúa siendo el espejo ESP de AMI-20261004-01, request bool de un byte, respuesta `[status, enabled]`, ACK intermedio seguido de mismo comando. Qt incorpora ambos IDs en catálogo/codec; Web opera el contrato F4 tipado.

GET_STATUS local F4 (`0x30`) conserva sus primeros 92 bytes y sus seis contadores, y agrega byte 92 `0xA1` como marca de formato: longitud nueva 93. Contadores u32LE en 68,72,76,80,84,88: timeouts AT, fallos de envío, drops de cola, errores de cabecera IPD, drops UDP y pares rechazados. Backend en 64 (0 detectando,1 AT,2 Web), protocolo 65 (0 TCP,1 UDP), ready 66, cola 67. La respuesta variable ESP no recibe esta marca. El consumidor Qt actualizado necesita la F4 nueva para reconocer esta extensión local; no atribuye formato F4 por longitud 92.

Los comandos WiFi solicitados por PC Qt a F4 sobre USB o AT UART conservan IDs. Operaciones asíncronas del módem ahora responden ACK de aceptación y luego mismo comando con estado final; CONNECT responde `FE + IPv4`, START_AP satisfactorio `00 + IPv4`, AUTO_RECONNECT `[status, enabled]`; operaciones restantes status de un byte. Escaneo responde `[status,count]`, lista cacheada `[status,count,lenSSID,SSID,...]`. Detalle cacheado conserva `[lenSSID,SSID,rssi:i8,security,channel]`; ausencia retorna NACK. Credenciales AT `0x12` responde status al actualizar el contexto y Qt solicita CONNECT separado. No se promete persistencia de ese contexto F4. Clear/reset limpian el contexto solo tras confirmación del módem.

## Flujo completo

1. Qt selecciona USB, ESP Web o AT TCP/UDP; AT usa UNER con PC Qt como origen y F4 como destino. USB conserva proxy ESP Web y se adapta al backend AT reportado por F4.
2. Un broker serializa heartbeat y solicitudes. ACK no concluye las operaciones que requieren respuesta final. Resultado funcional inválido de preferencias, PID, motion o AP fallback se informa como fallo.
3. F4 conserva la solicitud AT en espera; eventos finales del módem contestan al transporte/nodo solicitante. Timeout F4 20 s; Qt AT usa 21 s para la familia WiFi. Eventos de paso intermedio de configuración AP no acreditan finalización.
4. STOP adelanta la cola y cancela solicitudes de marcha/TEST/balance pendientes. Puede interrumpir la espera de otra solicitud; su resultado remoto queda incierto. No hay reenvío automático ni reanudación de movimiento al reconectar.
5. Autenticación y permisos F4 se mantienen. UDP conserva una trama completa por datagrama. Reconexión conserva el transporte seleccionado y vuelve a establecer sesión.

## Impacto

| Repositorio | Estado | Acción |
| --- | --- | --- |
| FirmwareF4 | origen | GET_AP_FALLBACK, extensión GET_STATUS y respuestas finales AT implementados localmente; árbol con trabajo previo preservado. |
| Firmware ESP01 | adaptado | Registro/especificación 0x1C y rutas JSON get/set con codec de respuesta. |
| Web ESP01 | adaptado | Control tipado de preferencia, estado sin confirmar y reintento STA en WiFi. |
| AutoMicroQtDesktop | adaptado | Catálogo/codec, broker, navegación QML, calibración local, herramientas OLED e identidad Web. |

## Orden de integración y límites

Actualizar F4 antes de usar la nueva consulta o extensión de estado, y ESP antes de las rutas JSON. Qt/Web anteriores no ganan estas operaciones por actualizar únicamente firmware. Firmware anterior puede rechazar 0x1C y la UI conserva estado sin confirmar.

OLED conserva el contrato 0x6D: chunks Qt 112 bytes, offsets y CRC confirmados, sin reintento automático. No se inventa framebuffer remoto. Captura Balance usa tiempos de llegada, no ciclos F4. Recorrido sigue siendo simulación local; sin nuevo ejecutor físico ni nuevo protocolo de ensayo.

Implementación y límites de UI/algoritmos: [informe](../docs/QT_OPERATIONAL_IMPLEMENTATION_20261005.md). Los artefactos de build existentes no corresponden a este cambio. Compatibilidad física sin verificar. Se necesitan nuevos builds y flasheo para usar estas fuentes.

## Compilación Qt solicitada posteriormente — 2026-10-05

Qt `df666e6` corrige una llave faltante en OledTransferService::sendOperation (error C1075), sin cambio wire. Target AutoMicroQtDesktop, Qt 6.9.0, Visual Studio 2022/MSVC, Release: segunda pasada finalizada con salida 0 y ejecutable build-msvc/Release/AutoMicroQtDesktop.exe generado. La configuración emitió un aviso no bloqueante de cabeceras Vulkan ausentes. No se ejecutaron tests, interfaz, revisión visual ni hardware; F4/ESP/Web no se recompilaron. El commit Qt permanece local.
