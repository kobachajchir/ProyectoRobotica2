# Compilación — AFR2-20261024-12

24/10/2026. Pedido expreso previo de compilar y corregir la integración.
Web `npm run build:esp`, AVR `make firmware communication-firmware library`
con avr-gcc7.3.0 y ESP `platformio run -e esp12s_candidate`: exit0.
Se corrigieron errores TypeScript de imports sin usar y tipado de un reduce.
El build Web ejecuta su generación de recursos OLED, TypeScript, Vite, minificación,
gzip y copia. No se ejecutan suites ni validadores independientes.

| Resultado | Valor |
| --- | --- |
| Activos Web | 7 |
| Raw / gzip | 3006111 /1472196bytes |
| Estimación LittleFS / presupuesto | 1556480 /2867200bytes |
| Margen estimado | 1310720bytes |
| AVR .text / .data / .bss / .noinit | 63596 /810 /4072 /9bytes |
| AVR flash / SRAM estática con noinit | 64406 /4891bytes |
| ESP RAM / flash | 46088 /424869bytes |

Logs [Web](web-build.txt), [AVR](atmega-build.txt), [tamaño AVR](atmega-size.txt),
[ESP](esp-build.txt), [manifest](manifest.json), [resumen](summary.json).
Lectura de hashes registra igualdad de los7activos entre paquete y ESP/data/dist.
GLB final827232bytes conservado. Esto es promoción local de archivos, sin upload.

El firmware AVR incluye fuente IRQ11 y distancia12, con Hall/memoria0. Los gates
pueden eliminar por optimización ramas físicas; SRAM estática no mide pila/ISR
ni acredita recursos con Hall/secuencias habilitadas. ESP conserva linker
candidato4MiB/FS3MiB; no identifica el chip físico. Sin imagen de filesystem.

No hay ejecución de navegador/animaciones/gestos, tests de interoperabilidad,
placas, serial/reset, flasheos ni CAD. No se reintenta acceso de navegador denegado.
Los logs se guardaron inicialmente en una carpeta11 de esta tarea y se trasladaron
a12 al encontrar que el ID11 ya correspondía al cambio IRQ; esa ficha histórica
y su ausencia de compilación original permanecen conservadas.
