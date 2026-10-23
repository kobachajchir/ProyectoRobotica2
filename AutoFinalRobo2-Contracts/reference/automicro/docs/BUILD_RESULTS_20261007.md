# Compilación GOTO y calibración de motores — 2026-10-07

Pedido expreso: «Compila todo a ver si algo falla». Se compilaron los cuatro checkouts modificados y se corrigieron errores encontrados. No se ejecutaron suites de tests, aplicación Qt, navegador, flasheo ni ensayos físicos.

| Proyecto | Configuración | Resultado final |
| --- | --- | --- |
| FirmwareF4 | GNU ARM14.3.1; make -j4 all en Debug, Core -O0 | exit0; enlace y empaquetado completados |
| Firmware ESP01 | PlatformIO run -e esp01_1m | exit0; firmware.bin generado |
| Web ESP01 | npm run build:esp | exit0; TypeScript, Vite, gzip y copia ESP completados |
| AutoMicroQtDesktop | CMake build-msvc, Release, target AutoMicroQtDesktop, parallel4 | exit0; ejecutable y recursos QML generados |

## Correcciones

- Web: el fieldset de Libre se cerraba después del contenedor exterior. Se reordenaron los cierres para resolver TS2657/TS17008/TS17002. Commit local443e729. Segundo build completo exitoso.
- Qt: separador inválido después del bloque onClicked del botón Curva en MotorCalibration.qml. qmlimportscanner detectó Unexpected token aunque MSBuild produjo exit0; se corrigió y se recompiló hasta que desapareció el diagnóstico. Commit local2cff854.
- F4: se incorporó auto_motion.c a C_SRCS/OBJS/C_DEPS y objects.list del Debug generado para incluir el módulo nuevo en el enlace. Recetas generadas locales; no se cambió optimización, linker ni fuentes F4.
- ESP: bundle Web actualizado por postbuild, commit local1d3d2a5. Modelo vigente copiado sin modificarlo. Sin cambio nuevo de wire.

## Artefactos

- F4: Debug/ProyectoAutoMicro2026_F411CEU6.bootable.bin, **387.656 bytes**; CRC32 0x6D427EB0. GNU size: text379.276, data8.276, bss27.040. Aplicación vigente de480 KiB: margen sobre BIN empaquetado **103.864 bytes (101,43 KiB)**. Ese margen corresponde al linker de producción vigente, no al modo alternativo que preserva sector7.
- ESP: .pio/build/esp01_1m/firmware.bin, **515.376 bytes**. RAM68.708/81.920 (83,9%); Flash511.223/1.044.464 (48,9%).
- Web: dist/index.html.gz y data/dist/index.html.gz ESP, **1.020.367 bytes**. Modelo copiado883.260 bytes; no se copió HTML sin comprimir. Vite procesó1183 módulos.
- Qt: build-msvc/Release/AutoMicroQtDesktop.exe, **14.819.840 bytes**.

Logs finales: Debug/build-goto.log en F4 y build-goto.log en raíces ESP/Web/Qt. Los artefactos y logs permanecen locales.

## Advertencias y límites

F4 conserva LOAD RWX. ESP conserva avisos ArduinoJson obsoleto y funciones sin uso. Web avisa Browserslist/caniuse-lite desactualizado. La configuración Qt informó Vulkan headers ausentes; no bloqueó enlace ni empaquetado. No se actualizaron dependencias para silenciar avisos.

Estos builds demuestran compilación, tipos/símbolos incluidos y enlace/empaquetado. No prueban integración física, exactitud del metro, signo de giro, evasión, Flash ni apariencia de las pantallas. El gzip está copiado al directorio ESP; no se generó/cargó una imagen de filesystem ni se instaló firmware en placas. AMI-20261007-01 se actualiza con este resultado; la central se publica, los commits de código quedan locales.

