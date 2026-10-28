# AFR2-20261027-29 — Loader0.7.3: comprobar cargador antes de verificar

27/10/2026. El usuario pidió detenerse antes de enviar el comando de
verificación si el ESP no sigue en el cargador, y avisar que se repitan los
botones. Adaptación propia asistida por Codex; autoría de Koba Chajchir y
cambios anteriores conservados.

## Flujo adaptado

Antes se ejecutaba `verify_flash` inmediatamente después de `write_flash`.
Ahora ambos caminos ESP, Web/Arduino/LittleFS y paquetes AT, llaman a una
comprobación compartida antes de construir/enviar la orden de verificación.

La consulta nueva usa Python `-u`, esptool con chip/COM/baudios del perfil,
`no_reset` antes/después, `--no-stub --connect-attempts 2 read_mac` en v4
(`read-mac` y opciones de reset con guiones en v5). Exige salida0 y respuesta
MAC de seis octetos. Tiene un máximo de15segundos. No escribe flash, sube
otro stub ni toma un log de modo1 anterior como evidencia de conexión actual.

Si falla el comando, su respuesta no es reconocible o vence el plazo:

1. Registra error/salida originales y `confirmed:false`; la verificación no
   se marca iniciada ni el contenido se declara verificado.
2. Detiene el flujo y muestra **Verificación detenida · repetir KEY / RST**.
3. Continuar abre el asistente en contexto de recuperación, aun si se había
   desactivado la guía inicial. Esperar PUERTO LISTO, mantener KEY, pulsar y
   soltar solo RST y seguir manteniendo KEY.
4. Tras un nuevo modo1 vuelve a consultar el cargador. Solo una respuesta
   correcta permite seguir con `verify_flash` sobre los mismos archivos y
   parámetros, comprobando además que los BIN/manifiesto no cambiaron.

No hay otra escritura en la recuperación. Si otra consulta vuelve a fallar,
requiere otra intervención explícita; no se pulsa reset ni se acepta modo1
como sustituto de la respuesta. Cancelar o fallar conserva escritura iniciada
y contenido sin verificación completada. El fallo de comparación de contenido
de `verify_flash` sigue terminando la operación; no dispara otra escritura.

Los eventos `checking_bootloader` y `verification_paused` mantienen Escritura
completada y Verificación pendiente. La evidencia de Conexión se vuelve a
comprobar. El asistente explica que se recupera para verificar y no para
reprogramar. GUI y acciones MCP comparten el mismo flujo; autoaceptación no
omite la intervención física.

## Límites y fuentes

La consulta acredita respuesta del cargador ROM/stub en ese momento. La orden
de verificación usa otro proceso y debe sincronizar también: no se promete
detectar un reset ocurrido después de la consulta ni un estado permanente.
Una ausencia de respuesta no aísla boot incorrecto frente a un fallo de cable,
puerto o alimentación; por eso el aviso dice que no pudo confirmarse.

Lectura de esptool4.5.1 instalado: `esptool/cmds.py::read_mac` lee la MAC y
la imprime con etiqueta `MAC`; CLI en `esptool/__init__.py`. No se ejecutó.
La documentación primaria describe [read-mac](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/basic-commands.html#read-built-in-mac-address-read-mac).
Estas lecturas no son una prueba de la nueva secuencia en la placa.

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.7.3 y documentación; sin compilar ni ejecutar esta revisión. |
| atmega / esp / web | Sin cambios de fuente, compilación o carga por esta ficha. |
| CAD | Sin cambios. |
| Contracts | Flujo, límites y procedencia registrados. |

No se ejecutaron tests, compilaciones, validadores, GUI/MCP, apertura COM,
flasheos ni pruebas físicas en esta revisión.

## Adaptaciones por archivo

| Archivo | Adaptación |
| --- | --- |
| `AutoFinalRobo2-Loader/src/Operations.cs` | Comprobación previa, respuesta positiva exigida, pausa/recuperación y contexto del asistente, sin repetir escritura. |
| `AutoFinalRobo2-Loader/src/Operations.EspModes.cs` | Mismo control antes de verificar los BIN de paquetes AT. |
| `AutoFinalRobo2-Loader/src/MainForm.Workflow.cs` | Estados pendientes y asistente KEY/RST específico para recuperar antes de verificar. |
| `AutoFinalRobo2-Loader/src/JobState.cs` | Versión nominal0.7.3. |
| `AutoFinalRobo2-Loader/README.md` | Secuencia, condiciones, recuperación y límites. |
| `AutoFinalRobo2-Loader/MCP.md` | Pausa/recuperación compartida, incluida guía inicial desactivada. |
| `AutoFinalRobo2-Loader/MANUAL.html` | Manual0.7.3 y aviso antes de verificar. |
| `AutoFinalRobo2-Contracts/README.md` | Enlace y estado de la entrega. |
| `AutoFinalRobo2-Contracts/INFORME_ADAPTACION.md` | Estado sin ejecución. |
| `AutoFinalRobo2-Contracts/CHANGELOG.md` | Entrada incremental. |
| `AutoFinalRobo2-Contracts/contracts/LOADER_FLASH_MODES.md` | Condición previa y recuperación compartidas para Web y AT. |
| `AutoFinalRobo2-Contracts/changes/2026-10-27-29-loader-verification-bootloader-gate.md` | Pedido, secuencia, fuentes y procedencia. |
