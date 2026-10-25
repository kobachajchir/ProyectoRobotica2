> **Vigente AFR2-20261024-15:** ROBO2/perfil2 + EX01v6 y afr2-ws-1. [Matriz actual](../docs/ALINEACION_MAESTRO.md); formatos inferiores históricos. system.info añade [salud FS/paquete](SYSTEM_INFO_FILESYSTEM_15.md) local ESP, sin nuevo comando/wire ni ScreenCode. Compilado, sin aceptación física; permisos/gates conservados.

> **Vigente AFR2-20261024-08:** [EX01v5](EX01_NETWORK_UI_5.md) añade estado0x53, avisos0x54 y job8 de reinicio ESP local. ROBO2/perfil2 y afr2-ws-1 conservados. Tres módulos compilados; sin pruebas físicas. Versiones inferiores históricas.

# Vigencia AFR2-20261024-07

ROBO2/perfil2 negocia EX01v4 para servicios de sistema y etiquetas de secuencias.
Consultar [formato v4](EX01_SYSTEM_SEQUENCE_4.md) y
[estado por módulo](../docs/ESTADO_IMPLEMENTACION.md). Los formatos históricos
inferiores no se deben mezclar: append v4 mide30bytes; INFO/REBOOT/BOOT usan
0x50/0x51/0x52. Compilado en Atmega/ESP/Web, sin ensayo de placas.

> Estado vigente **AFR2-20261024-02**, 24/10/2026: integración ROBO2/perfil2 + EX01v3 compilada en Atmega/ESP/Web; GOTO completo, configuración por catálogo compartido y jobs Wi-Fi OLED conectados. Paquete Web actualizado en ESP/data/dist. Sin pruebas de navegador/hardware ni flasheo; secuencias físicas mantienen bloqueo de memoria. Ver [matriz vigente](EX01_ALIGNMENT_3.md) y [evidencia](../changes/2026-10-24-02-project-alignment.md). Los estados previos que siguen son históricos.

# Contrato compartido AutoFinalRobo2

Actualización19,24/10: [ROBO2_PRODUCT_EXTENSION.md](ROBO2_PRODUCT_EXTENSION.md)
documenta el endpoint EX01 de producto ya escrito en ATmega sobre perfil2.
ESP/Web aún no negocian/consumen esa extensión; no anunciar capacidades por diseño.
El requisito de **un único WS** del flujo solicitado por el auto está pendiente
en ESP; el límite2 del servidor descrito abajo es el comportamiento anterior.
No confundir con perfil3/secuencias de21, diseñado y con IDs numéricos superpuestos.


Estado: sobre JSON Web↔ESP y perfil binario ESP↔ATmega2560 implementados en fuente; integración UART/TWI/UI en fuente sin verificar en AFR2-20261023-15. Perfil vigente: ROBO2/2.
- [Ficha inicial](../changes/2026-10-23-01-bootstrap.md)
- [Ficha Web y sobre WS AFR2-20261023-09](../changes/2026-10-23-09-web-app-implementation.md)
- [Servidor ESP y endpoint AVR AFR2-20261023-11](../changes/2026-10-23-11-esp-server-and-mega-link.md), [perfil UART 1](UART_PROFILE_1.md).
- No declara compatibilidad con `AutoMicro` ni reutiliza el namespace `stm.*`.

---

## 1. Arquitectura de enlaces y estados independientes

El sistema distingue explícitamente **tres estados independientes** que la interfaz Web nunca fusiona:

1. **Enlace Web ↔ ESP (`wsState`)**:
   - `disconnected` | `connecting` | `handshaking` | `ready` | `degraded`
   - Endpoint por defecto en el mismo origen (`/ws`) o `ws://<ip>/ws` (configurable en la sesión de la interfaz sin credenciales embebidas).
2. **Enlace ESP ↔ ATmega2560 (`megaLinkState`)**:
   - `unknown` | `disconnected` | `handshaking` | `ready` | `stale` | `fault`
   - Transporte físico UART0 ESP ↔ USART1 Mega (`D18/D19`) a `115200 8N1` mediante el selector `E_RX/E_TX` documentado en [WIFI_SHIELD](../hardware/WIFI_SHIELD.md). Sin comandos AT ni mezcla de logs con tramas.
