# AFR2-20261117-09 - El auto informa sus versiones en la autodetección del Loader

Autoría: Koba Chajchir / AFR2.
Versiones: producto **1.36.0** (subversión, función nueva compatible); Loader **2.5.0** (menor).

Estados:
- atmega = compilado sin cargar;
- esp = solo versión, compilado con LittleFS;
- web = solo versión (más la versión del Loader en Ayuda), compilada para el ESP;
- loader = compilado.

Sin comando UART nuevo: la línea `VER` viaja por el log de depuración USB (TAP), no por el enlace ESP↔ATmega.

## Pedido

- Con la autodetección del Loader, que el ESP y el ATmega informen sus versiones.
- Que el Loader marque que las detectó.
- Dejar documentados los valores actuales del Loader y de los firmwares, para saber cómo empezó la compatibilidad.
- Subir el firmware y el Loader en una versión menor (`.1`).

## Cambios

### ATmega

- `src/services/afr2_debug.c`:
  - Ante `#AFR2 TAP ?`, después de la confirmación `TAP mode=…`, emite `VER mega=<AFR2_FIRMWARE_VERSION> ext=<AFR2_EXT_VERSION> esp=<…> web=<…>`.
  - `esp` y `web` salen de lo que el ESP informa (`Afr2Debug_PeerVersion`). Si no llegó, `esp` sale del último `BT01` del ESP (mayor.menor.parche, ahora guardado en `d.esp_ver`). Si no hay ninguno, `?`.
  - Ocupa 34 B de RAM estática más (dos textos de 16 B y la versión numérica del `BT01`).
- `include/afr2_debug.h`: `Afr2Debug_PeerVersion(web, text)`, con stub vacío cuando el log está desactivado.
- `src/services/afr2_app_wire.c`: al recibir las versiones del ESP (`WIFI_UPDATE` subtipo 3) también las pasa al log.

### Loader 2.5.0

- `src/PortAvailability.cs`:
  - `ComProbeResult` suma `MegaVersion`, `EspVersion`, `WebVersion`, `ExtVersion` y `VersionsReported`.
  - Al detectar el firmware de producto espera hasta 700 ms la línea `VER` y arma el detalle «Firmware AFR2 · ATmega … · ESP … · Web …».
  - Sin `VER` informa «sin versiones: firmware anterior a 1.36.0».
- `src/MainForm.cs`:
  - La leyenda de autodetección suma «igual al repo» o «distinta del repo X», comparando con `afr2_build_info.h` del perfil (`{Repo}`).
  - El registro escribe «Versiones detectadas: …» y «Compatibilidad de versiones: …» con las diferencias.
- `src/JobState.cs`: `LoaderVersion.Value = "2.5.0"`.

### Web

- `src/features/loader/loaderFlow.ts`: `LOADER_VERSION` pasa de 2.0.0 (desactualizada) a 2.5.0.

### Documentación

- `docs/COMPATIBILIDAD_VERSIONES.md` (nuevo): línea base y formato de `VER`.
  - Línea base: Loader 2.5.0, producto 1.36.0, EX01 v13, `afr2-wire-catalog-1` y `afr2-ws-1`.
  - Incluye cómo lo muestra el Loader y una tabla de historial para las próximas versiones.

## Estado de validación

Compilado el 17/11/2026, a pedido del usuario:

| Módulo | Resultado |
|---|---|
| ATmega | `make -B firmware communication-firmware library`: exit 0. `.data` 12 B + `.bss` 6 886 B = 6 898 B de RAM estática (+40 B); flash 170 116 B. Una advertencia previa ajena: `afr2_product.c:262` |
| ESP | `platformio run`: SUCCESS. RAM 58,6 %; flash 479 239 B. LittleFS (`-t buildfs`): SUCCESS |
| Web ESP | `npm run build:esp` (con `tsc`): exit 0. 1.36.0 en `ESP/data/dist`; estimación FS 2 859 008 de 2 867 200 B |
| Loader | `Compilar.ps1` (csc .NET 4): exit 0. `AFR2Loader.exe` 2.5.0.0 en `%LOCALAPPDATA%\AutoFinalRobo2Loader\bin` |

- No probado con el auto: falta cargar ATmega, ESP y LittleFS y abrir el Loader con la autodetección activa.
- Hosting no se republicó: sigue en 1.35.1.

## Pendiente

- Cargar ATmega, ESP y LittleFS (1.36.0).
- Con «Autodetectar puerto COM al iniciar» activo, comprobar que la leyenda y el registro muestren «ATmega 1.36.0 · ESP 1.36.0 · Web 1.36.0 · igual al repo».
