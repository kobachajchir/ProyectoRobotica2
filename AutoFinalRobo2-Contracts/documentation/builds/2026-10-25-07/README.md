# Compilación — cintas anchas y punta intensa

25/10/2026, AFR2-20261025-07. Web build:esp exit0: TypeScript/Vite,
minificación, gzip y promoción local. Pedido previo de compilar/corregir vigente.
[Log](web-build.txt) final sin errores.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1482078 bytes |
| Diferencia gzip respecto06 |+31bytes |
| Raw |3043178 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |
| GLB final conservado |827232bytes |

[Manifest](manifest.json)/[resumen](summary.json): hashes coincidentes de
los siete activos y manifest entre paquete/ESP data. Firmware ESP/AVR conserva
[build01](../2026-10-25-01/README.md), sin recompilar ni cambiar gates0.

Tres fuentes propias adaptadas: SVG de bandas más gruesas y punta opaca,
CSS de extensión/alto y cámara manual con encuadre52% frente66%. No se cambia
malla/CAD, unidades, cámaras de otros mapas, captura, modo local04 o comandos.
Fade220ms/flujo1200ms conservados, recorrido±12unidadesSVG. Sin nuevas
dependencias, activos externos, filtros, flasheo o pruebas físicas.

Proporciones verificadas por lectura de fuente, no medidas de un render.
Sin navegador/teléfono, tests/validadores independientes, serie o imagen FS.
Compilar no acredita aceptación estética. Layout3MiB sigue candidato,
flash/SPI físicos pendientes.