3. **Permiso temporal de control Web (`controlLease`)**:
   - `none` | `requesting` | `held-by-me` | `held-by-other` | `local-override` | `expired`
   - El ATmega2560 es la autoridad final de seguridad, límites, timeout de control y `STOP`.

---

## 2. Sobre JSON en WebSocket (Web ↔ ESP)

Todas las tramas JSON entre la aplicación Web y el broker ESP8266 utilizan el esquema `afr2-ws-1`:

### 2.1. Petición (`req`)
```json
{
  "schema": "afr2-ws-1",
  "type": "req",
  "requestId": "req-1729699200000-1",
  "cmd": "system.hello",
  "payload": {
    "client": "afr2-web",
    "clientVersion": "0.1.0"
  },
  "ts": 1729699200000
}
```

### 2.2. Respuesta afirmativa (`res`)
```json
{
  "schema": "afr2-ws-1",
  "type": "res",
  "requestId": "req-1729699200000-1",
  "cmd": "system.hello",
  "ok": true,
  "status": "accepted",
  "payload": {
    "product": "AutoFinalRobo2",
    "brokerVersion": "0.1.0",
    "megaLink": "ready",
    "capabilities": {}
  },
  "ts": 1729699200050
}
```
- `status` distingue explícitamente:
  - `"accepted"`: encolado/aceptado por el broker o iniciado en el productor (no implica finalización física de una maniobra).
  - `"completed"`: acción confirmada por el productor autoritativo.

### 2.3. Respuesta de error (`err`)
```json
{
  "schema": "afr2-ws-1",
  "type": "err",
  "requestId": "req-1729699200000-2",
  "cmd": "robot.goto.start",
  "ok": false,
  "error": {
    "code": "CAPABILITY_UNAVAILABLE",
    "message": "Odometría de ruedas no conectada ni calibrada.",
    "retryable": false,
    "source": "atmega"
  },
  "ts": 1729699200100
}
```
- Códigos normalizados de error: `BAD_REQUEST`, `UNSUPPORTED_PRODUCT`, `MEGA_LINK_DOWN`, `LEASE_REQUIRED`, `LEASE_HELD_BY_OTHER`, `LOCAL_OVERRIDE`, `CAPABILITY_UNAVAILABLE`, `CALIBRATION_REQUIRED`, `BUSY_IN_FLIGHT`, `TIMEOUT`, `SUPERSEDED`, `INTERNAL_ERROR`.

### 2.4. Evento asíncrono (`evt`)
```json
{
  "schema": "afr2-ws-1",
  "type": "evt",
  "event": "robot.sensors.snapshot",
  "payload": {},
  "ts": 1729699200200
}
```
- Los eventos (`evt`) actualizan proyecciones de estado o progreso (`robot.goto.progress`, `robot.goto.finished`, `wifi.scan.result`, `robot.screen.snapshot`), pero **nunca resuelven promesas correlacionadas por `requestId`** salvo que el flujo de misión lo especifique como evento terminal separado de la aceptación inicial.

---

## 3. Handshake, Heartbeat y Reconexión

1. **Handshake inicial (`system.hello`)**:
   - Al abrir el WebSocket, el cliente envía `system.hello` con timeout de `5000 ms`.
   - El broker debe responder con `product: "AutoFinalRobo2"`, estado de `megaLink` y el objeto `capabilities`. Si `product !== "AutoFinalRobo2"`, el cliente cierra la sesión y reporta incompatibilidad.
2. **Latido (`system.alive`)**:
   - Una vez en `ready`, el cliente envía `system.alive` cada `2000 ms`.
   - Si no se recibe tráfico válido ni respuesta en `6000 ms`, la sesión pasa a desconectada, se cancelan todas las promesas en vuelo con `CONNECTION_CLOSED` y se libera el estado local de lease.
3. **Reconexión exponencial con jitter**:
   - Backoff `500 ms -> 1000 ms -> 2000 ms -> 4000 ms -> 8000 ms -> 15000 ms` (máximo) con `±20 %` de jitter aleatorio y contador de generación (`generationId`) para descartar callbacks de sockets obsoletos.

