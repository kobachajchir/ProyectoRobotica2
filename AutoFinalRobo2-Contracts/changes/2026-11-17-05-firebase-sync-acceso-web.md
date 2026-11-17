# AFR2-20261117-05 - Copia de la sincronización en Firebase, vinculación por código en el OLED y vista remota

Autoría: Koba Chajchir / AFR2.
Versión: 1.35.0 (subversión).

Estados:
- atmega = compilado sin cargar: CMD `0x57 SYNC_PAIR` y aviso en el OLED.
- esp = compilado sin cargar (firmware y LittleFS): `sync.key.get` / `sync.key.set` / `sync.pair.show` y envío de `SYNC_PAIR`.
- web = compilada; publicada en Firebase Hosting y promovida a `ESP/data/dist`.

Compilado y publicado el 17/11/2026, a pedido del usuario; sin cargar. El sobre `afr2-cloud-sync-1` no cambia.

## Pedido

1. Crear el backend en Firebase para el acceso web y conectarlo para que reciba la sincronización. Decisiones del usuario:
   - proyecto nuevo;
   - publicar en Hosting con una vista remota de solo lectura;
   - acceso por clave.
2. Ampliación del mismo cambio: como hay un solo auto, escribir en un solo lugar y sumar un «login» por código. El código aparece en el auto y se escribe en la compu para vincularla. Decisiones del usuario:
   - diseño completo: OLED, clave guardada en el ESP y Web;
   - la compu vinculada solo ve el estado.

## Recursos en Firebase (creados y publicados el 17/11/2026 con Firebase CLI)

- Proyecto `autofinalrobo2` («AutoFinalRobo2»), en la cuenta que tenía la sesión de Firebase CLI iniciada en el equipo. Consola: https://console.firebase.google.com/project/autofinalrobo2/overview
- Cloud Firestore `(default)` en `southamerica-east1`, plan Spark.
- Authentication: proveedor **anónimo** activado (`firebase.json` → `auth.providers.anonymous`).
- App web `AutoFinalRobo2 Web` (`1:470235617274:web:747343f836591ef63ff643`). Su configuración pública está en `Web/src/features/sync/firebaseConfig.ts`. La `apiKey` de Firebase identifica el proyecto, no es una credencial: la seguridad está en las reglas.
- Reglas publicadas (`firebase deploy --only firestore:rules,auth`).
- Hosting: configurado en `firebase.json`, **todavía no publicado** (requiere compilar).

## Modelo de seguridad (un solo auto)

Todos los navegadores inician una sesión anónima de Firebase Auth. El token se guarda en `afr2.firebase.session.v1` y se renueva solo.

| Ruta | Quién | Regla |
|---|---|---|
| `car/state` | escritor escribe; escritor o lector lee | Campos validados (`schema`, `rev`, `ts`, `layers`, `hashes`, `simulators`…) |
| `car/secret` | nadie lo lee | Lo crea el primer escritor. Lo compara la regla de `writers` y un escritor puede rotarlo |
| `writers/{uid}` | el propio navegador | Se crea solo si `w` coincide con `car/secret.w` (la clave del ESP) |
| `pairings/{código}` | lo crea un escritor | Vale 5 min (`expiresAt` ≤ ahora + 5,5 min). Se borra al canjearlo. Nadie puede listarlos |
| `viewers/{uid}` | el propio navegador o un escritor | Se crea en el mismo commit que borra un código vigente. Los escritores los listan y los quitan |

- **Clave de escritura:**
  - 32 caracteres, unos 190 bits.
  - La guarda el ESP en LittleFS (`/config/sync.key`, que no se sirve por HTTP). Así, cualquier navegador conectado al auto escribe en el mismo lugar.
  - El navegador no la guarda.
- **Código de vinculación:**
  - 6 caracteres de un alfabeto de 30 (sin 0/O, 1/I/L ni U), con el formato `K7M-4QX`: unos 7,3·10^8 combinaciones.
  - Dura 5 minutos y sirve una sola vez.
  - La compu acepta minúsculas, espacios y O→0, I/L→1.
- **Riesgo conocido:**
  - El primer navegador que active la copia reclama `car/secret`.
  - Si otro lo reclamara antes, el auto queda en «claimed». Se resuelve borrando `car/secret` desde la consola.
- **Wi-Fi:** la capa se publica sin SSID. Las contraseñas no forman parte del esquema.

## Contrato UART (skill afr2-wire-commands)

- `0x57 SYNC_PAIR` (familia NETWORK, ESP → ATmega), **sensible**:
  - Pedido de 12 bytes: `esp_boot_id` u32le, `ttl_s` u8 (1..60) y `code` ascii:7.
  - Respuesta sin cuerpo.
