# Compilación — estela de degradé direccional

25/10/2026, AFR2-20261025-05. Web build:esp exit0: TypeScript/Vite,
minificación, gzip y promoción local. Pedido previo de compilar/corregir vigente.
[Log](web-build.txt) de la fuente final, sin errores.

| Resultado | Valor |
| --- | --- |
| Web / firmware conservado / EX01 |0.2.0 /0.9.0 /7 |
| Activos / gzip |7 /1481952 bytes |
| Diferencia gzip respecto04 |+277bytes |
| Raw |3042329 bytes |
| LittleFS estimado / presupuesto |1572864 /2867200 bytes |
| GLB final conservado |827232bytes |

[Manifest](manifest.json) y [resumen](summary.json): hashes coincidentes de
los siete activos y manifest en paquete/ESP data. Firmware ESP/AVR conserva
[build01](../2026-10-25-01/README.md), sin recompilar ni cambiar gates0.

SVG inline/CSS propios: estela de tres trazos, degradé de acento, entrada/salida
140ms y ciclo800ms linear de±6px. Pausa al soltar, sin reiniciar frame antes
del fade; teclado/reduced motion estáticos. Sin nuevas dependencias/activos,
filtros, raster, CAD o modificación de captura/conexión/simulación04.

No se renderizó ni comprobó visualmente en navegador/teléfono. Sin tests o
validadores independientes, serie, imagen FS, flasheo o pruebas físicas.
Compilación no acredita contraste/animación aceptados. Layout3MiB sigue
candidato; flash/SPI físicos sin identificar.
