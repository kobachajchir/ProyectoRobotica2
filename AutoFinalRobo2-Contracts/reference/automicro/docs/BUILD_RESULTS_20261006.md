# Compilación de seguimiento de línea — 2026-10-06

Pedido expreso: «Compila todo y veamos si algo falla». Se compilaron los cuatro checkouts locales modificados, preservando cambios previos. No se ejecutaron suites de tests, aplicación Qt, validación visual, flasheo ni pruebas físicas.

| Proyecto | Comando/configuración | Resultado |
| --- | --- | --- |
| FirmwareF4 | GNU ARM 14.3.1, make -j4 all, Debug | exit 0; enlace y empaquetado completados |
| Firmware ESP01 | PlatformIO run -e esp01_1m | exit 0; firmware.bin generado |
| Web ESP01 | npm run build:esp | exit 0; TypeScript, Vite, gzip y copia ESP completados |
| AutoMicroQtDesktop | CMake build-msvc, Release, target AutoMicroQtDesktop, parallel 4 | exit 0; ejecutable generado |

## Fallos corregidos

- F4: las nuevas pantallas usaban helpers privados de oled_utils.c. Se agregaron entradas públicas de render para aviso/calibración; el menú mantiene transiciones y pasa estado al renderer. Se acortaron instrucciones OLED a líneas que entran con Font_7x10. Eliminada variable IR sin uso y aclarada indentación de reply/return.
- F4: Debug con -O0 excedía en 11.700 bytes la región Flash segura anterior al sector 7. Se configuró Debug con -Og en .cproject y la regla Core/Src generada vigente, conservando -g3; no se amplió Flash ni se cambió el linker. Este build aplica -Og al código Core y conserva las reglas restantes de Debug. La regeneración CubeIDE usa el nivel Debug configurado.
- ESP: config.h contenía un inicializador de miembro C++ que fallaba en wifi_scan_snapshot.c. Se preservó el valor inicial 1 en C++ y la declaración sin inicializador en C; no se cambió el layout ni la política AP.
- Web: un import de toggleButton usaba distinta capitalización; se alineó con los imports ToggleButton existentes. SensorCameraViewIcon usa ahora una función con tuplas Vector contextualizadas, sin casts de arrays de longitud incierta.
- Qt: compilación Release completada sin error; los recursos QML nuevos se incorporaron al módulo. Este resultado no demuestra que todas las vinculaciones QML estén correctas en ejecución.

## Artefactos y tamaños

- F4: Debug/ProyectoAutoMicro2026_F411CEU6.bootable.elf/.bin/.hex/.sha256. BIN empaquetado final: **274.096 bytes**, CRC32 0x4FE710C8. El postbuild también actualizó el ELF usado por CubeIDE.
- ESP: .pio/build/esp01_1m/firmware.bin. RAM **67.744/81.920 bytes (82,7%)**, Flash **506.267/1.044.464 bytes (48,5%)**.
- Web: dist/index.html.gz, **1.010.554 bytes**, copiado a data/dist/index.html.gz en ESP. Modelo vigente copiado: **883.260 bytes**. HTML sin comprimir no se copió; Draco y otros archivos se conservaron.
- Qt: build-msvc/Release/AutoMicroQtDesktop.exe; compiló y enlazó el target aplicación, sin ejecutar targets de tests.

## Advertencias y límites

F4 conserva advertencia linker LOAD RWX. ESP conserva avisos de APIs ArduinoJson obsoletas, funciones sin uso y signedness; no se hizo una migración general de dependencias. Qt informa Vulkan headers ausentes durante configuración, sin impedir el build. Web puede informar desactualización de Browserslist/caniuse-lite.

Los logs finales quedan como build-line-follow.log en Debug F4 y las raíces ESP/Web/Qt. No se añadió un commit de código que incorpore trabajo ajeno; solo se publica este resultado en la central y se actualiza AMI-20261006-01. Comunicación, calibración óptica, control de motores, persistencia física y apariencia de pantallas siguen sin comprobar en hardware.

## Comparación F4 -O2 solicitada posteriormente

Compilación limpia aislada en FirmwareF4/BuildSizeO2_20261006, con -O2 en todas las reglas que tenían nivel de optimización (Core y HAL/USB), conservando defines DEBUG, símbolos y linker. Se copiaron solo recetas/object list y se generaron objetos nuevos. Exit 0, incluido empaquetado. No se cambió .cproject ni se reemplazaron los artefactos Debug actuales. Solo comparación de tamaño; sin ejecución ni flasheo.

| Variante | BIN empaquetado | Margen sobre región de aplicación 360.448 bytes (352 KiB) |
| --- | --- | --- |
| Debug actual, -Og en Core y reglas restantes anteriores | 274.096 bytes | 86.352 bytes (84,33 KiB; 23,96%) |
| Comparativa -O2 en todas las reglas aplicables | 251.272 bytes | 109.176 bytes (106,62 KiB; 30,29%) |

Ahorro de BIN: 22.824 bytes (22,29 KiB). GNU size -O2: text 242.988, data 8.176, bss 26.256; datos+BSS 34.432 bytes. El tamaño físico de Flash total no es el presupuesto de aplicación: se conservan bootloader y sectores reservados. El margen se calcula con el BIN empaquetado, incluyendo huecos/manifest; text+data no sustituye esa medida. Este resultado es el margen con -O2, no una demostración del mínimo absoluto posible con otras optimizaciones. Permanece advertencia RWX.