- Archivos:
  - `contracts/afr2_network_wire.h`: `AFR2_EXT_SYNC_PAIR`, `AFR2_SYNC_PAIR_CODE_LEN`, `AFR2_SYNC_PAIR_BYTES`.
  - `contracts/wire_catalog.json`: entrada nueva; `updated_by` pasa a AFR2-20261117-05.
  - `contracts/commands.json`: `sync.key.get` y `sync.key.set` (locales del ESP, sensibles) y `sync.pair.show` (`wire_id` 0x57).
  - `Web/src/features/protocolStudio/catalog.ts`.
  - `Web/src/protocol/wsEnvelope.ts`.

## ATmega

- `src/services/afr2_network.c`: valida el largo, el `ttl_s` y el formato del código.
  - Si el boot del ESP no coincide, o hay teclado o cola llena, devuelve `UNAVAILABLE` y el ESP reintenta.
  - Si todo es válido, muestra el aviso informativo genérico «Codigo web» / `K7M-4QX` durante `ttl_s` (máximo 60 s). El límite es el `uint16_t` de la duración de los avisos.
  - Se cierra con corto o largo, como cualquier aviso.
- `src/services/afr2_app_wire.c`: deriva `0x57` al manejador de red.
- `src/services/afr2_debug.c`: `0x57` se suma a `sensitive_command()`, así el TAP lo redacta.
- Pantallas (skill afr2-screen-authoring):
  - No se crea pantalla ni ScreenCode nuevo: se usa el aviso de título y texto existente (`Afr2Ui_NotifyFlashTitle`).
  - Su renderer y su copia Web no cambian. Por eso no se tocan `screens.json` ni los hashes del renderer.

## ESP

- `src/storage.h/.cpp`: `loadSyncKey` y `saveSyncKey`, con escritura temporal y rename atómico. Una clave vacía borra el archivo.
- `src/broker.h`: caché de la clave, código pendiente, TTL y `validSyncKey` (32..48 `[A-Za-z0-9_-]`, porque el cuerpo de la petición tiene 56 bytes).
- `src/broker.cpp`:
  - valida `sync.key.set` y `sync.pair.show` (código `XXX-XXX`, `ttlS` 1..60);
  - los resuelve en local;
  - `sync.pair.show` exige el ATmega listo con `AFR2_EXT_F_NETWORK_UI`.
- `src/broker_network.cpp`:
  - envía `SYNC_PAIR` antes del estado periódico, sin cortar una transferencia de páginas;
  - reintenta ante timeout o `UNAVAILABLE` hasta que vence el TTL;
  - descarta el código ante OK o rechazo (por ejemplo, un ATmega anterior).

## Web

- `firebase.json`, `.firebaserc`, `firebase/firestore.rules`, `firebase/firestore.indexes.json`. `.gitignore` excluye `.firebase/` y los logs del CLI.
- `src/features/sync/firebaseConfig.ts` (nuevo).
- `src/features/sync/firebaseSync.ts` (nuevo): cliente REST mínimo, sin SDK, para no sumar peso al paquete del ESP. Incluye:
  - sesión anónima (`accounts:signUp` y renovación con `securetoken`);
  - registro de escritor (`ensureWriter`, reclama `car/secret` la primera vez);
  - `pushEnvelopeToFirebase` (`PATCH car/state` con `updateMask`: el snapshot reemplaza `layers` y el delta solo las capas sucias; simuladores como JSON aparte);
  - códigos (`createPairing`, `pairingPending`, `cancelPairing`);
  - lectores (`listViewers`, `revokeViewer`, `redeemPairCode` con commit atómico, `unlinkThisDevice`);
  - `readRemoteState`.
- `src/features/sync/CloudSyncController.tsx`:
  - Con el auto conectado: `sync.key.get` y registro como escritor.
  - «Activar» genera la clave, la guarda en el ESP (`sync.key.set`) y registra el escritor. «Apagar» borra la clave del ESP.
  - Solo escribe con el auto conectado, el escritor registrado y fuera de Hosting.
  - Escribe con capas sucias, un snapshot o simuladores; sin cambios, como mucho un latido por minuto.
  - Panel «Vincular una compu»:
    - crea el código y lo muestra en el OLED (`sync.pair.show`, 60 s) y en el modal, con cuenta de 5 min;
    - tiene «Mostrar de nuevo en el auto» y «Cancelar»;
    - detecta el canje y avisa «Compu vinculada».
  - Lista de compus vinculadas, con «Quitar».
  - En Hosting, la pill dice «Solo lectura».
