# Compilación y preparación de carga - AFR2-20261027-10

27/10/2026. Pedido explícito del usuario: compilar todos los módulos, ejecutar
build:esp y copiar la Web a ESP, crear el filesystem y explicar su carga por
USB-B del Mega. El usuario confirma ocho señales Hall conectadas, solicita
activar captura e identifica Arduino Mega 2560 en COM16; desconoce los jumpers.

**Todos los pasos de compilación y generación terminaron con exit0. No se
abrió COM16 ni se grabó firmware/filesystem en la placa.**

| Artefacto | Resultado |
| --- | --- |
| AVR producto, comunicación y biblioteca | make -B firmware communication-firmware library, exit0 |
| Captura Hall | AFR2_HALL_CONNECTED=1; A8-A11 A, A12-A15 B, ambos FR/FL/RR/RL; máscara0x0F |
| AVR producto flash / SRAM estática | 73.306 / 5.930 bytes |
| SRAM no asignada estáticamente | 2.262 bytes; no acredita pila/ISR ni margen de ejecución |
| ESP firmware | PlatformIO esp12s_candidate, exit0; RAM47.008 / programa433.787 bytes |
| Web | npm run build:esp, exit0, ocho activos y1.571.104bytes gzip |
| Copia a ESP | Ejecutada por build:esp; manifest y ocho activos en ESP/data/dist |
| FS estimado / presupuesto de empaquetado | 1.671.168 / 2.867.200 bytes |
| Imagen LittleFS | buildfs exit0;3.121.152bytes, generada DESPUÉS de copy:esp |
| Firmware / Web / EX01 | 0.10.0 / 0.3.0 / 8; formato wire conservado |

Los valores de programa y SRAM son salidas de las herramientas de build, no
mediciones físicas. La imagen LittleFS usa todo el rango reservado, por eso su
tamaño no es igual a la suma gzip de los activos. Además de /dist, buildfs
incluyó los dos Markdown ya existentes en ESP/data; se preservaron.

## Cambios necesarios

- `include/afr2_board.h`: gate Hall de0a1 por solicitud directa. La preferencia
  persistente `hall_wired` se conserva: si estaba0, activar **Calibracion >
  Geometria > Hall conectado=1** y Guardar EEPROM tras cargar. No se fuerza
  CPR, geometría, calibración ni autorización de memoria de secuencias.
- `collectorSimulation.ts`: anotación explícita de retorno `CollectorState`
  en `startManualScan`; evita ensanchar `phase: 'idle'` a string y resuelve
  TS2345 sin cambiar la lógica. Los primeros intentos TypeScript también
  encontraron cambios del brazo en curso; no se revirtieron esos cambios.
- `tools/upload-mega.ps1`: lanzador de avrdude para el HEX de producto,
  puerto obligatorio, sin compilación implícita. Script creado, no ejecutado.

La ejecución restringida de PlatformIO falló al escribir platforms.lock; Make
no pudo iniciar avr-gcc (Error623). Las compilaciones se repitieron con permiso
de ejecución adecuado y concluyeron. No se modificaron ACL ni se borraron
cachés globales. Los logs finales no contienen errores/advertencias de código.

## Archivos y evidencias

- [ATmega](atmega-build.txt) y [secciones ELF](atmega-size.txt).
- [ESP](esp-build.txt), [Web + copy:esp](web-esp-build.txt), [LittleFS](littlefs-build.txt).
- [Manifiesto del paquete promovido](manifest.json).
- [Resumen, rutas, tamaños y SHA-256 de artefactos](summary.json).
- [Procedimiento de carga para esta PC/COM16](../../../hardware/CARGA_USB_COM16.md).

Flash/SPI reales sin consultar. El perfil 4MiB / eagle.flash.4m3m.ld es el
candidato acordado: rango FS0x100000..0x3FA000,3.121.152bytes; no identifica la
capacidad de la unidad conectada. Antes de cargar ESP se necesita preparar
el puente por USB Mega y confirmar flash/recorrido de niveles. No se adivinan
jumpers desde el nombre COM16.

Sin tests, validadores independientes, navegador, flasheo, subida FS, pruebas
de hardware, ERC/DRC, commit ni push. Solo compilación, empaquetado/promoción,
construcción de imagen y registro de artefactos autorizados.
