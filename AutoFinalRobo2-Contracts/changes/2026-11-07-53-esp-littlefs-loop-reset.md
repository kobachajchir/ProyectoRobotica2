# AFR2-20261107-53 — El ESP se reiniciaba al descargar el modelo 3D

07/11/2026.

Estado real: esp = firmware y LittleFS compilados fuera del Loader, sin cargar; atmega = compilado (incluye AFR2-20261107-52), sin cargar; web = paquete esp generado, sin cargar.

## Síntoma

Informado por el usuario: al bajar el modelo 3D desde la Web servida por el ESP, el ESP se reinicia. Antes de hoy no pasaba.

## Causa

Viene de AFR2-20261106-37 y AFR2-20261107-38, que se cargaron en el ESP por primera vez hoy. En `WifiService::tick`, al estar conectado, el ESP intentaba marcar la red como «buena» (`staGood`) en `/config/wifi.json`. Solo dejaba de intentarlo cuando la escritura salía bien.

Con la red por defecto en RAM no hay `wifi.json`: por ejemplo, después de recargar LittleFS. Entonces `loadWifi` fallaba siempre y el loop abría LittleFS en cada vuelta. Lo mismo pasaba si fallaba la escritura. El servidor HTTP asincrónico lee el `.glb` de LittleFS desde el contexto de red al mismo tiempo. LittleFS no admite accesos simultáneos, y con una descarga larga (unos 880 KB) el choque termina en un reinicio.

## Corrección

`src/wifi_service.cpp`:

- El marcado de `staGood` se intenta **una sola vez por arranque**, en la primera conexión.
- Solo se intenta si la configuración está guardada (`persisted_==1`).
- Con la red por defecto en RAM no se toca LittleFS.

Queda una única escritura posible por arranque, en el momento de conectar.

## Compilación (commit 7e4f34b)

El Loader abierto desde la sesión del asistente no aparecía en la pantalla del usuario y no se pudo aprobar el pedido `build_all`. Por pedido del usuario, se compiló directo con las mismas herramientas y la configuración PlatformIO que generó el Loader (`build-configs/web-6b5da2de….ini`, layout 4m3m):

- ESP: `firmware.bin` 459 392 B, RAM 62,0 %, flash 43,6 %, SHA-256 `46856acbeccc7e066479cade68b89bb6ebb106bc56a888f292ffb9258011a23e`.
- ATmega (`make -B firmware communication-firmware library`): `afr2.hex` 372 013 B, programa 132 254 B (50,5 %), datos 6 335 B (77,3 %), SHA-256 `bbea4050df5f18410be428b5c2d6fddb6229d001fd48e736866cc80f74d69fdd`.
- Web: `npm run build:esp` con 17 archivos (2 495 615 B gzip), promovidos a `ESP/data/dist`. `littlefs.bin` 3 121 152 B, SHA-256 `81ecb4b7aa26c1e129600bad898783a51a7e269bc8d65ef0fb1ccc26dad7fbab`.

El Loader exige compilar cada sección dentro de su propia sesión antes de cargar (`BuildPreparation`), así que estas imágenes no se acreditan ahí. Para cargarlas con el Loader hay que repetir «Compilar todo» en él. No se modificó esa protección.

## Pendiente

- Cargar el ESP (firmware y, si corresponde, LittleFS) y el ATmega, y probar la descarga del modelo 3D.
- Si sigue reiniciando, leer `resetReason` en `/diagnostico` después del reinicio.
