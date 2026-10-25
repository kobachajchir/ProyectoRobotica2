# Compilación — chevrones bajo el chasis

25/10/2026, AFR2-20261025-06. Web build:esp exit0: TypeScript/Vite,
minificación, gzip y promoción local. Pedido previo de compilar/corregir vigente.
[Log](web-build.txt) de la fuente final, sin errores.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1482047 bytes |
| Diferencia gzip respecto05 |+95bytes |
| Raw |3043040 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |
| GLB final conservado |827232bytes |

[Manifest](manifest.json) y [resumen](summary.json): hashes coincidentes entre
los siete activos y manifest del paquete/ESP data. Firmware ESP/AVR conserva
[build01](../2026-10-25-01/README.md), sin recompilar ni cambiar gates0.

Dos fuentes propias: SVG/ManualDirection y CSS de presentación. Tres chevrones
grandes, origen central bajo el modelo, degradé/máscara transversal; fade220ms
y ciclo1200ms. Contenedor decorativo adapta dimensiones; Canvas/input/cámara
conservados. Sin nuevas dependencias, activos externos, blur/filtros o CAD.
Captura, modo local sin conexión04 y órdenes físicas no cambian.

No se renderizó ni comprobó en navegador/teléfono. Sin tests/validadores
independientes, serie, imagen FS, flasheo o pruebas físicas. Compilación no
acredita aceptación estética u oclusión observada. Layout3MiB sigue candidato,
flash/SPI físicos pendientes.
