# AFR2-20261116-02 - Corrección de sincronización ESP en puente ATmega y re-booteo manual previo a escritura

Autoría: Koba Chajchir / AFR2; análisis y corrección de fuente en rama `fix/loader-esp-bridge-boot`.
Versión de producto: sin cambio (`1.32.0` en `afr2_build_info.h`, servicio temporal ATmega y Loader según [VERSIONADO.md](../docs/VERSIONADO.md)). Loader: `2.4.1` (su propia línea, parche).

## Problema investigado

Al detectar la flash ESP y luego solicitar escritura de firmware y LittleFS (y también en detecciones `Detectar ESP` aisladas), las sesiones con `mega_bridge` + `EspKeyRst=auto` capturaban `boot mode:(1,6)` o `(1,7)` (`capture_status=0`), imprimían `WARNING: Pre-connection option "no_reset" was selected` y fallaban los 150 reintentos de `Connecting...` (`No serial data received.` o `Invalid (unsupported) command 0x8`). Además, el cierre `CMD_AUTO_OFF` fallaba tras `CMD_RUN`, y en modo manual/`usb-ttl` `FlashEsp` lanzaba `write_flash` después de `Probe` sin volver a pedir la secuencia `KEY/RST`.

### Causas identificadas en código y logs (`2026-11-10` y `2026-11-16`)

1. **`TX1/PD3` (D18→E_RX) en alta impedancia al salir de `capture_boot` (`AutoFinalRobo2-Atmega/service/esp_uart_bridge.c`):** `quiesce()` no fijaba `PD3` como salida en alto (`DDRD3=0, PORTD3=0`). Al poner `UCSR1B = 0` tras capturar los 51 bytes a 74880 con el ESP8266 ya escuchando en bootloader ROM (y con interrupciones globales habilitadas), `D18` caía momentáneamente de alto a baja impedancia/masa, desincronizando al receptor/autobaud del ESP8266 para todo ese arranque (los 150 reintentos posteriores contra el mismo boot nunca se recuperaban).
2. **Liberación prematura de `KEY`:** `capture_boot()` soltaba `KEY` (`release_all()`) antes de empezar `esptool`, a diferencia de `CMD_ENTER_BOOT` y de lo observado en la unidad física (`FLASHING.md`).
3. **Colisión `CMD_RUN` ↔ `CMD_AUTO_OFF` y respuesta `RSP` no atómica:** `close_bridge()` enviaba `CMD_RUN` (reiniciando al ESP a firmware normal, que emitía 74880/`MegaLink` sobre `RX1 -> to_pc`) antes de `CMD_AUTO_OFF`, y `reply()` encolaba los 11 bytes de respuesta en llamadas separadas con interrupciones habilitadas entre byte y byte.
4. **Apertura sin reset DTR del Mega:** `port.dtr = False` al abrir `COM` dejaba el flasher en estado residual (`auto=sí`) o bloqueado si una sesión previa disparó `overflow_stop()`.
5. **Flujo manual / `usb-ttl` en `Operations.FlashEsp` y `FlashAtPackage`:** no llamaba a re-entrar/re-confirmar `KEY/RST` después de `"Continuar para flashear ESP"` antes de lanzar `write_flash` con `--before no_reset`.

## Correcciones en fuente

- **Servicio ATmega (`AutoFinalRobo2-Atmega/service/esp_uart_bridge.c`):**
  - `quiesce()` deja `PE1` (`TX0`) y `PD3` (`TX1/D18`) configurados como salida en alto para mantener el nivel reposo UART aun cuando `TXEN0`/`TXEN1` estén apagados o ante `overflow_stop()`.
  - `capture_boot(cmd, hold_key)` realiza los cambios de `UBRR1`/`UCSR1B` bajo `cli/sei` manteniendo `PD3` en alto y estabilizando `2 ms` antes de responder; acorta la espera ociosa cuando ya recibió la trama completa (`>= 200 ms` totales y `>= 50 ms` tras el último byte, manteniendo techo `1,2 s` si no hay datos).
  - Nuevos comandos compatibles hacia atrás anunciados por `bit5` (`FLAG_SESSION_SAFE = 0x20`) de `RSP`:
    - `CMD_BOOT_HOLD` (`0x15`): captura arranque a 74880 y **retiene `KEY`** bajo el watchdog `Timer1` (`~600 s`) hasta `CMD_RUN`, `CMD_END_AUTO`, `CMD_RELEASE` o reset del Mega; `CMD_BOOT_TEST` (`0x14`) conserva liberación de pines para la ventana `--test-keyrst-boot`.
    - `CMD_END_AUTO` (`0x22`): suelta pines, pasa `auto=0`, envía `RSP` con `RXCIE1` inhibido y espera a vaciar `TX0` **antes** de pulsar `RST` del ESP.
  - `reply()` encola los 11 bytes de trama en una única sección crítica; `USART0_RX_vect` reinicia `ctl_stage`/`ctl_pos` ante error de trama UART0.