---

## 4. Prioridades, Transacción en Vuelo y `STOP`

1. **Prioridad de `robot.stop`**:
   - `robot.stop` es **idempotente y prioritario**. Se puede emitir incluso sin poseer `controlLease` o con otra petición en vuelo.
   - Emitir `robot.stop` invalida inmediatamente cualquier `robot.drive`, `robot.test.wheel` o `robot.goto.start` pendiente en el cliente (`SUPERSEDED`).
   - La UI muestra el envío inmediato de `STOP`, pero solo informa parada física confirmada cuando llega la confirmación del productor.
2. **Control manual (`robot.drive`)**:
   - Requiere `controlLease === "held-by-me"`.
   - Máximo una petición `robot.drive` en vuelo; si el usuario cambia la dirección en la Web mientras hay una en vuelo, se guarda únicamente la última consigna (`lastSampleSlot`) y se reemplaza la anterior.
   - Pérdida de foco (`blur`), `visibilitychange` a oculto o `pointercancel` detienen el envío periódico y emiten `robot.stop`.

---

## 5. Enlace binario ESP ↔ ATmega2560 (ROBO2, perfil 2)

Contrato vigente AFR2-20261023-15: [UART_PROFILE_2.md](UART_PROFILE_2.md),
[criterios comunes](ROBO2_VALIDACION.md) y parser/builder únicos [afr2_wire.h](afr2_wire.h).
Trama `ROBO2 N 3A A5 01 ROUTE CMD PAYLOAD XOR`: cinco bytes ASCII de cabecera,
dos tokens exactos, versión de framing 1, N<=64, total12+N<=76. XOR incluye
todos los bytes anteriores. Sustituye UNER/perfil1 sin fallback.

La ruta usa emisor en nibble alto y receptor en bajo, exactamente como F4:
ESP3→Mega1=0x31; Mega1→ESP3=0x13. Ambos validan framing y luego emisor/receptor
según rol, comando/longitud/cuerpo y correlación de sesión/transacción no cero.
Solo ESP inicia pedidos en este catálogo; no eventos UART ni pedidos Mega→ESP.
Tipos/endian/unidades/resultados/casos y tiempos en los dos documentos enlazados.
Fuente escrita en ambos extremos; no compilada ni probada. UI local integrada
no implica servicio UI remoto. STOP sigue sin hook de potencia en composiciones.

---

## 6. Telemetría Honesta y Capacidades

- **Sin telemetría inventada**: todo campo de sensor o actuador distingue `present`, `implemented`, `calibrated`, `valid` y `ageMs` (`fresh` | `stale` | `unknown`). Un valor desconocido jamás se rellena con `0`.
- **Cinco canales de línea (`IR1..IR5`) y ultrasonido (`HC-SR04`)**: independientes entre sí; nunca se infiere distancia desde los sensores de línea.
- **Cuatro ruedas Mecanum (`FL`, `FR`, `RL`, `RR`)**: los encoders Hall de rueda figuran como no cableados en la documentación suministrada (`unwired-in-supplied-docs`), por lo que la odometría y `GOTO` permanecen deshabilitados con motivo explícito hasta que el runtime reporte la capacidad implementada y calibrada.
- **Sin IMU/MPU ni balance**: este vehículo es un chasis Mecanum de 4 ruedas; no existe MPU6050 ni lazo de autobalance.

---

## 7. Réplica OLED 128×64 y Navegación con Encoder Único

- Los 4 códigos actuales en [screens.json](../documentation/screens.json) (`0x010001` Inicio, `0x010002` Menú principal, `0x020001` Sensores, `0x020002` Estado OLED) son identificadores locales del catálogo compartido.
- El visor Web implementa un rasterizador bit a bit de `128×64` píxeles (8 páginas verticales de 128 columnas) con la fuente `Font7x10` y las primitivas exactas de `afr2_oled.c` y `afr2_screens.c`, diferenciando claramente entre:
  - **Vista previa local determinista** (renderizada con el mismo algoritmo C portado a TypeScript).
  - **Snapshot remoto (`robot.screen.get`)** cuando el firmware lo reporte.
