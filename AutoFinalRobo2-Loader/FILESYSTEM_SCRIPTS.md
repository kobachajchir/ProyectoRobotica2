# FileSystem por scripts — Loader 1.1.0

AFR2-20261029-01 · Koba Chajchir / AFR2; adaptación mediante Codex.

## Flujo

Al preparar FileSystem (solo o junto al firmware), el Loader ejecuta un script del perfil. El script compila, transforma o copia lo que el proyecto necesite a `ESP/data`. Después el Loader ejecuta PlatformIO `buildfs`, comprueba el tamaño de `littlefs.bin` contra `FsBytes` y registra la preparación de esa imagen. Preparar no abre el puerto ni flashea. Cargar es una acción posterior con las comprobaciones de capacidad, layout y verificación existentes.

El núcleo del Loader no ejecuta npm ni requiere package.json o manifiestos web. Nuestro script `scripts/Prepare-Web.ps1` contiene `npm run build:esp` y la comprobación del manifiesto `esp`; conserva el promotor y el presupuesto de nuestra Web. Otro proyecto puede reemplazar ese script. Esta versión generaliza la preparación del contenido; la generación de la imagen sigue utilizando un proyecto ESP8266 PlatformIO con LittleFS y su carpeta de datos estándar `data`. No incorpora otros formatos de filesystem ni reemplaza el compilador AVR/ESP. El paquete AT mantiene su revisión de imágenes precompiladas, sin ejecutar el script.

## Vincular el script

En **Configuración → ESP → Preparación FileSystem**, elegí:

| Campo | Función |
| --- | --- |
| Script de preparación FileSystem (`FsScript`) | Ruta del archivo que creaste. Admite `{Repo}` y `{Env}` como otras rutas del perfil. |
| Fuentes del FileSystem (`FsSourceDir`) | Carpeta de trabajo e inputs; separada de ESP/data, sin contenerse entre sí. |
| Ejecutable del script (`FsScriptHost`) | Ruta de powershell.exe, pwsh.exe, python.exe u otro intérprete. |
| Argumentos del script (`FsScriptArguments`) | Lista JSON de argumentos separados. `{Script}` se reemplaza por la ruta completa. No es una línea de shell. |

Ejemplo PowerShell: `["-NoProfile","-ExecutionPolicy","Bypass","-File","{Script}"]`.

Ejemplo Python: `["{Script}"]`, eligiendo python.exe como ejecutable y un archivo .py como script.

Los argumentos adicionales pueden agregarse como elementos de la lista. No se expanden variables ni operadores de shell en esos elementos; el script puede leer las variables de entorno del contrato. Las rutas con espacios se pasan como un único argumento. El argumento -ExecutionPolicy Bypass se aplica solo al proceso PowerShell de preparación; no modifica la política persistente de Windows. Las políticas de grupo pueden prevalecer. Usalo con scripts propios o revisados.

Conservá **Proyecto ESP**, **PlatformIO**, **Entorno**, **Linker**, **Imagen FileSystem**, offset y tamaño acordes al firmware y a la flash real. Imagen FileSystem debe ser `.pio/build/<entorno>/littlefs.bin` dentro del proyecto ESP. Guardá el perfil para reutilizar la configuración.

## Error «running scripts is disabled»

AFR2-20261029-22: en Configuración → ESP → Preparación FileSystem, cambiá Argumentos del script (JSON) a `["-NoProfile","-ExecutionPolicy","Bypass","-File","{Script}"]` y guardá el perfil. La aplicación ya instalada admite este cambio sin recompilarse. Los perfiles existentes conservan sus argumentos: hay que corregirlos explícitamente. No ejecutes Set-ExecutionPolicy para cambiar la política general. Volvé a preparar FileSystem cuando corresponda; el cambio de argumentos invalida la preparación anterior.

## Nuestra Web

Los perfiles anteriores reciben los campos nuevos al cargarse. El script predeterminado es `{Repo}/AutoFinalRobo2-Loader/scripts/Prepare-Web.ps1`, el intérprete es Windows PowerShell y la carpeta de fuentes inicial es Proyecto Web. Configurá Node/npm como antes. Si movés Proyecto Web, actualizá también Fuentes del FileSystem.

El script Web ejecuta build:esp desde Proyecto Web. Nuestro promotor existente escribe en el proyecto ESP hermano; el script comprueba esta distribución antes de compilar. Para otras distribuciones o presupuestos hay que adaptar el script/promotor específico, no el lanzador genérico.

## Contrato del script

El Loader entrega estas variables de entorno al proceso:

