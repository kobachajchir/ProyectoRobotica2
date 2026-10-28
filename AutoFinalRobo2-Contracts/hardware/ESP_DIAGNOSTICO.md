# Diagnóstico único de conexión y flash ESP

AFR2-20261023-12. Script preparado para ejecutarlo **cuando esté conectada la placa**. No se ejecutó contra hardware al crearlo. Usa el mismo script primero para conexión y después para lectura; no genera un segundo programa ni carga el firmware del proyecto.

## Antes de lanzarlo

Seguir [FLASHING.md](FLASHING.md): el modo de servicio por USB Mega es distinto de operación D18/D19. Mantener el Mega en reset para usar su puente, aislar los demás emisores y usar D0→RX ESP / TX ESP→D1 por la ruta con niveles adecuados. El selector normal a D0/D1 no resuelve ese cruce. La cabecera directa ESP UART es 3,3 V. Alimentación/masa y ruta de adaptación del shield deben estar identificadas. Cerrar el monitor serie.

Entrar manualmente al cargador de la ESP según KEY/RST, una vez confirmada su ruta en la placa: KEY mantenido, pulsar y soltar solo RST y conservar KEY hasta finalizar la consulta. Si se continúa con una carga, mantenerlo también durante escritura y verificación. Procedimiento adoptado para esta unidad por la carga manual aportada en [AFR2-20261027-27](../changes/2026-10-27-27-loader-esp-command-key.md). El script usa `no_reset`/`no-reset`, no presupone auto-reset. Abrir un puerto puede afectar líneas de algunos adaptadores; no es una garantía de inmovilidad eléctrica. Hacer la operación con la potencia de motores desconectada.

Se necesita Python con **esptool 4.x o 5.x** y sus dependencias. El lanzador busca Python de PlatformIO, después `py -3` y después `python`; `-Python` permite elegir el intérprete. No instala software automáticamente. Si falta esptool, instalarlo en ese mismo Python mediante `python -m pip install esptool` (o sustituir `python` por la ruta elegida). No usar el Python de RoboDK por defecto si no tiene las dependencias/versiones adecuadas.

Actualización AFR2-20261023-13: si esptool no está en site-packages, el script también busca el paquete ya instalado en `~/.platformio/packages/tool-esptoolpy`. La ruta se habilita solo en el proceso y sus hijos; no cambia PATH ni instala paquetes. En esta PC quedó verificado Python PlatformIO **3.11.7**, pyserial **3.5** y esptool **4.5.1**. El Python de RoboDK 3.7.3 no tiene esptool importable. [Informe de la comprobación](diagnostics/20261023-155540-554529-6488/report.md). Solo se enumeraron COM Bluetooth; todavía no hay conexión ESP ni lectura de flash.

## Uso desde AutoFinalRobo2-Contracts

Validar herramientas y su CLI sin abrir ningún puerto:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\esp-probe.ps1 -CheckTools
```

Esto comprueba imports/versiones, ejecuta `python -m esptool --help`, enumera puertos y guarda informe. No requiere la placa; `tools-ok-no-device-opened` confirma el entorno, no el hardware.

Enumerar puertos sin abrir ninguno:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\esp-probe.ps1 -ListPorts
```

Una sola ejecución para conectar, consultar flash y leer cabecera (reemplazar COM7 por el puerto correcto):

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\esp-probe.ps1 -Port COM7
```

Para comprobar primero solo la conexión:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\esp-probe.ps1 -Port COM7 -CheckOnly
```

Si después se ejecuta la consulta completa, volver a colocar manualmente la ESP en el cargador antes de repetir. Si no se indica puerto, el script enumera y pide seleccionarlo en consola interactiva; no barre ni abre automáticamente todos los COM. `-Python 'C:\ruta\python.exe'` selecciona un intérprete concreto. También puede usarse directamente `python .\tools\esp_probe.py --port COM7`.

## Qué guarda y qué significa

Cada intento crea una carpeta nueva en `hardware/diagnostics/FECHA-HORA-ID/` con `report.md`, `report.json` y logs de los pasos ejecutados, incluso si la conexión falla o se interrumpe. No pisa informes anteriores. El informe conserva puerto/USB, intérprete, versión esptool, comandos, exit codes, ID/tamaño de flash detectados y configuración de cabecera reconocida. Los archivos de texto/JSON quedan disponibles para versionar; no se guardan volcados completos de firmware ni preferencias.

1. `chip_id` / `chip-id`, sin stub: sincroniza e identifica la ESP8266 seleccionada a 115200. Si falla, corta ahí y guarda el error. Esto acredita comunicación serie, no prueba continuidad de todos los cables, niveles eléctricos ni alimentación bajo carga.
2. `flash_id` / `flash-id`, sin stub: consulta fabricante, dispositivo y tamaño detectado por esptool. Si falla, no intenta leer la cabecera.
3. `read_flash` / `read-flash`, ocho bytes desde 0: usa un **stub temporal en RAM** de esptool para leer la cabecera; no escribe flash. Decodifica modo/frecuencia/tamaño configurados de una imagen reconocida. No certifica integridad del firmware ni que esos ajustes sean adecuados. El archivo temporal de ocho bytes se elimina al salir normalmente, también ante errores/interrupción manejada; tras cierre forzado podría quedar en una subcarpeta `header-*`, ignorada por Git.

Los pasos se guardan tras cada éxito y tienen un timeout de 45 s por comando. Un retorno no cero guarda el fallo y detiene el resto. La consulta completa devuelve 0 si se completaron las lecturas y se interpretó el tamaño; 2 si no se pudo interpretar ese tamaño; 1 ante error/interrupción. Un retorno 0 **no aprueba una partición ni el futuro flasheo**.

El reporte compara tamaño detectado con tamaño declarado en cabecera y payload gzip local con la flash total. Compara datos, no el margen real de LittleFS: que el bundle entre en todo el chip no significa que entre en su partición. No toma la imagen AT ni `esp12e` como layout real.

En Arduino ESP8266 el reparto firmware/OTA/FS depende del layout/linker y/o configuración del firmware. Esta lectura no puede identificar de forma universal el inicio/fin de LittleFS de un firmware desconocido. Se documenta `partition.status=not-determined` y offsets null. Para continuar usaremos la flash medida, el tamaño/mapa del firmware y un linker conocido; luego se verificará el filesystem con el firmware correspondiente. No se prueba QIO/DIO/DOUT mediante escrituras ni se buscan tablas ESP32 en offsets supuestos.

Al finalizar, KEY suelto y reset manual para salir del cargador; restaurar conexión de operación D18/D19 cuando corresponda. Conservar la carpeta de diagnóstico y retomar desde su `report.md` y [PLAN_ESP.md](../docs/PLAN_ESP.md). No hay erase, write_flash, formateo, build ni uploadfs en estos scripts.

Fuentes: [comandos esptool ESP8266](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/basic-commands.html), [modos de flash](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/flash-modes.html) y [layout de filesystem Arduino ESP8266](https://arduino-esp8266.readthedocs.io/en/stable/filesystem.html). La sintaxis antigua se cotejó con el paquete local esptool 4.5.1 de PlatformIO, sin ejecutarlo.