- Regla de entrada física y Web ([NAVIGATION.md](../documentation/NAVIGATION.md)):
  - Giro del encoder (`rotate`): desplaza selección o ajusta borrador.
  - Pulsación corta de `SW` (`select` < `800 ms`): confirma o ejecuta la opción activa.
  - Pulsación larga de `SW` (`menu` >= `800 ms`): abre `Menú principal` (`0x010002`) al cumplirse el umbral y suprime el clic corto al soltar.
  - Retroceso: **únicamente** seleccionando el ítem explícito `Volver` en pantalla.

---

## 8. Servido HTTP desde LittleFS (ESP8266)

- El empaquetador Web (`AutoFinalRobo2-Web/tools/package-web.mjs`) genera `artifacts/littlefs/dist/` con archivos comprimidos `.gz` y `manifest.json` (rutas lógicas, archivo servido, `Content-Type`, `Content-Encoding`, tamaño original/gzip y `sha256`); `copy-web-to-esp.mjs` los promueve a `ESP/data/dist/`. Ninguna de estas acciones se ejecutó en AFR2-20261023-11.
- La navegación de la SPA utiliza `HashRouter` (`#/`, `#/control`, `#/sensores`, `#/wifi`, `#/oled`, `#/modelo-3d`, `#/documentacion`) para que el servidor HTTP del ESP8266 solo necesite resolver `/` -> `/index.html.gz` y activos estáticos por ruta exacta sin reglas complejas de historia HTML5.

Fuente ESP AFR2-20261023-11: HTTP :80 y `/ws` en el mismo origen. Montaje LittleFS con autoformat=false; falla visible mediante `/api/health` y raíz 503, sin borrar archivos. Whitelist obtenida del manifiesto (hasta 16 archivos / manifiesto <=8192 bytes), comprobación de esquema/rutas/MIME/tamaños y archivo presente; no recalcula SHA-256 del contenido. `/` mapea a `/dist/index.html.gz`; assets exactos a `/dist` + servedPath. Exige Accept-Encoding gzip (406 si está excluido); Content-Encoding gzip, Vary Accept-Encoding, MIME explícito, index no-store y assets no-cache con ETag débil derivado del hash raw. Sin fallback SPA para rutas desconocidas. Solo GET; otros métodos 405. No hay editor, upload, formateo ni endpoint público de preferencias.

## 9. Servicios ESP implementados en fuente (AFR2-20261023-11 y 15)

Ampliación local AFR2-20261023-20: `wifi.status` añade campos opcionales `applying`, `staStatus`, `fallbackActive`, `scanRunning`, `configPersisted` y `configError`; tipos y semántica en la [ficha Wi-Fi](../changes/2026-10-23-20-wifi-esp-web.md). Web espera el evento terminal de scan en vez de finalizar al recibir accepted, timeout local 30 s; consulta status cada 3 s visible/WS listo sin depender del Mega y ofrece el persist:boolean existente. No cambia UART/ROBO2.

