# AFR2-20261116-01 — QR en Configuración, protección `/busy` (máx. 4 estaciones Wi-Fi / 1 cliente Web activo), sincronización online por capas (deltas 5–10 s), icono online y aviso tras 5 fallos, seguidor de línea y compensación por motor más lento de fábrica

- **ID:** `AFR2-20261116-01`
- **Fecha:** 16/11/2026
- **Versión:** `1.32.0 (subversión)`
- **Estados por módulo:**
  - `atmega`: `source-only-uncompiled`
  - `esp`: `source-only-uncompiled`
  - `web`: `source-only-uncompiled`

## Resumen de cambios

1. **Botón de códigos QR en el modal de Configuración (`RouteNav.tsx` y `QrAccessModal.tsx`):**
   - Dentro del modal de Configuración (`modal === 'settings'`) se agregó la sección **Acceso rápido** con el botón **«Mostrar QR de la app y Wi-Fi»**.
   - Abre el modal `QrAccessModal` con dos tarjetas QR generadas localmente con el codificador QR propio (`encodeQrText`):
     - **QR de la aplicación:** en la variante online muestra la URL pública de la aplicación; en la variante embebida del auto muestra `http://<IP_actual_del_auto>/` (tomando `staIp` o `apIp`, y permitiendo alternar si ambas están activas).
     - **QR de la red Wi-Fi:** codifica `WIFI:T:WPA;S:<SSID>;P:<clave>;;` (o `T:nopass` si es abierta).

2. **Límite de 4 clientes Wi-Fi en SoftAP, cliente Web único y redirección `/busy` (ESP + Web):**
   - **SoftAP limitado a 4 estaciones Wi-Fi:** `WiFi.softAP(ssid, pass, 1, false, 4)` en `startFallback`, `apply` y `startAp` (`wifi_service.cpp`), e informe `apMaxClients = 4`.
   - **Un único cliente Web activo para datos y redirección a `/busy`:** en `AutoFinalRobo2-ESP/src/main.cpp` y `broker.cpp`, mientras exista una sesión WebSocket activa (`sessions_[0]`), cualquier otro cliente HTTP que solicite `/` o cualquier ruta distinta de `/busy` y `/api/busy` recibe una redirección `HTTP 302` a `/busy` hasta que el cliente activo se desconecte.
   - **Página `/busy` embebida y en la aplicación Web (`#/busy`):** el ESP sirve `busyPage` desde `PROGMEM` en `GET /busy` y el estado JSON en `GET /api/busy` (con auto-refresco cada 3 s y reconexión automática al liberarse). En la aplicación Web (`BusyPage.tsx` y `BusySessionGuard`), la ruta `#/busy` muestra el estado de ocupación actual del auto (clientes Wi-Fi `N / 4`, tiempo de sesión activa, red e IP) y redirige automáticamente cuando la sesión se libera.

3. **Icono de estado Online / Sincronizando y notificación persistente cada 5 intentos fallidos:**
   - **Icono en pantalla:**
     - **Web (`CloudSyncPill` en `RouteNav.tsx`):** indicador en la barra superior que muestra cuando el auto/app está online (Wi-Fi con conexión a Internet y sincronizando) y el estado de la sincronización.
     - **OLED (`afr2_dashboard.c` y `dashboardRenderer.ts`):** icono `AFR2_ICON_LINK` (`12×12` en `x = 93, y = 1`) en la cabecera del dashboard cuando la sincronización online está activa (`sync_online`), transmitido por el ESP en el bit 1 de `p[4]` de `AFR2_EXT_NETWORK_STATE` (página 0).
   - **Falla cada 5 intentos fallidos:**
     - Al acumular **5 intentos consecutivos sin poder conectar/sincronizar**, se emite el evento de red `AFR2_NET_SYNC_FAIL_5` (`13`) y una notificación persistente tanto en Web (`CloudSyncFaultModal`) como en OLED (`0x04000D` / selector `0x03005B` `AFR2_N_SYNC_ASK`).
     - La opción seleccionada por defecto es **«Cerrar»** (`cursor = 0`), de modo que una confirmación rápida con el encoder o clic no apague la función accidentalmente.
     - La segunda opción es **«Desactivar sincronización» / «Desactivar funcion»** (`cursor = 1`), que envía `AFR2_WIFI_OP_SYNC_DISABLE` (`17`) / `sync.configure({ enabled: false })`, apagando la función para que deje de intentar actualizar.

