# Compilación coordinada — 2026-10-02

Pedido expreso del usuario: compilar y corregir errores de compilación. Se ejecutaron los builds de los checkouts locales modificados; no se ejecutaron suites de tests, CI, validación funcional ni flasheo.

| Repositorio | Comando/configuración | Resultado | Artefacto |
|---|---|---|---|
| FirmwareF4 | STM32CubeIDE 2.1.1 GNU ARM 14.3.1, `make -j4 all` en Debug | OK, incluido post-build | Debug/ProyectoAutoMicro2026_F411CEU6.bootable.elf/.bin/.hex/.sha256 |
| Firmware ESP01 | PlatformIO `run -e esp01_1m` | OK | .pio/build/esp01_1m/firmware.bin |
| Web ESP01 | `npm run build`: tsc -b, Vite, gzip postbuild | OK | dist/index.html y dist/index.html.gz |
| AutoMicroQtDesktop | CMake `--build build-msvc --config Debug --parallel 4`, Qt 6.9 MSVC2022 | OK | build-msvc/Debug/AutoMicroQtDesktop.exe |

## Correcciones realizadas

- Web: imports de ToggleButton y RealtimeEulerPanel con la misma capitalización del archivo real; eliminados TS1261.
- WebGLCanvas: tipo de parámetros derivado de CanvasProps instalado, fábrica síncrona y error boundary que conserva aviso/reintento ante fallo de inicialización; eliminados TS2305/TS2322. No se probó el visor en navegador por este pedido.
- F4: empaquetado SHA256 mediante .NET en lugar de Get-FileHash, ausente en el PowerShell usado por post-build. Se conserva comparación BIN/ELF, formato de manifest, CRC y emisión de hashes; guardas de limpieza de directorio temporal. Primer intento compiló/enlazó pero falló post-build; segundo completó `all` con exit0.

No cambian IDs, payloads ni completion de los contratos A–F. Los cambios de código siguen locales y no se incorporó trabajo previo Web a commits por arrastre.

## Advertencias y límites

- F4: linker advierte segmento LOAD con permisos RWX. ELF text316336, data7108, bss25172; bin empaquetado323548 bytes. No se cambió linker script.
- ESP: advertencias de APIs obsoletas ArduinoJson, redefinición WIFI_CONNECT_TIMEOUT_TICKS, signedness y funciones no utilizadas. Build exitoso: RAM64472/81920 (78.7%), flash493323/1044464 (47.2%). No se convirtió la compilación en una migración general de librerías.
- Web: Browserslist/caniuse-lite desactualizado; build exitoso, 1148 módulos. No se actualizaron dependencias.
- Qt: configuración MinGW bloqueada por Qt6WebSockets no instalado. Alternativa MSVC existente tiene los módulos y compiló/enlazó correctamente. Aviso de headers Vulkan ausentes no bloqueó configuración MSVC. No se instalaron módulos ni se alteraron funciones para ocultar dependencias.
- El build Qt también compiló targets de tests definidos por CMake; no ejecutó sus binarios ni ctest.

La compilación comprueba tipos, símbolos y enlace de las rutas incluidas. No demuestra comunicación F4↔ESP/USB/WebSocket ni comportamiento físico. Para instalar: F4 antes de ESP, luego consumidores. Hardware, tests y despliegue continúan pendientes.

## Actualización modelo final

AMI-20261002-06: ESP volvió a compilar correctamente tras modificar caché de respuesta GLB. PlatformIO esp01_1m exit0; firmware.bin generado. RAM64472 bytes; flash493355 bytes. Sin ejecución de tests ni carga al hardware.

## Dependencia Qt MinGW instalada

2026-10-02, pedido expreso del usuario: Qt MaintenanceTool instaló correctamente `qt.qt6.690.addons.qtwebsockets.win64_mingw` versión6.9.0-0-202503301022, junto con documentación/ejemplos del componente. Confirmados `C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6WebSockets/Qt6WebSocketsConfig.cmake` y `bin/Qt6WebSockets.dll`. Se usaron los repositorios oficiales Qt 6.9.0 desktop y documentación; no se actualizó la versión del kit ni otros módulos. El bloqueo de dependencia WebSockets quedó resuelto; el build MinGW no se volvió a ejecutar en esta instalación. El build MSVC exitoso previo conserva su resultado.
