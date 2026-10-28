> **Interfaz gráfica AFR2-20261027-13:** disponible [AFR2 Loader para Windows](../../AutoFinalRobo2-Loader/README.md), con rutas/COM configurables, asistentes y [manual portable](../../AutoFinalRobo2-Loader/MANUAL.html). Fuente implementada; aún sin compilación ni pruebas de la herramienta. Esta guía conserva los comandos manuales.

# Carga de AutoFinalRobo2 desde USB-B del Mega

27/10/2026, AFR2-20261027-10. **Compilaciones y LittleFS preparados; no cargados.**
El usuario identifica Arduino Mega 2560 en **COM16**, con USB conectado. No sabe
cómo están los jumpers del shield. Por ello no se abre el puerto ni se inicia
una carga en esta entrega. [Resultados de build](../documentation/builds/2026-10-27-10/README.md).

## Herramientas y qué se carga

Se puede usar la terminal PowerShell integrada de VS Code. Para el ESP,
PlatformIO también ofrece las tareas Upload y Upload Filesystem Image. El
firmware Mega usa Makefile C/avr-libc y avrdude; no es un sketch Arduino ni un
proyecto PlatformIO AVR. VS Code es opcional: los mismos comandos funcionan
desde una terminal PowerShell normal.

| Destino | Archivo generado | Herramienta |
| --- | --- | --- |
| ATmega2560, producto completo | `AutoFinalRobo2-Atmega/build/afr2.hex` | avrdude / bootloader Mega |
| ESP8266, firmware | `AutoFinalRobo2-ESP/.pio/build/esp12s_candidate/firmware.bin` | PlatformIO Upload |
| ESP8266, Web y filesystem | `AutoFinalRobo2-ESP/.pio/build/esp12s_candidate/littlefs.bin` | PlatformIO Upload Filesystem Image |

`afr2-communication.hex` es una composición de comunicaciones separada; **no
sustituir afr2.hex por ella** para usar el robot completo. La biblioteca
`libafr2_ui.a` es un resultado de desarrollo y no se flashea por separado.

El comando `npm run build:esp` ya ejecutó sincronización OLED, TypeScript,
Vite en modo esp, gzip/manifiesto y copy:esp. La Web quedó en `ESP/data/dist`,
y después se ejecutó `pio run -e esp12s_candidate -t buildfs`. No hay que cargar
la Web al Mega: se almacena en la flash del ESP.

## USB-B: mismo cable, dos conexiones de programación

**El USB-B del Mega puede servir para ambas placas, pero no con el mismo
cableado de señales.** COM16 solo identifica el puente USB-serie del Mega;
no demuestra que el ESP esté conectado al puente ni en modo cargador.

| Modo | RESET del ATmega | Conexión serie |
| --- | --- | --- |
| Carga del Mega | Libre; quitar puente RESET-GND | USB-serie hacia D0/D1 del Mega; ESP aislado de D0/D1 |
| Carga del ESP usando puente del Mega | RESET Mega unido a GND | Nodo D0/RX0 hacia RX del ESP; TX del ESP hacia nodo D1/TX0, por ruta con niveles adecuados |
| Operación del robot | Libre | Mega D18/TX1 a E_RX del selector; E_TX del selector a Mega D19/RX1 |

En la carga ESP, D0 es la **salida del puente USB** y D1 es su entrada: los
rótulos RX/TX están nombrados desde el Mega. Los jumpers habituales del shield
pueden conectar la UART en el otro sentido. No basta elegir D0/D1 en el
selector; primero hay que identificar su trazado. Ver [FLASHING.md](FLASHING.md)
y [WIFI_SHIELD.md](WIFI_SHIELD.md).

Para preparar la carga: identificar los jumpers E_RX/E_TX, los cables a
D18/D19, la ruta de adaptación de niveles del shield y el RESET del Mega.
Una foto nítida de la cara superior completa del conjunto y otra de los
jumpers permite indicar qué retirar y dónde puentear. **No aplicar 5 V al
puerto directo ESP UART: es de 3,3 V.** Mantener desconectada la potencia de
motores durante la programación y cerrar monitores serie que ocupen COM16.

## 1. Cargar el Mega, una vez preparado su modo

