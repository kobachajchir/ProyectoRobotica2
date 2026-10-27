# AFR2 Loader - Windows

Herramienta gráfica en español para compilar y cargar el software de
AutoFinalRobo2. Hecho por Koba Chajchir. Versión **0.2.0**,
AFR2-20261027-16, 27/10/2026. Desde **AFR2-20261027-14** es un módulo propio
en la raíz, `AutoFinalRobo2-Loader`, al mismo nivel que Atmega, ESP, Web,
Contracts y CAD. Comparte su único Git. [Registro de integración](../AutoFinalRobo2-Contracts/changes/2026-10-27-14-loader-root-module.md).

## Abrir

Hacer doble clic en **Iniciar.cmd**. En el primer inicio, el lanzador genera
el ejecutable WinForms con el compilador de .NET Framework de Windows y lo
guarda en `%LOCALAPPDATA%\AutoFinalRobo2Loader\bin\AFR2Loader.exe`. Si cambian
los fuentes, vuelve a generarlo al iniciar. No requiere Visual Studio ni un
proyecto C# instalado. Requiere Windows 10/11, Windows PowerShell 5.1 y .NET
Framework 4.8 con `csc.exe`; no usa PowerShell 7 para la compilación del programa.

Este arranque solo construye la aplicación. **No compila firmware, no abre COM,
no descarga dependencias ni carga placas hasta elegir una acción.**
No pide elevación ni cambia la política global: ExecutionPolicy Bypass se usa
solo en el proceso del lanzador. No hace falta ejecutar como administrador.