4. **Aplicación Online (copia fiel), modelo JSON clave-valor por capas y sincronización parcial cada 5–10 s:**
   - **Esquema JSON en capas (`cloudSyncSchema.ts`):** modela todas las configuraciones y estados del robot en capas anidadas clave-valor (`system`, `wifi`, `robot`, `config.motion`, `config.motors`, `config.odometry`, `config.line`, `config.radar`, `config.ui`, `simulators`) con huella por capa para detectar cambios.
   - **Sincronización incremental (deltas cada 5–10 s):** `CloudSyncController.tsx` y `broker.cpp` (`sync.status`, `sync.configure`, `sync.dismiss`, `sync.push`) envían únicamente el subconjunto de capas modificadas (`dirtyLayers`) cada 5 a 10 segundos (7,5 s por defecto, configurable entre 5 y 10 s).
   - **Exclusión de simuladores y botón explícito «Sincronizar a web»:** el uso de los simuladores no dispara actualizaciones periódicas; la capa `simulators` solo se incluye cuando el usuario pulsa explícitamente el botón **«Sincronizar a web»** (`SyncSimulatorButton`) incorporado en el simulador GOTO (`GotoSimulatorPanel.tsx`), el editor de secuencias (`SequenceEditorPanel.tsx`) y el simulador 3D (`SimulatorPage.tsx`).

5. **Seguidor de línea (`LineFollowerPanel.tsx` y `afr2_product.c`):**
   - **Panel Web dedicado (`LineFollowerPanel.tsx`):** nueva sección **Programas → Seguidor de línea** en `ControlPage.tsx` con vista superior 3D de los 5 sensores IR (`IR1..IR5`), diagnóstico geométrico en vivo (posición, orientación, cruce), controles de velocidad base (`10..500 mm/s`), ganancia `Kp` (`0..1000`), encendido de sensores IR, calibración rápida sobre blanco/línea e inicio/parada del modo `AFR2_LINE`.
   - **Control en ATmega (`afr2_product.c`):** en `AFR2_LINE` se agregó recuperación de pérdida breve de línea de hasta `250 ms` manteniendo el giro de búsqueda según el último signo de error (`line_last_error`), y parada limpia al detectar línea transversal completa (`mask == 0x1Fu`, 5 sensores activos durante `>= 150 ms`).

6. **Compensación por software de motor más lento de fábrica (`motor_trim[4]`):**
   - **Persistencia en EEPROM v14 (`afr2_preferences.c`):** se incorporó `uint16_t motor_trim[4]` (`FL, FR, RL, RR` en permille `500..1500`, valor por defecto `1000 = 1,000 = 100 %`), con migración transparente desde EEPROM v1..v13 inicializando `1000` en las 4 ruedas y confirmación obligatoria Sí/No antes de guardar o descartar.
   - **Compensación con normalización anti-saturación (`afr2_product.c`):**
     - En lazo abierto (`Afr2Product_DrivePwm`), cada rueda se escala por `motor_trim[i] / 1000`. Si alguna rueda supera `pwm_limit` al subir el factor de un motor lento (por ejemplo `1,150`), las 4 ruedas se reducen proporcionalmente para conservar la dirección exacta del vector mecanum sin saturar.
     - En lazo cerrado PI (`Afr2Product_Task`), la salida del controlador por rueda se multiplica por `motor_trim[i] / 1000` para compensar la menor ganancia física del motor más lento sin exigir error integral acumulado.
   - **Edición en OLED y Web:** acciones `121..124` (`AFR2_A_MOTOR_TRIM_FL..RR`), pantallas `0x030057..0x03005A` dentro de `Calibrar motores` (`0x03001C`) y controles de `Compensación por rueda` en `CalibrationPanel.tsx` (`Puesta a punto → Motores y PI`).