- **Wrapper Python (`AutoFinalRobo2-Loader/tools/esp_bridge_esptool.py`):**
  - Usa `CMD_BOOT_HOLD` y `CMD_END_AUTO` cuando el flasher anuncia `bit5`, manteniendo respaldo automático para flashers anteriores (`CMD_BOOT_TEST`/`CMD_ENTER_BOOT` y `CMD_RUN`+`CMD_AUTO_OFF`).
  - Recuperación automática por pulso `DTR` si `CMD_QUERY` no responde tras apertura silenciosa en `--auto-key-rst`, y normalización de `auto=sí` residual al iniciar.
  - Gancho sobre `ESPLoader.connect` en `--auto-key-rst` cuando `attempts > 3`: intenta hasta 3 ciclos `SYNC` por arranque y repite limpiamente la secuencia `KEY/RST` hasta 2 veces ante fallo en lugar de insistir 150 veces contra el mismo booteo sordo.
  - Nuevo modo `--flasher-keyrst probe` (exit `0` tanto si responde como si no) que informa `AFR2-FLASHER: present=yes/no` y `session_safe`.
- **Loader C# (`AutoFinalRobo2-Loader/src/`):**
  - Nuevo método `Operations.PrepareEspBootForWrite` (`Operations.MegaBridge.cs`), invocado en `Operations.FlashEsp` (`Operations.cs`) y `FlashAtPackage` (`Operations.EspModes.cs`) tras la confirmación de escritura en modos manuales y `usb-ttl`.
  - **`Detectar ATmega` (`Operations.ProbeMega` + `esp_bridge_esptool.py --flasher-keyrst probe`):** tras validar firma por `avrdude`, consulta sin escribir ni accionar pines si el ATmega responde como flasher AFR2 al `BridgeBaud` del perfil, renderiza condicionalmente `"Flasher encontrado (...)"` / `"Flasher no encontrado (...)"` junto a *Variante ATmega* (`MainForm.EspModes.cs` / `MainForm.Design.cs` / `MainForm.Integration.cs`), actualiza *Conexión y evidencias* y marca la etapa en *Etapas de la operación* (`MainForm.Workflow.cs`). Se oculta al cambiar puerto, destino o baud, o tras sobrescribir el Mega con firmware/backup.
  - **Autodetección opcional de puerto COM al iniciar (`AutoDetectComOnStartup`, por defecto inactiva):** nueva casilla persistida en `LoaderPreferences` y visible en *Configuración → General* (`MainForm.Configuration.cs`). Cuando está desactivada (predeterminado), no abre puertos al arrancar y muestra junto a *Puerto*/*Actualizar puertos* la leyenda clicable `"Autodetectar COM inactivo · Configuración -> General"`. Cuando está activada, rige desde el próximo inicio: al mostrarse la ventana (`Shown`), `StartupComDetector.ProbeAllAsync` (`PortAvailability.cs`) envía en paralelo por cada `COM` válido consultas no destructivas (`DTR/RTS=false`: `CMD_QUERY` binario del flasher a `BridgeBaud`/115200 y `#AFR2 TAP ?` ASCII a 115200 buscando `TAP mode=... baud=...`), selecciona automáticamente el `COM` que responda (salvo cambio manual previo del usuario), muestra la leyenda en amarillo (`#facc15` en oscuro / `#9a6700` en claro en `MainForm.Appearance.cs`) con el puerto hallado o estado activo, y se cancela liberando el puerto antes de abrir operaciones, el Monitor serie o cerrar la ventana (`MainForm.cs` / `MainForm.Serial.cs`).
  - Versión del cargador actualizada a `2.4.1` en `JobState.cs` y documentación en `AutoFinalRobo2-Loader/README.md` y `AutoFinalRobo2-Atmega/service/README.md`.

## Estado de validación

**Fuente en rama `fix/loader-esp-bridge-boot`**: la adición de detección/indicador de flasher en *Detectar ATmega* y la autodetección opcional de COM al iniciar quedaron **en fuente sin recompilar esta adición, sin probar ni abrir puertos** (se había compilado previamente `AFR2Loader.exe` / `AFR2Loader.Mcp.exe` 2.4.1 antes de estas dos últimas adiciones de UI). Tampoco se compiló/cargó el servicio ATmega ni se ejecutaron pruebas físicas (`AGENTS.md`). Próximo paso bajo pedido del usuario: cerrar si está abierto y recompilar el Loader (`Compilar.ps1`), y recompilar/cargar el flasher temporal en ATmega (`Loader → Flasher ATmega → Cargar flasher en ATmega`) antes de probar `Detectar ATmega`, autodetección COM al iniciar, `Detectar ESP` y `Flashear`.