Para generar el EXE sin abrir la ventana, cuando se autorice esa compilación:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Iniciar.ps1 -SoloCompilar
```

## Incluye

- Puerto COM seleccionable y actualización de puertos presentes.
- Configuración de carpetas, herramientas, HEX/BIN, entorno y layout; perfiles
  JSON importables/exportables, sin nombres de usuario ni COM fijos.
- Build de ATmega producto/comunicación/biblioteca, ESP y Web perfil esp.
- Web `build:esp` con promoción a `ESP/data/dist`, seguido de `buildfs`.
- Detección de `boot mode:(1,...)` a74880 con puerto liberado al terminar;
  sincronización ROM y lectura de flash con esptool antes de cada carga ESP.
- Asistentes de cableado ATmega/ESP, confirmación explícita previa a escritura,
  y advertencia de reemplazo de preferencias al cargar LittleFS.
- Carga con avrdude y verificación AVR incorporada; escritura ESP y comando
  `verify_flash`/`verify-flash` posterior contra los BIN seleccionados.
- Registro vivo de stdout/stderr, errores, códigos de salida, SHA-256,
  capacidad detectada, resumen JSON y cancelación de procesos hijos.
- Opciones / ayuda: manual local, carpeta de logs y Acerca de con datos del
  desarrollador tomados de DeveloperPage.tsx, sin agregar correos inventados.
- MCP stdio para Codex, conectado a la misma ventana mediante un canal local
  del usuario. Consultas de estado/perfil/puertos y solicitudes revisables de
  build, diagnóstico, carga y cambios de perfil.
- Pestaña Codex / MCP con solicitud pendiente, comparación de perfil y botones
  Revisar/Rechazar; historial de sesión, filtro de configuración y seguimiento
  de registro opcional. Evidencias UART y ROM diferenciadas por puerto/hora.
- Una operación por sesión; deduplicación de solicitudes MCP, registros
  incrementales y límites de memoria; interrupción de procesos ante fallos.

## Conectar Codex

Abrir **Iniciar.cmd**. Ejecutar **RegistrarMcp.ps1** desde PowerShell para
registrar `afr2-loader`, o copiar el comando de la pestaña Codex / MCP.
Reiniciar/reconectar el cliente MCP. Guía completa, alternativas sin CLI y
herramientas en [MCP.md](MCP.md). La app permanece abierta: el servidor comparte
su trabajo/registro y no flashea fuera de esa sesión.

```powershell
cd "D:\ProyectoRobotica2\AutoFinalRobo2-Loader"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\Iniciar.ps1"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\RegistrarMcp.ps1"
```

El registro modifica la configuración de Codex de quien lo ejecuta. No se
ejecutó en esta entrega. Registrar o listar herramientas no demuestra conexión:
la primera consulta `loader_status` debe devolver la sesión de la ventana.
`awaiting_approval` significa que todavía no se ejecutó la operación.

## Para otro compañero

Copiar **toda la carpeta AutoFinalRobo2-Loader**, incluyendo `src/`, `MANUAL.html`, lanzadores y
plantilla JSON. Para compilar firmware, copiar/clonar también los módulos
Atmega, ESP, Web y Contracts, conservando su relación de carpetas. Para cargar
archivos ya compilados se pueden seleccionar HEX/BIN externos sin compilar;
ESP requiere además el linker correspondiente y esptool/Python.

En Configuración, seleccionar la raíz de su copia, Detectar rutas vacías y
completar lo que falte con `...`. Guardar. Las dependencias de build/carga
son externas: GNU Make y avr-gcc/avr-ar/avr-objcopy, avrdude/conf, PlatformIO,
Node/npm y Python/esptool. No se redistribuyen ni instalan silenciosamente.
Abrir **MANUAL.html** para el recorrido completo y las conexiones explicadas.

El perfil usa `{Repo}`, `{Env}` y variables de Windows como `%USERPROFILE%`.
Las herramientas se detectan usando PATH, Arduino15 y la instalación PlatformIO
del usuario que abre el programa. Todos esos resultados se pueden reemplazar.

## Arquitectura

| Archivo | Responsabilidad |
| --- | --- |
| src/Program.cs | Entrada STA y manejo de errores de interfaz |
| src/Profile.cs | Esquema, valores, resolución de rutas y persistencia de perfiles |
| src/Runner.cs | Procesos externos asíncronos, salida, timeout y cancelación |
| src/Operations.cs | Builds, escucha UART, consulta ROM, layout, escritura y verificación |
| src/MainForm.cs | Ventana, configuración, asistentes, estados y registro |
| src/MainForm.Integration.cs | Revisión MCP, historial, indicadores y filtro |
| src/JobState.cs | Trabajo, estado y registro paginado en memoria |
| src/LocalBridge.cs | Named pipe limitada al usuario, comparte la sesión GUI |
| src/McpServer.cs | Protocolo MCP stdio, herramientas y manual como recurso |
| Compilar.ps1 | Generación local de EXE WinForms o consola MCP; diagnósticos por stderr |
| Iniciar.ps1 / Iniciar.cmd | Apertura de la interfaz |
| Mcp.ps1 / RegistrarMcp.ps1 | Entrada stdio y registro optativo en Codex |
| MCP.md / codex-mcp.example.toml | Configuración portable y contrato operativo MCP |
| MANUAL.html | Manual portable que se abre desde Opciones / ayuda |

No hay Git anidado: Loader es el sexto módulo del repositorio global.
Contracts mantiene los registros compartidos; el código y manual del cargador
pertenecen a esta carpeta propia.
El EXE, los perfiles y los logs de usuario quedan fuera del repositorio.

## Límites de la entrega

Implementada en fuente con lanzador y manual. **No se compiló ni ejecutó la
aplicación en esta entrega; no se hicieron tests, validación de interfaz,
pruebas de puerto, flasheos ni comprobación física.** La instrucción del
proyecto exige un pedido explícito para esas actividades. No se acredita una
versión probada por haber entregado estos archivos. El primer lanzamiento
realizará la compilación local del programa; cualquier error se verá en la
consola del lanzador y no iniciará una carga.

La recepción del mensaje ROM prueba ESP→PC y su modo en ese reset; no prueba
PC→ESP. La aplicación exige una consulta ROM exitosa y flash identificada antes
de escribir. La verificación de flash no acredita sensores, Wi-Fi ni movimiento.
Los asistentes no pueden medir tensiones, mover cables ni garantizar que las
confirmaciones manuales coincidan con el montaje.