- `/ws`: 2 sesiones, texto de hasta 1024 bytes, cola común de 4 pedidos y ranura STOP aparte; handshake requerido, heartbeat válido <=6000 ms, identificadores acotados y detección de repetición reciente/en vuelo, límite 40 pedidos/s por sesión salvo STOP. Acepta segmentación TCP de una trama WS final; frames de continuación/binary se cierran. Un consumidor lento se desconecta en lugar de acumular salida ilimitada. Origin debe coincidir con `http://Host`; no hay bypass de desarrollo. La sesión es de transporte, no permiso de movimiento.
- `system.hello` y `system.alive` incluyen capacidades vigentes. Evento `system.status` cada 1000 ms actualiza el cliente fuente aun cuando el Mega aparezca/desaparezca después del saludo. Timestamps ESP son uptime monotónico en ms (wrap de millis); el navegador usa su reloj local para caducidad y no los resta de Date.now.
- `wifi.status`, `wifi.scan` asíncrono (hasta 16 resultados, truncated, resultado fallido explícito) y `wifi.configure` diferido 350 ms tras aceptación. Modo AP: ssid/password del AP; STA/AP_STA: ssid/password de la estación, AP_STA conserva AP de aprovisionamiento. STA sin conexión por 15 s añade AP de fallback; reintento STA cada 30 s. Una conexión STA puede seguir en progreso tras accepted: consultar wifi.status. No se informa completed por el mero envío de credenciales.
- SSID: 1..32 bytes; password vacío para red abierta o 8..63 bytes de frase WPA. Campo opcional persist:boolean, default true, guarda `/config/wifi.json` con esquema afr2-wifi-1 mediante archivo temporal y rename LittleFS; persist=false solo RAM. SDK Wi-Fi con persistent(false). Sin contraseñas en respuestas, eventos, logs o fuentes. Sin preferencias válidas: AP **abierto** `AutoFinalRobo2-<chipId>` para aprovisionamiento; el estado real lo informa wifi.status.
- Una nueva imagen/uploadfs reemplaza el volumen y puede borrar preferencias: preservar configuración antes de una futura carga autorizada. Copiar assets a data/dist en el host y servir archivos en ejecución son operaciones distintas de subir una imagen completa.
- robot.state admite mode=unknown/stopped=null y ageMs=null. robot.sensors/encoders devuelven snapshots inválidos mientras no haya productores. control.acquire/release, movimiento, calibración, GOTO y UI remota devuelven CAPABILITY_UNAVAILABLE. STOP solo completed con respuesta autoritativa positiva del Mega; la composición actual carece de callback de potencia y devuelve error explícito.

## Consumidor Web flotante — AFR2-20261023-14

[Perfil OLED/modo](WEB_OLED_CONSUMER.md): consultas sin toast por tick, captura estricta, edad visible, SW único sin predicción, errores locales y sin repetición de acciones. No cambia el sobre afr2-ws-1 ni asigna wire_id a OLED/UI. El objeto state semántico completo es un perfil Propuesta del consumidor, pendiente de productores. ESP conserva rechazo CAPABILITY_UNAVAILABLE y capabilities no se elevan por la presencia de esta Web.

## Mapa Web — AFR2-20261023-18

[Perfil consumidor](WEB_CONTROL_MAP.md): modo/mapa comparten lectura robot.state; pose opcional versionada como diseño afr2-pose-1. No modifica el perfil UART vigente ni incorpora productores de odometría. Pose ausente/inválida/stale no se reemplaza por consignas o coordenadas cero. Capabilities no se elevan desde Web.


## Controles OLED, avisos cerrables y secuencias — AFR2-20261023-21

- **Sobre WebSocket (`afr2-ws-1`)**: se incorporan en el cliente Web y catálogo compartido `control.renew` (lease de `1500 ms` renovado cada `500 ms` con Control visible y supervisado) y los comandos de secuencia `robot.sequence.begin`, `robot.sequence.append`, `robot.sequence.commit`, `robot.sequence.start`, `robot.sequence.cancel` y `robot.sequence.status` (consulta cada `250 ms` sin solapamiento).
- **Documento de programa (`afr2-sequence-1`)**: unidades explícitas `mm`, `ms`, `mrad`; dos niveles (`movements` y `wheels` sincronizadas `FL/FR/RL/RR`); límites de editor de `64` bloques por pista y `600 000 ms` (`10 min`) en pasos de `10 ms`; límites físicos de `32` segmentos normalizados y `120 000 ms` (`120 s`), rechazando programas mayores sin truncarlos.
- **Perfil de aplicación ROBO2 3**: especificado en [UART_PROFILE_3.md](UART_PROFILE_3.md) (`0x07–0x0E` y `0x10–0x15`) conservando framing ROBO2 y máximo de 64 bytes de payload. Estado: Web adaptada en fuente; ESP y ATmega en estado diseñado hasta integrar y medir SRAM/pila.


## Integración posterior — AFR2-20261024-01

La ruta vigente se precisa en [EX01_WEB_INTEGRATION.md](EX01_WEB_INTEGRATION.md).
Se conserva perfil2; el borrador3 se sustituye por EX01v2 sin colisión de IDs.
OLED usa framebuffer del catálogo ampliado y revision por acción; estado/pose
provienen del producto19. Fuente adaptada sin nuevo build/ensayo. No se aplican
los estados históricos de productor pendiente a la fuente actual ni se los
confunde con funcionamiento físico comprobado.

