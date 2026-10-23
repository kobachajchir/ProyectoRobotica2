# AMI-20261005-05 — Clientes Wi-Fi y páginas Web con avisos separados y numerados

- Fecha: 2026-10-05 (America/Argentina/Buenos_Aires)
- Origen: FirmwareF4 / Firmware ESP01 / Web ESP01.
- Referencia: checkouts locales modificados. F4 sobre `5f49df1d8c903dc9c77c37d116daa20f74762460`; Web sobre `eca75e542b06ebc49a693aaa18e0a2b7d4a46ed4`. Estos hashes no representan las fuentes locales completas.
- Estado: contrato definido antes de implementar y cambios aplicados en las fuentes locales de F4, ESP y Web; sin build ni instalación.
- Área: eventos UNER / WebSocket / notificaciones OLED / carga de página.

## Contrato anterior y nuevo

La distinción AP/Web ya existía: `0x84/0x85` avisaban asociación/salida del SoftAP, y `0x89/0x8A` apertura/cierre de WebSocket. La F4 mostraba Dispositivo conectado/desconect y Web conectada/desconect, sin número. Un WebSocket Qt también podía generar el aviso Web.

- `0x84 EVT_AP_CLIENT_JOIN` y `0x85 EVT_AP_CLIENT_LEAVE`: conservan `ap_clients:u8` en el byte 0. Los callbacks reales de estación agregan `client_number:u8` en el byte 1, usando el AID del SDK. La numeración identifica la asociación Wi-Fi mientras permanece activa. Los avisos antiguos de un byte y el fallback por conteo no inventan una identidad.
- `0x86/0x87 EVT_APP_USER_CONNECTED/DISCONNECTED`: conservan `client_id:u32 LE`, cuatro bytes, para apertura/cierre del transporte WebSocket, incluido Qt Wi-Fi.
- `0x89/0x8A EVT_WEBSERVER_CLIENT_CONNECTED/DISCONNECTED`: conservan `client_id:u32 LE` y agregan `client_number:u8` en el byte 4. Se emiten exclusivamente cuando la página confirma estar cargada y cuando se cierra ese socket ya marcado como página. La numeración Web es el slot 1 o 2 del ESP, estable durante esa conexión; es independiente del AID Wi-Fi. Los payloads antiguos de cuatro bytes siguen aceptados, sin número.
- Nuevo comando JSON v1 `esp.web.page.ready`, destino ESP, `args:{}`. Lo envía Web después de `hello`, de `window.load` o `document.readyState === complete` y de un frame de render. Respuesta correlacionada `{ready:true, clientNumber:1|2}`. Es idempotente por socket y no concede permisos, autenticación ni streams. Puede usarse antes del PIN. Se anuncia mediante `hello.features.pageReady`.
- Pantallas sin renumerar: `0x020502` Web conectada, `0x020503` Web desconectada, `0x020601` cliente Wi-Fi conectado, `0x020602` cliente Wi-Fi desconectado. Textos: Web N + conectada/desconectada y Cliente N + conectado/desconectado, con fuente pequeña para mostrar las palabras completas.
- La cola OLED y el aviso diferido conservan `clientNumber` por entrada. Los reportes de notificación `screen.changed` y el snapshot `GET_CURRENT_SCREEN` conservan el prefijo de nueve bytes (`screenCode:u32 LE, source:u8=3, remainingMs:u16 LE, totalMs:u16 LE`) y agregan `clientNumber:u8` en el byte 9 solo para avisos numerados. El ESP lo expone como `data.clientNumber`; el visor Web reproduce el mismo texto.

## Flujo completo

1. Asociarse al SoftAP sin abrir la página solo produce el evento y aviso Cliente N. Cargar archivos HTTP no equivale a una página operativa.
2. Abrir un socket aceptado (máximo dos) produce `0x86`; el `hello` continúa independiente del PIN. La Web confirma la carga con `esp.web.page.ready`.
3. El ESP marca únicamente ese socket y emite `0x89`. La F4 encola el aviso con su número propio. Repetir el request no duplica el aviso.
4. Al cerrar la página, `pagehide` solicita el cierre del socket y el evento de desconexión del ESP emite `0x8A` solo si esa conexión había confirmado la carga. El cierre abrupto se informa cuando el transporte detecta la desconexión.
5. Salir de la red produce `0x85` con el AID correspondiente; no se confunde con cerrar la página. Si también cae su socket Web, ambos cambios se informan por separado.
6. La petición de página lista usa el envelope y correlación normales JSON v1, sin ACK intermedio. Web usa timeout de 3 s; argumentos extra son rechazados y un socket sin slot produce error busy. `hello.features.pageReady` permite omitir el comando con ESP anteriores. La presencia Web sigue siendo distinta de la autenticación.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo |
| --- | --- | --- |
| FirmwareF4 | origen | Eventos opcionales y cola de avisos numerados implementados en uner_app.c, oled_utils.c y oled_handle.h; snapshot/reportes extendidos. |
| Firmware ESP01 | adaptado | AID Wi-Fi, página lista por socket, eventos independientes, capability y decodificación implementados en webserver_ws.cpp, callbacks y ws_bridge. |
| Web ESP01 | adaptado | Confirmación de carga, cierre de página y visor numerado; catálogo y manifest de eventos actualizados. |
| AutoMicroQtDesktop | pendiente | Sin cambios en esta tarea. Catálogo admite 0..255 bytes y el codec de pantalla >=9: conserva el prefijo y progreso. El diagnóstico estructurado de client_id en 0x89/0x8A exige cuatro bytes y la numeración queda pendiente; los cinco bytes nuevos siguen disponibles como payload. |

## Compatibilidad y orden de integración

Actualizar las fuentes de ESP/F4 y el bundle Web de forma coordinada. La Web nueva consulta la capability para no enviar el comando a ESP antiguos. F4 y Web toleran números ausentes; no los deducen a partir del conteo total. Qt Wi-Fi conserva sus eventos de transporte y no emite página lista. Los builds servidos por Qt aún no incluyen el nuevo comportamiento Web hasta regenerar su bundle.

## Evidencia y límites

Inspección de productores, callbacks del SDK y renderers. No se ejecutan tests, compilaciones, validaciones, flasheo ni prueba física por instrucción del usuario. El registro publicado no publica ni instala el código de los consumidores.

Fuentes principales: F4 `Core/Src/uner_app.c`, `Core/Src/oled_utils.c`, `Core/Inc/oled_handle.h`; ESP `src/webserver_ws.cpp`, `src/uner_app_handlers.cpp`, `src/ws_bridge/dispatch.inc`, `src/ws_bridge/reports.inc`; Web `src/contexts/WebSocketContext.tsx`, `src/screens/connectivityScreens.ts`, `src/screens/screenRenderRegistry.ts`. Se preservó el trabajo local anterior, incluidos los streams concurrentes AMI-20261005-04. El código consumidor queda local sin commit ni push.

## Actualización de implementación

- 2026-10-05: adaptación F4/ESP/Web aplicada en fuente. El límite de dos WebSockets se mantiene. Documentación local agregada en los tres consumidores. Regeneración de bundles, compilación, flasheo y comprobación física diferidos por instrucción del usuario.
