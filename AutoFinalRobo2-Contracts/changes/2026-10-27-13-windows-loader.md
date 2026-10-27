> **Ubicación posterior AFR2-20261027-14:** la herramienta creada en esta ficha fue trasladada a [AutoFinalRobo2-Loader](../../AutoFinalRobo2-Loader/README.md), módulo propio en la raíz. El cuerpo describe su ubicación original en13. [Traslado](2026-10-27-14-loader-root-module.md).

# AFR2-20261027-13 - AFR2 Loader para Windows

27/10/2026. Pedido humano: un programa similar a XLoader, configurable para
los compañeros, con compilación/carga de ATmega, ESP y LittleFS, monitor que
detecte modo programación, asistentes de cableado/botones, confirmación de
escritura, verificación automática, registro, manual y datos del autor.

## Entrega

[AFR2 Loader0.1.0](../../AutoFinalRobo2-Loader/README.md), dentro de tools de Contracts,
sin crear un sexto módulo ni Git anidado. Aplicación WinForms en C# compatible
con el compilador de .NET Framework4.x; lanzador CMD/PowerShell que genera el
EXE en LOCALAPPDATA en el primer inicio explícito del usuario. No requiere
Visual Studio/VS Code. [Manual local portable](../../AutoFinalRobo2-Loader/MANUAL.html).

- Rutas configurables y exploradores: proyectos, Make, GCC/AR/objcopy,
  avrdude/conf, PlatformIO, Node/npm, Python/esptool, HEX, BIN, FS y linker.
  No fija D:/ProyectoRobotica2, nombre de usuario ni COM16 en el programa.
- Perfiles JSON por usuario, importación/exportación, detección de rutas
  habituales y plantilla sin credenciales. Puerto COM seleccionable.
- Builds secuenciales: AVR producto/comunicación/biblioteca; ESP; Web build:esp
  con copy:esp y después buildfs. Se detiene al primer fallo. No compila ni
  descarga dependencias al simplemente abrir la ventana.
- Escucha74880: detecta modo1, diferencia modo3 y cierra el puerto antes de
  esptool. La detección de boot no se toma como comunicación bidireccional;
  flash_id exitoso es obligatorio antes de cualquier escritura ESP.
- Asistentes separados de cableado Mega/ESP y confirmaciones con casillas.
  Paso final muestra puerto, archivos, tamaños, hashes y offsets. FS exige
  reconocer reemplazo de preferencias. No mueve cables ni botones por software.
- Flash ESP con esptool4/5, sin auto-reset supuesto. Mismo linker/rangos y
  capacidad medida exigidos. write_flash y verify_flash separados; falla de
  verificación no se declara éxito ni reescribe automáticamente.
- ATmega por avrdude, sin -V, con firma y mensaje de bytes verificados
  exigidos para éxito. Sin fuses/EEPROM/bootloader/erase completo.
- Procesos asíncronos, salida stdout/stderr en vivo, timeout, cancelación de
  árbol, puerto liberado al terminar y guardado de log/JSON por operación.
- Acerca de: Koba Chajchir, GitHub kobachajchir, LinkedIn kchajchir y teléfono
  +543884889973, tomados de DeveloperPage.tsx; no se inventa correo.

## Distinciones de hardware y resultados

El Mega ya tuvo un flasheo manual exitoso: el usuario aportó avrdude con firma
0x1e9801 y73306bytes escritos/verificados. El monitor manual ESP mostró
boot mode:(1,6), pero las consultas chip_id posteriores no obtuvieron datos.
Eso no acredita carga ESP ni funcionamiento de esta herramienta nueva.

El programa conserva esa distinción: modo1 observado prueba recepción del
mensaje de arranque; flash_id prueba una conversación con ROM; verificación
de contenido escrito no prueba Wi-Fi/sensores/motores. Guardar perfiles no
comprueba tensiones, continuidad, alimentación o colocación de jumpers.

## Estado real por módulo

| Módulo | Estado de esta entrega |
| --- | --- |
| contracts | Programa, lanzadores, plantilla y manual escritos |
| atmega | Sin cambio de firmware/build/flash en esta ficha |
| esp | Sin cambio de firmware/build/filesystem/flash en esta ficha |
| web | Solo lectura de datos públicos de la sección Desarrollador; sin edición |
| cad | Sin intervención |

**No se compiló ni ejecutó la aplicación, no se ejecutaron tests, validadores,
monitor serie, esptool, avrdude, builds de firmware/FS, flasheos ni pruebas
visuales/físicas en esta entrega**, conforme a la instrucción del proyecto.
El primer lanzamiento explícito del usuario ejecutará la compilación local
de la herramienta. No se acredita interoperabilidad por inspección del fuente.
No hubo commit/push ni instalación/elevación de herramientas.

## Referencias de implementación

- Guía local CARGA_USB_COM16.md, FLASHING.md, WIFI_SHIELD.md y Makefile/scripts.
- CLI real de esptool4.5.1 instalada, consultada por lectura de sus fuentes.
- [Espressif: operaciones de flash](https://docs.espressif.com/projects/esptool/en/latest/esp8266/esptool/basic-commands.html).
- [Espressif: arranque y logROM](https://docs.espressif.com/projects/esptool/en/latest/esp8266/advanced-topics/boot-mode-selection.html).
- Datos de autoría en AutoFinalRobo2-Web/src/features/developer/DeveloperPage.tsx.
