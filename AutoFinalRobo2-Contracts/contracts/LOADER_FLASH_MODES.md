> **AFR2-20261028-23 / Loader0.9.5:** GUI usa FileSystem y oculta Sección en Paquete AT; exige confirmación humana de capacidad antes de mostrar particiones, manteniendo consulta física previa a escritura. Se exige preparación por sección en sesión (compilación propia o revisión de paquete AT), perfil/fuentes/imágenes correspondientes. La verificación ESP usa hash integrado en la misma sesión write_flash y requiere una confirmación por imagen y salida0; no consulta read_mac ni ejecuta verify_flash adicionales. Aviso KEY al primer porcentaje de escritura, nunca por Connecting. Las reglas históricas de reconexión posterior quedan sustituidas. Esto es fuente compilada del Loader y revisión de GUI, sin prueba física.

> **AFR2-20261028-16 / 0.9.5:** el selector visible separa Destino ATmega/ESP y, dentro de ESP, Variante y Sección (Firmware/Web/ambos). La sección decide imágenes a escribir; ya no hay casilla filesystem visible. Se conservan layouts/linker y reglas del paquete AT; no se inventa filesystem en un BIN monolítico. Los índices internos de acciones y el esquema MCP siguen iguales. Envío de monitor explícito por su mismo worker, sin relación con el mapa de flash. [Ficha](../changes/2026-10-28-16-loader-095-interfaz-envio-serie.md).

> **AFR2-20261028-15 / Loader0.9.4:** AtPackage admite selección directa del BIN completo Ai-Thinker AT v1.5.4.1 identificado por SHA-256. El mapa equivalente se construye en memoria (4 MiB, offset0, DIO/40 MHz del archivo), sin convertir el BIN. Otros BIN necesitan JSON del proveedor. Esta excepción reemplaza la exclusividad JSON de la descripción original inferior. Mapa candidato de imagen completa, sin instalación ni prueba física. [Ficha](../changes/2026-10-28-15-loader-at-bin-directo.md).

> AFR2-20261028-11 / Loader0.9.1: integra flasher remoto0.9.0 y mejoras locales0.7.4. EspTransport conserva mega_reset (D0/D1, RESET-GND), mega_bridge (D18/D19, RESET libre y baud fijo) y usb-ttl externo3,3V; mega-usb es alias anterior de mega_reset. La recuperación previa a verificar respeta el transporte sin cambiar cableado ni reescribir. Monitor HEX y backups/Preferencias conservados. Fuente sin compilar/ejecutar; [integración](../changes/2026-10-28-11-integracion-local-remoto.md).

# Loader: firmware Web o AT por USB — AFR2-20261027-22

> **Ampliación AFR2-20261027-29 / Loader0.7.3:** antes de enviar la
> verificación de contenido se exige una consulta nueva al cargador ROM/stub.
> Si no se confirma respuesta, se pausa y pide recuperar con KEY/RST, incluso
> con la guía inicial desactivada. Tras recuperarlo consulta nuevamente y
> continúa solo con la verificación; no repite la escritura. GUI/MCP y ambas
> familias ESP comparten el control. [Flujo y límites](../changes/2026-10-27-29-loader-verification-bootloader-gate.md).
> Solo fuente/documentación, sin compilar ni ejecutar esta revisión.

> **Ampliación AFR2-20261027-27 / Loader0.7.1:** perfil schema1 con
> `EspTransport` (`mega-usb` o `usb-ttl`), instrucciones de conexión separadas
> y KEY sostenido hasta finalizar la operación, incluidas escritura y
> verificación. Perfiles previos reciben `mega-usb`; COM y baudios siguen
> configurables. La invocación de Python agrega `-u` antes de esptool y
> conserva `no_reset` antes/después. Escritura Web predeterminada: stub normal,
> `keep/keep/4MB`, offset0; AT conserva su mapa/SPI. La consulta ROM/flash
> previa y la verificación explícita posterior siguen separadas de la escritura.
> [Comparación y evidencia del usuario](../changes/2026-10-27-27-loader-esp-command-key.md).
> Fuente sin compilar/ejecutar esta revisión; el éxito manual aportado no
> acredita el Loader corregido ni modifica ROBO2/EX01.