- `src/features/sync/RemoteSyncPage.tsx`, ruta `#/remoto`:
  - Sin vincular, pide el código.
  - Vinculada, lee `car/state` cada 5 s con la pestaña visible y muestra «En vivo» si hubo escritura en los últimos 90 s. Tarjetas de Robot, Sistema, Wi-Fi, configuración y simuladores.
  - Tiene «Desvincular».
- `src/app/App.tsx`, `src/app/navigation.ts`: ruta y título «Vista remota».
- `src/styles/line-follower.css`: estilos de la sección, del código y de la vista remota.
- Ilustraciones de los accesos de Inicio: pasan de `public/home/` a `src/features/home/shots/` y se incrustan en el bundle como data URI.
  - Como archivos sueltos superaban el límite de 24 activos del Storage del ESP (`kMaxAssets`) y sumaban 3 bloques LittleFS.
  - Antes faltaban en la compilación porque `copyPublicDir` está en `false`.
  - El fondo con `url("…")` va entre comillas porque el data URI de la SVG lleva espacios.
- `public/brand/apple-touch-icon-robot.png` (ícono de iOS, 180×180): pasa a paleta de 256 colores, de 29 533 a 8 164 B, sin cambio visible. Libera 2 bloques LittleFS: con el crecimiento del bundle (Firebase y cambios 1.33.2/1.34.0), el paquete ESP superaba el presupuesto por un bloque.

## Estado de validación

### Compilación y publicación (17/11/2026, pedido expreso del usuario)

Se usaron las mismas herramientas y argumentos que el perfil del Loader.

| Módulo | Comando | Resultado |
|---|---|---|
| ATmega | `make -B firmware communication-firmware library` (avr-gcc 7.3.0) | exit 0. Flash 169 064 B (.text 168 710 + .data 354); SRAM estática 7 202 B de 8 192. Una advertencia previa ajena: `-Wmisleading-indentation` en `afr2_product.c:262` |
| ESP | `platformio run -e esp12s_candidate` | SUCCESS. RAM 48 024 B (58,6 %); flash 479 143 B (45,9 %); `firmware.bin` de 483 296 B. Una advertencia previa ajena: `draftToken` sin uso en `broker_wifi.cpp:11` |
| Web ESP | `npm run build:esp` (con `tsc -b`) | exit 0. 23 activos, 2 678 526 B gzip, estimación FS 2 859 008 de 2 867 200 B. Promovida a `ESP/data/dist` |
| LittleFS | `platformio run -e esp12s_candidate -t buildfs` | SUCCESS: `littlefs.bin` de 3 121 152 B |
| Web completa | `npm run build` (con `tsc -b`) | exit 0. 23 activos |
| Hosting | `firebase deploy --only hosting` | Publicado en https://autofinalrobo2.web.app |

Dos intentos previos de `build:esp` fallaron por el presupuesto y por el límite de 24 activos. Se corrigieron con los dos cambios de imágenes descritos arriba.

### Comprobaciones

- Firebase:
  - proyecto, Firestore, Auth anónimo, app web, reglas y Hosting creados y publicados;
  - el CLI compiló las reglas sin errores.
- Navegador, en el servidor de desarrollo:
  - sin auto, el modal muestra «Conectá el auto para publicar su estado en Internet»;
  - un código inventado (`abc 2x9`, normalizado a `ABC-2X9`) inició una sesión anónima real y Firestore rechazó el canje;
  - la sesión de prueba se borró del navegador.
- Navegador, en Hosting:
  - `#/remoto` pide el código y la pill muestra «Solo lectura»;
  - las tres ilustraciones de Inicio cargan como data URI;
  - en la consola solo aparecen los fallos esperables del WebSocket, porque Hosting no tiene auto.
- No verificado:
  - la carga de ATmega y ESP;
  - la escritura de un escritor y el canje válido;
  - el aviso en el OLED;
  - la prueba con el auto.

## Pendiente

1. Cargar ATmega (`build/afr2.hex`), firmware ESP y LittleFS desde el Loader. Hacen falta los tres, porque la Web del auto necesita `sync.key.*` del ESP 1.35.0.
2. Primera prueba con el auto:
   - En la Web del auto con Internet (STA, o datos móviles en el dispositivo): Sync web → Copia en Firebase → Activar.
   - Después, «Vincular una compu»: el código aparece en el OLED.
   - En la compu, abrir https://autofinalrobo2.web.app/#/remoto, escribir el código y comprobar «En vivo».
3. El paquete ESP queda a un bloque (8 KB) del presupuesto. Cualquier función Web nueva probablemente necesite liberar espacio otra vez.
