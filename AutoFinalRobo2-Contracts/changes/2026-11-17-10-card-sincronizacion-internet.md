# AFR2-20261117-10 - Card de sincronización activable solo con Internet (ATmega y Web)

Autoría: Koba Chajchir / AFR2.  
Versión: 1.37.0 (subversión compatibilizada con EX01 v13; integrada en rama `fix/loader-esp-bridge-boot` junto al tope compartido 1.38.0 con `AFR2-20261117-11`).

Estados:
- `atmega = fuente editada (sin compilar, probar ni flashear)`
- `esp = fuente editada (sin compilar, probar ni flashear)`
- `web = fuente editada (hashes en firmwareRenderer.sources.json actualizados; generador sync-oled-library, compilación y pruebas pendientes)`

## Pedido

Agregar una card de sincronización en **Controles rápidos** del ATmega (`010003`) y dentro del modal **Sincronización web** de la barra superior Web, activable **únicamente si hay conexión real a Internet**; en caso contrario debe mostrar **«Sin Internet»** y bloquear la activación. En Web incluye interruptor completo de sincronización (activando también Copia en Firebase cuando hay conexión al auto) y acceso a la vista remota (`#/remoto`).

## Contrato y compatibilidad (EX01 v13 + AFR2_EXT_F_SYNC_CONTROL)

Se mantiene `AFR2_EXT_VERSION == 13u` y la compatibilidad hacia atrás mediante un bit nuevo de capacidad y una operación nueva sobre `WIFI_POLL`:

- `afr2_product_wire.h`:
  - `AFR2_WIFI_OP_SYNC_ENABLE = 18u` (`AFR2_WIFI_OP_MAX = 18u`).
  - `AFR2_EXT_F_SYNC_CONTROL = 0x100000UL` (bit 20 en `AFR2_EXT_FEATURE_MASK = 0x1fffffUL`).
- `0x30 WIFI_POLL`: `op 17` desactiva sincronización; `op 18` la activa en el ESP solo si `internetOk_ == true` (devuelve falla `4` si no hay Internet).
- `0x53 NETWORK_STATE` (página 0, `p[4]`): conserva bit 0 (`web_ready`) y bit 1 (`sync_online`); cuando el ATmega anuncia `AFR2_EXT_F_SYNC_CONTROL`, el ESP agrega bit 2 (`sync_enabled`) y bit 3 (`internet_ok`). Un ATmega 1.36.0 anterior sigue recibiendo `p[4] <= 3`.
- WebSocket `sync.configure`: acepta `internetOk?: boolean` (empaquetado en bits 2..3 de `body[0]`) junto a `enabled?: boolean` (bits 0..1), rechaza `enabled: true` sin Internet con código `INTERNET_UNAVAILABLE`, y `system.status.cloudSync` expone `internetOk`, `online` y `configRev`.

## Detección real de Internet

- **ESP**: sondeo TCP asíncrono (`AsyncClient` de `ESPAsyncTCP`) a `firestore.googleapis.com:443` sin bloquear el `loop()` cuando `WiFi.status() == WL_CONNECTED` (10 s sin Internet / 20 s con Internet, timeout 5 s). Mientras haya una sesión WebSocket activa con reporte reciente (<= 25 s), `recomputeInternet` prioriza `webInternetOk_` porque la escritura a Firebase ocurre desde el navegador; al desconectarse el WebSocket vuelve automáticamente al sondeo del ESP. `syncEnabled_` arranca en `false`.
- **Web**: `checkInternet()` combina `navigator.onLine` con un `fetch('https://firestore.googleapis.com/', { mode: 'no-cors', cache: 'no-store' })` con aborto a los 3 s (evita falsos positivos de `navigator.onLine` en modo AP sin salida a Internet), disparado al montar, eventos `online`/`offline`, cambios de enlace/Wi-Fi, visibilidad, cada 15 s y antes de activar o sincronizar.

## Cambios por módulo

### ATmega (`AutoFinalRobo2-Atmega`)
- `include/afr2_menu_catalog.h`, `src/ui/afr2_menu_catalog.c`, `contracts/ATMEGA_CONFIG_ACTIONS.json`: acción `AFR2_A_SYNC_TOGGLE = 126`, ítem `extra[50]` «Sincronizacion», `AFR2_N_QUICK` ampliado de 8 a 9 tarjetas (índice 7 «Sincroniz.», `Ajustes` pasa a índice 8).
- `src/ui/afr2_icons.c`: asigna `AFR2_ICON_LINK` a `AFR2_A_SYNC_TOGGLE`.
- `include/afr2_product.h`, `include/afr2_screens.h`, `src/services/afr2_network.c`, `src/services/afr2_app_wire.c`, `src/services/afr2_app.c`: campos `sync_enabled` e `internet_ok` en `Afr2Wifi` / `Afr2UiSnapshot` (limpios en revocación y timeout de 5 s), `quick_flags` bit 7 (`0x80u`), bloqueo con motivo `Sin Internet` en `available()`/`reason()`/`invoke()`.
- `src/ui/afr2_screens.c`: en `Controles rapidos` tarjeta 8 (`selection == 7`) muestra `SIN ESTADO / Sin ESP`, `Sin Internet / No activable` (o `Desactivar` si ya venía activa), o `ONLINE`/`ACTIVA`/`APAGADA` con espera de confirmación `2000 ms`.

### ESP (`AutoFinalRobo2-ESP`)
- `src/broker.h`, `src/broker.cpp`, `src/broker_wifi.cpp`, `src/broker_network.cpp`: `syncEnabled_` inicial `false`, `syncConfigRev_` para sincronía bidireccional ATmega <-> ESP <-> Web, `tickInternetProbe`, `recomputeInternet`, manejo de `AFR2_WIFI_OP_SYNC_ENABLE` y flags en `NETWORK_STATE`.

### Web (`AutoFinalRobo2-Web`)
- `src/features/connection/ConnectionContext.tsx`: expone `espCloudSync` (`enabled, online, internetOk, configRev, failCount`) desde `system.status`.
- `src/features/sync/CloudSyncController.tsx`, `src/components/navigation/RouteNav.tsx`, `src/styles/line-follower.css`: `loadSyncSettings` por defecto `enabled: false`, `CloudSyncCard` arriba en `LocalSyncDetails` y `HostedSyncDetails` mostrando `Sin Internet` y deshabilitando activación sin red (con botón `Comprobar Internet`), activación automática de `Copia en Firebase` al activar con auto conectado (y bloqueo de su botón sin Internet), enlace `#/remoto` cerrando el modal, `CloudSyncPill` mostrando `Sin Internet` si corresponde y adopción bidireccional de `configRev`.
- `src/features/oled/firmwareRenderer.ts`, `src/features/developer/uiSimulation.ts`, `src/features/oled/UiSimulationPreview.tsx`, `src/features/oled/firmwareRenderer.sources.json`, `src/features/protocolStudio/catalog.ts`: paridad vinculada AVR/Web de la card 8/9 y hashes SHA-256 LF actualizados.

## Pendiente

- Ejecutar `node tools/sync-oled-library.mjs` antes de compilar la Web para regenerar `generated/firmwareLibrary.ts` con `extra[50]` y `AFR2_A_SYNC_TOGGLE` cuando el usuario lo solicite.
- Compilar ATmega, ESP y Web, y probar en placa (AP puro sin Internet, AP+STA offline/online, toggle bidireccional ATmega <-> Web).