| Variable | Contenido |
| --- | --- |
| `AFR2_FS_SOURCE_DIR` | Carpeta absoluta de entradas y trabajo. |
| `AFR2_FS_DATA_DIR` | Carpeta absoluta de salida: ESP/data. |
| `AFR2_ESP_DIR` | Proyecto ESP absoluto. |
| `AFR2_FS_BYTES` | Tamaño de la partición, en bytes decimales; no equivale al espacio neto para archivos. |
| `AFR2_WEB_DIR` | Proyecto Web, opcional para scripts genéricos. |
| `AFR2_NPM` | Ejecutable npm, opcional para scripts genéricos. |

El script debe producir el conjunto completo de archivos deseados dentro de `AFR2_FS_DATA_DIR` y devolver código 0 solo si terminó correctamente. La ruta relativa dentro de esa carpeta será la ruta dentro de LittleFS. No necesita generar manifest.json, gzip ni HTML: esos son requisitos de nuestra Web. No debe abrir COM, flashear, modificar perfil/linker ni escribir archivos generados dentro de las fuentes registradas. Las dependencias que estén fuera de FsSourceDir deben incorporarse a esa carpeta para quedar cubiertas por el registro de preparación.

Usá stdout/stderr para informar avances y errores. Fallo de salida, cancelación o timeout detienen el flujo antes de buildfs y no acreditan una nueva preparación. La salida parcial queda en disco; corregí o limpiá su contenido antes de repetir. El script tiene los permisos del usuario; elegí scripts propios o revisados. El Loader registra comando, salida, código y hash del script y permite cancelar con el mecanismo de procesos existente.

El Loader no limpia ESP/data automáticamente: incluye todo lo que quede allí, también archivos anteriores. Cada script debe gestionar su contenido explícitamente. Un directorio vacío se rechaza. El ejemplo de archivos exige destino vacío para evitar mezclar contenidos. Esto no acredita que los archivos tengan sentido para el firmware: sus nombres/formato deben coincidir con lo que este espera leer.

## Ejemplo sin Web

1. Creá una carpeta de fuentes, por ejemplo `MiContenido`, y dentro una carpeta `payload` con datos, imágenes, configuración u otros archivos.
2. Copiá `scripts/Prepare-Files.example.ps1` a una ubicación propia y elegila en Script de preparación. Usá `MiContenido` como Fuentes del FileSystem.
3. Elegí PowerShell como ejecutable y `["-NoProfile","-ExecutionPolicy","Bypass","-File","{Script}"]` como argumentos.
4. Respaldá y vaciá manualmente ESP/data si contiene otra colección de archivos. El ejemplo se detiene si encuentra contenido previo; no lo borra por su cuenta.
5. Consultá la capacidad del ESP, elegí su layout y FileSystem, y pulsá Compilar. El script copiará payload y el Loader generará littlefs.bin.
6. Revisá el resultado y cargá FileSystem cuando corresponda. La carga reemplaza el filesystem del dispositivo.

Fragmento esencial de un script PowerShell propio:

```powershell
$ErrorActionPreference = 'Stop'
try {
    $entrada = Join-Path $env:AFR2_FS_SOURCE_DIR 'payload/datos.json'
    $salida = Join-Path $env:AFR2_FS_DATA_DIR 'datos.json'
    Copy-Item -LiteralPath $entrada -Destination $salida -Force
    # Si llamás un compilador externo, comprobá también $LASTEXITCODE.
    exit 0
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
```

Este fragmento actualiza un archivo; no sincroniza ni elimina otros existentes. El ejemplo completo suministrado copia un árbol y exige destino vacío.

## Preparación y límites

Cambiar perfil, argumentos, intérprete, script, entradas registradas, layout o contenido de salida invalida la preparación previa. El hash completo se comprueba antes de cargar. Se excluyen de las entradas las carpetas habituales de compilación/caché (`dist`, `build`, `.pio`, `node_modules`, `.git`, `__pycache__`, `.cache`) y enlaces de directorios; no pongas entradas necesarias solo en esas carpetas. ESP/data completo sí se registra después de preparar. No modifiques entradas durante la ejecución.

La acción MCP conserva el identificador histórico `build_web_fs`, pero usa el mismo script del perfil que la GUI. Su nombre no exige contenido Web. Se mantienen las aprobaciones existentes y los detalles muestran el lanzador configurado.

Entrega 1.1.0 en fuente y documentación; scripts suministrados sin ejecutar, sin compilaciones, tests, validadores ni pruebas físicas de esta revisión.
