# AFR2-20261027-27 — Loader0.7.1: comando ESP y KEY sostenido

27/10/2026. El usuario pidió corregir el Loader y revisar que su escritura
use el mismo comando PowerShell que completó una carga por USB–TTL en COM5
a 115200. Informó mantener KEY apretado desde el reset hasta terminar la carga,
soltando solo RST. Adaptación asistida por Codex; autoría de Koba Chajchir y
cambios anteriores conservados.

## Evidencia aportada por el usuario

Comando y salida ejecutados por el usuario, no repetidos por el agente para
registrarlos. Las variables apuntan al Python/esptool de PlatformIO y al BIN
del proyecto descritos en la conversación. Hora y código de salida numérico
no aportados. La siguiente salida se transcribe de su mensaje:

```powershell
& $afr2Python -u $afr2Esptool `
  --chip esp8266 --port COM5 --baud 115200 `
  --before no_reset --after no_reset `
  write_flash --flash_mode keep --flash_freq keep --flash_size 4MB `
  0x00000 $afr2Firmware
```

```text
esptool.py v4.5.1
Serial port COM5
WARNING: Pre-connection option "no_reset" was selected. Connection may fail if the chip is not in bootloader or flasher stub mode.
Connecting................
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: ec:64:c9:c2:21:8e
Uploading stub...
Running stub...
Stub running...
Configuring flash size...
Flash will be erased from 0x00000000 to 0x0006afff...
Compressed 437936 bytes to 311758...
Wrote 437936 bytes (311758 compressed) at 0x00000000 in 27.8 seconds (effective 126.2 kbit/s)...
Hash of data verified.

Leaving...
Staying in bootloader.
```

Esto acredita una escritura manual de firmware y su hash según la salida
aportada. No acredita arranque desde flash, Wi-Fi, LittleFS, el robot ni el
Loader corregido. El éxito con KEY sostenido se adopta como procedimiento
para esta unidad; no se afirma que soltarlo sea la causa aislada del corte
anterior ni se convierte en requisito eléctrico universal del ESP8266.

## Comparación del comando

| Elemento | Antes y adaptación |
| --- | --- |
| Python | Faltaba `-u`. Se agrega antes de la ruta de esptool para salida de Python sin buffer. |
| Chip, baudios y puerto | Ya usa `--chip esp8266`, baudios del perfil (115200 por defecto) y COM seleccionado. COM5 sigue siendo elección de sesión. |
| Reset | Ya usa `--before no_reset --after no_reset` en esptool4; conserva la traducción con guiones de esptool5. |
| Escritura | Ya usa `write_flash` y el stub normal, igual que el comando aportado; `--no-stub` solo en la consulta ROM previa. |
| SPI y capacidad Web | Ya usa `keep / keep / 4MB` por defecto, configurables. `keep` conserva la cabecera del BIN; no identifica modo/frecuencia de la flash instalada. |
| Archivo y offset | BIN del perfil en offset0; `0x0` y `0x00000` son equivalentes. No se cargan imágenes extra al elegir solo firmware. |
| Flujo adicional del Loader | Conserva consulta ROM/flash previa, comprobación de layout y `verify_flash` posterior. No equivale a una sola invocación ni autoriza reintentos automáticos de escritura. |

En AT los parámetros y regiones siguen viniendo del manifiesto del paquete;
el caso manual aportado corresponde al firmware Web/Arduino del producto.

## Secuencia y conexión

El asistente anterior pedía soltar KEY antes de la consulta/carga. Ahora
instrucciones, aviso PUERTO LISTO, consulta, confirmación final, escritura y
verificación piden sostenerlo. Cerrar la pantalla al ver modo1 no indica soltar
KEY. Al finalizar la operación, incluso ante error o cancelación, resultado y
registro avisan que ya puede soltarse. No se afirma medir la posición de KEY
ni se usa ese recordatorio para declarar éxito.

`EspTransport` distingue `mega-usb` y `usb-ttl` en el perfil y cambia las
confirmaciones de conexión. En USB–TTL se indican lógica3,3V, TX/RX cruzados,
GND común, UART del Mega aislada y alimentación del shield sin VCC del
adaptador. No se fija COM5 ni se deduce el conector físico usado de un número
de puerto. Los perfiles previos reciben `mega-usb`; no se reescriben perfiles
guardados del usuario. GUI y MCP llaman al mismo flujo y generador de órdenes.