Con ESP aislado de D0/D1 y RESET Mega libre, ejecutar:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "D:\ProyectoRobotica2\AutoFinalRobo2-Atmega\tools\upload-mega.ps1" -Port COM16
```

El script utiliza avrdude instalado en Arduino15, protocolo `wiring`,
ATmega2560 y 115200 baudios. Solo escribe la flash de aplicación; no pide
cambio de fuses, bootloader ni EEPROM. La comprobación de escritura que realiza
avrdude al cargar es parte de la futura carga; no se ejecutó en esta entrega.

## 2. Identificar el ESP antes de cargar

Pasar al modo de carga ESP de la tabla anterior. Una vez identificados KEY y
RST: mantener KEY (GPIO0 bajo), pulsar y soltar solo RST del ESP y mantener
KEY hasta finalizar la operación, incluidas escritura y verificación.
Cerrar un asistente o ver modo1 no indica soltar KEY. Procedimiento actualizado
por la carga USB–TTL aportada en [AFR2-20261027-27](../changes/2026-10-27-27-loader-esp-command-key.md);
ese éxito COM5 no valida la ruta por USB Mega descrita aquí.
El RESET del Mega se mantiene a GND para evitar otro emisor sobre UART.
Entrar al cargador no se deduce de que aparezca COM16.

La configuración de build sigue siendo candidata: `esp12e`, flash 4 MiB,
linker `eagle.flash.4m3m.ld`. La imagen LittleFS ocupa 3.121.152 bytes y empieza
en offset de flash 0x100000, según el linker instalado. **Eso describe el
build, no una lectura de la flash física.** Antes de subir, consultar la
flash real y su cabecera con el script existente, solo tras preparar el puente:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File "D:\ProyectoRobotica2\AutoFinalRobo2-Contracts\tools\esp-probe.ps1" -Port COM16
```

Este diagnóstico lee identidad/tamaño/cabecera, sin grabar ni borrar flash.
Volver a entrar manualmente al cargador antes de cada operación si hace falta.
Si el resultado no corresponde al perfil de 4 MiB, ajustar el perfil y
reconstruir firmware/FS antes de escribir. No usar offsets de firmware AT ni
suponer que el cable USB identifica el tamaño de flash.

## 3. Cargar firmware ESP y luego LittleFS

Después de identificar flash, ruta de niveles y modo cargador:

```powershell
$afr2Pio = "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe"
& $afr2Pio run --project-dir "D:\ProyectoRobotica2\AutoFinalRobo2-ESP" -e esp12s_candidate -t upload --upload-port COM16
```

Volver a poner el ESP en modo cargador si el primer upload lo sacó de él. Luego:

```powershell
& $afr2Pio run --project-dir "D:\ProyectoRobotica2\AutoFinalRobo2-ESP" -e esp12s_candidate -t uploadfs --upload-port COM16
```

En VS Code/PlatformIO equivalen a **Upload** y **Upload Filesystem Image** del
entorno `esp12s_candidate`, eligiendo COM16. `Build Filesystem Image` solo
genera la imagen; `Upload Filesystem Image` la escribe. Subir LittleFS completo
reemplaza su contenido: si hay preferencias Wi-Fi que conservar, respaldarlas
antes. No se solicita erase_flash ni se formatea automáticamente.

## 4. Restaurar operación y activar Hall en configuración

Quitar RESET-GND del Mega y restablecer D18/TX1 a E_RX y E_TX a D19/RX1.
Retirar los puentes de servicio D0/D1, mantener Bluetooth aislado y resetear
el ESP con KEY libre para ejecutar el firmware.

La fuente ahora compila **AFR2_HALL_CONNECTED=1**, autorizado por el usuario
al confirmar las ocho señales. La configuración persistente sigue siendo
independiente: en OLED, pulsación larga SW abre menú; ir a
**Calibracion > Geometria > Hall conectado = 1**, volver a Calibracion y
**Guardar EEPROM**. Una EEPROM anterior con `hall_wired=0` no se fuerza ni
se borra al cargar. La captura no inventa CPR/signos/geometría: calibrarlos
es un paso posterior a la programación, no ejecutado aquí.

## Fuentes

- [PlatformIO ESP8266: firmware y uploadfs](https://docs.platformio.org/en/latest/platforms/espressif8266.html#uploading-files-to-file-system).
- [Espressif: modo de arranque GPIO0 y reset](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html).
- [OSOYOO: shield UART y selectores](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/).
- Documentos locales FLASHING.md, WIFI_SHIELD.md, ESP_DIAGNOSTICO.md y linker
  instalado; la identificación del montaje físico sigue pendiente.
