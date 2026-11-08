# AFR2-20261107-53 — El ESP se reiniciaba al descargar el modelo 3D

07/11/2026.

Estado real: esp = fuente corregida, sin compilar ni cargar; atmega = sin cambios; web = sin cambios.

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

## Pendiente

- Compilar y cargar el ESP con el Loader, y probar la descarga del modelo 3D.
- Si sigue reiniciando, leer `resetReason` en `/diagnostico` después del reinicio.
