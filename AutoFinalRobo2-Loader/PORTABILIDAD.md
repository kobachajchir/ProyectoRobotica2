# AFR2 Loader 2.0.0 — Distribución compartible

Hecho por Koba Chajchir / AFR2. Incluye el Loader y el servidor MCP precompilados,
recursos de marca, manual y scripts. Extraé toda la carpeta antes de abrir
**AFR2 Loader.exe**. El EXE de esta distribución abre directamente la GUI;
no necesita fuentes, Visual Studio ni compilador C#.

La pestaña ATmega redundante se retiró. **Flasher** conserva instalación,
comprobación y restauración; **Preparar firmware del Mega** lleva al destino
ATmega de Operación para compilar/cargar su programa. Las opciones por placa
de Configuración y la lectura del monitor permanecen disponibles.

## Requisitos externos

| Uso | Requisito |
| --- | --- |
| Abrir GUI/monitor | Windows 10/11, .NET Framework 4.8 y controlador USB de la placa |
| Scripts y MCP incluidos | Windows PowerShell 5.1 |
| Compilar ATmega/servicio | GNU Make, avr-gcc, avr-ar, avr-objcopy y fuentes propias |
| Cargar/comprobar Mega | avrdude y su avrdude.conf de la misma instalación |
| Compilar ESP/FileSystem | PlatformIO, proyecto ESP, entorno y linker compatibles |
| Cargar/consultar ESP | Python con pyserial y dependencias de la versión esptool4/5 elegida |
| Preparar la Web AFR2 | Node.js/npm, proyecto Web y sus dependencias; script incluido |
| FileSystem de otros archivos | Script e intérprete configurados por el usuario |

Las herramientas, proyectos, BIN/HEX, paquetes AT y dependencias no vienen
incluidos. No se instalan al iniciar. Un build solicitado puede descargar
dependencias mediante las herramientas externas.

## Configuración de rutas

Configuración permite editar cada herramienta, carpeta, script, imagen y opción.
**Detectar rutas vacías** consulta PATH, instalaciones del usuario actual,
Arduino15 y PlatformIO. Si usás una instalación propia de PlatformIO, se respeta
`PLATFORMIO_CORE_DIR`; también podés indicar cada archivo manualmente.
Se conservan los valores existentes; descubrir no sustituye rutas ya elegidas.

| Referencia | Se resuelve en la computadora receptora |
| --- | --- |
| `{App}` | Carpeta que contiene esta distribución |
| Ruta relativa | Desde `{App}`, independiente de dónde se lanzó el programa |
| `{Repo}` | Raíz del árbol de proyectos configurada por el usuario |
| `{AtmegaDir}` | Carpeta del proyecto AVR configurada |
| `{EspDir}` | Carpeta del proyecto ESP configurada |
| `{WebDir}` | Carpeta de la Web configurada |
| `{Env}` | Nombre del entorno PlatformIO seleccionado |
| `%VARIABLE%` | Variable de entorno del usuario/computadora actuales |

Ejemplos predeterminados: `{App}\scripts\Prepare-Web.ps1`,
`{AtmegaDir}\build\afr2.hex` y `{EspDir}\.pio\build\{Env}\firmware.bin`.
Si trasladás toda la carpeta, el script incluido sigue encontrándose.
Si cambiás EspDir, las rutas nuevas de sus imágenes siguen ese módulo.
Podés colocar los tres proyectos en carpetas o unidades diferentes: no es
obligatorio usar `{Repo}`; configurá cada carpeta directamente.

Las rutas absolutas que elijas son válidas, pero pueden requerir ajustes si
copiás ese perfil a otra computadora. Los perfiles viejos conservan exactamente
sus rutas personalizadas: para convertirlos, elegí las referencias anteriores
y guardá. La plantilla JSON incluida no contiene herramientas de otra persona.
Una variable desconocida, una referencia circular o una raíz sin configurar
producen un error explicativo, en lugar de usar la carpeta de trabajo por accidente.

## Datos de usuario y traslado

Los perfiles, preferencias, backups y registros se crean en
`%LOCALAPPDATA%\AutoFinalRobo2Loader` de **cada usuario receptor**.
No se copian desde el equipo del desarrollador. En una PC que ya usaba Loader
se conservan sus propios ajustes: revisalos antes de cargar.

Para otro destino, definí `AFR2_LOADER_DATA_DIR` con una ruta absoluta antes de
abrir, o usá `Iniciar.ps1 -Datos <carpeta>` en el paquete. No se modifica una
variable global ni se escribe en la carpeta de instalación por defecto.
La GUI y MCP deben recibir el mismo valor, en la misma cuenta de Windows.
El prompt de la GUI incluye su ubicación efectiva; puede regenerarse al mover
el paquete. `Mcp.ps1 -Datos <carpeta>` permite el mismo ajuste explícito.

Compartí el ZIP original de distribución. No agregues los perfiles exportados,
registros, backups, credenciales Wi-Fi ni las dependencias de tu computadora.

## FileSystem y memoria

El script Web suministrado es específico del árbol Web/ESP de AFR2: espera el
proyecto ESP hermano del proyecto Web, por cómo funciona su promotor. Para
proyectos separados u otros archivos, elegí un script propio en Configuración
→ ESP → Preparación FileSystem. El Loader entrega las carpetas elegidas por
variables de entorno; el script no contiene una ruta del desarrollador.
Ver FILESYSTEM_SCRIPTS.md para su contrato y argumentos.

Entorno PlatformIO, capacidad de flash, linker, offsets y velocidades son
configurables. Los valores de ejemplo no identifican ni prueban una placa.
Las comprobaciones de capacidad, preparación previa, hash y cableado siguen activas.

## Servidor MCP

MCP es opcional. El paquete incluye `runtime/AFR2Loader.Mcp.exe` y `Mcp.ps1`,
que calcula su ruta desde la carpeta extraída. En Servidor MCP, generá el
prompt de alta en la ubicación final y el prompt de prueba de esa ventana.
Mover el paquete requiere actualizar el registro del cliente, no reconstruir
el Loader. No hay una URL HTTP ni credenciales de otro usuario.

## Integridad del paquete

`distribucion.json` lista archivos, tamaños y SHA-256 relativos al paquete.
El archivo `.zip.sha256` acompaña el ZIP. No incluyen rutas del equipo de build.
El empaquetador usa una lista explícita y no copia datos de LOCALAPPDATA,
directorios de proyectos, cachés, dependencias ni firmware.

Antes de comprimir, `Preparar-Distribucion.ps1` revisa cada archivo, incluidos
los EXE, y cancela el ZIP si encuentra la carpeta del proyecto, el perfil o
LOCALAPPDATA de quien lo arma, su usuario, el nombre del equipo o una ruta con
letra de unidad. Las rutas del equipo de build se calculan al ejecutar;
el script no tiene ninguna fija y puede correrse desde cualquier carpeta clonada.

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\Preparar-Distribucion.ps1"
```

Genera `AFR2-Loader-dist-<versión>` y su `.zip` junto a la carpeta del Loader
(o en `-Destination`). No sobrescribe una entrega existente.

## Apariencia local
Opciones → Apariencia permite Claro, Oscuro o Seguir al sistema. Se guarda
en preferences.json del usuario receptor; no se incluye una preferencia del
autor en el ZIP. Los perfiles de herramientas y rutas son independientes.
