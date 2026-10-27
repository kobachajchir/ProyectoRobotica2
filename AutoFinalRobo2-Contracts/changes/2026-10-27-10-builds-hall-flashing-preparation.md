# AFR2-20261027-10 - Builds, Hall activo y preparación de carga

27/10/2026. El usuario pide compilar ATmega/ESP/Web, ejecutar build:esp y la
copia a ESP, generar filesystem y explicar el flasheo por USB-B del Mega/VS
Code. Confirma nuevo cableado Hall y solicita activar captura. Informa COM16,
pero desconoce el estado del selector y jumpers del shield.

## Resultado

ATmega producto/comunicación/biblioteca, firmware ESP, Web perfil esp,
promoción a ESP/data/dist e imagen LittleFS completados con exit0.
[Evidencias y límites](../documentation/builds/2026-10-27-10/README.md).

Ocho activos,1.571.104bytes gzip, FS estimado1.671.168bytes; imagen LittleFS
real3.121.152bytes del perfil candidato de4MiB. No se interpreta la salida
PlatformIO "4MB Flash" como detección física: no se abrió puerto serie.

## Fuente

- ATmega: AFR2_HALL_CONNECTED=1, por confirmación/pedido del usuario. El mapa
  agrupado de07 y contadores lógicos FL/FR/RL/RR permanecen. La preferencia
  hall_wired y calibraciones de EEPROM se conservan; activación en menú si
  estaba0. Captura habilitada en build no demuestra funcionamiento físico.
- Web: retorno `CollectorState` explícito en startManualScan para corregir
  TS2345 de phase:string. Sin cambio de lógica de simulación ni wire.
- ESP: sin cambio de firmware fuente; paquete Web promovido localmente.
- Herramienta Mega: upload-mega.ps1 prepara un comando avrdude reproducible
  con puerto obligatorio; no ejecutada.

No cambian IDs, versiones wire, payloads, unidades, CPR, geometría ni
AFR2_SEQUENCE_MEMORY_APPROVED. Los cambios físicos de alimentación del panel
a5V siguen siendo montaje comunicado; CAD+3V3 no se modifica.

## Carga pendiente

[Guía específica USB/COM16](../hardware/CARGA_USB_COM16.md). Mega se programa
por su bootloader con RESET libre y ESP aislado de D0/D1. ESP y FS pueden
usar el puente USB del Mega con Mega en reset, el cruce correcto y niveles
adecuados. Requiere identificar los jumpers y flash real antes de escribir.
VS Code es opcional; se documentan terminal PowerShell y tareas PlatformIO.

| Módulo | Estado real |
| --- | --- |
| atmega | Fuente Hall habilitada; tres artefactos compilados; sin carga |
| esp | Firmware compilado, Web promovida e imagen FS generada; sin carga |
| web | Perfil esp compilado y empaquetado; sin navegador |
| contracts | Logs, resumen/hashes, guía y cambios registrados |
| cad/hardware | Sin edición CAD ni prueba física; COM16 informado por usuario |

Sin tests/validadores independientes, monitor serie, apertura COM16, flasheos,
erase/format, pruebas físicas, commit ni push. Se preserva trabajo ajeno sucio.
