# AFR2-20261118-04 - Refresco de puertos relanza autodetección COM y pedido de versión en el Loader

Autoría: Koba Chajchir / AFR2.
Versión de producto: sin cambio (`Loader` lleva su propia línea; `2.5.1`, parche).

Estados:
- atmega = sin cambios;
- esp = sin cambios;
- web = sin cambios;
- loader = fuente editada (`2.5.1`), sin compilar ni probar.

## Pedido

Cuando la autodetección COM está activa en el Loader, al pulsar **Actualizar puertos** (en *Operación*) o **Actualizar** (en *Monitor serie*), relanzar el escaneo de puertos COM y el pedido de versiones (`#AFR2 TAP ?` / `VER` y consulta de flasher), re-seleccionar el puerto detectado y actualizar la leyenda junto al selector de puerto.

## Cambios en `AutoFinalRobo2-Loader`

- `src/MainForm.cs`:
  - `RefreshPortsAndAutoDetect()` / `RefreshPortsAndAutoDetectAsync()`: refresca las listas de puertos (`RefreshPorts()`, y `RefreshSerialPorts()` si el Monitor serie no está abierto) y, si `preferences.AutoDetectComOnStartup` está activo, `!busy` y el Monitor serie no está activo, reinicia `manualPortSelected = false` y lanza `StartComDetectionAsync(true)`.
  - `StartComDetectionAsync(bool fromRefresh)`: unifica el arranque (`StartStartupComDetectionAsync()`) y el refresco manual; si ya había un sondeo previo en curso al pulsar actualizar, lo detiene con `StopStartupComProbeAsync()` antes de enumerar los COM nuevamente y usa `comScanGeneration` para descartar resultados obsoletos. Sincroniza tanto `ports` como `serialPorts` con el puerto detectado.
  - `UpdateComAutoDetectHint()`: diferencia los mensajes según el origen (`"Autodetectando COM al actualizar..."` / `"sin respuesta al actualizar"` frente a `"al iniciar"`) y actualiza la leyenda amarilla `comAutoDetectHint` con el puerto, detalle y comparación de versiones contra el repo cuando concluye el escaneo.
- `src/MainForm.EspModes.cs`: el botón **Actualizar puertos** invoca `RefreshPortsAndAutoDetect` y el tooltip de `comAutoDetectHint` refleja que aplica al iniciar y al actualizar puertos.
- `src/MainForm.Serial.cs`: el botón **Actualizar** de *Monitor serie* invoca `RefreshPortsAndAutoDetect()` cuando el monitor no está capturando.
- `src/MainForm.Configuration.cs`: nota y log de *Autodetectar puerto COM al iniciar* actualizados para indicar que también aplica al actualizar puertos; al desactivar la casilla cancela cualquier sondeo en curso y limpia el estado de autodetección.
- `src/JobState.cs`: `LoaderVersion.Value = "2.5.1"`.
- `README.md`: entrada de **Loader 2.5.1 / AFR2-20261118-04**.

## Estado de validación

- Solo fuente editada; **sin compilar ni probar** (`AGENTS.md`).
- Pendiente bajo pedido del usuario: compilar el Loader (`Compilar.ps1`) y verificar en vivo con el auto que al pulsar **Actualizar puertos** con autodetección activa se re-enumera el COM, se reenvía `#AFR2 TAP ?` / `VER` y se actualiza la leyenda amarilla.