27/10/2026. Loader nominal0.6.0, implementación en fuente sin compilar/probar.
El usuario aclaró: elegir firmware AT o Web y cargar por USB, sin OTA/HTTP.
El producto AutoFinalRobo2 conserva Arduino ESP8266 + LittleFS y su protocolo
propio; esta ampliación del cargador permite otros perfiles para compañeros.
El firmware del Mega no adquiere soporte AT por seleccionar otro firmware ESP.

## Familias y acciones

| Selección | Preparar | Flashear y verificar |
| --- | --- | --- |
| ATmega | Make y herramientas AVR del perfil | avrdude, HEX, firma y verificación AVR existentes. |
| ESP Web / Arduino | PlatformIO; Web/LittleFS si está activado | esptool por ROM, BIN Arduino e imagen LittleFS según linker. |
| ESP AT / paquete | Revisar BIN precompilados del proveedor | esptool por ROM, archivos y offsets del paquete AT. |

AT describe el protocolo del firmware instalado. No existe un formato genérico
de archivo AT ni se usa AT+GMR para escribir firmware. La instalación de paquetes
AT usa bootloader/ROM y esptool como la otra familia; el mapa de regiones cambia.
El firmware puede ser factory combinado o varias imágenes: el manifiesto debe
describir exactamente la distribución oficial del paquete concreto.

`build_esp` con familia AT revisa el paquete: no compila el proyecto Arduino.
`build_web_fs` con AT revisa sus imágenes precompiladas; requiere FS activado.
`build_all` compila ATmega, prepara ESP según familia y solo construye Web/FS
si es Web con FS activado. Revisión de paquete tiene indicador propio y no se
presenta como compilación. Las acciones `flash_esp`, `flash_fs`, `flash_esp_fs`
seleccionan firmware/sistema, filesystem o ambos según la familia del perfil.
La opción FS desactivada impide pedir cargas filesystem.

## Memoria Web

La lista se forma con linkers instalados `eagle.flash.<FlashMB>m*.ld` de la
carpeta del linker del perfil. Se leen `_FS_start`, `_FS_end` e `irom0_0_seg`;
direcciones de flash CPU se convierten restando0x40200000. La UI muestra flash
esperada, máximo de código, bytes útiles FS y otras reservas/espacio, en B/MiB.
Es una propuesta de layout: la flash física se consulta antes de toda escritura.

Ejemplo leído del SDK local para 4MiB: `4m1m` declara FS1.024.000B y `4m3m`
FS3.121.152B. Ambos declaran el límite de código1.044.464B: el espacio restante
no se convierte automáticamente en un programa de3MiB. El preset sin FS usa
su linker específico y reserva0B de FS. Un firmware que sirve activos desde
LittleFS necesita esos activos y un layout compatible para funcionar.

Seleccionar preset actualiza linker/offset/tamaño/límite en el perfil. Al construir
se genera un INI local en LOCALAPPDATA/build-configs y se pasa a PlatformIO con
`--project-dir` y `--project-conf`. El INI del producto no se reescribe. Los BIN
guardan recibos locales con hash de imagen y de layout/INI; flashear un preset
exige recibo coincidente. Cambiar selección no reetiqueta un BIN viejo como válido.
`custom` mantiene el flujo de imágenes externas y verifica el linker/entorno
declarados por el perfil; no demuestra la procedencia de ese BIN externo.

## Paquete AT `afr2-esp-at-package-1`

Seleccionar con **Paquete AT...** un JSON junto al conjunto de BIN del proveedor.
La plantilla distribuida contiene campos deliberadamente incompletos y se
rechaza hasta completarlos con información real; no contiene offsets candidatos
presentados como compatibles. No se redistribuye ni descarga firmware AT aquí.

| Campo | Regla |
| --- | --- |
| `Schema` | `afr2-esp-at-package-1`. |
| `Chip` | `esp8266`; rechaza paquetes ESP32 u otras familias. |
| `Name` | Identificación de módulo/versión del proveedor. |
| `FlashBytes` | 1,2,4,8 o16MiB en bytes; coincide con FlashMB y detección física. |
| `FlashMode` | dio/dout/qio/qout, concreto del paquete. |
| `FlashFreq` | 20m/26m/40m/80m, concreto del paquete. |
| `Layouts` | Variantes provistas/compiladas para cada distribución de memoria. |
| Layout `Id`, `Label` | Identificador único y nombre visible. |
| Layout `Filesystem` | none/littlefs/spiffs/vendor, coherente con sus regiones. |
| Layout `Regions` | Regiones explícitas, no solapadas, dentro de flash. |
| Región `Name`, `Role` | Nombre único; firmware, system o filesystem. |
| Región `Offset` | Decimal o hexadecimal0x; múltiplo de4096. |
| Región `Capacity` | Reserva en bytes, positiva y múltiplo de4096. |
| Región `File` | Ruta relativa al JSON, sin raíz ni ascensos `..`. |
| Región `Sha256` | Opcional:64hex del BIN que se exige antes de usarlo. |