## Estado real por módulo

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.7.1 adaptada y leída; sin compilar ni ejecutar esta revisión. |
| esp | Firmware manual escrito y hash verificado según salida del usuario. C++/BIN sin cambios por esta ficha. Sin nueva escritura del agente, prueba de arranque, Wi-Fi ni FS. |
| atmega | Sin cambios, compilación ni carga por esta ficha. |
| web | Sin cambios, build, promoción ni carga FS por esta ficha. |
| CAD | Sin cambios. |
| Contracts | Evidencia, comparación, procedimiento y adaptaciones por archivo registrados. |

No se ejecutaron tests, compilaciones, validadores, GUI/MCP, COM, flasheos ni
pruebas físicas por el agente. La lectura del diff es revisión de fuente,
no evidencia de ejecución. El EXE abierto no se actualiza por editar fuentes;
el lanzador debe reconstruirlo en un inicio posterior autorizado. Tampoco se
aplicaron las preferencias STA locales de la ficha26 a la placa.

## Adaptaciones por archivo

| Archivo | Adaptación |
| --- | --- |
| `AutoFinalRobo2-Loader/src/Operations.cs` | Python `-u`; KEY sostenido durante la operación; instrucciones de conexión según perfil; marca de sesión ESP iniciada y mensajes de consulta/escritura/verificación. |
| `AutoFinalRobo2-Loader/src/Operations.EspModes.cs` | Mismo perfil de conexión y KEY sostenido para paquetes AT, sin cambiar sus regiones/SPI. |
| `AutoFinalRobo2-Loader/src/Profile.cs` | Campo `EspTransport`, valores admitidos y valor predeterminado para perfiles previos. |
| `AutoFinalRobo2-Loader/perfil-ejemplo.json` | Campo de conexión editable, sin COM fijo. |
| `AutoFinalRobo2-Loader/src/MainForm.Workflow.cs` | Elimina orden de soltar KEY al entrar en modo1; cierre del asistente no indica liberarlo; recordatorio de cableado contempla USB–TTL. |
| `AutoFinalRobo2-Loader/src/MainForm.cs` | Detección con perfil; recordatorio al finalizar sesión ESP y resultados conservados ante error/cancelación. |
| `AutoFinalRobo2-Loader/src/MainForm.Integration.cs` | Acción MCP de diagnóstico pasa el perfil al asistente compartido. |
| `AutoFinalRobo2-Loader/src/FailureDetails.cs` | Sugerencia de sincronización contempla ambas conexiones y KEY sostenido. |
| `AutoFinalRobo2-Loader/src/JobState.cs` | Versión nominal0.7.1. |
| `AutoFinalRobo2-Loader/README.md` | Comparación de comando, conexión, secuencia y límites. |
| `AutoFinalRobo2-Loader/MCP.md` | Campo de conexión y flujo compartido con la GUI. |
| `AutoFinalRobo2-Loader/MANUAL.html` | Manual0.7.1 con secuencia y comando actualizados. |
| `AutoFinalRobo2-Contracts/contracts/LOADER_FLASH_MODES.md` | Ampliación del perfil y secuencia común, sin cambios del protocolo de producto. |
| `AutoFinalRobo2-Contracts/hardware/FLASHING.md` | Escritura manual aportada y KEY sostenido en el procedimiento vigente. |
| `AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md` | Referencia a la escritura/hash aportados, con límites de ejecución. |
| `AutoFinalRobo2-Contracts/hardware/CARGA_USB_COM16.md` | Secuencia de botones alineada; mapa USB Mega separado de la evidencia COM5. |
| `AutoFinalRobo2-Contracts/hardware/ESP_DIAGNOSTICO.md` | Mantener KEY hasta finalizar la consulta. |
| `AutoFinalRobo2-Contracts/hardware/diagnostics/README.md` | Índice de la nueva evidencia sin sobrescribir las consultas de la ficha25. |
| `AutoFinalRobo2-Contracts/README.md` | Estado0.7.1 y enlace a esta ficha. |
| `AutoFinalRobo2-Contracts/INFORME_ADAPTACION.md` | Estado de la revisión y separación de evidencia manual. |
| `AutoFinalRobo2-Contracts/CHANGELOG.md` | Entrada incremental conservando la configuración local de la ficha26. |
| `AutoFinalRobo2-Contracts/changes/2026-10-27-27-loader-esp-command-key.md` | Evidencia del usuario, comparación y procedencia por archivo. |