El tamaño de cada BIN cabe en su reserva. Para filesystem debe ser exactamente
su capacidad; datos sueltos no se tratan como una imagen FS. El constructor de
imágenes es el del proveedor del firmware AT: LittleFS Arduino no se aplica a
una partición AT arbitraria. Todos los layouts declaran alguna región firmware.
Los tamaños AT del dropdown provienen de `Layouts`; no se recorta un factory BIN
ni se mueve una partición al elegir un número distinto. Para otra reserva FS se
necesita otra variante compatible y sus imágenes, generadas por su proveedor.

La casilla AT incluye/excluye las imágenes filesystem de la escritura; no modifica
la tabla de particiones del paquete. La UI conserva visible su reserva aun cuando
no se escriba. Instalar firmware incluye también regiones system. Una imagen
factory combinada se declara como una región: no se añaden otras que se superpongan.
Filesystem solo exige confirmar que el firmware ya instalado usa ese layout.

Antes de escribir se comparan capacidades, se muestran hashes/rutas/offsets y
se confirma la selección. Manifiesto y BIN no pueden cambiar durante el flujo.
Después se ejecuta verify_flash/verify-flash contra todos los BIN elegidos.
Los logs/resumen guardan familia, mapa y hashes aun si una operación posterior falla.
Sin respuesta ROM/tamaño reconocidos no se escribe. No hay erase global automático.

## Lanzador, perfiles y MCP

`Iniciar.ps1 -FirmwareEsp web|at -Perfil nombre.json` abre la selección solicitada.
Perfil es un JSON de la biblioteca LOCALAPPDATA; no se inicia carga ni COM. El
modo de inicio no se guarda hasta Guardar. Los perfiles schema1 previos reciben
valores predeterminados Web/custom/FS activado sin alterar sus rutas anteriores.
Los perfiles exportados por0.6 agregan campos y no se prometen legibles en0.4.

Campos nuevos: EspFirmwareMode, WebUseFilesystem, WebLayoutPreset, AtPackage,
AtLayoutId y AtUseFilesystem. `loader_status` informa familia/FS; `loader_profile`
informa valores/rutas y resumen de memoria. Las herramientas MCP mantienen sus
nombres y el flujo de deduplicación, revisión/autoaceptación y una operación.

## Fuentes y límites

Consulta de solo lectura a AutoMicro: `FirmwareF4/Core/Src/wifi_bridge_at.c`
y `AutoMicroQtDesktop/src/protocol/AtProtocol.cpp`: comandos CRLF y consulta
AT/AT+GMR para operar/identificar, no instalador de firmware. No se copia su
backend DMA ni se modifica su código/registro. Autorías preservadas.

Fuentes primarias: [Espressif, instalación de firmware AT](https://docs.espressif.com/projects/esp-at/en/release-v2.2.0.0_esp8266/Get_Started/Downloading_guide.html),
[comandos AT ESP8266](https://docs.espressif.com/projects/esp-at/en/release-v2.1.0.0_esp8266/AT_Command_Set/Basic_AT_Commands.html),
[Core ESP8266: layouts y filesystem](https://arduino-esp8266.readthedocs.io/en/3.1.2/filesystem.html).
La guía AT usa también ejemplos ESP32: no se trasladan sus offsets al ESP8266.
AT+FS está marcado solo ESP32 en la referencia citada: no se promete escritura
genérica de archivos por ese comando en nuestro ESP8266.

Solo fuente y documentación de Loader0.6. No se ejecutaron builds, tests,
validadores, aplicación, MCP ni puertos/flasheos de esta revisión. No se certifican
paquetes concretos, tamaños físicos ni compatibilidad del robot con AT.
